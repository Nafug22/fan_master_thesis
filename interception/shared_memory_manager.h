#ifndef SHARED_MEMORY_MANAGER_H
#define SHARED_MEMORY_MANAGER_H

#include <cstring>
#include <sys/shm.h>
#include <stdio.h>
#include <iostream>
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>
#include <vector>
#include <string>
#include <cstdint>

#define STRING_ARG_PORT 1
#define VGPU_PORT 2
#define COMMAND_PORT 3
#define RESPONSE_PORT 4
#define SCALAR_ARG_PORT 5
#define CUDA_ARG_PORT 6

/**
 * @class SharedMemoryManager
 * @brief The generic shared memory manager.
 */
class SharedMemoryManager{
  public:
    /**
     * @param memory_name name of the shared memory region
     * @param memory_size memory size of the shared memory region
     */
    explicit SharedMemoryManager(int id, size_t memory_size){
        shm_id = shmget(id, memory_size, IPC_CREAT | 0666);
        if (shm_id == -1) perror("shmget failed");

        shm_addrs_ = shmat(shm_id, NULL, 0);
        if(shm_addrs_ == (void *)-1) perror("shmat failed");
    };
    ~SharedMemoryManager(){
        shmdt(shm_addrs_);
        shmctl(shm_id, IPC_RMID, NULL);
    }

  protected:
    void* shm_addrs_ = nullptr;

  private:
    int shm_id;
};

class CUDAArgs : public SharedMemoryManager{
  public:
    CUDAArgs() : SharedMemoryManager(CUDA_ARG_PORT, sizeof(size_t) + sizeof(uint64_t) * 100),
                 size_ptr_(static_cast<size_t*>(shm_addrs_)),
                 args_(reinterpret_cast<void*>(size_ptr_ + 1)){};
    ~CUDAArgs(){};

    size_t get_size() {return *size_ptr_; };
    void* get_ptr() {return args_; };

  private:
    size_t *size_ptr_;
    void *args_;
};

/**
 * @class ScalarArg
 * @brief Stores the kernel arguments when launching cuda functions.
 */
class ScalarArgs : public SharedMemoryManager{
  public:
    ScalarArgs() : SharedMemoryManager(SCALAR_ARG_PORT, sizeof(size_t) + sizeof(uint64_t) * 100),
                   size_ptr_(static_cast<size_t*>(shm_addrs_)),
                   args_(reinterpret_cast<uint64_t*>(size_ptr_ + 1)){};
    ~ScalarArgs(){};

    void set(std::vector<uint64_t> &scalar_args){
      *size_ptr_ = scalar_args.size();
      memcpy(args_, scalar_args.data(), scalar_args.size() * sizeof(uint64_t));
    }

    uint64_t* get_scalar_ptr() {return args_; };
    uint64_t &operator[](size_t index){
      if(index >= *size_ptr_){
        throw std::out_of_range("Index out of range. Requested: " +
                                std::to_string(index) + ", but max: " +
                                std::to_string(*size_ptr_));
      }

      return args_[index];
    }

    const uint64_t &operator[](size_t index) const {
      if (index >= *size_ptr_) {
        throw std::out_of_range("Index out of range. Requested: " +
                                std::to_string(index) + ", but max: " +
                                std::to_string(*size_ptr_));
      }
      return args_[index];
    }

    size_t size() { return *size_ptr_; };

  private:
    size_t *size_ptr_;
    uint64_t *args_;
};

/**
 * @class StringArg
 * @brief Stores the string argument of a cuda command.
 * 
 * ! noted there are at most 1 string arg in the current CUDA driver api
 */
class StringArg : public SharedMemoryManager{
  public:
    StringArg(int id) : SharedMemoryManager(id, sizeof(char) * 100),
                        size_ptr_(static_cast<int*>(shm_addrs_)),
                        arg(reinterpret_cast<char*>(size_ptr_ + 1)){};
    ~StringArg(){};

    const char* get() { return arg; };
    void set(const char* content) {
      *size_ptr_ = std::strlen(content) + 1;
      strncpy(arg, content, *size_ptr_);
    }

  private:
    int* size_ptr_;
    char* arg;
};

/**
 * @class VirtualGPU
 * @brief Stored the data to be sent to the physical GPU on the host.
 */
class VirtualGPU : public SharedMemoryManager{
  public:
    VirtualGPU(size_t memory_size) : SharedMemoryManager(VGPU_PORT, memory_size){};
    ~VirtualGPU(){};

    void *get() {return shm_addrs_; };
    void to_device(const void* data_ptr, size_t byte_size) { memcpy(shm_addrs_, data_ptr, byte_size); };
    void from_device(void* data_ptr, size_t byte_size) { memcpy(data_ptr, shm_addrs_, byte_size); };
};

class ResponseBuffer : private SharedMemoryManager{
  private:
    struct Response {
      uint64_t scalar;
      CUresult curesult;
    };
  public:
    ResponseBuffer() : SharedMemoryManager(RESPONSE_PORT, sizeof(Response)),
                       response_(static_cast<Response*>(shm_addrs_)){};
    ~ResponseBuffer(){};

    void set_scalar(uint64_t scalar){ response_->scalar = scalar; };
    uint64_t get_scalar(){ return response_->scalar; };
    void set_curesult(CUresult curesult){ response_->curesult = curesult; };
    CUresult get_curesult(){ return response_->curesult; };

  private:
    Response *response_;
};

class CommandBuffer : private StringArg{
  public:
    CommandBuffer() : StringArg(COMMAND_PORT){
      sem_host_trigger_ = sem_open("/command_sem_data", O_CREAT, 0644, 0);
      if (sem_host_trigger_ == SEM_FAILED) {
        perror("sem_open failed");
        // return 1; TODO error denoting
      }
      sem_guest_trigger_ = sem_open("/command_sem_finish", O_CREAT, 0644, 0);
      if (sem_guest_trigger_ == SEM_FAILED) {
        perror("sem_open failed");
        // return 1; TODO error denoting
      }
    };
    ~CommandBuffer(){ release(); };

    /* used by the client to push commands to the buffer with synchronization. */
    void push(const char* command_name){
      set(command_name);
      sem_post(sem_host_trigger_);
      sem_wait(sem_guest_trigger_);
    }

    /* return the CUDA command triggered from the client with synchronization. */
    std::string pull() {
      sem_wait(sem_host_trigger_);
      std::string result = get();
      return result;
    }

    void set_scalar_result(uint64_t scalar) { response_.set_scalar(scalar); };
    uint64_t get_scalar_result() { return response_.get_scalar(); };
    void set_curesult(CUresult curesult) { response_.set_curesult(curesult); };
    CUresult get_curesult() { return response_.get_curesult(); };

    void impl_finish() { sem_post(sem_guest_trigger_); };

    void release(){
      sem_close(sem_host_trigger_);
      sem_close(sem_guest_trigger_);
      sem_unlink("/command_sem_data");
      sem_unlink("/command_sem_finish");
    }
  private:
    sem_t *sem_host_trigger_;   /* for the client to notice the host that a CUDA command is ready. */
    sem_t *sem_guest_trigger_;  /* for the host to notice the client that a CUDA command has been finished. */

    ResponseBuffer response_{};

    //TODO refactor to clean code for the initialization
    void create_sem(sem_t *target, char* sem_name){
      target = sem_open(sem_name, O_CREAT, 0644, 0);
      if (target == SEM_FAILED) {
        perror("sem_open failed\n");
        // return 1; TODO error denoting
      }
    }
};
#endif