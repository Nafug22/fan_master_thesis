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

    CUresult CallCudaFunction(const char* func_name){
      command_buffer_.push(func_name);
      return get_curesult();
    }
    void* get_args_ptr() { return cuda_args_.get_ptr(); };
    void set_scalar_args(std::vector<uint64_t> &vals) { scalar_args_.set(vals); };
    void set_string_args(const char* content) { string_args_.set(content); };

    uint64_t get_scalar_result() { return command_buffer_.get_scalar_result(); };
    CUresult get_curesult() { return command_buffer_.get_curesult(); };
  private:
    ScalarArgs scalar_args_{};
    StringArg string_args_{STRING_ARG_PORT};
    CommandBuffer command_buffer_{};
    CUDAArgs cuda_args_{};
};

#endif