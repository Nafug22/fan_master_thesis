#ifndef VGPU_H
#define VGPU_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

#include <iostream>
#include <unistd.h>

/*****************************************************************************
 * @class VirtualGPU                                                         *
 * @brief The shared memory region used for data transfer between the client *
 *        and host.                                                          *
 *****************************************************************************/
class VirtualGPU{
  public:
    /**
     * @param memory_size memory size of the virtual gpu
     */
    explicit VirtualGPU(size_t memory_size) : memory_size_(memory_size){ initialize(); };
    ~VirtualGPU(){
        close(shm_fd);
        munmap(vgpu_ptr_, memory_size_ * sizeof(float));
        shm_unlink(SHARED_MEMORY_NAME);
    }

    /**
     * @param data_ptr pointing the start of the data to be written to the vgpu.
     * @param data_size the size (in bytes) of the data to be written to the vgpu.
     */
    void to_device(const void *data_ptr, size_t data_size){
        std::memcpy(vgpu_ptr_, data_ptr, data_size);
        std::cout << "Data pushed to vgpu.\n";
    }

    /**
     * @param data_ptr pointing the starting address that the data to be stored.
     * @param data_size the size (in bytes) of the data to be accessed in the vgpu.
     */
    void from_device(void *data_ptr, size_t data_size){
        std::memcpy(data_ptr, vgpu_ptr_, data_size);
        std::cout << "Data accessed from vgpu.\n";
    }

    void* vgpu_ptr_ = nullptr;
  private:
    const char* SHARED_MEMORY_NAME = "/vgpu";
    size_t memory_size_;
    int shm_fd;

    /**
     * @brief initialize the shared memory region used as the vgpu.
     */
    void initialize(){
      // open the shared memory in the given file
      //HACK consider to have finer flag control
      shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
      if (shm_fd == -1) {
          perror("shm_open for the vgpu memory failed");
          return;
      }

      // set the size of the shared memory
      if (ftruncate(shm_fd, memory_size_ * sizeof(float)) == -1) {
          perror("ftruncate for the vgpu memory failed");
          return;
      }

      // map shared memory to the process address space
      vgpu_ptr_ = mmap(0, memory_size_ * sizeof(float), PROT_WRITE, MAP_SHARED, shm_fd, 0);
      if(vgpu_ptr_ == MAP_FAILED){
          perror("mmap failed");
          return;
      }
    }
};
#endif