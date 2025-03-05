#include "gpu_instance.h"

static std::unordered_map<string, 

//======================================================================================//
void implement_cuda_function(){
  cuCtxSetCurrent(cucontext);
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

  if(function_fit(function_name, "cuMemAlloc") == 0){
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
      using param_t = FunctionTraits<decltype(cuMemAlloc)>::ParameterTuple;
      param_t args; deserializer_ >> args;

      device_ptrs_.push_back(std::make_unique<CUdeviceptr>());
      std::get<0>(args) = device_ptrs_.back().get();
      CUresult result = std::apply(cuMemAlloc, args);
      response_.set_curesult(result);
      response_.set_cuscalar(*device_ptrs_.back());
  } else if(function_fit(function_name, "cuMemcpyHtoD") == 0){
      //HACK consider controled access pattern
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      using param_t = FunctionTraits<decltype(cuMemcpyHtoD)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      size_t bytesize = std::get<2>(args);
      char* vgpu_ptr = (char*)vgpu.prepare(bytesize);
      size_t total_recv = 0;
      while(total_recv < bytesize){
          ssize_t local_recv = recv(client_fd_, vgpu_ptr + total_recv, bytesize - total_recv, 0);
          total_recv += local_recv;
          std::cout << "total_recv = " << total_recv << std::endl;
      }
      std::cout << "right before the cuda function " << std::endl;
      std::get<1>(args) = (void*)vgpu_ptr;
      CUresult result = std::apply(cuMemcpyHtoD, args);
      std::cout << "function HtoD finished" << std::endl;
      response_.set_curesult(result);
  } else if(function_fit(function_name, "cuModuleLoad") == 0){
      //HACK consider access control in multi-client case
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      using param_t = FunctionTraits<decltype(cuModuleLoad)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      cumodules_.push_back(std::make_unique<CUmodule>());
      std::get<0>(args) = cumodules_.back().get();
      CUresult result = std::apply(cuModuleLoad, args);

      response_.set_cuscalar((uint64_t) *cumodules_.back());
      response_.set_curesult(result);
  } else if(function_fit(function_name, "cuModuleGetFunction") == 0){
      //HACK consider access control in multi-client case
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      using param_t = FunctionTraits<decltype(cuModuleGetFunction)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      cufuncs_.push_back(std::make_unique<CUfunction>());
      std::get<0>(args) = cufuncs_.back().get();
      CUresult result = std::apply(cuModuleGetFunction, args);

      response_.set_cuscalar((uint64_t) *cufuncs_.back());
      response_.set_curesult(result);
  } else if(function_fit(function_name, "cuLaunchKernel") == 0){
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      using param_t = FunctionTraits<decltype(cuLaunchKernel)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      std::vector<uint64_t*> kernel_args; deserializer_ >> kernel_args;
      std::get<9>(args) = (void**)kernel_args.data();
      CUresult result = std::apply(cuLaunchKernel, args);

      response_.set_curesult(result);
  } else if(function_fit(function_name, "cuMemcpyDtoH") == 0){
      //HACK consider controlled access pattern
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;
      
      using param_t = FunctionTraits<decltype(cuMemcpyDtoH)>::ParameterTuple;
      param_t args; deserializer_ >> args;
      size_t bytesize = std::get<2>(args);
      std::get<0>(args) = vgpu.prepare(bytesize);
      CUresult result = std::apply(cuMemcpyDtoH, args);
      send(client_fd_, vgpu.get(), bytesize, 0);
      response_.set_curesult(result);
  } else {
      std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

      auto functor = func_map[function_name];
      using param_t = FunctionTraits<decltype(func_map[function_name])>::ParameterTuple;
      param_t args; deserializer_ >> args;
      CUresult result = std::apply(functor, args);

      response_.set_curesult(result);
  }

  if(func_map.count(function_name)) func_map[function_name]();
  return_result();
}
//======================================================================================//

