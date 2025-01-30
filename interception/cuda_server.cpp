#include <cstdio>
#include <stdio.h>
#include <cstring>
#include <memory>
#include <csignal>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <cuda.h>

#include "shared_memory_manager.h"
#include "type_decl.h"

#define ADD_SYMBOL(symbol) {SYMBOL_TO_STR(symbol), [this]() { this->call_##symbol(); }}

/* generate the corresponding cuda function with arguments stored in shared memory. */
#define CUDA_API_IMPL(symbol) \
void call_##symbol(){ \
  std::cout << "<<<<<<<<<<implemented as " << #symbol << std::endl; \
  using param_t = FunctionTraits<decltype(symbol)>::ParameterTuple;  \
  param_t &args = *(param_t*)(cuda_args_.get_ptr());         \
  CUresult result = std::apply(symbol, args);                \
  command_buffer_.set_curesult(result);                              \
}

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
    CUDAArgs cuda_args_{};
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

          using param_t = FunctionTraits<decltype(cuMemAlloc)>::ParameterTuple;
          param_t &args = *(param_t*)(cuda_args_.get_ptr());
          device_ptrs_.push_back(std::make_unique<CUdeviceptr>());
          std::get<0>(args) = device_ptrs_.back().get();
          CUresult result = std::apply(cuMemAlloc, args);

          command_buffer_.set_scalar_result(*device_ptrs_.back());
          command_buffer_.set_curesult(result);
      }

      //HACK consider controled access pattern
      if(function_fit(function_name, "cuMemcpyHtoD") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuMemcpyHtoD)>::ParameterTuple;
          param_t &args = *(param_t*)(cuda_args_.get_ptr());
          std::get<1>(args) = vgpu.get();
          CUresult result = std::apply(cuMemcpyHtoD, args);

          command_buffer_.set_curesult(result);
      }

      //HACK consider access control in multi-client case
      if(function_fit(function_name, "cuModuleLoad") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuModuleLoad)>::ParameterTuple;
          param_t &args = *(param_t*)(cuda_args_.get_ptr());
          cumodules_.push_back(std::make_unique<CUmodule>());
          std::get<0>(args) = cumodules_.back().get();
          std::get<1>(args) = string_arg_.get();
          CUresult result = std::apply(cuModuleLoad, args);

          command_buffer_.set_scalar_result((uint64_t) *cumodules_.back());
          command_buffer_.set_curesult(result);
      }

      //HACK consider access control in multi-client case
      if(function_fit(function_name, "cuModuleGetFunction") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuModuleGetFunction)>::ParameterTuple;
          param_t &args = *(param_t*)(cuda_args_.get_ptr());
          cufuncs_.push_back(std::make_unique<CUfunction>());
          std::get<0>(args) = cufuncs_.back().get();
          std::get<2>(args) = string_arg_.get();
          CUresult result = std::apply(cuModuleGetFunction, args);

          command_buffer_.set_scalar_result((uint64_t) *cufuncs_.back());
          command_buffer_.set_curesult(result);
      }

      //TODO specific manipulation about kernel_args needed
      if(function_fit(function_name, "cuLaunchKernel") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
          void* kernel_args[scalar_args_.size()];
          for(int i = 0; i < scalar_args_.size(); i++){
            kernel_args[i] = scalar_args_.get_scalar_ptr() + i;
          }

          using param_t = FunctionTraits<decltype(cuLaunchKernel)>::ParameterTuple;
          param_t &args = *(param_t*)(cuda_args_.get_ptr());
          std::get<9>(args) = kernel_args;
          CUresult result = std::apply(cuLaunchKernel, args);

          command_buffer_.set_curesult(result);
      }

      //HACK consider controlled access pattern
      if(function_fit(function_name, "cuMemcpyDtoH") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuMemcpyDtoH)>::ParameterTuple;
          param_t &args = *(param_t*)(cuda_args_.get_ptr());
          std::get<0>(args) = vgpu.get();
          CUresult result = std::apply(cuMemcpyDtoH, args);

          command_buffer_.set_curesult(result);
      }

      if(func_map.count(function_name)) func_map[function_name]();

      command_buffer_.impl_finish();
    }

    CUDA_API_IMPL(cuMemFree)
    CUDA_API_IMPL(cuModuleUnload)

  private:
    std::unordered_map<std::string, std::function<void()>> func_map = {
      ADD_SYMBOL(cuMemFree),
      ADD_SYMBOL(cuModuleUnload)
    };
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