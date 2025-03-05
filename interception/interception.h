#ifndef INTERCEPTION_H
#define INTERCEPTION_H

#include "cuda_client.h"
#include "compiler.h"
#include <cstdint>
#include <cuda.h>
#include <dlfcn.h>
#include <iostream>
#include <unordered_map>

#include "type_decl.h"
#ifndef PTX_SRC
  #define PTX_SRC "./vector_add.ptx"
#endif

extern "C" {
void *__libc_dlsym(void *map, const char *name);
void *__libc_dlopen_mode(const char *name, int mode);
}

extern "C" CUresult CUDAAPI getProcAddressBySymbol(const char *symbol, void **pfn, int driverVersion, cuuint64_t flags,
                                                   CUdriverProcAddressQueryResult *symbolStatus);

typedef void *(*fnDlsym)(void *, const char *);
void* libcuda_driver_handle = dlopen("libcuda.so", RTLD_LAZY);

CUDAClient client{};
static CUFuncProto func_proto{PTX_SRC};
//HACK consider integrate into the vgpu
static std::unordered_map<CUfunction, std::string> hashfunc;
#endif