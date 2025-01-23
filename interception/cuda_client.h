#ifndef CUDA_CLIENT_H
#define CUDA_CLIENT_H
#include <stdio.h>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>

#include <cuda.h>
#include "shared_memory_manager.h"

class CUDAClient{
  public:
    CUDAClient(){};
    ~CUDAClient(){};

    //TODO refactor to return `CUresult`
    void CallCudaFunction(const char* func_name,
                          std::vector<std::string> &string_arg,
                          std::vector<uint64_t> &scalar_args){
      scalar_args_.set(scalar_args);
      if(!string_arg.empty()) string_args_.set(string_arg[0]);
      command_buffer_.push(func_name);
    }
    void CallCudaFunction(const char* func_name){
      command_buffer_.push(func_name);
    }
    void* get_args_ptr() { return cuda_args_.get_ptr(); };

    uint64_t get_scalar_result() { return command_buffer_.get_scalar_result(); };
    CUresult get_curesult() { return command_buffer_.get_curesult(); };
  private:
    ScalarArgs scalar_args_{};
    StringArg string_args_{STRING_ARG_PORT};
    CommandBuffer command_buffer_{};
    CUDAArgs cuda_args_{};
};

#endif