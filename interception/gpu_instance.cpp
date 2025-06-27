#include "gpu_instance.h"

#define FUNC_COMP(func_name, symbol) strcmp(func_name, SYMBOL_TO_STR(symbol)) == 0

//======================================================================================//
void GPUInstance::implement_cuda_function(){
  cuCtxSetCurrent(cucontext_);
  char* function_name = pull_command();
  std::cout << "Received GPU commands: " << function_name << std::endl;

  //! change the buffer size here (how about transfer the size in the beginning?)
  //TODO client name should be used in the future
  //HACK use FUNC_MAP
  // // if(function_name == "cuDeviceGet"){
  // //     cuDeviceGet(&device, 0);
  // // }

  // // if(function_name == "cuCtxCreate"){
  // //     cuCtxCreate(&cucontext, 0, device);
  // // }

  if(FUNC_COMP(function_name, cuMemAlloc)){
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
      using param_t = FunctionTraits<decltype(cuMemAlloc)>::ParameterTuple;
      param_t args; deserializer_ >> args;

      device_ptrs_.push_back(std::make_unique<CUdeviceptr>());
      std::get<0>(args) = device_ptrs_.back().get();
      CUresult result = std::apply(cuMemAlloc, args);
      response_.set_curesult(result);
      response_.set_cuscalar(*device_ptrs_.back());
  } else if(FUNC_COMP(function_name, cuMemcpyHtoD)){
      //HACK consider controled access pattern
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      using param_t = FunctionTraits<decltype(cuMemcpyHtoD)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      long long bytesize = std::get<2>(args);
      std::get<1>(args) = vgpu_ptr_;
      CUresult result = std::apply(cuMemcpyHtoD, args);
      float temp = ((float*)vgpu_ptr_)[0];
      std::cout << "cuMemcpyHtoD gives " << temp << std::endl;
      response_.set_curesult(result);
  } else if(FUNC_COMP(function_name, cuModuleLoad)){
      //HACK consider access control in multi-client case
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      using param_t = FunctionTraits<decltype(cuModuleLoad)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      cumodules_.push_back(std::make_unique<CUmodule>());
      std::get<0>(args) = cumodules_.back().get();
      CUresult result = std::apply(cuModuleLoad, args);

      response_.set_cuscalar((uint64_t) *cumodules_.back());
      response_.set_curesult(result);
  } else if(FUNC_COMP(function_name, cuModuleGetFunction)){
      //HACK consider access control in multi-client case
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      using param_t = FunctionTraits<decltype(cuModuleGetFunction)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      cufuncs_.push_back(std::make_unique<CUfunction>());
      std::get<0>(args) = cufuncs_.back().get();
      CUresult result = std::apply(cuModuleGetFunction, args);

      response_.set_cuscalar((uint64_t) *cufuncs_.back());
      response_.set_curesult(result);
  } else if(FUNC_COMP(function_name, cuLaunchKernel)){
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      using param_t = FunctionTraits<decltype(cuLaunchKernel)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      std::vector<uint64_t*> kernel_args; deserializer_ >> kernel_args;
      std::get<9>(args) = (void**)kernel_args.data();
      CUresult result = std::apply(cuLaunchKernel, args);

      response_.set_curesult(result);
  } else if(FUNC_COMP(function_name, cuMemcpyDtoH)){
      //HACK consider controlled access pattern
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
      
      using param_t = FunctionTraits<decltype(cuMemcpyDtoH)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      size_t bytesize = std::get<2>(args);
      std::get<0>(args) = vgpu_ptr_;
      CUresult result = std::apply(cuMemcpyDtoH, args);
      vgpu_.sync(bytesize);
      float temp = ((float*)vgpu_ptr_)[0];
      std::cout << "cuMemcpyDtoH gives " << temp << std::endl;

      response_.set_curesult(result);
  } else if(FUNC_COMP(function_name, cuCtxDestroy)){
      close(client_fd_);
      client_fd_ = -1;
  } else {
      func_map[function_name]();
  }

  return_result();
}
//======================================================================================//

