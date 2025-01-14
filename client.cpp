#include "client.h"

//=================================================================================//
ResponseMessage &CUDAAPIClient::CallCudaFunction(const std::string& function_name,
                                                 std::vector<std::string> &string_args,
                                                 std::vector<uint64_t> &scalar_args) {
    RequestMessage request;
    request.set_function_name(function_name);

    for(auto &arg : string_args) request.add_string_parameters(arg);
    for(auto &arg : scalar_args) request.add_scalar_parameters(arg);

    ResponseMessage response;
    ClientContext context;
    Status status = stub_->CallCudaFunction(&context, request, &response);

    if (status.ok()) {
        std::cout << "Server Response: CUresult = " << response.curesult() << std::endl;
    } else {
        std::cerr << "gRPC call failed: " << status.error_message() << std::endl;
    }

    return response;
}