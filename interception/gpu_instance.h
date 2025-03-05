#ifndef GPU_INSTANCE_H
#define GPU_INSTANCE_H

#include <vector>
#include <cuda.h>
#include <iostream>
#include <unordered_map>

#include <sys/socket.h>
#include <sys/un.h>

#include "stream.h"
#include "type_decl.h"

#define ADD_SYMBOL(symbol) {SYMBOL_TO_STR(symbol), [this]() { this->symbol##_handler(); }}
/* generate the corresponding cuda function with arguments stored in shared memory. */
#define CUDA_API_IMPL(symbol) \
void symbol##_handler(){ \
    std::cout << "<<<<<<<<<<implemented as " << #symbol << std::endl; \
    using param_t = FunctionTraits<decltype(symbol)>::ParameterTuple;  \
    param_t args; deserializer_ >> args;  \
    CUresult result = std::apply(symbol, args);                \
    response_.set_curesult(result);                              \
}

/**
 * @class GPUinstance
 * @brief virtual gpu instance to be dedicated to a requesting
 *        microvm.
 * @details handles command recv and cuda respond respond. There is 
 *          also a virtual gpu used for store data from the guest os
 *          as host data to be transfered to the gpu.
 */
class GPUInstance{
  public:
    explicit GPUInstance(int device_id, int client_fd) : client_fd_(client_fd){
        cuDeviceGet(&device_, device_id);
        cuCtxCreate(&cucontext_, 0, device_);
    }
    ~GPUInstance() {
        cuCtxDestroy(cucontext);
        close(client_fd_);
    };

    void handle(){
        while(true){
            implement_cuda_function();
        }
    }

  /******************************************************
   *        host CUDA allocated pointers storage        *
   ******************************************************/
  private:
    std::vector<std::unique_ptr<CUdeviceptr>> device_ptrs_;
    std::vector<std::unique_ptr<CUmodule>> cumodules_;
    std::vector<std::unique_ptr<CUfunction>> cufuncs_;

    CUdevice device_;
    CUcontext cucontext_;

  private:
    int client_fd_;
    Response response_;
    void return_result(){
        send(client_fd_, response_.data(), response_.size(), 0);
    }
    vsock::VirtualGPU vgpu{};

  private:
    Deserializer deserializer_;
    char* pull_command(){
        deserializer_.clean();
        int bytes_read = read(client_fd_, deserializer_.data(), deserializer_.size());
        char* func_name;
        deserializer_ >> func_name;
        return func_name;
    }

    void implement_cuda_function();

    CUDA_API_IMPL(cuMemFree)
    CUDA_API_IMPL(cuModuleUnload)

  private:
    std::unordered_map<std::string, std::function<void()>> func_map = {
      ADD_SYMBOL(cuMemFree),
      ADD_SYMBOL(cuModuleUnload)
    };
};