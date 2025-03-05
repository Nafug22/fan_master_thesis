#ifndef STREAM_H
#define STREAM_H

#include <cstring>
#include <cuda.h>
#include <utility>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <unistd.h>

#include <sys/socket.h>
#include <linux/vm_sockets.h>

#include <stdexcept>
#define BUFFER_SIZE 10000
#define GPU_SIZE 10000

/**
 * @class VsockHandle
 * @brief vsock socket management for a client with corresponding cid and port.
 * @details responsible for the construction and destruction of the vsock, and handles
 *          all the details for correct send and receive data.
 */
class VsockHandle{
  public:
    VsockHandle(unsigned int cid, unsigned int port) : sock_(initialize_sock(cid, port)){};
    ~VsockHandle(){ if(sock_ != -1) close(sock_); };

    void transmit(const void* data_ptr, size_t data_size){
        char* remain_data = (char*)data_ptr;
        while(data_size > 0){
            ssize_t bytes_send = send(sock_, remain_data, data_size, 0);
            if(bytes_send <= 0)
                throw std::runtime_error("Socket closed or error occurred during recv()");
            data_size -= bytes_send;
            remain_data += bytes_send;
        }
    }

    void receive(void* data_ptr, size_t data_size){
        ssize_t total_size = 0;
        char* data = (char*) data_ptr;
        while(total_size < data_size){
          ssize_t local_size = recv(sock_, data + total_size, data_size - total_size, 0);
          total_size += local_size;
        }
    }

    void close_socket() {
        close(sock_);
        sock_ = -1;
    }

  private:
    int sock_;
    int initialize_sock(unsigned int cid, unsigned int port){
        int sock = socket(AF_VSOCK, SOCK_STREAM, 0);
        if(sock < 0) perror("socket");

        sockaddr_vm sa = {};
        sa.svm_family = AF_VSOCK;
        sa.svm_cid = cid;
        sa.svm_port = port;

        std::cout << "Connecting to host ..." << std::endl;
        if(connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0) perror("connect");

        return sock;
    }
};

/**
 * @class Response
 * @brief Used by the server to write and send response.
 *        Used by the client to receive and read response.
 */
class Response {
  public:
    Response() : mdata_(std::malloc(msize_)),
                 curesult_buffer_(reinterpret_cast<CUresult*>(mdata_)),
                 scalar_result_buffer_(reinterpret_cast<uint64_t*>(curesult_buffer_ + 1)){};
    ~Response(){ free(mdata_); };

    CUresult curesult() { return *curesult_buffer_; };
    uint64_t cuscalar() { return *scalar_result_buffer_; };
    void set_curesult(CUresult curesult) { *curesult_buffer_ = curesult; };
    void set_cuscalar(uint64_t cuscalar) { *scalar_result_buffer_ = cuscalar; };

    void* data() { return mdata_; };
    static size_t size() { return msize_; };

  private:
    static const size_t msize_ = sizeof(CUresult) + sizeof(uint64_t);
    void* mdata_;
    CUresult *curesult_buffer_;
    uint64_t *scalar_result_buffer_;
};

/**
 * @class Serializer
 * @brief Prepares original cuda function information as a flattened message,
 *        which will then be transferred via vsock to the host.
 */
class Serializer {
  public:
    explicit Serializer() : mdata_((char*)std::malloc(BUFFER_SIZE)), mcount_(0){};
    ~Serializer(){ free(mdata_); };

    /** the generic template for normal types */
    template <typename T>
    Serializer& operator<<(T &content){
        std::memcpy(mdata_ + mcount_, &content, sizeof(T));
        mcount_ += sizeof(T);

        return *this;
    }

    /** for `char*` particularly to get it correct. */
    Serializer& operator<<(const char* str_content){
        size_t size = std::strlen(str_content);
        operator<<(size);

        std::memcpy(mdata_ + mcount_, str_content, size * sizeof(char));
        mcount_ += size * sizeof(char);

        static const char term = '\0';
        std::memcpy(mdata_ + mcount_, &term, sizeof(char));
        mcount_ += sizeof(char);

        return *this;
    }

    /*********************************************
     *            For composite types            *
     *********************************************/
    template <typename... Args>
    Serializer& operator<<(const std::tuple<Args...> &args){
        serialize_tuple(args, std::index_sequence_for<Args...>{});
        return *this;
    }

    template <typename... Args>
    Serializer& operator<<(Args... args){
        operator<<(args...);
        return *this;
    }

    Serializer& operator<<(std::vector<uint64_t> &kernel_args){
        size_t args_count = kernel_args.size();
        operator<<(args_count);
        std::memcpy(mdata_ + mcount_, kernel_args.data(), args_count * sizeof(uint64_t));
        mcount_ += args_count * sizeof(uint64_t);

        return *this;
    }

    /** returns the size of the buffer that stores the flattened message. */
    size_t size() { return mcount_; };

    /** returns the starting of the buffer that stores the flattened message. */
    void* data() {return mdata_; };

    /** cleans the read count with no deletion. */
    void clean() { mcount_ = 0; };

  private:
    char* mdata_;
    size_t mcount_;

    template <typename Tuple, std::size_t... I>
    void serialize_tuple(Tuple &tuple, std::index_sequence<I...>){
        (operator<<(std::get<I>(tuple)), ...);
    }
};

/**
 * @class Deserializer
 * @brief Deserialize the flattened data in the receive buffer.
 */
class Deserializer {
  public:
    explicit Deserializer() : mdata_((char*)std::malloc(BUFFER_SIZE)), mcount_(0){};
    ~Deserializer(){ free(mdata_); };

    template <typename T>
    Deserializer& operator>>(T &content){
        std::memcpy(&content, mdata_ + mcount_, sizeof(T));
        mcount_ += sizeof(T);

        return *this;
    }

    Deserializer& operator>>(const char* &result){
        size_t size; operator>>(size);

        result = (const char*)(mdata_ + mcount_);
        mcount_ += (size + 1) * sizeof(char); //plus the terminator

        return *this;
    }

    Deserializer& operator>>(char* &result){
        size_t size; operator>>(size);

        result = (char*)(mdata_ + mcount_);
        mcount_ += (size + 1) * sizeof(char); //plus the terminator

        return *this;
    }

    /*********************************************
     *            For composite types            *
     *********************************************/
    template <typename... Args>
    Deserializer& operator>>(std::tuple<Args...> &args){
        deserialize_tuple(args, std::index_sequence_for<Args...>{});
        return *this;
    }

    //! read through the doc to check if any other cuda func use this type of data.
    //! if so, have another func for getting the kernel!
    Deserializer& operator>>(std::vector<uint64_t*> &kernel_args){
        size_t size; operator>>(size);

        kernel_args.resize(size);
        for(int i = 0; i < size; i++){
            kernel_args[i] = reinterpret_cast<uint64_t*>(mdata_ + mcount_);
            mcount_ += sizeof(uint64_t);
        }

        return *this;
    }

    void clean() { mcount_ = 0; };
    void* data() {return mdata_; };
    size_t size() { return BUFFER_SIZE; };
  private:
    char* mdata_;
    size_t mcount_;

    template <typename Tuple, size_t... I>
    void deserialize_tuple(Tuple &tuple, std::index_sequence<I...>){
        (operator>>(std::get<I>(tuple)), ...);
    }
};

// deprecated due to unknown cache coherency issues
class VirtualGPU {
  public:
    VirtualGPU(const char* shm_path, size_t mem_size)
        : vgpu_size_(mem_size),
          shm_path_(shm_path){ initialize_vgpu(shm_path); };
    ~VirtualGPU(){ close_vgpu(); };

    void *get() { return vgpu_ptr_; };
    void to_device(const void* data_ptr, size_t byte_size) {
      memcpy(vgpu_ptr_, data_ptr, byte_size);
      msync(vgpu_ptr_, byte_size, MS_SYNC | MS_INVALIDATE);
      std::cout << "to_device: the first element is " << *(int*)(vgpu_ptr_) << std::endl;
    }
    void from_device(void* data_ptr, size_t byte_size) {
      memcpy(data_ptr, vgpu_ptr_, byte_size);
    }

    void sync(){ if(fsync(fd_) == -1) printf("error on fsync\n"); };
    void remap() {
        close_vgpu();
        initialize_vgpu(shm_path_);
    }
  private:
    void initialize_vgpu(const char* shm_path){
        fd_ = open(shm_path, O_RDWR | O_SYNC | O_DIRECT);
        if (fd_ < 0) std::cerr << "Failed to open shared memory file\n";

        vgpu_ptr_ = mmap(nullptr, vgpu_size_, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_SYNC, fd_, 0);
        if (vgpu_ptr_ == MAP_FAILED) std::cerr << "mmap failed\n";
    }

    void close_vgpu(){
        munmap(vgpu_ptr_, vgpu_size_);
        close(fd_);
    }

    int fd_;
    void* vgpu_ptr_;
    size_t vgpu_size_;
    const char* shm_path_;
};

namespace vsock
{
/**
 * @class VirtualGPU
 * @brief use `vsock` as the underlying implementation.
 * @details the commands and data transfer share the same socket, but
 *          different buffers. May be no buffer at all. Because the array
 *          of data can be a buffer itself.
 */
class VirtualGPU {
  public:
    VirtualGPU() : vgpu_(nullptr), vgpu_size_(0){};
    ~VirtualGPU(){ if(!vgpu_) free(vgpu_); };

    void to_device(void *data_ptr, size_t data_size){
        if(data_size > vgpu_size_) allocate_vgpu(data_size);
        memcpy(vgpu_, data_ptr, data_size);
    }
    void from_device(size_t data_size){
        if(data_size > vgpu_size_) allocate_vgpu(data_size);
    }

    void* prepare(size_t data_size){
        if(data_size > vgpu_size_) allocate_vgpu(data_size);
        return vgpu_;
    }

    void* get() { return vgpu_; };

  private:
    void* vgpu_;
    size_t vgpu_size_;

    void allocate_vgpu(size_t size){
        if(vgpu_) free(vgpu_);
        vgpu_ = malloc(size);
        vgpu_size_ = size;
    }
};
}
#endif