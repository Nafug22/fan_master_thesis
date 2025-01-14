#ifndef INTERCEPTION_H
#define INTERCEPTION_H

#include "client.h"
#include "compiler.h"

#include <cuda.h>
#include <dlfcn.h>
#include <iostream>
#include <unordered_map>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" {
void *__libc_dlsym(void *map, const char *name);
void *__libc_dlopen_mode(const char *name, int mode);
}
extern "C" CUresult CUDAAPI getProcAddressBySymbol(const char *symbol, void **pfn, int driverVersion, cuuint64_t flags,
                                           CUdriverProcAddressQueryResult *symbolStatus);

typedef void *(*fnDlsym)(void *, const char *);
void* libcuda_driver_handle = dlopen("libcuda.so", RTLD_LAZY);

std::string server_address = "localhost:50051";
CUDAAPIClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

#define CUDA_CHECK(err) \
    if (err != CUDA_SUCCESS) { \
        std::cerr << "In gpu_instance, CUDA error: " << err << " at line " << __LINE__ << std::endl; \
        exit(EXIT_FAILURE); \
    }

static CUFuncProto func_proto("vector_add.ptx");
static std::unordered_map<CUfunction, std::string> hashfunc;
/**
 * @class VirtualGPU
 * @brief The shared memory region used for data transfer between the client
 *        and host.
 */
class VirtualGPU{
  public:
    /**
     * @param memory_size memory size of the virtual gpu
     */
    explicit VirtualGPU(size_t memory_size)
        : memory_size_(memory_size){
            initialize();
        }
    ~VirtualGPU(){
        munmap(vgpu_ptr_, memory_size_ * sizeof(float));
        close(shm_fd);
    }

    /**
     * @param data_ptr pointing the start of the data to be written to the vgpu.
     * @param data_size the size (in bytes) of the data to be written to the vgpu.
     */
    void to_device(void *data_ptr, size_t data_size){
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

  private:
    const char* SHARED_MEMORY_NAME = "/vgpu";
    size_t memory_size_;
    void* vgpu_ptr_ = nullptr;

    /**
     * @brief initialize the shared memory region used as the vgpu.
     */
    void initialize(){
      // open the shared memory in the given file
      int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
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
}

#endif