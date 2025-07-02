#include <cuda.h>
#include <iostream>
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define CHECK_CUDA(call) do { \
    CUresult err = (call); \
    if (err != CUDA_SUCCESS) { \
        const char* err_str = nullptr; \
        cuGetErrorString(err, &err_str); \
        std::cerr << "CUDA error: " << err_str << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        exit(1); \
    } \
} while (0)

int main() {
    CHECK_CUDA(cuInit(0));

    CUdevice device;
    CHECK_CUDA(cuDeviceGet(&device, 0));

    CUcontext context;
    CHECK_CUDA(cuCtxCreate(&context, 0, device));

    const size_t N = 10;
    const size_t bytes = N * sizeof(int);

    // Step 1: Allocate regular host memory
    // int* host_mem = (int*)malloc(bytes);
    int fd = open("/dev/shm/cuda_pin", O_RDWR | O_SYNC | O_DIRECT);
    void* ptr = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_SYNC, fd, 0);
    int* host_mem = static_cast<int*>(ptr);
    for (int i = 0; i < N; ++i) host_mem[i] = i * 2;

    // Step 2: Register the memory
    CHECK_CUDA(cuMemHostRegister(host_mem, bytes, 0));

    // Step 3: Allocate device memory
    CUdeviceptr device_mem;
    CHECK_CUDA(cuMemAlloc(&device_mem, bytes));

    // Step 4: Copy host to device
    CHECK_CUDA(cuMemcpyHtoD(device_mem, host_mem, bytes));

    // Step 5: Zero host memory to verify copy-back later
    memset(host_mem, 0, bytes);

    // Step 6: Copy device to host
    CHECK_CUDA(cuMemcpyDtoH(host_mem, device_mem, bytes));

    // Step 7: Verify correctness
    for (int i = 0; i < N; ++i) {
        assert(host_mem[i] == i * 2);
    }

    std::cout << "Test passed: host memory successfully registered and used." << std::endl;

    // Step 8: Cleanup
    CHECK_CUDA(cuMemHostUnregister(host_mem));
    CHECK_CUDA(cuMemFree(device_mem));
    // free(host_mem);
    CHECK_CUDA(cuCtxDestroy(context));
    munmap(ptr, bytes);
    close(fd);
    return 0;
}
