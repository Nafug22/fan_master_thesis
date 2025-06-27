#include <iostream>
#include <cuda_runtime.h>

int main() {
    const size_t size = 1024 * sizeof(float);  // Allocate space for 1024 floats
    float* d_ptr = nullptr;
    float* a_ptr = nullptr;
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;

    // Allocate device memory
    cudaError_t err = cudaMalloc((void**)&d_ptr, size);
    // if (err != cudaSuccess) {
    //     std::cerr << "cudaMalloc failed: " << cudaGetErrorString(err) << std::endl;
    //     return 1;
    // }

    std::cout << "cudaMalloc 1 succeeded. Device pointer: " << d_ptr << std::endl;
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;

    cudaError_t err1 = cudaMalloc((void**)&a_ptr, size);
    std::cout << "cudaMalloc 2 succeeded. Device pointer: " << a_ptr << std::endl;
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;

    // Free device memory
    err = cudaFree(d_ptr);
    err = cudaFree(a_ptr);
    if (err != cudaSuccess) {
        std::cerr << "cudaFree failed: " << cudaGetErrorString(err) << std::endl;
        return 1;
    }

    std::cout << "cudaFree succeeded." << std::endl;
    return 0;
}
