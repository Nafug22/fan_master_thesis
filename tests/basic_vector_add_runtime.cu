#include <iostream>
#include <cuda_runtime.h>

// CUDA kernel for vector addition
extern "C" __global__ void vectorAdd(const int *A, const int *B, int *C, int N) {
    int index = threadIdx.x + blockIdx.x * blockDim.x;
    if (index < N) {
        C[index] = A[index] + B[index];
    }
}

int main() {
    int N = 1024;  // Size of the vectors
    int size = N * sizeof(int);

    // Host vectors
    int *h_A = new int[N];
    int *h_B = new int[N];
    int *h_C = new int[N];

    // Initialize host vectors
    for (int i = 0; i < N; i++) {
        h_A[i] = i;
        h_B[i] = i * 2;
    }

    // Device vectors
    int *d_A, *d_B, *d_C;
    cudaMalloc((void**)&d_A, size);
    cudaMalloc((void**)&d_B, size);
    cudaMalloc((void**)&d_C, size);

    // Copy data from host to device
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    // Define block and grid sizes
    int blockSize = 256;
    int numBlocks = (N + blockSize - 1) / blockSize;

    // Launch the kernel
    vectorAdd<<<numBlocks, blockSize>>>(d_A, d_B, d_C, N);

    // Copy result back from device to host
    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

    // Verify the result
    bool success = true;
    for (int i = 0; i < N; i++) {
        if (h_C[i] != h_A[i] + h_B[i]) {
            success = false;
            std::cout << "Mismatch at index " << i << ": expected " << h_A[i] + h_B[i] << " but got " << h_C[i] << std::endl;
            break;
        }
    }

    if (success) {
        std::cout << "Vector addition successful!" << std::endl;
    }

    // Free allocated memory
    delete[] h_A;
    delete[] h_B;
    delete[] h_C;
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}
