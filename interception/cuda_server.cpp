#include <cstdio>
#include <stdio.h>
#include <cstring>
#include <memory>
#include <csignal>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cuda.h>

#include "type_decl.h"
#include "stream.h"

#define SOCKET_PATH "./v.sock_1234"
#define ADD_SYMBOL(symbol) {SYMBOL_TO_STR(symbol), [this]() { this->call_##symbol(); }}

/* generate the corresponding cuda function with arguments stored in shared memory. */
#define CUDA_API_IMPL(symbol) \
void call_##symbol(){ \
    std::cout << "<<<<<<<<<<implemented as " << #symbol << std::endl; \
    using param_t = FunctionTraits<decltype(symbol)>::ParameterTuple;  \
    param_t args; deserializer_ >> args;  \
    CUresult result = std::apply(symbol, args);                \
    response_.set_curesult(result);                              \
}

class CUDAServer{
  public:
    CUDAServer(){
        initialize_gpu();
        server_fd_ = initialize_server();
        client_fd_ = initialize_client();
    }
    ~CUDAServer(){
        release_gpu();
        close(server_fd_);
        close(client_fd_);
        unlink(SOCKET_PATH);
    };

    void run(){
      while(true){
        implement_cuda_function();
      }
    }

  /******************************************************
   *        host CUDA allocated pointers storage        *
   ******************************************************/
  #pragma region
  private:
    std::vector<std::unique_ptr<CUdeviceptr>> device_ptrs_;
    std::vector<std::unique_ptr<CUmodule>> cumodules_;
    std::vector<std::unique_ptr<CUfunction>> cufuncs_;

    bool function_fit(std::string str1, std::string str2){
      return strncmp(str1.c_str(), str2.c_str(), str2.size());
    }
    //TODO: - refactor into GPU instances
    CUdevice device;
    CUcontext cucontext;
    void initialize_gpu(){
        cuDeviceGet(&device, 0);
        cuCtxCreate(&cucontext, 0, device);
    }
  #pragma endregion
  /******************************************************
   *                vsock related setup                 *
   ******************************************************/
  #pragma region
  private:
    int server_fd_;
    int initialize_server(){
        int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if(server_fd == -1) perror("server socket failed");

        struct sockaddr_un server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
        unlink(SOCKET_PATH);

        if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
            perror("server bind failed");
            close(server_fd);
        }

        if(listen(server_fd, 5) == -1){
            perror("server listen failed");
            close(server_fd);
        }

        std::cout << "Serverlistening on " << SOCKET_PATH << std::endl;
        return server_fd;
    }

    int client_fd_;
    int initialize_client(){
        struct sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
        if(client_fd == -1){
            perror("accept failed");
            close(server_fd_);
        }

        std::cout << "Client connected!" << std::endl;
        return client_fd;
    }

    void release_gpu(){
      cuCtxDestroy(cucontext);
    }

    Response response_;
    void return_result(){
        send(client_fd_, response_.data(), response_.size(), 0);
    }

    Deserializer deserializer_;
    char* pull_command(){
        deserializer_.clean();
        int bytes_read = read(client_fd_, deserializer_.data(), deserializer_.size());
        char* func_name;
        deserializer_ >> func_name;
        return func_name;
    }
  #pragma endregion
  /******************************************************
   *         cuda implementation related setup          *
   ******************************************************/
  private:
    //HACK consider adding constraints that are consistant with the gpu partition
    //TODO vgpu initialization with configured size
    VirtualGPU vgpu{(1 << 20) * sizeof(float)};
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

          response_.set_cuscalar(*device_ptrs_.back());
          response_.set_curesult(result);
      }

      //HACK consider controled access pattern
      if(function_fit(function_name, "cuMemcpyHtoD") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuMemcpyHtoD)>::ParameterTuple;
          param_t args; deserializer_ >> args;
          std::get<1>(args) = vgpu.get();
          CUresult result = std::apply(cuMemcpyHtoD, args);

          response_.set_curesult(result);
      }

      //HACK consider access control in multi-client case
      if(function_fit(function_name, "cuModuleLoad") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuModuleLoad)>::ParameterTuple;
          param_t args; deserializer_ >> args;
          cumodules_.push_back(std::make_unique<CUmodule>());
          std::get<0>(args) = cumodules_.back().get();
          CUresult result = std::apply(cuModuleLoad, args);

          response_.set_cuscalar((uint64_t) *cumodules_.back());
          response_.set_curesult(result);
      }

      //HACK consider access control in multi-client case
      if(function_fit(function_name, "cuModuleGetFunction") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuModuleGetFunction)>::ParameterTuple;
          param_t args; deserializer_ >> args;
          cufuncs_.push_back(std::make_unique<CUfunction>());
          std::get<0>(args) = cufuncs_.back().get();
          CUresult result = std::apply(cuModuleGetFunction, args);

          response_.set_cuscalar((uint64_t) *cufuncs_.back());
          response_.set_curesult(result);
      }

      if(function_fit(function_name, "cuLaunchKernel") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuLaunchKernel)>::ParameterTuple;
          param_t args; deserializer_ >> args;
          std::vector<void*> kernel_args; deserializer_ >> kernel_args;
          std::get<9>(args) = kernel_args.data();
          CUresult result = std::apply(cuLaunchKernel, args);

          response_.set_curesult(result);
      }

      //HACK consider controlled access pattern
      if(function_fit(function_name, "cuMemcpyDtoH") == 0){
          std::cout << "<<<<<<<<<<implemented as " << function_name << std::endl;

          using param_t = FunctionTraits<decltype(cuMemcpyDtoH)>::ParameterTuple;
          param_t args; deserializer_ >> args;
          std::get<0>(args) = vgpu.get();
          CUresult result = std::apply(cuMemcpyDtoH, args);

          response_.set_curesult(result);
      }

      if(func_map.count(function_name)) func_map[function_name]();
    }

    CUDA_API_IMPL(cuMemFree)
    CUDA_API_IMPL(cuModuleUnload)

  private:
    std::unordered_map<std::string, std::function<void()>> func_map = {
      ADD_SYMBOL(cuMemFree),
      ADD_SYMBOL(cuModuleUnload)
    };
};

CUDAServer *server;

void handle_signal(int signal){
  std::cout << "\nexiting cuda_server with signal " << signal << "..." << std::endl;
  delete server;

  std::exit(0);
}

int main(){
  std::cout << "cuda_server running ..." << std::endl;
  cuInit(0);
  server = new CUDAServer{};
  signal(SIGINT, handle_signal);
  signal(SIGABRT, handle_signal);
  signal(SIGSEGV, handle_signal);
  server->run();

  return 0;
}