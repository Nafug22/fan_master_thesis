#ifndef INTERCEPTION_H
#define INTERCEPTION_H

#include "client.h"
#include "compiler.h"

#include <cuda.h>
#include <dlfcn.h>
#include <iostream>
#include <unordered_map>

extern "C" {
void *__libc_dlsym(void *map, const char *name);
void *__libc_dlopen_mode(const char *name, int mode);
}
extern "C" CUresult CUDAAPI getProcAddressBySymbol(const char *symbol, void **pfn, int driverVersion, cuuint64_t flags,
                                           CUdriverProcAddressQueryResult *symbolStatus);

typedef void *(*fnDlsym)(void *, const char *);
void* libcuda_driver_handle = dlopen("libcuda.so", RTLD_LAZY);

std::string server_address = "localhost:50051";
CUDAAPIClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

#define CUDA_CHECK(err) \
    if (err != CUDA_SUCCESS) { \
        std::cerr << "In gpu_instance, CUDA error: " << err << " at line " << __LINE__ << std::endl; \
        exit(EXIT_FAILURE); \
    }

static CUFuncProto func_proto("vector_add.ptx");
static std::unordered_map<CUfunction, std::string> hashfunc;

#endif