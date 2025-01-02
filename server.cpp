#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "hvcomm.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using hvcomm::CUDAAPIService;
using hvcomm::RequestMessage;
using hvcomm::ResponseMessage;

/**
 * @brief Implement the `CUDAAPIService` service
 */
class CUDAAPIServiceImpl final : public CUDAAPIService::Service {
  public:
    Status CallCudaFunction(ServerContext* context, const RequestMessage* request, ResponseMessage* response) override {
        std::string function_name = request->function_name();
        response->set_status(1);

        std::cout << "Received GPU commands: " << function_name;

        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    CUDAAPIServiceImpl service;

    // Set up the gRPC server
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
}

int main() {
    RunServer();
    return 0;
}
