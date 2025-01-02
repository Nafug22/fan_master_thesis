#include "client.h"

//=============================================================================
void CUDAAPIClient::CallCudaFunction(const std::string& function_name) {
    RequestMessage request;
    request.set_function_name(function_name);

    ResponseMessage response;
    ClientContext context;

    // Make the gRPC call
    Status status = stub_->CallCudaFunction(&context, request, &response);

    if (status.ok()) {
        std::cout << "Server Response: " << response.message() << std::endl;
    } else {
        std::cerr << "gRPC call failed: " << status.error_message() << std::endl;
    }
}