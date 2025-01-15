#include <iostream>
#include <memory>
#include <string>

#include "vgpu.h"

#include "hvcomm.grpc.pb.h"

#include <cuda.h>
#include <vector>
#include <iostream>
#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using hvcomm::CUDAAPIService;
using hvcomm::RequestMessage;
using hvcomm::ResponseMessage;

/* convert string into the target type. */
// #define INTERPRET(target, index)  \
//     static std::stringstream sstream(parameters[index]);  \
//     sstream >> target;

/**
 * @brief Implement the `CUDAAPIService` service
 */
class CUDAAPIServiceImpl final : public CUDAAPIService::Service {
  private:
    /**
     * TODO: - refactor into GPU instances
     */
    CUdevice device;
    CUcontext cucontext;
    std::vector<std::unique_ptr<CUdeviceptr>> device_ptrs_;
    std::vector<std::unique_ptr<CUmodule>> cumodules_;
    std::vector<std::unique_ptr<CUfunction>> cufuncs_;
    //HACK consider adding constraints that are consistant with the gpu partition
    //TODO vgpu initialization with configured size
    VirtualGPU vgpu((1 << 20) * sizeof(float));

    void initialize(){
        cuDeviceGet(&device, 0);
        cuCtxCreate(&cucontext, 0, device);
    }
  public:
    CUDAAPIServiceImpl(){ initialize(); };
    Status CallCudaFunction(ServerContext* context, const RequestMessage* request, ResponseMessage* response) override {
        std::string function_name = request->function_name();
        std::cout << "Received GPU commands: " << function_name << std::endl;
        std::vector<std::string> string_parameters(request->string_parameters().begin(), request->string_parameters().end());
        std::vector<uint64_t> scalar_parameters(request->scalar_parameters().begin(), request->scalar_parameters().end());
        

        //TODO client name should be used in the future
        //HACK use FUNC_MAP
        // // if(function_name == "cuDeviceGet"){
        // //     cuDeviceGet(&device, 0);
        // // }

        // // if(function_name == "cuCtxCreate"){
        // //     cuCtxCreate(&cucontext, 0, device);
        // // }

        if(function_name == "cuMemAlloc"){
            size_t size = scalar_parameters[1];
            device_ptrs_.push_back(std::make_unique<CUdeviceptr>());

            CUresult result = cuMemAlloc(device_ptrs_.back().get(), size);

            response->set_scalar(*device_ptrs_.back());
            response->set_curesult(result);
        }

        //HACK consider controled access pattern
        if(function_name == "cuMemcpyHtoD"){
            size_t size = scalar_parameters[2];

            CUresult result = cuMemcpyHtoD(scalar_parameters[0], vgpu.vgpu_ptr_, size);

            response->set_curesult(result);
        }

        //HACK consider access control in multi-client case
        if(function_name == "cuModuleLoad"){
            cumodules_.push_back(std::make_unique<CUmodule>());
            CUresult result = cuModuleLoad(cumodules_.back().get(), string_parameters[0].c_str());

            response->set_scalar((uint64_t) *cumodules_.back());
            response->set_curesult(result);
        }

        //HACK consider access control in multi-client case
        if(function_name == "cuModuleGetFunction"){
            cufuncs_.push_back(std::make_unique<CUfunction>());
            CUresult result = cuModuleGetFunction(cufuncs_.back().get(), (CUmodule) scalar_parameters[0], string_parameters[0].c_str());

            response->set_scalar((uint64_t) *cufuncs_.back());
            response->set_curesult(result);
        }

        if(function_name == "cuLaunchKernel"){
            void *args[scalar_parameters.size() - 9];
            for(int i = 9; i < scalar_parameters.size(); i++){
                args[i - 9] = &scalar_parameters[i];
            }
            CUresult result = cuLaunchKernel((CUfunction) scalar_parameters[0],
                                             scalar_parameters[1], scalar_parameters[2], scalar_parameters[3],
                                             scalar_parameters[4], scalar_parameters[5], scalar_parameters[6],
                                             scalar_parameters[7], (CUstream) scalar_parameters[8],
                                             args, nullptr);

            response->set_curesult(result);
        }

        //HACK consider controlled access pattern
        if(function_name == "cuMemcpyDtoH"){
            CUresult result = cuMemcpyDtoH(vgpu.vgpu_ptr_, scalar_parameters[1], scalar_parameters[2]);
            response->set_curesult(result);
        }

        if(function_name == "cuMemFree"){
            CUresult result = cuMemFree(scalar_parameters[0]);
            response->set_curesult(result);
        }

        if(function_name == "cuModuleUnload"){
            CUresult result = cuModuleUnload((CUmodule) scalar_parameters[0]);
            response->set_curesult(result);
        }

        // // if(function_name == "cuCtxDestroy"){
        // //     cuCtxDestroy(cucontext);
        // // }

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
    cuInit(0);
    RunServer();
    return 0;
}
