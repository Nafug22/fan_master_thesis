#include "interception.h"
#include <stdio.h>
#include <cstring>

#define STRINGIFY(x) #x

/* Get the real `dlsym` handler in `libdl` */
static void *real_dlsym(void *handle, const char *symbol) {
  static fnDlsym internal_dlsym =
    (fnDlsym)__libc_dlsym(__libc_dlopen_mode("libdl.so.2", RTLD_LAZY), "dlsym");
  return (*internal_dlsym)(handle, symbol);
}

/* Intercept the `dlsym` function, which is used by `libcudart.so` to link to `libcuda.so` */
void *dlsym(void *handle, const char *symbol) {
  // for CUDA func
  if(strcmp(symbol, CUDA_SYMBOL_STRING(cuGetProcAddress)) == 0){
    printf("intercepted: %s\n", symbol);
    return (void *) &getProcAddressBySymbol;
  }

  // for all other func
  return (real_dlsym(handle, symbol));
}

/* Interception version for `cuGetProcAddress` and all needed CUDA funcs */
extern "C" CUresult getProcAddressBySymbol(const char* symbol, void** pfn, int cudaVersion, cuuint64_t flags,
                                          CUdriverProcAddressQueryResult* symbolStatus) {
  printf("Intercepted cuGetProcAddress: symbol=%s\n", symbol);
  if(strcmp(symbol, "cuGetProcAddress") == 0){
    *pfn = (void *) &getProcAddressBySymbol;
    return CUDA_SUCCESS;
  }

  // If no need to intercept, call the corresponding CUDA function directly
  CUresult result = cuGetProcAddress_v2(symbol, pfn, cudaVersion, flags, symbolStatus);

  return result;
}


// Define the function pointer type for cuMemAlloc
// typedef CUresult (*cuMemAlloc_t)(CUdeviceptr *dptr, size_t size);
// typedef CUresult (*cuCtxSetCurrent_t)(CUcontext ctx);

// Global variable to hold the original function pointer
// static cuMemAlloc_t original_cuMemAlloc = NULL;
// static cuCtxSetCurrent_t original_cuCtxSetCurrent = NULL;

/* Macro used to generate hooks to CUDA driver functions (that need to be intercepted) */
#define CU_HOOK_DRIVER_FUNC(symbol, params, ...) \
  extern "C" CUresult CUDAAPI symbol params {   \
    using symbol##handler = CUresult CUDAAPI (params);  \
    auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, CUDA_SYMBOL_STRING(symbol)); \
    printf("Intercepted: %s\n", STRINGIFY(symbol)); \
    CUresult result = real_func(__VA_ARGS__); \
    return result;  \
  }

CU_HOOK_DRIVER_FUNC(cuInit,
                    (unsigned int Flags),
                    Flags)
CU_HOOK_DRIVER_FUNC(cuDeviceGet,
                    (CUdevice* device, int ordinal),
                    device, ordinal)
CU_HOOK_DRIVER_FUNC(cuCtxCreate,
                    (CUcontext* pctx, unsigned int flags, CUdevice dev),
                    pctx, flags, dev)
CU_HOOK_DRIVER_FUNC(cuMemAlloc,
                    (CUdeviceptr* dptr, size_t bytesize),
                    dptr, bytesize)
CU_HOOK_DRIVER_FUNC(cuMemcpyHtoD,
                    (CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount),
                    dstDevice, srcHost, ByteCount)
CU_HOOK_DRIVER_FUNC(cuModuleLoad,
                    (CUmodule* module, const char* fname),
                    module, fname)
CU_HOOK_DRIVER_FUNC(cuModuleGetFunction,
                    (CUfunction* hfunc, CUmodule hmod, const char* name),
                    hfunc, hmod, name)
CU_HOOK_DRIVER_FUNC(cuLaunchKernel,
                    (CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ,
                     unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ,
                     unsigned int sharedMemBytes, CUstream hStream,
                     void** kernelParams, void** extra),
                    f, gridDimX, gridDimY, gridDimZ,
                    blockDimX, blockDimY, blockDimZ,
                    sharedMemBytes, hStream,
                    kernelParams, extra)
CU_HOOK_DRIVER_FUNC(cuMemcpyDtoH,
                    (void* dstHost, CUdeviceptr srcDevice, size_t ByteCount)
                    dstHost, srcDevice, ByteCount)
CU_HOOK_DRIVER_FUNC(cuMemFree,
                    (CUdeviceptr dptr),
                    dptr)
CU_HOOK_DRIVER_FUNC(cuModuleUnload,
                    (CUmodule hmod),
                    hmod)
CU_HOOK_DRIVER_FUNC(cuCtxDestroy,
                    (CUcontext ctx),
                    ctx)
// #define CU_HOOK_DRIVER_FUNC_VOID(symbol, params, ...) \
//   extern "C" CUresult CUDAAPI symbol params {   \
//     using symbol##handler = CUresult CUDAAPI (params);  \
//     auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, CUDA_SYMBOL_STRING(symbol)); \
//     printf("[NO EXEC]Intercepted: %s\n", STRINGIFY(symbol)); \
//     return CUDA_SUCCESS;  \
//   }

// CU_HOOK_DRIVER_FUNC(cuDeviceGetCount,
//                     (int *count),
//                     count)
// CU_HOOK_DRIVER_FUNC(cuMemcpyHtoD,
//                     (CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount),
//                     dstDevice, srcHost, ByteCount)
// CU_HOOK_DRIVER_FUNC(cuDeviceGet,
//                         (CUdevice* device, int ordinal),
//                         device, ordinal)
// CU_HOOK_DRIVER_FUNC(cuCtxCreate,
//                         (CUcontext* pctx, unsigned int flags, CUdevice dev),
//                         pctx, flags, dev)


// CU_HOOK_DRIVER_FUNC(cuCtxSetCurrent,
//                     (CUcontext ctx),
//                     ctx)

// This will be called instead of cuMemAlloc
// extern "C" CUresult CUDAAPI cuCtxSetCurrent(CUcontext ctx){
//   using symbol##handler = CUresult CUDAAPI (params);  \
//   auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, CUDA_SYMBOL_STRING(symbol)); \
//   printf("Intercepted: %s\n", STRINGIFY(symbol)); \
//   CUresult result = real_func(__VA_ARGS__); \
//   return result;  \
//     if (!original_cuCtxSetCurrent) {
//         original_cuCtxSetCurrent = (cuCtxSetCurrent_t) dlsym(RTLD_NEXT, "cuCtxSetCurrent");
//         if (!original_cuCtxSetCurrent) {
//             fprintf(stderr, "Error: dlsym failed to find cuCtxSetCurrent\n");
//             return CUDA_ERROR_UNKNOWN;
//         }
//     }

//     CUresult result = original_cuCtxSetCurrent(ctx);

//     return result;
// }

// extern "C" CUresult CUDAAPI cuMemAlloc(CUdeviceptr *dptr, size_t size) {
//     // Get the original cuMemAlloc function using dlsym
//     if (!original_cuMemAlloc) {
//         original_cuMemAlloc = (cuMemAlloc_t) dlsym(RTLD_NEXT, "cuMemAlloc");
//         if (!original_cuMemAlloc) {
//             fprintf(stderr, "Error: dlsym failed to find cuMemAlloc\n");
//             return CUDA_ERROR_UNKNOWN;
//         }
//     }

//     CUresult result = original_cuMemAlloc(dptr, size);
//     // CUresult result = gpu_instance.MemAlloc(dptr, size);
    
//     // Log or modify memory allocation result after calling the original function
//     if (result == CUDA_SUCCESS) {
//         printf("\tMemory allocated at device pointer: %p\n", *dptr);
//     } else {
//         printf("\tcuMemAlloc failed with error code %d\n", result);
//     }

//     return result;
// }
