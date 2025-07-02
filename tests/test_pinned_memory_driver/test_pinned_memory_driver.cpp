#include <cuda.h>
#include <iostream>
#include <vector>
#include <cassert>

#define CHECK_CUDA(call)                                                       \
    do {                                                                       \
        CUresult err = call;                                                   \
        if (err != CUDA_SUCCESS) {                                             \
            const char* errStr;                                                \
            cuGetErrorString(err, &errStr);                                    \
            std::cerr << "CUDA Driver API error at " << __FILE__ << ":"        \
                      << __LINE__ << " - " << errStr << std::endl;             \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while (0)


int main() {
    const int N = 1 << 20;
    const size_t size = N * sizeof(float);

    // Initialize CUDA Driver API
    CHECK_CUDA(cuInit(0));

    CUdevice device;
    CHECK_CUDA(cuDeviceGet(&device, 0));

    CUcontext context;
    CHECK_CUDA(cuCtxCreate(&context, 0, device));

    // Allocate pinned host memory
    float *h_A, *h_B, *h_C;
    CHECK_CUDA(cuMemHostAlloc((void**)&h_A, size, CU_MEMHOSTALLOC_PORTABLE));
    CHECK_CUDA(cuMemHostAlloc((void**)&h_B, size, CU_MEMHOSTALLOC_PORTABLE));
    CHECK_CUDA(cuMemHostAlloc((void**)&h_C, size, CU_MEMHOSTALLOC_PORTABLE));

    for (int i = 0; i < N; ++i) {
        h_A[i] = static_cast<float>(i);
        h_B[i] = static_cast<float>(2 * i);
    }

    std::cout << "h_A[0] = " << h_A[0] << std::endl;
    std::cout << "h_B[0] = " << h_B[0] << std::endl;

    // Allocate device memory
    CUdeviceptr d_A, d_B, d_C;
    CHECK_CUDA(cuMemAlloc(&d_A, size));
    CHECK_CUDA(cuMemAlloc(&d_B, size));
    CHECK_CUDA(cuMemAlloc(&d_C, size));

    // Copy data to device
    CHECK_CUDA(cuMemcpyHtoD(d_A, h_A, size));
    CHECK_CUDA(cuMemcpyHtoD(d_B, h_B, size));

    // Load PTX module and get function handle
    CUmodule module;
    CHECK_CUDA(cuModuleLoad(&module, "/home/ubuntu/fan_thesis/fan_master_thesis/vector_add.ptx"));

    CUfunction kernel;
    CHECK_CUDA(cuModuleGetFunction(&kernel, module, "vectorAdd"));

    // Setup kernel parameters
    void* args[] = { &d_A, &d_B, &d_C, (void*)&N };

    // Launch kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
    CHECK_CUDA(cuLaunchKernel(kernel,
                              blocksPerGrid, 1, 1,
                              threadsPerBlock, 1, 1,
                              0, 0,
                              args, 0));

    std::cout << "kernel finished" << std::endl;
    // CHECK_CUDA(cuCtxSynchronize());

    // Copy result back to host
    CHECK_CUDA(cuMemcpyDtoH(h_C, d_C, size));

    // Verify first 10 results
    for (int i = 0; i < 10; ++i) {
        std::cout << h_A[i] << " + " << h_B[i] << " = " << h_C[i] << std::endl;
    }

    // Cleanup
    cuMemFree(d_A);
    cuMemFree(d_B);
    cuMemFree(d_C);

    // cuMemFreeHost(h_A);
    // cuMemFreeHost(h_B);
    // cuMemFreeHost(h_C);

    cuModuleUnload(module);
    cuCtxDestroy(context);

    return 0;
}
