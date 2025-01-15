#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "hvcomm.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using hvcomm::CUDAAPIService;
using hvcomm::RequestMessage;
using hvcomm::ResponseMessage;

/**
 * @brief grpc class to transfer data from virtual gpu to the host
 */
class CUDAAPIClient {
  public:
    CUDAAPIClient(std::shared_ptr<Channel> channel)
        : stub_(CUDAAPIService::NewStub(channel)){}

    ResponseMessage &CallCudaFunction(const std::string &function_name,
                                      std::vector<std::string> &string_args,
                                      std::vector<uint64_t> &scalar_args);

  private:
    std::unique_ptr<CUDAAPIService::Stub> stub_;
};

#endif