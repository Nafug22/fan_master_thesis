#ifndef CUDA_CLIENT_H
#define CUDA_CLIENT_H
#include <stdio.h>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>

#include <sys/socket.h>
#include <linux/vm_sockets.h>
#include "stream.h"

#include <cuda.h>

#define VSOCK_HOST_CID 2
#define VSOCK_PORT 1234
#define BUFFER_RECV 200

//TODO add size exception control (when the BUFFER_SIZE is too small)
class CUDAClient{
  public:
    CUDAClient() : sock_(initialize_sock()){};
    ~CUDAClient(){
        close(sock);
    }

    //TODO consider combine the two `CallCudaFunction` into one.
    /**
     * The metadata of a intercepted cuda function will be packed as two parts:
     * 1. (char*) null-terminated function name.
     * 2. (std::tuple<Args...>) the tuple constructed by forwarding the cuda
     *    function arguments.
     * 
     * @tparam TupleArgs Argument types of the function parameters tuple.
     * @param func_name the function name of of the calling function.
     * @param args a tuple composing of arguments of the calling cuda function.
     * @return The execution result on the host.
     */
    template <typename... TupleArgs>
    CUresult CallCudaFunction(const char* &func_name, const std::tuple<TupleArgs> &args){
        serializer_ << func_name << args;

        send(sock, serializer_.data(), serializer_.size(), 0);
        wait_recv();
        serializer_.clean();
        return response_.curesult();
    }

    /**
     * @brief for `cuLaunchKernel`
     */
    template <typename... TupleArgs>
    CUresult CallCudaFunction(const char* func_name,
                              const std::tuple<TupleArgs> &args,
                              const ::vector<uint64_t> &kernel_args){
        serializer_ << func_name << args << kernel_args;
        send(sock, serializer_.data(), serializer_.size(), 0);
        wait_recv();
        serializer_.clean();
        return response_.curesult();
    }

    uint64_t get_scalar_result() { return response_.cuscalar(); };

  private:
    int sock_;
    Response response_;
    Serializer serializer_;

    int initialize_sock(){
        int sock = socket(AF_VSOCK, SOCK_STREAM, 0);
        if(sock < 0) perror("socket");

        sockaddr_vm sa = {};
        sa.svm_family = AF_VSOCK;
        sa.svm_cid = VSOCK_HOST_CID;
        sa.svm_port = VSOCK_PORT;

        std::cout << "Connecting to host ..." << std::endl;
        if(connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0) perror("connect");

        return sock;
    }

    void wait_recv(){
        recv(sock, response_.data(), response_.size(), 0);
    }
};

#endif