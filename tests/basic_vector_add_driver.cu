#include <cuda.h>
#include <iostream>
#include <vector>

// CUDA kernel source code as a string
const char* vectorAddKernel = R"(
extern "C" __global__ void vectorAdd(const float* A, const float* B, float* C, int N) {
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < N) {
        C[idx] = A[idx] + B[idx];
    }
}
)";

void checkCudaError(CUresult err, const char* msg) {
    if (err != CUDA_SUCCESS) {
        const char* errorStr;
        cuGetErrorString(err, &errorStr);
        std::cerr << msg << " failed with error: " << errorStr << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    int N = 1024; // Vector size
    const size_t size = N * sizeof(float);

    // Host vectors
    std::vector<float> h_A(N, 1.0f); // Initialize with 1.0
    std::vector<float> h_B(N, 2.0f); // Initialize with 2.0
    std::vector<float> h_C(N);       // Result vector

    // Initialize CUDA Driver API
    checkCudaError(cuInit(0), "cuInit");

    // Get the first CUDA device
    CUdevice device;
    checkCudaError(cuDeviceGet(&device, 0), "cuDeviceGet");

    // Create a CUDA context
    CUcontext context;
    checkCudaError(cuCtxCreate(&context, 0, device), "cuCtxCreate");

    // Compile the kernel
    CUmodule module;
    CUfunction kernel;
    checkCudaError(cuModuleLoadDataEx(&module, vectorAddKernel, 0, nullptr, nullptr), "cuModuleLoadDataEx");
    checkCudaError(cuModuleGetFunction(&kernel, module, "vectorAdd"), "cuModuleGetFunction");

    // Allocate device memory
    CUdeviceptr d_A, d_B, d_C;
    checkCudaError(cuMemAlloc(&d_A, size), "cuMemAlloc for d_A");
    checkCudaError(cuMemAlloc(&d_B, size), "cuMemAlloc for d_B");
    checkCudaError(cuMemAlloc(&d_C, size), "cuMemAlloc for d_C");

    // Copy host data to device
    checkCudaError(cuMemcpyHtoD(d_A, h_A.data(), size), "cuMemcpyHtoD for d_A");
    checkCudaError(cuMemcpyHtoD(d_B, h_B.data(), size), "cuMemcpyHtoD for d_B");

    // Set up execution configuration
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

    // Launch the kernel
    void* args[] = { &d_A, &d_B, &d_C, &N };
    checkCudaError(cuLaunchKernel(kernel,
                                  blocksPerGrid, 1, 1,       // Grid dimensions
                                  threadsPerBlock, 1, 1,     // Block dimensions
                                  0, nullptr, args, nullptr), "cuLaunchKernel");

    // Copy result back to host
    checkCudaError(cuMemcpyDtoH(h_C.data(), d_C, size), "cuMemcpyDtoH for d_C");

    // Verify results
    bool success = true;
    for (int i = 0; i < N; ++i) {
        if (h_C[i] != h_A[i] + h_B[i]) {
            success = false;
            std::cerr << "Mismatch at index " << i << ": " << h_C[i] << " != " << h_A[i] + h_B[i] << std::endl;
            break;
        }
    }

    if (success) {
        std::cout << "Vector addition successful (driver version)!" << std::endl;
    }

    // Clean up
    checkCudaError(cuMemFree(d_A), "cuMemFree for d_A");
    checkCudaError(cuMemFree(d_B), "cuMemFree for d_B");
    checkCudaError(cuMemFree(d_C), "cuMemFree for d_C");
    checkCudaError(cuModuleUnload(module), "cuModuleUnload");
    checkCudaError(cuCtxDestroy(context), "cuCtxDestroy");

    return 0;
}