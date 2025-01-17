#include <cstdio>
#include <stdio.h>
#include <semaphore.h>
#include <cstring>
#include <memory>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <csignal>
#include <cuda.h>

#include "shared_memory_manager.h"

#define SEM_NAME "/command_sem"
#define SHM_NAME "command.txt"

class CUDAServer{
  public:
    CUDAServer(){ initialize(); };
    ~CUDAServer(){ release(); };

    void run(){
      while(true){
        implement_cuda_function();
      }
    }

  private:
    CommandBuffer command_buffer_{};
    ScalarArgs scalar_args_{};
    StringArg string_arg_{STRING_ARG_PORT};
    //TODO consider to refactor into the [] overload version
    std::string string_content_;
    std::vector<std::unique_ptr<CUdeviceptr>> device_ptrs_;
    std::vector<std::unique_ptr<CUmodule>> cumodules_;
    std::vector<std::unique_ptr<CUfunction>> cufuncs_;

    bool function_fit(std::string str1, std::string str2){
      return strncmp(str1.c_str(), str2.c_str(), str2.size());
    }
  private:
    /**
     * TODO: - refactor into GPU instances
     */
    CUdevice device;
    CUcontext cucontext;
    void initialize(){
        cuDeviceGet(&device, 0);
        cuCtxCreate(&cucontext, 0, device);
    }

    void release(){
      cuCtxDestroy(cucontext);
    }

    //HACK consider adding constraints that are consistant with the gpu partition
    //TODO vgpu initialization with configured size
    VirtualGPU vgpu{(1 << 20) * sizeof(float)};


    void implement_cuda_function(){
      cuCtxSetCurrent(cucontext);
      std::string function_name = command_buffer_.pull();
      string_content_ = string_arg_.get();
      std::cout << "Received GPU commands: " << function_name << std::endl;

      //TODO client name should be used in the future
      //HACK use FUNC_MAP
      // // if(function_name == "cuDeviceGet"){
      // //     cuDeviceGet(&device, 0);
      // // }

      // // if(function_name == "cuCtxCreate"){
      // //     cuCtxCreate(&cucontext, 0, device);
      // // }

      if(function_fit(function_name, "cuMemAlloc") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          size_t size = scalar_args_[1];
          device_ptrs_.push_back(std::make_unique<CUdeviceptr>());

          CUresult result = cuMemAlloc(device_ptrs_.back().get(), size);
          std::cout << "Allocating device memory " << *device_ptrs_.back() << std::endl;
          command_buffer_.set_scalar_result(*device_ptrs_.back());
          command_buffer_.set_curesult(result);
      }

      //HACK consider controled access pattern
      if(function_fit(function_name, "cuMemcpyHtoD") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          size_t size = scalar_args_[2];

          CUresult result = cuMemcpyHtoD(scalar_args_[0], vgpu.get(), size);

          command_buffer_.set_curesult(result);
      }

      //HACK consider access control in multi-client case
      if(function_fit(function_name, "cuModuleLoad") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          cumodules_.push_back(std::make_unique<CUmodule>());
          CUresult result = cuModuleLoad(cumodules_.back().get(), string_content_.c_str());

          command_buffer_.set_scalar_result((uint64_t) *cumodules_.back());
          command_buffer_.set_curesult(result);
      }

      //HACK consider access control in multi-client case
      if(function_fit(function_name, "cuModuleGetFunction") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          cufuncs_.push_back(std::make_unique<CUfunction>());
          CUresult result = cuModuleGetFunction(cufuncs_.back().get(), (CUmodule) scalar_args_[1], string_content_.c_str());
          command_buffer_.set_scalar_result((uint64_t) *cufuncs_.back());
          std::cout << "<<<<<<<<<<accessing function " << *cufuncs_.back() << std::endl;
          command_buffer_.set_curesult(result);
      }

      if(function_fit(function_name, "cuLaunchKernel") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          void* kernel_args[scalar_args_.size()];
          for(int i = 9; i < scalar_args_.size(); i++){
            kernel_args[i - 9] = scalar_args_.get_scalar_ptr() + i;
          }

          CUresult result = cuLaunchKernel((CUfunction)scalar_args_[0],
                                            scalar_args_[1], scalar_args_[2], scalar_args_[3],
                                            scalar_args_[4], scalar_args_[5], scalar_args_[6],
                                            scalar_args_[7], (CUstream) scalar_args_[8],
                                            kernel_args, nullptr);

          // CUresult result = CUDA_SUCCESS;
          command_buffer_.set_curesult(result);
      }

      //HACK consider controlled access pattern
      if(function_fit(function_name, "cuMemcpyDtoH") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          CUresult result = cuMemcpyDtoH(vgpu.get(), scalar_args_[1], scalar_args_[2]);
          command_buffer_.set_curesult(result);
      }

      if(function_fit(function_name, "cuMemFree") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          CUresult result = cuMemFree(scalar_args_[0]);
          command_buffer_.set_curesult(result);
      }

      if(function_fit(function_name, "cuModuleUnload") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          CUresult result = cuModuleUnload((CUmodule) scalar_args_[0]);
          command_buffer_.set_curesult(result);
      }

      // // if(function_name == "cuCtxDestroy"){
      // //     cuCtxDestroy(cucontext);
      // // }

      command_buffer_.impl_finish();
    }
};

CUDAServer *server;

void handle_signal(int signal){
  std::cout << "\nexiting cuda_server with signal " << signal << "..." << std::endl;
  delete server;

  std::exit(0);
}

int main(){
  std::cout << "cuda_server running ..." << std::endl;
  cuInit(0);
  server = new CUDAServer{};
  signal(SIGINT, handle_signal);
  signal(SIGABRT, handle_signal);
  signal(SIGSEGV, handle_signal);
  server->run();

  return 0;
}