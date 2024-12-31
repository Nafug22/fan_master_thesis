#include "interception.h"
#include <stdio.h>
#include <cstring>

#define STRINGIFY(x) #x
#define CUDA_SYMBOL_STRING(x) STRINGIFY(x)

static GPUInstance gpu_instance;

static void *real_dlsym(void *handle, const char *symbol) {
  static fnDlsym internal_dlsym =
      (fnDlsym)__libc_dlsym(__libc_dlopen_mode("libdl.so.2", RTLD_LAZY), "dlsym");
  return (*internal_dlsym)(handle, symbol);
}

void *dlsym(void *handle, const char *symbol) {
  // Early out if not a CUDA driver symbol
  // if (strncmp(symbol, "cu", 2) != 0) {
  //   return (real_dlsym(handle, symbol));
  // }

  printf("intercepted: %s\n", symbol);
  if (strcmp(symbol, CUDA_SYMBOL_STRING(cuGetProcAddress)) == 0) {
      printf("intercepted: %s\n", symbol);
      return (void *) &getProcAddressBySymbol;
      // return (real_dlsym(handle, symbol));
  }
  // omit cuDeviceTotalMem here so there won't be a deadlock in cudaEventCreate when we are in
  // initialize(). Functions called by cliet are still being intercepted.
  return (real_dlsym(handle, symbol));
}


// Pointer to hold the original function
static cuGetProcAddress_t real_cuGetProcAddress = NULL;
// static void *real_cuGetProcAddress = NULL;

// Our custom implementation
extern "C" CUresult getProcAddressBySymbol(const char* symbol, void** pfn, int cudaVersion,
                          cuuint64_t flags, CUdriverProcAddressQueryResult* symbolStatus) {
    printf("Intercepted cuGetProcAddress: symbol=%s\n", symbol);
    // if(strcmp(symbol, "cuGetProcAddress") == 0){
    //     *pfn = (void *) &getProcAddressBySymbol;
    //     return CUDA_SUCCESS;
    // }
    // Call the original function
    if(strcmp(symbol, "cuGetProcAddress") == 0){
        *pfn = (void *) &getProcAddressBySymbol;
        return CUDA_SUCCESS;
    }
    CUresult result = cuGetProcAddress_v2(symbol, pfn, cudaVersion, flags, symbolStatus);

    // Optional: Modify the returned function pointer (pfn) if needed
    // Example: Inject a custom function
    // if (strcmp(symbol, "someFunction") == 0) {
    //     *pfn = (void*)myCustomFunction;
    // }

    return result;
}


// Define the function pointer type for cuMemAlloc
typedef CUresult (*cuMemAlloc_t)(CUdeviceptr *dptr, size_t size);
typedef CUresult (*cuCtxSetCurrent_t)(CUcontext ctx);

// Global variable to hold the original function pointer
static cuMemAlloc_t original_cuMemAlloc = NULL;
static cuCtxSetCurrent_t original_cuCtxSetCurrent = NULL;

#define CU_HOOK_DRIVER_FUNC(symbol, params, ...) \
  extern "C" CUresult CUDAAPI symbol params {   \
    using symbol##handler = CUresult CUDAAPI (params);  \
    auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, CUDA_SYMBOL_STRING(symbol)); \
    printf("Intercepted: %s\n", STRINGIFY(symbol)); \
    CUresult result = real_func(__VA_ARGS__); \
    return result;  \
  }
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
extern "C" CUresult CUDAAPI cuCtxSetCurrent(CUcontext ctx){
    if (!original_cuCtxSetCurrent) {
        original_cuCtxSetCurrent = (cuCtxSetCurrent_t) dlsym(RTLD_NEXT, "cuCtxSetCurrent");
        if (!original_cuCtxSetCurrent) {
            fprintf(stderr, "Error: dlsym failed to find cuCtxSetCurrent\n");
            return CUDA_ERROR_UNKNOWN;
        }
    }

    CUresult result = original_cuCtxSetCurrent(ctx);

    return result;
}

extern "C" CUresult CUDAAPI cuMemAlloc(CUdeviceptr *dptr, size_t size) {
    // Get the original cuMemAlloc function using dlsym
    if (!original_cuMemAlloc) {
        original_cuMemAlloc = (cuMemAlloc_t) dlsym(RTLD_NEXT, "cuMemAlloc");
        if (!original_cuMemAlloc) {
            fprintf(stderr, "Error: dlsym failed to find cuMemAlloc\n");
            return CUDA_ERROR_UNKNOWN;
        }
    }

    CUresult result = original_cuMemAlloc(dptr, size);
    // CUresult result = gpu_instance.MemAlloc(dptr, size);
    
    // Log or modify memory allocation result after calling the original function
    if (result == CUDA_SUCCESS) {
        printf("\tMemory allocated at device pointer: %p\n", *dptr);
    } else {
        printf("\tcuMemAlloc failed with error code %d\n", result);
    }

    return result;
}
