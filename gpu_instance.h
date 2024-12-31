#ifndef GPU_INSTANCE_H
#define GPU_INSTANCE_H

#include <cuda.h>
#include <iostream>

#define CUDA_CHECK(err) \
    if (err != CUDA_SUCCESS) { \
        std::cerr << "In gpu_instance, CUDA error: " << err << " at line " << __LINE__ << std::endl; \
        exit(EXIT_FAILURE); \
    }

class GPUInstance
{
  private:
    CUdevice device_;
    CUcontext context_;
    int blocksPerGrid;
    int threadsPerBlock;

    /* Initialize the device and context on the GPU instance. */
    void prepareGPU(){
        CUDA_CHECK(cuDeviceGet(&device_, 0));
        CUDA_CHECK(cuCtxCreate(&context_, 0, device_));
    }

  public:
    GPUInstance(){ prepareGPU(); };
    ~GPUInstance(){ CUDA_CHECK(cuCtxDestroy(context_)); };

    /**
     * @param device_ptr the `CUdeviceptr` object to be assigned (passed by reference)
     * @param data_size  the size on GPU to be allocated to this pointer
     */
    CUresult MemAlloc(CUdeviceptr *device_ptr, size_t data_size){
        CUresult result = cuCtxSetCurrent(context_);
        result = cuMemAlloc(device_ptr, data_size);

        return result;
    }

    /**
     * @param kernel_function the `CUfunction` object to be executed on the GPU instance
     * @param args related parameters of the kernel function
     */
    void offloadFunction(CUfunction &kernel_function, void **args){
        cuCtxSetCurrent(context_);
        CUDA_CHECK(cuLaunchKernel(kernel_function,
                                  blocksPerGrid, 1, 1,      // Grid dimensions
                                  threadsPerBlock, 1, 1,    // Block dimensions
                                  0, nullptr,               // Shared memory and stream
                                  args, nullptr));          // Kernel arguments
    }

    void setThreadsPerBlock(int thread_size){ threadsPerBlock = thread_size; };
    void setBlocksPerGrid(int block_size){ blocksPerGrid = block_size; };
};

#endif