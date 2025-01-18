#include <cuda.h>
#include <iostream>
#include <vector>

// Error checking macro
#define CUDA_CHECK(err) \
    if (err != CUDA_SUCCESS) { \
        std::cerr << "CUDA error: " << err << " at line " << __LINE__ << std::endl; \
        exit(EXIT_FAILURE); \
    }

int main() {
    CUDA_CHECK(cuInit(0));
    int N = 1 << 20;
    size_t size = N * sizeof(float);

    CUdevice device;
    CUDA_CHECK(cuDeviceGet(&device, 0));

    CUcontext context;
    CUDA_CHECK(cuCtxCreate(&context, 0, device));

    // Host memory allocation
    std::vector<float> h_A(N, 1.0f), h_B(N, 2.0f), h_C(N);

    // Device pointers
    CUdeviceptr d_A, d_B, d_C;
    cuMemAlloc(&d_A, size);
    cuMemAlloc(&d_B, size);
    cuMemAlloc(&d_C, size);

    // Copy data to device
    CUDA_CHECK(cuMemcpyHtoD(d_A, h_A.data(), size));
    CUDA_CHECK(cuMemcpyHtoD(d_B, h_B.data(), size));

    // Load module and kernel
    CUmodule module;
    CUDA_CHECK(cuModuleLoad(&module, "/home/ubuntu/fan_thesis/fan_master_thesis/vector_add.ptx"));
    CUfunction vectorAdd;
    CUDA_CHECK(cuModuleGetFunction(&vectorAdd, module, "vectorAdd"));

    // Launch kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
    void *args[] = { &d_A, &d_B, &d_C, &N };
    cuLaunchKernel(vectorAdd,
                  blocksPerGrid, 1, 1,      // Grid dimensions
                  threadsPerBlock, 1, 1,    // Block dimensions
                  0, nullptr,              // Shared memory and stream
                  args, nullptr);         // Kernel arguments


    // Copy result back to host
    CUDA_CHECK(cuMemcpyDtoH(h_C.data(), d_C, size));

    // Verify results
    bool success = true;
    for (int i = 0; i < N; i++) {
        if (std::abs(h_C[i] - (h_A[i] + h_B[i])) > 1e-5) {
            success = false;
            break;
        }
    }

    std::cout << (success ? "Test PASSED!" : "Test FAILED!") << std::endl;

    // Cleanup
    CUDA_CHECK(cuMemFree(d_A));
    CUDA_CHECK(cuMemFree(d_B));
    CUDA_CHECK(cuMemFree(d_C));
    CUDA_CHECK(cuModuleUnload(module));
    CUDA_CHECK(cuCtxDestroy(context));

    return 0;
}
