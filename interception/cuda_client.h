#ifndef CUDA_CLIENT_H
#define CUDA_CLIENT_H
#include <stdio.h>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include "stream.h"
#include "type_decl.h"
#include <cuda.h>

#define VSOCK_HOST_CID 2
#define VSOCK_PORT 1234
#define BUFFER_RECV 200

//TODO add size exception control (when the BUFFER_SIZE is too small)
class CUDAClient{
  public:
    CUDAClient() : vsock_handle_(VsockHandle(VSOCK_HOST_CID, VSOCK_PORT)){};
    ~CUDAClient(){};

    /**
     * @brief redirect the intercepted cuda function to the host.
     * @param func_name the function name of the cuda function.
     * @param args arguments to be passed from various intercepted functions.
     */
    template <typename... Args>
    void CallCudaFunction(const char* func_name, Args... args){
        serializer_ << func_name;
        (serializer_ << ... << args);

        vsock_handle_.transmit(serializer_.data(), serializer_.size());
        serializer_.clean();
    }

    /**
     * @brief synchronize the operation with the host when finished.
     * @return the exact `CUresult` from the host.
     */
    CUresult wait_recv(){
        vsock_handle_.receive(response_.data(), response_.size());
        return response_.curesult();
    }

    /**
     * @brief send guest data to the virtual gpu for later offloading on the host.
     */
    void to_device(const void* data_ptr, size_t data_size){
        vsock_handle_.transmit(data_ptr, data_size);
    }

    /**
     * @brief read data from vsock to get data from the virtual gpu.
     */
    void from_device(void* data_ptr, size_t data_size){
        vsock_handle_.receive(data_ptr, data_size);
    }

    uint64_t get_scalar_result() { return response_.cuscalar(); };
    CUresult get_curesult() { return response_.curesult(); };
    void close() { vsock_handle_.close_socket(); };
  private:
    VsockHandle vsock_handle_;
    Response response_;
    Serializer serializer_;
};

#endif