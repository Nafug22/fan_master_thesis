#include "interception.h"
#include <stdio.h>
#include <cstring>
#include <unordered_map>
#include <unistd.h>
#include <cuda_runtime.h>
#include <cassert>
//==================================================================================================================
extern "C" cudaError_t cudaGetDeviceCount(int *count) {
  printf("[Intercepted runtime execution] cudaGetDeviceCount\n");
  client.CallCudaFunction(__func__, count);
  cudaError_t result;
  client.wait_recv(result, count);
  return result;
}
//==================================================================================================================
extern "C" cudaError_t cudaGetDevice(int *device) {
  printf("[Intercepted runtime execution] cudaGetDevice\n");
  client.CallCudaFunction(__func__);
  cudaError_t result;
  client.wait_recv(result, device);
  return result;
}
//==================================================================================================================
extern "C" cudaError_t cudaGetDeviceProperties(cudaDeviceProp *prop, int device) {
  printf("[Intercepted runtime execution] cudaGetDeviceProperties\n");
  client.CallCudaFunction(__func__, device);
  cudaError_t result;
  client.wait_recv(result, prop);
  return result;
}
//==================================================================================================================
/* Get the real `dlsym` handler in `libdl` */
//FIXME better compatibility for all sys
static void *real_dlsym(void *handle, const char *symbol) {
  static fnDlsym internal_dlsym =
    (fnDlsym) dlvsym(dlopen("libdl.so.2", RTLD_LAZY), "dlsym", "GLIBC_2.34");
  return (*internal_dlsym)(handle, symbol);
}
//==================================================================================================================
/* Intercept the `dlsym` function, which is used by `libcudart.so` to link to `libcuda.so` */
// void *dlsym(void *handle, const char *symbol) {
//   // for CUDA func
//   if(strcmp(symbol, SYMBOL_TO_STR(cuGetProcAddress)) == 0){
//     printf("intercepted: %s\n", symbol);
//     return (void *) &getProcAddressBySymbol;
//   }

//   // for all other func
//   return (real_dlsym(handle, symbol));
// }
void dummy() {printf("this function is not intercepted \n");}
//==================================================================================================================
#define TRY_INTERCEPT(text, target) \
  if(strcmp(symbol, text) == 0){ \
      *pfn = (void *) (&target); \
      return CUDA_SUCCESS; \
  }
//==================================================================================================================
#define TRY_DLSYM(text, target) \
  if(strcmp(symbol, SYMBOL_TO_STR(target)) == 0){ \
      return (void *) &target; \
  }
//==================================================================================================================
#define NO_INTERCEPT(text) \
  if(strcmp(symbol, text) == 0){ \
      *pfn = (void *) (&dummy); \
      return CUDA_SUCCESS; \
  }
//==================================================================================================================
    // using symbol##handler = CUresult CUDAAPI (params);
/* Macro used to generate hooks to CUDA driver functions (that need to be intercepted) */
    // static auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, SYMBOL_TO_STR(symbol));
    // CUresult result = real_func(__VA_ARGS__);
#define CU_HOOK_DRIVER_FUNC(name, symbol, params, ...) \
  extern "C" CUresult CUDAAPI name params {   \
    using symbol##handler = CUresult CUDAAPI (params); \
    static auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, SYMBOL_TO_STR(symbol)); \
    CUresult result = real_func(__VA_ARGS__); \
    return result; \
  }
    // printf("Intercepted execution: %s\n", SYMBOL_TO_STR(symbol)); \
    return CUDA_SUCCESS;  \
  }
//==================================================================================================================
/* Macro used to intercept and then generate corresponding remoting API for CUDA driver functions. */
#define CU_HOOK_REMOTE(symbol, params, ...) \
  extern "C" CUresult CUDAAPI symbol params{  \
    printf("Intercepted execution (redirected): %s\n", __func__); \
    client.CallCudaFunction(__func__, __VA_ARGS__); \
    CUresult result = client.wait_recv(__VA_ARGS__); \
    printf(">>>> success! \n"); \
    return result; \
  }
//==================================================================================================================
// #undef cudaGetDeviceProperties
// CU_HOOK_REMOTE((cudaGetDeviceProperties), ( cudaDeviceProp* prop, int  device ), prop, device)

CU_HOOK_REMOTE((cuInit), (unsigned int Flags), Flags)
CU_HOOK_REMOTE((cuDevicePrimaryCtxRelease), (CUdevice dev), dev)
CU_HOOK_REMOTE((cuCtxSetCurrent), (CUcontext ctx), ctx)
CU_HOOK_REMOTE((cuCtxPushCurrent), (CUcontext ctx), ctx)
CU_HOOK_REMOTE((cuLibraryUnload), (CUlibrary library), library)
CU_HOOK_REMOTE((cuMemsetD8Async), (CUdeviceptr dstDevice, unsigned char uc, size_t N, CUstream hStream), dstDevice, uc, N, hStream)
CU_HOOK_REMOTE((cuEventRecord), (CUevent hEvent, CUstream hStream), hEvent, hStream)
CU_HOOK_REMOTE((cuStreamSynchronize), (CUstream hStream), hStream)
CU_HOOK_REMOTE((cuCtxDestroy), (CUcontext ctx), ctx)
CU_HOOK_REMOTE((cuMemFree), (CUdeviceptr dptr), dptr)
CU_HOOK_REMOTE((cuModuleUnload), (CUmodule hmod), hmod)


CU_HOOK_REMOTE((cuDeviceGet), (CUdevice *device, int ordinal), device, ordinal)
CU_HOOK_REMOTE((cuCtxCreate), (CUcontext* pctx, unsigned int flags, CUdevice dev), pctx, flags, dev)
CU_HOOK_REMOTE((cuDeviceGetCount), (int *count), count)
#undef cuDeviceTotalMem
CU_HOOK_REMOTE((cuDeviceTotalMem_v2), (size_t *bytes, CUdevice dev), bytes, dev)
CU_HOOK_REMOTE((cuDeviceGetAttribute), (int *pi, CUdevice_attribute attrib, CUdevice dev), pi, attrib, dev)
CU_HOOK_REMOTE((cuDriverGetVersion), (int *driverVersion), driverVersion)
CU_HOOK_REMOTE((cuDeviceGetUuid), (CUuuid *uuid, CUdevice dev), uuid, dev)
CU_HOOK_REMOTE((cuDevicePrimaryCtxRetain), (CUcontext *pctx, CUdevice dev), pctx, dev)
CU_HOOK_REMOTE((cuCtxGetCurrent), (CUcontext *pctx), pctx)
CU_HOOK_REMOTE((cuCtxGetDevice), (CUdevice *device), device)
CU_HOOK_REMOTE((cuCtxGetStreamPriorityRange), (int *leastPriority, int *greatestPriority), leastPriority, greatestPriority)
CU_HOOK_REMOTE((cuCtxPopCurrent), (CUcontext *pctx), pctx)
CU_HOOK_REMOTE((cuModuleGetLoadingMode), (CUmoduleLoadingMode *mode), mode)
CU_HOOK_REMOTE((cuLibraryGetModule), (CUmodule *pMod, CUlibrary library), pMod, library)
CU_HOOK_REMOTE((cuMemHostGetDevicePointer), (CUdeviceptr *pdptr, void *p, unsigned int Flags), pdptr, p, Flags)
CU_HOOK_REMOTE((cuEventCreate), (CUevent *phEvent, unsigned int Flags), phEvent, Flags)
CU_HOOK_REMOTE((cuStreamCreate), (CUstream *phStream, unsigned int Flags), phStream, Flags)
CU_HOOK_REMOTE((cuDeviceGetName), (char *name, int len, CUdevice dev), name, len, dev)
#undef cuModuleGetGlobal
CU_HOOK_REMOTE((cuModuleGetGlobal), (CUdeviceptr *dptr, unsigned int *bytes, CUmodule hmod, const char *name), dptr, bytes, hmod, name)
CU_HOOK_REMOTE((cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags), (int *numBlocks, CUfunction func, int blockSize, size_t dynamicSMemSize, unsigned int flags), numBlocks, func, blockSize, dynamicSMemSize, flags)

/* `cuGetExportTable` is not included in the official CUDA api calls.
 * Check http://forums.developer.nvidia.com/t/cugetexporttable-explanation/259109 to get some info.
*/
// CU_HOOK_REMOTE((cuGetExportTable), (const void **ppExportTable, const CUuuid *pExportTableId), ppExportTable, pExportTableId)
//==================================================================================================================
extern "C" CUresult CUDAAPI cuOccupancyMaxPotentialBlockSize(int* minGridSize, int* blockSize, CUfunction func,
                                                              CUoccupancyB2DSize blockSizeToDynamicSMemSize,
                                                              size_t dynamicSMemSize, int  blockSizeLimit){
    printf("Intercepted execution (redirected): %s\n", __func__);
    client.CallCudaFunction(__func__, minGridSize, blockSize, func, blockSizeToDynamicSMemSize,
                                      dynamicSMemSize, blockSizeLimit);
    CUresult result = client.wait_recv(minGridSize, blockSize);
    printf(">>> success!\n");

    return result;
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuModuleLoadData(CUmodule *module, const void *image){
    printf("Intercepted execution (redirected): %s\n", __func__);
    client.to_device(image, std::strlen((const char*)image) + 1);
    client.CallCudaFunction(__func__, module, image);
    CUresult result = client.wait_recv(module);
    printf(">>> success!\n");

    return result;
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuGetExportTable(const void **ppExportTable, const CUuuid *pExportTableId){
    printf("Intercepted execution (redirected): %s\n", __func__);
    client.CallCudaFunction(__func__, *pExportTableId);
    CUresult result = client.wait_recv(ppExportTable);
    if(*ppExportTable == nullptr) printf("ppExportTable is null! \n");
    else printf(">>> success!\n");
    std::cout << "ppExportTable = " << ppExportTable << std::endl;
    return result;
    // return CUDA_SUCCESS;
}
//==================================================================================================================
//todo where is the data in srchost located?
// extern "C" CUresult CUDAAPI cuMemcpyHtoDAsync(CUdeviceptr dstDevice, const void *srcHost, unsigned int ByteCount, CUstream hStream){
//     pinned_memory.sync(srcHost);
//     client.CallCudaFunction(__func__, dstDevice, srcHost, ByteCount, hStream);
//     CUresult result = client.wait_recv();

//     printf("Intercepted execution (redirected): %s\n", __func__);
//     return result;
// }
//==================================================================================================================
//todo 
// extern "C" CUresult CUDAAPI cuGetExportTable(const void **ppExportTable, const CUuuid *pExportTableId){
//   client.CallCudaFunction(__func__, ppExportTable, pExportTableId);
//   CUresult result = client.wait_recv();
//   *priority = client.get_scalar_result();

//     return result;
// }
//==================================================================================================================
CU_HOOK_REMOTE((cuStreamIsCapturing), (CUstream hStream, CUstreamCaptureStatus *captureStatus), hStream, captureStatus)
CU_HOOK_REMOTE((cuStreamGetPriority), (CUstream hStream, int *priority), hStream, priority)
CU_HOOK_REMOTE((cuMemAlloc), (CUdeviceptr* dptr, size_t bytesize), dptr, bytesize)
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemHostAlloc(void **pp, size_t bytesize, unsigned int Flags){
    //! [todo] there would be waste for the allocated memory on the microvm
    std::cout << "cumemhostalloc invoked" << std::endl;
    client.CallCudaFunction(__func__, *pp, bytesize, Flags);
    CUresult result = client.wait_recv();
    printf("Intercepted execution (redirected): %s\n", __func__);
    return result;
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemcpyHtoD(CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount){
    client.to_device(srcHost, ByteCount);
    client.CallCudaFunction(__func__, dstDevice, srcHost, ByteCount);
    CUresult result = client.wait_recv();
    printf("Intercepted execution (redirected): %s\n", __func__);
    return result;
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemcpyDtoH(void* dstHost, CUdeviceptr srcDevice, size_t ByteCount){
    client.CallCudaFunction(__func__, dstHost, srcDevice, ByteCount);
    CUresult result = client.wait_recv();
    client.from_device(dstHost, ByteCount);
    printf("Intercepted execution (redirected): %s\n", __func__);
    return result;
}
//==================================================================================================================
//* methods needed to process the const char*
extern "C" CUresult CUDAAPI cuModuleLoad(CUmodule* cu_module, const char* fname){
    printf("Intercepted execution (redirected): %s\n", __func__);
    std::string source_file = std::string(SOURCE_DIR) + "/vector_add.ptx";
    std::ifstream ptx_file(source_file);
    client.CallCudaFunction(__func__, cu_module, fname);
    func_proto.add_module(ptx_file);
    ptx_file.close();
    CUresult result = client.wait_recv(cu_module);
    return result;
}
//==================================================================================================================
//* methods needed to process the const char*
extern "C" CUresult CUDAAPI cuModuleGetFunction(CUfunction* hfunc, CUmodule hmod, const char* name){
    printf("Intercepted execution (redirected): %s\n", __func__);
    client.CallCudaFunction(__func__, hfunc, hmod, name);
    CUresult result = client.wait_recv();
    *hfunc = (CUfunction) client.get_scalar_result();
    hashfunc[*hfunc] = name;
    return result;
    return CUDA_SUCCESS;
}
//==================================================================================================================
//FIXME how to deal with the `extra`? where is it used? =
extern "C" CUresult CUDAAPI cuLaunchKernel(CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ,
                                           unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ,
                                           unsigned int sharedMemBytes, CUstream hStream,
                                           void** kernelParams, void** extra){
    /******************************************
     *    Set parameters for the cuda func    *
     ******************************************/
    if(!hashfunc.count(f)) std::cout << "cuFunc " << f << " not found!\n" << std::endl;
    std::vector<std::string> &param_type = func_proto.get_params(hashfunc[f]);
    int param_count = param_type.size();

    std::vector<std::uint64_t> scalar_args(param_count, 0);
    for(int i = 0; i < param_count; i++){
        //TODO temp impl to be updated
        //// if(param_type[i] == ".u32") scalar_args.push_back(*reinterpret_cast<uint32_t*>(kernelParams[i]));
        //// if(param_type[i] == ".u64") scalar_args.push_back(*reinterpret_cast<uint64_t*>(kernelParams[i]));
        scalar_args[i] = *reinterpret_cast<std::uint64_t*>(kernelParams[i]);
    }

    std::cout << "calling function " << f << std::endl;
    client.CallCudaFunction(__func__, f, gridDimX, gridDimY, gridDimZ,
                            blockDimX, blockDimY, blockDimZ,
                            sharedMemBytes, hStream, kernelParams, extra, scalar_args);
    CUresult result = client.wait_recv();

    printf("Intercepted execution (redirected): %s\n", __func__);
    return result;
}
//==================================================================================================================
/* Interception version for `cuGetProcAddress` and all needed CUDA funcs */
// extern "C" CUresult getProcAddressBySymbol(const char* symbol, void** pfn, int cudaVersion, cuuint64_t flags,
//                                             CUdriverProcAddressQueryResult* symbolStatus) {
//   // printf("Intercepted cuGetProcAddress: symbol=%s\n", symbol);
//   if(strcmp(symbol, "cuGetProcAddress") == 0){
//   *pfn = (void *) &getProcAddressBySymbol;
//   return CUDA_SUCCESS;
//   }
//   TRY_INTERCEPT("cuInit", cuInit)
//   TRY_INTERCEPT("cuDriverGetVersion", cuDriverGetVersion)
//   TRY_INTERCEPT("cuGetExportTable", cuGetExportTable)
//   TRY_INTERCEPT("cuModuleGetLoadingMode", cuModuleGetLoadingMode)
//   TRY_INTERCEPT("cuDeviceGetCount", cuDeviceGetCount)
//   TRY_INTERCEPT("cuDeviceGet", cuDeviceGet)
//   TRY_INTERCEPT("cuDeviceGetName", cuDeviceGetName)
//   TRY_INTERCEPT("cuDeviceTotalMem", cuDeviceTotalMem)
//   TRY_INTERCEPT("cuDeviceGetAttribute", cuDeviceGetAttribute)
//   TRY_INTERCEPT("cuDeviceGetUuid", cuDeviceGetUuid)
//   TRY_INTERCEPT("cuCtxGetDevice", cuCtxGetDevice)
//   TRY_INTERCEPT("cuCtxGetCurrent", cuCtxGetCurrent)
//   TRY_INTERCEPT("cuCtxSetCurrent", cuCtxSetCurrent)
//   TRY_INTERCEPT("cuDevicePrimaryCtxRetain", cuDevicePrimaryCtxRetain)
//   TRY_INTERCEPT("cuCtxGetStreamPriorityRange", cuCtxGetStreamPriorityRange)
//   TRY_INTERCEPT("cuStreamIsCapturing", cuStreamIsCapturing)
//   TRY_INTERCEPT("cuMemAlloc", cuMemAlloc)
//   TRY_INTERCEPT("cuMemcpyHtoDAsync", cuMemcpyHtoDAsync)
//   TRY_INTERCEPT("cuStreamSynchronize", cuStreamSynchronize)
//   TRY_INTERCEPT("cuStreamCreate", cuStreamCreate)
//   TRY_INTERCEPT("cuMemsetD8Async", cuMemsetD8Async)
//   TRY_INTERCEPT("cuEventCreate", cuEventCreate)
//   TRY_INTERCEPT("cuMemHostAlloc", cuMemHostAlloc)
//   TRY_INTERCEPT("cuMemHostGetDevicePointer", cuMemHostGetDevicePointer)
//   TRY_INTERCEPT("cuMemFree", cuMemFree)
//   TRY_INTERCEPT("cuEventRecord", cuEventRecord)
//   TRY_INTERCEPT("cuStreamGetPriority", cuStreamGetPriority)
//   TRY_INTERCEPT("cuCtxPushCurrent", cuCtxPushCurrent)
//   TRY_INTERCEPT("cuLibraryGetModule", cuLibraryGetModule)
//   TRY_INTERCEPT("cuCtxPopCurrent", cuCtxPopCurrent)
//   TRY_INTERCEPT("cuModuleGetFunction", cuModuleGetFunction)
//   TRY_INTERCEPT("cuLaunchKernel", cuLaunchKernel)
//   TRY_INTERCEPT("cuModuleGetGlobal", cuModuleGetGlobal)
//   TRY_INTERCEPT("cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags", cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags)
//   TRY_INTERCEPT("cuLibraryUnload", cuLibraryUnload)
//   TRY_INTERCEPT("cuDevicePrimaryCtxRelease", cuDevicePrimaryCtxRelease)

//   // If no need to intercept, call the corresponding CUDA function directly
//   CUresult result = cuGetProcAddress_v2(symbol, pfn, cudaVersion, flags, symbolStatus);

//   return result;
// }
//==================================================================================================================

//==================================================================================================================
// #define CU_HOOK_DRIVER_FUNC(symbol, params, ...) \
//   extern "C" CUresult CUDAAPI symbol params {   \
//     using symbol##handler = CUresult CUDAAPI (params);  \
//     auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, CUDA_SYMBOL_STRING(symbol)); \
//     printf("Intercepted: %s\n", STRINGIFY(symbol)); \
//     CUresult result = real_func(__VA_ARGS__); \
//     return result;  \
//   }

// CU_HOOK_DRIVER_FUNC(cuInit,
//                     (unsigned int Flags),
//                     Flags)
// CU_HOOK_DRIVER_FUNC(cuDeviceGet,
//                     (CUdevice* device, int ordinal),
//                     device, ordinal)
// CU_HOOK_DRIVER_FUNC(cuCtxCreate,
//                     (CUcontext* pctx, unsigned int flags, CUdevice dev),
//                     pctx, flags, dev)
// CU_HOOK_DRIVER_FUNC(cuMemAlloc,
//                     (CUdeviceptr* dptr, size_t bytesize),
//                     dptr, bytesize)
// CU_HOOK_DRIVER_FUNC(cuMemcpyHtoD,
//                     (CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount),
//                     dstDevice, srcHost, ByteCount)
// CU_HOOK_DRIVER_FUNC(cuModuleLoad,
//                     (CUmodule* module, const char* fname),
//                     module, fname)
// CU_HOOK_DRIVER_FUNC(cuModuleGetFunction,
//                     (CUfunction* hfunc, CUmodule hmod, const char* name),
//                     hfunc, hmod, name)
// CU_HOOK_DRIVER_FUNC(cuLaunchKernel,
//                     (CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ,
//                      unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ,
//                      unsigned int sharedMemBytes, CUstream hStream,
//                      void** kernelParams, void** extra),
//                     f, gridDimX, gridDimY, gridDimZ,
//                     blockDimX, blockDimY, blockDimZ,
//                     sharedMemBytes, hStream,
//                     kernelParams, extra)
// CU_HOOK_DRIVER_FUNC(cuMemcpyDtoH,
//                     (void* dstHost, CUdeviceptr srcDevice, size_t ByteCount)
//                     dstHost, srcDevice, ByteCount)
// CU_HOOK_DRIVER_FUNC(cuMemFree,
//                     (CUdeviceptr dptr),
//                     dptr)
// CU_HOOK_DRIVER_FUNC(cuModuleUnload,
//                     (CUmodule hmod),
//                     hmod)
// CU_HOOK_DRIVER_FUNC(cuCtxDestroy,
//                     (CUcontext ctx),
//                     ctx)

// CU_HOOK_DRIVER_FUNC(cuInit,
//                     (unsigned int Flags),
//                     Flags)

// CU_HOOK_DRIVER_FUNC(cuDriverGetVersion,
//                     (int *driverVersion),
//                     driverVersion)

// CU_HOOK_DRIVER_FUNC(cuDeviceGet,
//                     (CUdevice *device, int ordinal),
//                     device, ordinal)

// CU_HOOK_DRIVER_FUNC(cuDeviceGetCount,
//                     (int *count),
//                     count)

// CU_HOOK_DRIVER_FUNC(cuDeviceGetName,
//                     (char *name, int len, CUdevice device),
//                     name, len, device)

// CU_HOOK_DRIVER_FUNC(cuDeviceGetProperties,
//                     (CUdevprop *prop, CUdevice device),
//                     prop, device)

// CU_HOOK_DRIVER_FUNC(cuCtxCreate,
//                     (CUcontext *pctx, unsigned int flags, CUdevice dev),
//                     pctx, flags, dev)

// CU_HOOK_DRIVER_FUNC(cuCtxDestroy,
//                     (CUcontext ctx),
//                     ctx)

// CU_HOOK_DRIVER_FUNC(cuCtxPushCurrent,
//                     (CUcontext ctx),
//                     ctx)

// CU_HOOK_DRIVER_FUNC(cuCtxPopCurrent,
//                     (CUcontext *pctx),
//                     pctx)

// CU_HOOK_DRIVER_FUNC(cuCtxGetDevice,
//                     (CUdevice *device),
//                     device)

// CU_HOOK_DRIVER_FUNC(cuMemAlloc,
//                     (CUdeviceptr *dptr, size_t size),
//                     dptr, size)

// CU_HOOK_DRIVER_FUNC(cuMemFree,
//                     (CUdeviceptr dptr),
//                     dptr)

// CU_HOOK_DRIVER_FUNC(cuMemAllocHost,
//                     (void **pp, size_t bytesize),
//                     pp, bytesize)

// CU_HOOK_DRIVER_FUNC(cuMemFreeHost,
//                     (void *p),
//                     p)

// CU_HOOK_DRIVER_FUNC(cuMemHostRegister,
//                     (void *p, size_t bytesize, unsigned int flags),
//                     p, bytesize, flags)

// CU_HOOK_DRIVER_FUNC(cuMemHostUnregister,
//                     (void *p),
//                     p)

// CU_HOOK_DRIVER_FUNC(cuStreamCreate,
//                     (CUstream *phStream, unsigned int flags),
//                     phStream, flags)

// CU_HOOK_DRIVER_FUNC(cuStreamDestroy,
//                     (CUstream hStream),
//                     hStream)

// CU_HOOK_DRIVER_FUNC(cuStreamSynchronize,
//                     (CUstream hStream),
//                     hStream)

// CU_HOOK_DRIVER_FUNC(cuStreamQuery,
//                     (CUstream hStream),
//                     hStream)

// CU_HOOK_DRIVER_FUNC(cuStreamAddCallback,
//                     (CUstream hStream, CUstreamCallback callback, void *userData, unsigned int flags),
//                     hStream, callback, userData, flags)

// CU_HOOK_DRIVER_FUNC(cuModuleLoad,
//                     (CUmodule *module, const char *fname),
//                     module, fname)

// CU_HOOK_DRIVER_FUNC(cuModuleUnload,
//                     (CUmodule hModule),
//                     hModule)

// CU_HOOK_DRIVER_FUNC(cuModuleGetFunction,
//                     (CUfunction *hfunc, CUmodule hModule, const char *name),
//                     hfunc, hModule, name)

// CU_HOOK_DRIVER_FUNC(cuModuleGetGlobal,
//                     (CUdeviceptr *dptr, size_t *bytes, CUmodule hModule, const char *name),
//                     dptr, bytes, hModule, name)

// CU_HOOK_DRIVER_FUNC(cuLaunchKernel,
//                     (CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ,
//                      unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ,
//                      unsigned int sharedMemBytes, CUstream hStream, void **kernelParams, void **extra),
//                     f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, hStream, kernelParams, extra)

// CU_HOOK_DRIVER_FUNC(cuFuncGetAttributes,
//                     (CUfunction_attribute *attr, CUfunction hfunc),
//                     attr, hfunc)

// CU_HOOK_DRIVER_FUNC(cuGetErrorString,
//                     (CUresult error, const char **pStr),
//                     error, pStr)

// CU_HOOK_DRIVER_FUNC(cuGetErrorName,
//                     (CUresult error, const char **pStr),
//                     error, pStr)

// CU_HOOK_DRIVER_FUNC(cuEventCreate,
//                     (CUevent *phEvent, unsigned int flags),
//                     phEvent, flags)

// CU_HOOK_DRIVER_FUNC(cuEventRecord,
//                     (CUevent hEvent, CUstream hStream),
//                     hEvent, hStream)

// CU_HOOK_DRIVER_FUNC(cuEventSynchronize,
//                     (CUevent hEvent),
//                     hEvent)

// CU_HOOK_DRIVER_FUNC(cuEventQuery,
//                     (CUevent hEvent),
//                     hEvent)

// CU_HOOK_DRIVER_FUNC(cuEventDestroy,
//                     (CUevent hEvent),
//                     hEvent)

// CU_HOOK_DRIVER_FUNC(cuCtxSynchronize,
//                     (),
//                     )

// CU_HOOK_DRIVER_FUNC(cuMemcpyHtoD,
//                     (CUdeviceptr dstDevice, const void *srcHost, size_t ByteCount),
//                     dstDevice, srcHost, ByteCount)

// CU_HOOK_DRIVER_FUNC(cuMemcpyDtoH,
//                     (void *dstHost, CUdeviceptr srcDevice, size_t ByteCount),
//                     dstHost, srcDevice, ByteCount)

// CU_HOOK_DRIVER_FUNC(cuMemcpyDtoD,
//                     (CUdeviceptr dstDevice, CUdeviceptr srcDevice, size_t ByteCount),
//                     dstDevice, srcDevice, ByteCount)

// CU_HOOK_DRIVER_FUNC(cuOccupancyMaxActiveBlocksPerMultiprocessor,
//                     (int *numBlocks, CUfunction hfunc, int blockSize, int dynamicSMemSize),
//                     numBlocks, hfunc, blockSize, dynamicSMemSize)

// CU_HOOK_DRIVER_FUNC(cuProfilerStart,
//                     (),
//                     )

// CU_HOOK_DRIVER_FUNC(cuProfilerStop,
//                     (),
//                     )

// CU_HOOK_DRIVER_FUNC(cuGraphCreate,
//                     (CUgraph *phGraph, unsigned int flags),
//                     phGraph, flags)

// CU_HOOK_DRIVER_FUNC(cuGraphAddKernelNode,
//                     (CUgraphNode *phNode, CUgraph hGraph, CUgraphNode *dependencies, unsigned int numDependencies, 
//                      CUfunction hfunc, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, 
//                      unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, 
//                      void **kernelParams),
//                     phNode, hGraph, dependencies, numDependencies, hfunc, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, kernelParams)

// CU_HOOK_DRIVER_FUNC(cuGraphLaunch,
//                     (CUgraph hGraph, CUstream hStream),
//                     hGraph, hStream)

// CU_HOOK_DRIVER_FUNC(cuGraphDestroy,
//                     (CUgraph hGraph),
//                     hGraph)

// CU_HOOK_DRIVER_FUNC(cuDeviceTotalMem,
//                     (size_t *mem, CUdevice dev),
//                     mem, dev)

// CU_HOOK_DRIVER_FUNC(cuDeviceGetAttribute,
//                     (int *pi, CUdevice_attribute attrib, CUdevice dev),
//                     pi, attrib, dev)

// CU_HOOK_DRIVER_FUNC(cuDeviceGetPCIBusId,
//                     (char *pciBusId, int len, CUdevice dev),
//                     pciBusId, len, dev)

// CU_HOOK_DRIVER_FUNC(cuDeviceGetUuid,
//                     (CUuuid *uuid, CUdevice dev),
//                     uuid, dev)

// CU_HOOK_DRIVER_FUNC(cuCtxSetCurrent,
//                     (CUcontext ctx),
//                     ctx)

// CU_HOOK_DRIVER_FUNC(cuCtxGetCurrent,
//                     (CUcontext *pctx),
//                     pctx)

// CU_HOOK_DRIVER_FUNC(cuMemAlloc_v2,
//                     (CUdeviceptr *dptr, size_t bytesize),
//                     dptr, bytesize)

// CU_HOOK_DRIVER_FUNC(cuMemFree_v2,
//                     (CUdeviceptr dptr),
//                     dptr)

// CU_HOOK_DRIVER_FUNC(cuMemGetInfo,
//                     (size_t *freeMem, size_t *totalMem),
//                     freeMem, totalMem)

// CU_HOOK_DRIVER_FUNC(cuDriverGetVersion,
//                     (int *driverVersion),
//                     driverVersion)

// CU_HOOK_DRIVER_FUNC(cuMemAllocManaged,
//                     (CUdeviceptr *dptr, size_t bytesize, unsigned int flags),
//                     dptr, bytesize, flags)

// CU_HOOK_DRIVER_FUNC(cuMemAllocPitch,
//                     (CUdeviceptr *dptr, size_t *pPitch, size_t WidthInBytes, size_t Height, unsigned int ElementSizeBytes),
//                     dptr, pPitch, WidthInBytes, Height, ElementSizeBytes)

// CU_HOOK_DRIVER_FUNC(cuMemFreeHost,
//                     (void *p),
//                     p)

// CU_HOOK_DRIVER_FUNC(cuCtxEnablePeerAccess,
//                     (CUcontext peerCtx, unsigned int flags),
//                     peerCtx, flags)

// CU_HOOK_DRIVER_FUNC(cuCtxDisablePeerAccess,
//                     (CUcontext peerCtx),
//                     peerCtx)

// CU_HOOK_DRIVER_FUNC(cuDeviceCanAccessPeer,
//                     (int *canAccessPeer, CUdevice dev, CUdevice peerDev),
//                     canAccessPeer, dev, peerDev)

// CU_HOOK_DRIVER_FUNC(cuMemAttach,
//                     (CUmemAttachType attachFlags),
//                     attachFlags)

// CU_HOOK_DRIVER_FUNC(cuGLGetDevices,
//                     (int *count, CUdevice *devices, int maxDevices),
//                     count, devices, maxDevices)

// CU_HOOK_DRIVER_FUNC(cuGLInit,
//                     (),
//                     )

// CU_HOOK_DRIVER_FUNC(cuGraphicsResourceGetMappedPointer,
//                     (CUdeviceptr *dptr, size_t *size, CUgraphicsResource resource),
//                     dptr, size, resource)

// CU_HOOK_DRIVER_FUNC(cuGraphicsMapResources,
//                     (unsigned int count, CUgraphicsResource *resources, CUstream hStream),
//                     count, resources, hStream)

// CU_HOOK_DRIVER_FUNC(cuGraphicsUnmapResources,
//                     (unsigned int count, CUgraphicsResource *resources, CUstream hStream),
//                     count, resources, hStream)

// CU_HOOK_DRIVER_FUNC(cuTexRefCreate,
//                     (CUtexref *pTexRef),
//                     pTexRef)

// CU_HOOK_DRIVER_FUNC(cuTexRefDestroy,
//                     (CUtexref hTexRef),
//                     hTexRef)

// CU_HOOK_DRIVER_FUNC(cuTexRefSetFilterMode,
//                     (CUtexref hTexRef, CUfilter_mode mode),
//                     hTexRef, mode)

// CU_HOOK_DRIVER_FUNC(cuTexRefSetAddress,
//                     (CUtexref hTexRef, size_t *ByteOffset, CUdeviceptr dptr),
//                     hTexRef, ByteOffset, dptr)

// CU_HOOK_DRIVER_FUNC(cuArrayCreate,
//                     (CUarray *pArray, const CUDA_ARRAY_DESCRIPTOR *pAllocateArray),
//                     pArray, pAllocateArray)

// CU_HOOK_DRIVER_FUNC(cuArrayDestroy,
//                     (CUarray hArray),
//                     hArray)

// CU_HOOK_DRIVER_FUNC(cuArrayGetDescriptor,
//                     (CUDA_ARRAY_DESCRIPTOR *pArrayDescriptor, CUarray hArray),
//                     pArrayDescriptor, hArray)

#pragma region hook function
CU_HOOK_DRIVER_FUNC(cuInit_intercepted, cuInit, (unsigned int Flags), Flags)

CU_HOOK_DRIVER_FUNC(cuDeviceGet_intercepted, cuDeviceGet, (CUdevice *device, int ordinal), device, ordinal)

CU_HOOK_DRIVER_FUNC(cuDeviceGetCount_intercepted, cuDeviceGetCount, (int *count), count)

CU_HOOK_DRIVER_FUNC(cuDeviceGetName_intercepted, cuDeviceGetName, (char *name, int len, CUdevice dev), name, len, dev)

CU_HOOK_DRIVER_FUNC(cuDeviceTotalMem_intercepted, cuDeviceTotalMem, (unsigned int *bytes, CUdevice dev), bytes, dev)

CU_HOOK_DRIVER_FUNC(cuDeviceGetAttribute_intercepted, cuDeviceGetAttribute, (int *pi, CUdevice_attribute attrib, CUdevice dev), pi, attrib, dev)

CU_HOOK_DRIVER_FUNC(cuDeviceGetP2PAttribute_intercepted, cuDeviceGetP2PAttribute, (int* value, CUdevice_P2PAttribute attrib, CUdevice srcDevice, CUdevice dstDevice), value, attrib, srcDevice, dstDevice)

CU_HOOK_DRIVER_FUNC(cuDriverGetVersion_intercepted, cuDriverGetVersion, (int *driverVersion), driverVersion)

CU_HOOK_DRIVER_FUNC(cuDeviceGetByPCIBusId_intercepted, cuDeviceGetByPCIBusId, (CUdevice *dev, const char *pciBusId), dev, pciBusId)

CU_HOOK_DRIVER_FUNC(cuDeviceGetPCIBusId_intercepted, cuDeviceGetPCIBusId, (char *pciBusId, int len, CUdevice dev), pciBusId, len, dev)

CU_HOOK_DRIVER_FUNC(cuDeviceGetUuid_intercepted, cuDeviceGetUuid, (CUuuid *uuid, CUdevice dev), uuid, dev)

CU_HOOK_DRIVER_FUNC(cuDeviceGetTexture1DLinearMaxWidth_intercepted, cuDeviceGetTexture1DLinearMaxWidth, (size_t *maxWidthInElements, CUarray_format format, unsigned numChannels, CUdevice dev), maxWidthInElements, format, numChannels, dev)

CU_HOOK_DRIVER_FUNC(cuDeviceGetDefaultMemPool_intercepted, cuDeviceGetDefaultMemPool, (CUmemoryPool *pool_out, CUdevice dev), pool_out, dev)

CU_HOOK_DRIVER_FUNC(cuDeviceSetMemPool_intercepted, cuDeviceSetMemPool, (CUdevice dev, CUmemoryPool pool), dev, pool)

CU_HOOK_DRIVER_FUNC(cuDeviceGetMemPool_intercepted, cuDeviceGetMemPool, (CUmemoryPool *pool, CUdevice dev), pool, dev)

CU_HOOK_DRIVER_FUNC(cuFlushGPUDirectRDMAWrites_intercepted, cuFlushGPUDirectRDMAWrites, (CUflushGPUDirectRDMAWritesTarget target, CUflushGPUDirectRDMAWritesScope scope), target, scope)

CU_HOOK_DRIVER_FUNC(cuDevicePrimaryCtxRetain_intercepted, cuDevicePrimaryCtxRetain, (CUcontext *pctx, CUdevice dev), pctx, dev)

CU_HOOK_DRIVER_FUNC(cuDevicePrimaryCtxRelease_intercepted, cuDevicePrimaryCtxRelease, (CUdevice dev), dev)

CU_HOOK_DRIVER_FUNC(cuDevicePrimaryCtxSetFlags_intercepted, cuDevicePrimaryCtxSetFlags, (CUdevice dev, unsigned int flags), dev, flags)

CU_HOOK_DRIVER_FUNC(cuDevicePrimaryCtxGetState_intercepted, cuDevicePrimaryCtxGetState, (CUdevice dev, unsigned int *flags, int *active), dev, flags, active)

CU_HOOK_DRIVER_FUNC(cuDevicePrimaryCtxReset_intercepted, cuDevicePrimaryCtxReset, (CUdevice dev), dev)

CU_HOOK_DRIVER_FUNC(cuCtxCreate_intercepted, cuCtxCreate, (CUcontext *pctx, unsigned int flags, CUdevice dev), pctx, flags, dev)

CU_HOOK_DRIVER_FUNC(cuCtxGetFlags_intercepted, cuCtxGetFlags, (unsigned int *flags), flags)

CU_HOOK_DRIVER_FUNC(cuCtxSetCurrent_intercepted, cuCtxSetCurrent, (CUcontext ctx), ctx)

CU_HOOK_DRIVER_FUNC(cuCtxGetCurrent_intercepted, cuCtxGetCurrent, (CUcontext *pctx), pctx)

CU_HOOK_DRIVER_FUNC(cuCtxDetach_intercepted, cuCtxDetach, (CUcontext ctx), ctx)

CU_HOOK_DRIVER_FUNC(cuCtxGetApiVersion_intercepted, cuCtxGetApiVersion, (CUcontext ctx, unsigned int *version), ctx, version)

CU_HOOK_DRIVER_FUNC(cuCtxGetDevice_intercepted, cuCtxGetDevice, (CUdevice *device), device)

CU_HOOK_DRIVER_FUNC(cuCtxGetLimit_intercepted, cuCtxGetLimit, (size_t *pvalue, CUlimit limit), pvalue, limit)

CU_HOOK_DRIVER_FUNC(cuCtxSetLimit_intercepted, cuCtxSetLimit, (CUlimit limit, size_t value), limit, value)

CU_HOOK_DRIVER_FUNC(cuCtxGetCacheConfig_intercepted, cuCtxGetCacheConfig, (CUfunc_cache *pconfig), pconfig)

CU_HOOK_DRIVER_FUNC(cuCtxSetCacheConfig_intercepted, cuCtxSetCacheConfig, (CUfunc_cache config), config)

CU_HOOK_DRIVER_FUNC(cuCtxGetSharedMemConfig_intercepted, cuCtxGetSharedMemConfig, (CUsharedconfig *pConfig), pConfig)

CU_HOOK_DRIVER_FUNC(cuCtxGetStreamPriorityRange_intercepted, cuCtxGetStreamPriorityRange, (int *leastPriority, int *greatestPriority), leastPriority, greatestPriority)

CU_HOOK_DRIVER_FUNC(cuCtxSetSharedMemConfig_intercepted, cuCtxSetSharedMemConfig, (CUsharedconfig config), config)

CU_HOOK_DRIVER_FUNC(cuCtxSynchronize_intercepted, cuCtxSynchronize, (), )

CU_HOOK_DRIVER_FUNC(cuCtxResetPersistingL2Cache_intercepted, cuCtxResetPersistingL2Cache, (), )

CU_HOOK_DRIVER_FUNC(cuCtxPopCurrent_intercepted, cuCtxPopCurrent, (CUcontext *pctx), pctx)

CU_HOOK_DRIVER_FUNC(cuCtxPushCurrent_intercepted, cuCtxPushCurrent, (CUcontext ctx), ctx)

CU_HOOK_DRIVER_FUNC(cuModuleLoad_intercepted, cuModuleLoad, (CUmodule *module, const char *fname), module, fname)

// CU_HOOK_DRIVER_FUNC(cuModuleLoadData_intercepted, cuModuleLoadData, (CUmodule *module, const void *image), module, image)

CU_HOOK_DRIVER_FUNC(cuModuleLoadFatBinary_intercepted, cuModuleLoadFatBinary, (CUmodule *module, const void *fatCubin), module, fatCubin)

CU_HOOK_DRIVER_FUNC(cuModuleUnload_intercepted, cuModuleUnload, (CUmodule hmod), hmod)

// CU_HOOK_DRIVER_FUNC(cuModuleGetFunction_intercepted, cuModuleGetFunction, (CUfunction *hfunc, CUmodule hmod, const char *name), hfunc, hmod, name)

CU_HOOK_DRIVER_FUNC(cuModuleGetGlobal_intercepted, cuModuleGetGlobal, (CUdeviceptr *dptr, unsigned int *bytes, CUmodule hmod, const char *name), dptr, bytes, hmod, name)

CU_HOOK_DRIVER_FUNC(cuModuleGetTexRef_intercepted, cuModuleGetTexRef, (CUtexref *pTexRef, CUmodule hmod, const char *name), pTexRef, hmod, name)

CU_HOOK_DRIVER_FUNC(cuModuleGetSurfRef_intercepted, cuModuleGetSurfRef, (CUsurfref *pSurfRef, CUmodule hmod, const char *name), pSurfRef, hmod, name)

CU_HOOK_DRIVER_FUNC(cuModuleGetLoadingMode_intercepted, cuModuleGetLoadingMode, (CUmoduleLoadingMode *mode), mode)

CU_HOOK_DRIVER_FUNC(cuLibraryUnload_intercepted, cuLibraryUnload, (CUlibrary library), library)

CU_HOOK_DRIVER_FUNC(cuLibraryGetKernel_intercepted, cuLibraryGetKernel, (CUkernel *pKernel, CUlibrary library, const char *name), pKernel, library, name)

CU_HOOK_DRIVER_FUNC(cuLibraryGetModule_intercepted, cuLibraryGetModule, (CUmodule *pMod, CUlibrary library), pMod, library)

CU_HOOK_DRIVER_FUNC(cuKernelGetFunction_intercepted, cuKernelGetFunction, (CUfunction *pFunc, CUkernel kernel), pFunc, kernel)

CU_HOOK_DRIVER_FUNC(cuLibraryGetGlobal_intercepted, cuLibraryGetGlobal, (CUdeviceptr *dptr, size_t *bytes, CUlibrary library, const char *name), dptr, bytes, library, name)

CU_HOOK_DRIVER_FUNC(cuLibraryGetManaged_intercepted, cuLibraryGetManaged, (CUdeviceptr *dptr, size_t *bytes, CUlibrary library, const char *name), dptr, bytes, library, name)

CU_HOOK_DRIVER_FUNC(cuLibraryGetUnifiedFunction_intercepted, cuLibraryGetUnifiedFunction, (void **fptr, CUlibrary library, const char *symbol), fptr, library, symbol)

CU_HOOK_DRIVER_FUNC(cuLibraryGetKernelCount_intercepted, cuLibraryGetKernelCount, (unsigned int *count, CUlibrary lib), count, lib)

CU_HOOK_DRIVER_FUNC(cuLibraryEnumerateKernels_intercepted, cuLibraryEnumerateKernels, (CUkernel *kernels, unsigned int numKernels, CUlibrary lib), kernels, numKernels, lib)

CU_HOOK_DRIVER_FUNC(cuKernelGetAttribute_intercepted, cuKernelGetAttribute, (int *pi, CUfunction_attribute attrib, CUkernel kernel, CUdevice dev), pi, attrib, kernel, dev)

CU_HOOK_DRIVER_FUNC(cuKernelSetAttribute_intercepted, cuKernelSetAttribute, (CUfunction_attribute attrib, int val, CUkernel kernel, CUdevice dev), attrib, val, kernel, dev)

CU_HOOK_DRIVER_FUNC(cuKernelSetCacheConfig_intercepted, cuKernelSetCacheConfig, (CUkernel kernel, CUfunc_cache config, CUdevice dev), kernel, config, dev)

CU_HOOK_DRIVER_FUNC(cuKernelGetName_intercepted, cuKernelGetName, (const char **name, CUkernel hfunc), name, hfunc)

CU_HOOK_DRIVER_FUNC(cuKernelGetParamInfo_intercepted, cuKernelGetParamInfo, (CUkernel kernel, size_t paramIndex, size_t *paramOffset, size_t *paramSize), kernel, paramIndex, paramOffset, paramSize)

CU_HOOK_DRIVER_FUNC(cuLinkCreate_intercepted, cuLinkCreate, (unsigned int numOptions, CUjit_option *options, void **optionValues, CUlinkState *stateOut), numOptions, options, optionValues, stateOut)

CU_HOOK_DRIVER_FUNC(cuMemGetInfo_intercepted, cuMemGetInfo, (unsigned int *free, unsigned int *total), free, total)

CU_HOOK_DRIVER_FUNC(cuMemAllocManaged_intercepted, cuMemAllocManaged, (CUdeviceptr *dptr, size_t bytesize, unsigned int flags), dptr, bytesize, flags)

CU_HOOK_DRIVER_FUNC(cuMemAlloc_intercepted, cuMemAlloc, (CUdeviceptr *dptr, unsigned int bytesize), dptr, bytesize)

CU_HOOK_DRIVER_FUNC(cuMemAllocPitch_intercepted, cuMemAllocPitch, (CUdeviceptr *dptr, unsigned int *pPitch, unsigned int WidthInBytes, unsigned int Height, unsigned int ElementSizeBytes), dptr, pPitch, WidthInBytes, Height, ElementSizeBytes)

CU_HOOK_DRIVER_FUNC(cuMemFree_intercepted, cuMemFree, (CUdeviceptr dptr), dptr)

CU_HOOK_DRIVER_FUNC(cuMemGetAddressRange_intercepted, cuMemGetAddressRange, (CUdeviceptr *pbase, unsigned int *psize, CUdeviceptr dptr), pbase, psize, dptr)

CU_HOOK_DRIVER_FUNC(cuMemFreeHost_intercepted, cuMemFreeHost, (void *p), p)

CU_HOOK_DRIVER_FUNC(cuMemHostAlloc_intercepted, cuMemHostAlloc, (void **pp, size_t bytesize, unsigned int Flags), pp, bytesize, Flags)

CU_HOOK_DRIVER_FUNC(cuMemHostGetDevicePointer_intercepted, cuMemHostGetDevicePointer, (CUdeviceptr *pdptr, void *p, unsigned int Flags), pdptr, p, Flags)

CU_HOOK_DRIVER_FUNC(cuMemHostGetFlags_intercepted, cuMemHostGetFlags, (unsigned int *pFlags, void *p), pFlags, p)

CU_HOOK_DRIVER_FUNC(cuMemHostRegister_intercepted, cuMemHostRegister, (void *p, size_t bytesize, unsigned int Flags), p, bytesize, Flags)

CU_HOOK_DRIVER_FUNC(cuMemHostUnregister_intercepted, cuMemHostUnregister, (void *p), p)

CU_HOOK_DRIVER_FUNC(cuPointerGetAttribute_intercepted, cuPointerGetAttribute, (void *data, CUpointer_attribute attribute, CUdeviceptr ptr), data, attribute, ptr)

CU_HOOK_DRIVER_FUNC(cuPointerGetAttributes_intercepted, cuPointerGetAttributes, (unsigned int numAttributes, CUpointer_attribute *attributes, void **data, CUdeviceptr ptr), numAttributes, attributes, data, ptr)

CU_HOOK_DRIVER_FUNC(cuMemAllocAsync_intercepted, cuMemAllocAsync, (CUdeviceptr *dptr, size_t bytesize, CUstream hStream), dptr, bytesize, hStream)

CU_HOOK_DRIVER_FUNC(cuMemAllocFromPoolAsync_intercepted, cuMemAllocFromPoolAsync, (CUdeviceptr *dptr, size_t bytesize, CUmemoryPool pool, CUstream hStream), dptr, bytesize, pool, hStream)

CU_HOOK_DRIVER_FUNC(cuMemFreeAsync_intercepted, cuMemFreeAsync, (CUdeviceptr dptr, CUstream hStream), dptr, hStream)

CU_HOOK_DRIVER_FUNC(cuMemPoolTrimTo_intercepted, cuMemPoolTrimTo, (CUmemoryPool pool, size_t minBytesToKeep), pool, minBytesToKeep)

CU_HOOK_DRIVER_FUNC(cuMemPoolSetAttribute_intercepted, cuMemPoolSetAttribute, (CUmemoryPool pool, CUmemPool_attribute attr, void *value), pool, attr, value)

CU_HOOK_DRIVER_FUNC(cuMemPoolGetAttribute_intercepted, cuMemPoolGetAttribute, (CUmemoryPool pool, CUmemPool_attribute attr, void *value), pool, attr, value)

CU_HOOK_DRIVER_FUNC(cuMemPoolSetAccess_intercepted, cuMemPoolSetAccess, (CUmemoryPool pool, const CUmemAccessDesc *map, size_t count), pool, map, count)

CU_HOOK_DRIVER_FUNC(cuMemPoolGetAccess_intercepted, cuMemPoolGetAccess, (CUmemAccess_flags *flags, CUmemoryPool memPool, CUmemLocation *location), flags, memPool, location)

CU_HOOK_DRIVER_FUNC(cuMemPoolCreate_intercepted, cuMemPoolCreate, (CUmemoryPool *pool, const CUmemPoolProps *poolProps), pool, poolProps)

CU_HOOK_DRIVER_FUNC(cuMemPoolDestroy_intercepted, cuMemPoolDestroy, (CUmemoryPool pool), pool)

CU_HOOK_DRIVER_FUNC(cuMemPoolExportToShareableHandle_intercepted, cuMemPoolExportToShareableHandle, (void *handle_out, CUmemoryPool pool, CUmemAllocationHandleType handleType, unsigned long long flags), handle_out, pool, handleType, flags)

CU_HOOK_DRIVER_FUNC(cuMemPoolExportPointer_intercepted, cuMemPoolExportPointer, (CUmemPoolPtrExportData *shareData_out, CUdeviceptr ptr), shareData_out, ptr)

CU_HOOK_DRIVER_FUNC(cuMemPoolImportPointer_intercepted, cuMemPoolImportPointer, (CUdeviceptr *ptr_out, CUmemoryPool pool, CUmemPoolPtrExportData *shareData), ptr_out, pool, shareData)

CU_HOOK_DRIVER_FUNC(cuMemcpy_intercepted, cuMemcpy, (CUdeviceptr dst, CUdeviceptr src, size_t ByteCount), dst, src, ByteCount)

CU_HOOK_DRIVER_FUNC(cuMemcpyAsync_intercepted, cuMemcpyAsync, (CUdeviceptr dst, CUdeviceptr src, size_t ByteCount, CUstream hStream), dst, src, ByteCount, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpyPeer_intercepted, cuMemcpyPeer, (CUdeviceptr dstDevice, CUcontext dstContext, CUdeviceptr srcDevice, CUcontext srcContext, size_t ByteCount), dstDevice, dstContext, srcDevice, srcContext, ByteCount)

CU_HOOK_DRIVER_FUNC(cuMemcpyPeerAsync_intercepted, cuMemcpyPeerAsync, (CUdeviceptr dstDevice, CUcontext dstContext, CUdeviceptr srcDevice, CUcontext srcContext, size_t ByteCount, CUstream hStream), dstDevice, dstContext, srcDevice, srcContext, ByteCount, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpyHtoD_intercepted, cuMemcpyHtoD, (CUdeviceptr dstDevice, const void *srcHost, unsigned int ByteCount), dstDevice, srcHost, ByteCount)

CU_HOOK_DRIVER_FUNC(cuMemcpyHtoDAsync_intercepted, cuMemcpyHtoDAsync, (CUdeviceptr dstDevice, const void *srcHost, unsigned int ByteCount, CUstream hStream), dstDevice, srcHost, ByteCount, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpyDtoH_intercepted, cuMemcpyDtoH, (void *dstHost, CUdeviceptr srcDevice, unsigned int ByteCount), dstHost, srcDevice, ByteCount)

CU_HOOK_DRIVER_FUNC(cuMemcpyDtoHAsync_intercepted, cuMemcpyDtoHAsync, (void *dstHost, CUdeviceptr srcDevice, unsigned int ByteCount, CUstream hStream), dstHost, srcDevice, ByteCount, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpyDtoD_intercepted, cuMemcpyDtoD, (CUdeviceptr dstDevice, CUdeviceptr srcDevice, unsigned int ByteCount), dstDevice, srcDevice, ByteCount)

CU_HOOK_DRIVER_FUNC(cuMemcpyDtoDAsync_intercepted, cuMemcpyDtoDAsync, (CUdeviceptr dstDevice, CUdeviceptr srcDevice, unsigned int ByteCount, CUstream hStream), dstDevice, srcDevice, ByteCount, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpy2DUnaligned_intercepted, cuMemcpy2DUnaligned, (const CUDA_MEMCPY2D *pCopy), pCopy)

CU_HOOK_DRIVER_FUNC(cuMemcpy2DAsync_intercepted, cuMemcpy2DAsync, (const CUDA_MEMCPY2D *pCopy, CUstream hStream), pCopy, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpy3D_intercepted, cuMemcpy3D, (const CUDA_MEMCPY3D *pCopy), pCopy)

CU_HOOK_DRIVER_FUNC(cuMemcpy3DAsync_intercepted, cuMemcpy3DAsync, (const CUDA_MEMCPY3D *pCopy, CUstream hStream), pCopy, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpy3DPeer_intercepted, cuMemcpy3DPeer, (const CUDA_MEMCPY3D_PEER *pCopy), pCopy)

CU_HOOK_DRIVER_FUNC(cuMemcpy3DPeerAsync_intercepted, cuMemcpy3DPeerAsync, (const CUDA_MEMCPY3D_PEER *pCopy, CUstream hStream), pCopy, hStream)

CU_HOOK_DRIVER_FUNC(cuMemsetD8_intercepted, cuMemsetD8, (CUdeviceptr dstDevice, unsigned char uc, unsigned int N), dstDevice, uc, N)

CU_HOOK_DRIVER_FUNC(cuMemsetD8Async_intercepted, cuMemsetD8Async, (CUdeviceptr dstDevice, unsigned char uc, size_t N, CUstream hStream), dstDevice, uc, N, hStream)

CU_HOOK_DRIVER_FUNC(cuMemsetD2D8_intercepted, cuMemsetD2D8, (CUdeviceptr dstDevice, unsigned int dstPitch, unsigned char uc, unsigned int Width, unsigned int Height), dstDevice, dstPitch, uc, Width, Height)

CU_HOOK_DRIVER_FUNC(cuMemsetD2D8Async_intercepted, cuMemsetD2D8Async, (CUdeviceptr dstDevice, size_t dstPitch, unsigned char uc, size_t Width, size_t Height, CUstream hStream), dstDevice, dstPitch, uc, Width, Height, hStream)

CU_HOOK_DRIVER_FUNC(cuFuncSetCacheConfig_intercepted, cuFuncSetCacheConfig, (CUfunction hfunc, CUfunc_cache config), hfunc, config)

CU_HOOK_DRIVER_FUNC(cuFuncSetSharedMemConfig_intercepted, cuFuncSetSharedMemConfig, (CUfunction hfunc, CUsharedconfig config), hfunc, config)

CU_HOOK_DRIVER_FUNC(cuFuncGetAttribute_intercepted, cuFuncGetAttribute, (int *pi, CUfunction_attribute attrib, CUfunction hfunc), pi, attrib, hfunc)

CU_HOOK_DRIVER_FUNC(cuFuncSetAttribute_intercepted, cuFuncSetAttribute, (CUfunction hfunc, CUfunction_attribute attrib, int value), hfunc, attrib, value)

CU_HOOK_DRIVER_FUNC(cuFuncGetName_intercepted, cuFuncGetName, (const char **name, CUfunction hfunc), name, hfunc)

CU_HOOK_DRIVER_FUNC(cuFuncGetParamInfo_intercepted, cuFuncGetParamInfo, (CUfunction func, size_t paramIndex, size_t *paramOffset, size_t *paramSize), func, paramIndex, paramOffset, paramSize)

CU_HOOK_DRIVER_FUNC(cuArrayCreate_intercepted, cuArrayCreate, (CUarray *pHandle, const CUDA_ARRAY_DESCRIPTOR *pAllocateArray), pHandle, pAllocateArray)

CU_HOOK_DRIVER_FUNC(cuArrayGetDescriptor_intercepted, cuArrayGetDescriptor, (CUDA_ARRAY_DESCRIPTOR *pArrayDescriptor, CUarray hArray), pArrayDescriptor, hArray)

CU_HOOK_DRIVER_FUNC(cuArrayGetSparseProperties_intercepted, cuArrayGetSparseProperties, (CUDA_ARRAY_SPARSE_PROPERTIES *sparseProperties, CUarray array), sparseProperties, array)

CU_HOOK_DRIVER_FUNC(cuArrayGetPlane_intercepted, cuArrayGetPlane, (CUarray *pPlaneArray, CUarray hArray, unsigned int planeIdx), pPlaneArray, hArray, planeIdx)

CU_HOOK_DRIVER_FUNC(cuArray3DCreate_intercepted, cuArray3DCreate, (CUarray *pHandle, const CUDA_ARRAY3D_DESCRIPTOR *pAllocateArray), pHandle, pAllocateArray)

CU_HOOK_DRIVER_FUNC(cuArray3DGetDescriptor_intercepted, cuArray3DGetDescriptor, (CUDA_ARRAY3D_DESCRIPTOR *pArrayDescriptor, CUarray hArray), pArrayDescriptor, hArray)

CU_HOOK_DRIVER_FUNC(cuArrayDestroy_intercepted, cuArrayDestroy, (CUarray hArray), hArray)

CU_HOOK_DRIVER_FUNC(cuMipmappedArrayCreate_intercepted, cuMipmappedArrayCreate, (CUmipmappedArray *pHandle, const CUDA_ARRAY3D_DESCRIPTOR *pMipmappedArrayDesc, unsigned int numMipmapLevels), pHandle, pMipmappedArrayDesc, numMipmapLevels)

CU_HOOK_DRIVER_FUNC(cuMipmappedArrayGetLevel_intercepted, cuMipmappedArrayGetLevel, (CUarray *pLevelArray, CUmipmappedArray hMipmappedArray, unsigned int level), pLevelArray, hMipmappedArray, level)

CU_HOOK_DRIVER_FUNC(cuMipmappedArrayGetSparseProperties_intercepted, cuMipmappedArrayGetSparseProperties, (CUDA_ARRAY_SPARSE_PROPERTIES *sparseProperties, CUmipmappedArray mipmap), sparseProperties, mipmap)

CU_HOOK_DRIVER_FUNC(cuMipmappedArrayDestroy_intercepted, cuMipmappedArrayDestroy, (CUmipmappedArray hMipmappedArray), hMipmappedArray)

CU_HOOK_DRIVER_FUNC(cuArrayGetMemoryRequirements_intercepted, cuArrayGetMemoryRequirements, (CUDA_ARRAY_MEMORY_REQUIREMENTS *memoryRequirements, CUarray array, CUdevice device), memoryRequirements, array, device)

CU_HOOK_DRIVER_FUNC(cuMipmappedArrayGetMemoryRequirements_intercepted, cuMipmappedArrayGetMemoryRequirements, (CUDA_ARRAY_MEMORY_REQUIREMENTS *memoryRequirements, CUmipmappedArray mipmap, CUdevice device), memoryRequirements, mipmap, device)

CU_HOOK_DRIVER_FUNC(cuTexObjectCreate_intercepted, cuTexObjectCreate, (CUtexObject *pTexObject, const CUDA_RESOURCE_DESC *pResDesc, const CUDA_TEXTURE_DESC *pTexDesc, const CUDA_RESOURCE_VIEW_DESC *pResViewDesc), pTexObject, pResDesc, pTexDesc, pResViewDesc)

CU_HOOK_DRIVER_FUNC(cuTexObjectDestroy_intercepted, cuTexObjectDestroy, (CUtexObject texObject), texObject)

CU_HOOK_DRIVER_FUNC(cuTexObjectGetResourceDesc_intercepted, cuTexObjectGetResourceDesc, (CUDA_RESOURCE_DESC *pResDesc, CUtexObject texObject), pResDesc, texObject)

CU_HOOK_DRIVER_FUNC(cuTexObjectGetTextureDesc_intercepted, cuTexObjectGetTextureDesc, (CUDA_TEXTURE_DESC *pTexDesc, CUtexObject texObject), pTexDesc, texObject)

CU_HOOK_DRIVER_FUNC(cuTexObjectGetResourceViewDesc_intercepted, cuTexObjectGetResourceViewDesc, (CUDA_RESOURCE_VIEW_DESC *pResViewDesc, CUtexObject texObject), pResViewDesc, texObject)

CU_HOOK_DRIVER_FUNC(cuSurfObjectCreate_intercepted, cuSurfObjectCreate, (CUsurfObject *pSurfObject, const CUDA_RESOURCE_DESC *pResDesc), pSurfObject, pResDesc)

CU_HOOK_DRIVER_FUNC(cuSurfObjectDestroy_intercepted, cuSurfObjectDestroy, (CUsurfObject surfObject), surfObject)

CU_HOOK_DRIVER_FUNC(cuSurfObjectGetResourceDesc_intercepted, cuSurfObjectGetResourceDesc, (CUDA_RESOURCE_DESC *pResDesc, CUsurfObject surfObject), pResDesc, surfObject)

CU_HOOK_DRIVER_FUNC(cuImportExternalMemory_intercepted, cuImportExternalMemory, (CUexternalMemory *extMem_out, const CUDA_EXTERNAL_MEMORY_HANDLE_DESC *memHandleDesc), extMem_out, memHandleDesc)

CU_HOOK_DRIVER_FUNC(cuExternalMemoryGetMappedBuffer_intercepted, cuExternalMemoryGetMappedBuffer, (CUdeviceptr *devPtr, CUexternalMemory extMem, const CUDA_EXTERNAL_MEMORY_BUFFER_DESC *bufferDesc), devPtr, extMem, bufferDesc)

CU_HOOK_DRIVER_FUNC(cuExternalMemoryGetMappedMipmappedArray_intercepted, cuExternalMemoryGetMappedMipmappedArray, (CUmipmappedArray *mipmap, CUexternalMemory extMem, const CUDA_EXTERNAL_MEMORY_MIPMAPPED_ARRAY_DESC *mipmapDesc), mipmap, extMem, mipmapDesc)

CU_HOOK_DRIVER_FUNC(cuDestroyExternalMemory_intercepted, cuDestroyExternalMemory, (CUexternalMemory extMem), extMem)

CU_HOOK_DRIVER_FUNC(cuImportExternalSemaphore_intercepted, cuImportExternalSemaphore, (CUexternalSemaphore *extSem_out, const CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC *semHandleDesc), extSem_out, semHandleDesc)

CU_HOOK_DRIVER_FUNC(cuSignalExternalSemaphoresAsync_intercepted, cuSignalExternalSemaphoresAsync, (const CUexternalSemaphore *extSemArray, const CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS *paramsArray, unsigned int numExtSems, CUstream stream), extSemArray, paramsArray, numExtSems, stream)

CU_HOOK_DRIVER_FUNC(cuWaitExternalSemaphoresAsync_intercepted, cuWaitExternalSemaphoresAsync, (const CUexternalSemaphore *extSemArray, const CUDA_EXTERNAL_SEMAPHORE_WAIT_PARAMS *paramsArray, unsigned int numExtSems, CUstream stream), extSemArray, paramsArray, numExtSems, stream)

CU_HOOK_DRIVER_FUNC(cuDestroyExternalSemaphore_intercepted, cuDestroyExternalSemaphore, (CUexternalSemaphore extSem), extSem)

CU_HOOK_DRIVER_FUNC(cuDeviceGetNvSciSyncAttributes_intercepted, cuDeviceGetNvSciSyncAttributes, (void *nvSciSyncAttrList, CUdevice dev, int flags), nvSciSyncAttrList, dev, flags)

CU_HOOK_DRIVER_FUNC(cuLaunchKernel_intercepted, cuLaunchKernel, (CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, CUstream hStream, void **kernelParams, void **extra), f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, hStream, kernelParams, extra)

CU_HOOK_DRIVER_FUNC(cuLaunchCooperativeKernel_intercepted, cuLaunchCooperativeKernel, (CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, CUstream hStream, void **kernelParams), f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, hStream, kernelParams)

CU_HOOK_DRIVER_FUNC(cuLaunchCooperativeKernelMultiDevice_intercepted, cuLaunchCooperativeKernelMultiDevice, (CUDA_LAUNCH_PARAMS *launchParamsList, unsigned int numDevices, unsigned int flags), launchParamsList, numDevices, flags)

CU_HOOK_DRIVER_FUNC(cuLaunchHostFunc_intercepted, cuLaunchHostFunc, (CUstream hStream, CUhostFn fn, void *userData), hStream, fn, userData)

CU_HOOK_DRIVER_FUNC(cuLaunchKernelEx_intercepted, cuLaunchKernelEx, (const CUlaunchConfig *config, CUfunction f, void **kernelParams, void **extra), config, f, kernelParams, extra)

CU_HOOK_DRIVER_FUNC(cuEventCreate_intercepted, cuEventCreate, (CUevent *phEvent, unsigned int Flags), phEvent, Flags)

CU_HOOK_DRIVER_FUNC(cuEventRecord_intercepted, cuEventRecord, (CUevent hEvent, CUstream hStream), hEvent, hStream)

CU_HOOK_DRIVER_FUNC(cuEventRecordWithFlags_intercepted, cuEventRecordWithFlags, (CUevent hEvent, CUstream hStream, unsigned int flags), hEvent, hStream, flags)

CU_HOOK_DRIVER_FUNC(cuEventQuery_intercepted, cuEventQuery, (CUevent hEvent), hEvent)

CU_HOOK_DRIVER_FUNC(cuEventSynchronize_intercepted, cuEventSynchronize, (CUevent hEvent), hEvent)

CU_HOOK_DRIVER_FUNC(cuEventDestroy_intercepted, cuEventDestroy, (CUevent hEvent), hEvent)

CU_HOOK_DRIVER_FUNC(cuEventElapsedTime_intercepted, cuEventElapsedTime, (float *pMilliseconds, CUevent hStart, CUevent hEnd), pMilliseconds, hStart, hEnd)

CU_HOOK_DRIVER_FUNC(cuStreamWaitValue32_intercepted, cuStreamWaitValue32, (CUstream stream, CUdeviceptr addr, cuuint32_t value, unsigned int flags), stream, addr, value, flags)

CU_HOOK_DRIVER_FUNC(cuStreamWriteValue32_intercepted, cuStreamWriteValue32, (CUstream stream, CUdeviceptr addr, cuuint32_t value, unsigned int flags), stream, addr, value, flags)

CU_HOOK_DRIVER_FUNC(cuStreamWaitValue64_intercepted, cuStreamWaitValue64, (CUstream stream, CUdeviceptr addr, cuuint64_t value, unsigned int flags), stream, addr, value, flags)

CU_HOOK_DRIVER_FUNC(cuStreamWriteValue64_intercepted, cuStreamWriteValue64, (CUstream stream, CUdeviceptr addr, cuuint64_t value, unsigned int flags), stream, addr, value, flags)

CU_HOOK_DRIVER_FUNC(cuStreamBatchMemOp_intercepted, cuStreamBatchMemOp, (CUstream stream, unsigned int count, CUstreamBatchMemOpParams *paramArray, unsigned int flags), stream, count, paramArray, flags)

CU_HOOK_DRIVER_FUNC(cuStreamCreate_intercepted, cuStreamCreate, (CUstream *phStream, unsigned int Flags), phStream, Flags)

CU_HOOK_DRIVER_FUNC(cuStreamCreateWithPriority_intercepted, cuStreamCreateWithPriority, (CUstream *phStream, unsigned int flags, int priority), phStream, flags, priority)

CU_HOOK_DRIVER_FUNC(cuStreamGetPriority_intercepted, cuStreamGetPriority, (CUstream hStream, int *priority), hStream, priority)

CU_HOOK_DRIVER_FUNC(cuStreamGetFlags_intercepted, cuStreamGetFlags, (CUstream hStream, unsigned int *flags), hStream, flags)

CU_HOOK_DRIVER_FUNC(cuStreamGetCtx_intercepted, cuStreamGetCtx, (CUstream hStream, CUcontext *pctx), hStream, pctx)

CU_HOOK_DRIVER_FUNC(cuStreamGetId_intercepted, cuStreamGetId, (CUstream hStream, unsigned long long *streamId), hStream, streamId)

CU_HOOK_DRIVER_FUNC(cuStreamDestroy_intercepted, cuStreamDestroy, (CUstream hStream), hStream)

CU_HOOK_DRIVER_FUNC(cuStreamWaitEvent_intercepted, cuStreamWaitEvent, (CUstream hStream, CUevent hEvent, unsigned int Flags), hStream, hEvent, Flags)

CU_HOOK_DRIVER_FUNC(cuStreamAddCallback_intercepted, cuStreamAddCallback, (CUstream hStream, CUstreamCallback callback, void *userData, unsigned int flags), hStream, callback, userData, flags)

CU_HOOK_DRIVER_FUNC(cuStreamSynchronize_intercepted, cuStreamSynchronize, (CUstream hStream), hStream)

CU_HOOK_DRIVER_FUNC(cuStreamQuery_intercepted, cuStreamQuery, (CUstream hStream), hStream)

CU_HOOK_DRIVER_FUNC(cuStreamAttachMemAsync_intercepted, cuStreamAttachMemAsync, (CUstream hStream, CUdeviceptr dptr, size_t length, unsigned int flags), hStream, dptr, length, flags)

CU_HOOK_DRIVER_FUNC(cuStreamCopyAttributes_intercepted, cuStreamCopyAttributes, (CUstream dstStream, CUstream srcStream), dstStream, srcStream)

CU_HOOK_DRIVER_FUNC(cuStreamGetAttribute_intercepted, cuStreamGetAttribute, (CUstream hStream, CUstreamAttrID attr, CUstreamAttrValue *value), hStream, attr, value)

CU_HOOK_DRIVER_FUNC(cuStreamSetAttribute_intercepted, cuStreamSetAttribute, (CUstream hStream, CUstreamAttrID attr, const CUstreamAttrValue *param), hStream, attr, param)

CU_HOOK_DRIVER_FUNC(cuDeviceCanAccessPeer_intercepted, cuDeviceCanAccessPeer, (int *canAccessPeer, CUdevice dev, CUdevice peerDev), canAccessPeer, dev, peerDev)

CU_HOOK_DRIVER_FUNC(cuCtxEnablePeerAccess_intercepted, cuCtxEnablePeerAccess, (CUcontext peerContext, unsigned int Flags), peerContext, Flags)

CU_HOOK_DRIVER_FUNC(cuCtxDisablePeerAccess_intercepted, cuCtxDisablePeerAccess, (CUcontext peerContext), peerContext)

CU_HOOK_DRIVER_FUNC(cuIpcGetEventHandle_intercepted, cuIpcGetEventHandle, (CUipcEventHandle *pHandle, CUevent event), pHandle, event)

CU_HOOK_DRIVER_FUNC(cuIpcOpenEventHandle_intercepted, cuIpcOpenEventHandle, (CUevent *phEvent, CUipcEventHandle handle), phEvent, handle)

CU_HOOK_DRIVER_FUNC(cuIpcGetMemHandle_intercepted, cuIpcGetMemHandle, (CUipcMemHandle *pHandle, CUdeviceptr dptr), pHandle, dptr)

CU_HOOK_DRIVER_FUNC(cuIpcOpenMemHandle_intercepted, cuIpcOpenMemHandle, (CUdeviceptr *pdptr, CUipcMemHandle handle, unsigned int Flags), pdptr, handle, Flags)

CU_HOOK_DRIVER_FUNC(cuIpcCloseMemHandle_intercepted, cuIpcCloseMemHandle, (CUdeviceptr dptr), dptr)

CU_HOOK_DRIVER_FUNC(cuGraphicsUnregisterResource_intercepted, cuGraphicsUnregisterResource, (CUgraphicsResource resource), resource)

CU_HOOK_DRIVER_FUNC(cuGraphicsMapResources_intercepted, cuGraphicsMapResources, (unsigned int count, CUgraphicsResource *resources, CUstream hStream), count, resources, hStream)

CU_HOOK_DRIVER_FUNC(cuGraphicsUnmapResources_intercepted, cuGraphicsUnmapResources, (unsigned int count, CUgraphicsResource *resources, CUstream hStream), count, resources, hStream)

CU_HOOK_DRIVER_FUNC(cuGraphicsResourceSetMapFlags_intercepted, cuGraphicsResourceSetMapFlags, (CUgraphicsResource resource, unsigned int flags), resource, flags)

CU_HOOK_DRIVER_FUNC(cuGraphicsSubResourceGetMappedArray_intercepted, cuGraphicsSubResourceGetMappedArray, (CUarray *pArray, CUgraphicsResource resource, unsigned int arrayIndex, unsigned int mipLevel), pArray, resource, arrayIndex, mipLevel)

CU_HOOK_DRIVER_FUNC(cuGraphicsResourceGetMappedMipmappedArray_intercepted, cuGraphicsResourceGetMappedMipmappedArray, (CUmipmappedArray *pMipmappedArray, CUgraphicsResource resource), pMipmappedArray, resource)

CU_HOOK_DRIVER_FUNC(cuGraphicsResourceGetMappedPointer_intercepted, cuGraphicsResourceGetMappedPointer, (CUdeviceptr *pDevPtr, unsigned int *pSize, CUgraphicsResource resource), pDevPtr, pSize, resource)

CU_HOOK_DRIVER_FUNC(cuGetExportTable_intercepted, cuGetExportTable, (const void **ppExportTable, const CUuuid *pExportTableId), ppExportTable, pExportTableId)

CU_HOOK_DRIVER_FUNC(cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags_intercepted, cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags, (int *numBlocks, CUfunction func, int blockSize, size_t dynamicSMemSize, unsigned int flags), numBlocks, func, blockSize, dynamicSMemSize, flags)

CU_HOOK_DRIVER_FUNC(cuOccupancyAvailableDynamicSMemPerBlock_intercepted, cuOccupancyAvailableDynamicSMemPerBlock, (size_t *dynamicSmemSize, CUfunction func, int numBlocks, int blockSize), dynamicSmemSize, func, numBlocks, blockSize)

CU_HOOK_DRIVER_FUNC(cuOccupancyMaxPotentialClusterSize_intercepted, cuOccupancyMaxPotentialClusterSize, (int *clusterSize, CUfunction func, const CUlaunchConfig *config), clusterSize, func, config)

CU_HOOK_DRIVER_FUNC(cuOccupancyMaxActiveClusters_intercepted, cuOccupancyMaxActiveClusters, (int *numClusters, CUfunction func, const CUlaunchConfig *config), numClusters, func, config)

CU_HOOK_DRIVER_FUNC(cuMemAdvise_intercepted, cuMemAdvise, (CUdeviceptr devPtr, size_t count, CUmem_advise advice, CUdevice device), devPtr, count, advice, device)

CU_HOOK_DRIVER_FUNC(cuMemPrefetchAsync_intercepted, cuMemPrefetchAsync, (CUdeviceptr devPtr, size_t count, CUdevice dstDevice, CUstream hStream), devPtr, count, dstDevice, hStream)

CU_HOOK_DRIVER_FUNC(cuMemRangeGetAttribute_intercepted, cuMemRangeGetAttribute, (void *data, size_t dataSize, CUmem_range_attribute attribute, CUdeviceptr devPtr, size_t count), data, dataSize, attribute, devPtr, count)

CU_HOOK_DRIVER_FUNC(cuMemRangeGetAttributes_intercepted, cuMemRangeGetAttributes, (void **data, size_t *dataSizes, CUmem_range_attribute *attributes, size_t numAttributes, CUdeviceptr devPtr, size_t count), data, dataSizes, attributes, numAttributes, devPtr, count)

CU_HOOK_DRIVER_FUNC(cuGetErrorString_intercepted, cuGetErrorString, (CUresult error, const char **pStr), error, pStr)

CU_HOOK_DRIVER_FUNC(cuGetErrorName_intercepted, cuGetErrorName, (CUresult error, const char **pStr), error, pStr)

CU_HOOK_DRIVER_FUNC(cuGraphCreate_intercepted, cuGraphCreate, (CUgraph *phGraph, unsigned int flags), phGraph, flags)

CU_HOOK_DRIVER_FUNC(cuGraphAddKernelNode_intercepted, cuGraphAddKernelNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, const CUDA_KERNEL_NODE_PARAMS_v1 *nodeParams), phGraphNode, hGraph, dependencies, numDependencies, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphKernelNodeGetParams_intercepted, cuGraphKernelNodeGetParams, (CUgraphNode hNode, CUDA_KERNEL_NODE_PARAMS_v1 *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphKernelNodeSetParams_intercepted, cuGraphKernelNodeSetParams, (CUgraphNode hNode, const CUDA_KERNEL_NODE_PARAMS_v1 *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphAddMemcpyNode_intercepted, cuGraphAddMemcpyNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, const CUDA_MEMCPY3D *copyParams, CUcontext ctx), phGraphNode, hGraph, dependencies, numDependencies, copyParams, ctx)

CU_HOOK_DRIVER_FUNC(cuGraphMemcpyNodeGetParams_intercepted, cuGraphMemcpyNodeGetParams, (CUgraphNode hNode, CUDA_MEMCPY3D *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphMemcpyNodeSetParams_intercepted, cuGraphMemcpyNodeSetParams, (CUgraphNode hNode, const CUDA_MEMCPY3D *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphAddMemsetNode_intercepted, cuGraphAddMemsetNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, const CUDA_MEMSET_NODE_PARAMS *memsetParams, CUcontext ctx), phGraphNode, hGraph, dependencies, numDependencies, memsetParams, ctx)

CU_HOOK_DRIVER_FUNC(cuGraphMemsetNodeGetParams_intercepted, cuGraphMemsetNodeGetParams, (CUgraphNode hNode, CUDA_MEMSET_NODE_PARAMS *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphMemsetNodeSetParams_intercepted, cuGraphMemsetNodeSetParams, (CUgraphNode hNode, const CUDA_MEMSET_NODE_PARAMS *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphAddHostNode_intercepted, cuGraphAddHostNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, const CUDA_HOST_NODE_PARAMS *nodeParams), phGraphNode, hGraph, dependencies, numDependencies, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphHostNodeGetParams_intercepted, cuGraphHostNodeGetParams, (CUgraphNode hNode, CUDA_HOST_NODE_PARAMS *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphHostNodeSetParams_intercepted, cuGraphHostNodeSetParams, (CUgraphNode hNode, const CUDA_HOST_NODE_PARAMS *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphAddChildGraphNode_intercepted, cuGraphAddChildGraphNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, CUgraph childGraph), phGraphNode, hGraph, dependencies, numDependencies, childGraph)

CU_HOOK_DRIVER_FUNC(cuGraphChildGraphNodeGetGraph_intercepted, cuGraphChildGraphNodeGetGraph, (CUgraphNode hNode, CUgraph *phGraph), hNode, phGraph)

CU_HOOK_DRIVER_FUNC(cuGraphAddEmptyNode_intercepted, cuGraphAddEmptyNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies), phGraphNode, hGraph, dependencies, numDependencies)

CU_HOOK_DRIVER_FUNC(cuGraphAddEventRecordNode_intercepted, cuGraphAddEventRecordNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, CUevent event), phGraphNode, hGraph, dependencies, numDependencies, event)

CU_HOOK_DRIVER_FUNC(cuGraphEventRecordNodeGetEvent_intercepted, cuGraphEventRecordNodeGetEvent, (CUgraphNode hNode, CUevent *event_out), hNode, event_out)

CU_HOOK_DRIVER_FUNC(cuGraphEventRecordNodeSetEvent_intercepted, cuGraphEventRecordNodeSetEvent, (CUgraphNode hNode, CUevent event), hNode, event)

CU_HOOK_DRIVER_FUNC(cuGraphAddEventWaitNode_intercepted, cuGraphAddEventWaitNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, CUevent event), phGraphNode, hGraph, dependencies, numDependencies, event)

CU_HOOK_DRIVER_FUNC(cuGraphEventWaitNodeGetEvent_intercepted, cuGraphEventWaitNodeGetEvent, (CUgraphNode hNode, CUevent *event_out), hNode, event_out)

CU_HOOK_DRIVER_FUNC(cuGraphEventWaitNodeSetEvent_intercepted, cuGraphEventWaitNodeSetEvent, (CUgraphNode hNode, CUevent event), hNode, event)

CU_HOOK_DRIVER_FUNC(cuGraphAddExternalSemaphoresSignalNode_intercepted, cuGraphAddExternalSemaphoresSignalNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, const CUDA_EXT_SEM_SIGNAL_NODE_PARAMS *nodeParams), phGraphNode, hGraph, dependencies, numDependencies, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphExternalSemaphoresSignalNodeGetParams_intercepted, cuGraphExternalSemaphoresSignalNodeGetParams, (CUgraphNode hNode, CUDA_EXT_SEM_SIGNAL_NODE_PARAMS *params_out), hNode, params_out)

CU_HOOK_DRIVER_FUNC(cuGraphExternalSemaphoresSignalNodeSetParams_intercepted, cuGraphExternalSemaphoresSignalNodeSetParams, (CUgraphNode hNode, const CUDA_EXT_SEM_SIGNAL_NODE_PARAMS *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphAddExternalSemaphoresWaitNode_intercepted, cuGraphAddExternalSemaphoresWaitNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, const CUDA_EXT_SEM_WAIT_NODE_PARAMS *nodeParams), phGraphNode, hGraph, dependencies, numDependencies, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphExternalSemaphoresWaitNodeGetParams_intercepted, cuGraphExternalSemaphoresWaitNodeGetParams, (CUgraphNode hNode, CUDA_EXT_SEM_WAIT_NODE_PARAMS *params_out), hNode, params_out)

CU_HOOK_DRIVER_FUNC(cuGraphExternalSemaphoresWaitNodeSetParams_intercepted, cuGraphExternalSemaphoresWaitNodeSetParams, (CUgraphNode hNode, const CUDA_EXT_SEM_WAIT_NODE_PARAMS *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphExecExternalSemaphoresSignalNodeSetParams_intercepted, cuGraphExecExternalSemaphoresSignalNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_EXT_SEM_SIGNAL_NODE_PARAMS *nodeParams), hGraphExec, hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphExecExternalSemaphoresWaitNodeSetParams_intercepted, cuGraphExecExternalSemaphoresWaitNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_EXT_SEM_WAIT_NODE_PARAMS *nodeParams), hGraphExec, hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphAddMemAllocNode_intercepted, cuGraphAddMemAllocNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, CUDA_MEM_ALLOC_NODE_PARAMS *nodeParams), phGraphNode, hGraph, dependencies, numDependencies, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphMemAllocNodeGetParams_intercepted, cuGraphMemAllocNodeGetParams, (CUgraphNode hNode, CUDA_MEM_ALLOC_NODE_PARAMS *params_out), hNode, params_out)

CU_HOOK_DRIVER_FUNC(cuGraphAddMemFreeNode_intercepted, cuGraphAddMemFreeNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, CUdeviceptr dptr), phGraphNode, hGraph, dependencies, numDependencies, dptr)

CU_HOOK_DRIVER_FUNC(cuGraphMemFreeNodeGetParams_intercepted, cuGraphMemFreeNodeGetParams, (CUgraphNode hNode, CUdeviceptr *dptr_out), hNode, dptr_out)

CU_HOOK_DRIVER_FUNC(cuDeviceGraphMemTrim_intercepted, cuDeviceGraphMemTrim, (CUdevice device), device)

CU_HOOK_DRIVER_FUNC(cuDeviceGetGraphMemAttribute_intercepted, cuDeviceGetGraphMemAttribute, (CUdevice device, CUgraphMem_attribute attr, void* value), device, attr, value)

CU_HOOK_DRIVER_FUNC(cuDeviceSetGraphMemAttribute_intercepted, cuDeviceSetGraphMemAttribute, (CUdevice device, CUgraphMem_attribute attr, void* value), device, attr, value)

CU_HOOK_DRIVER_FUNC(cuGraphClone_intercepted, cuGraphClone, (CUgraph *phGraphClone, CUgraph originalGraph), phGraphClone, originalGraph)

CU_HOOK_DRIVER_FUNC(cuGraphNodeFindInClone_intercepted, cuGraphNodeFindInClone, (CUgraphNode *phNode, CUgraphNode hOriginalNode, CUgraph hClonedGraph), phNode, hOriginalNode, hClonedGraph)

CU_HOOK_DRIVER_FUNC(cuGraphNodeGetType_intercepted, cuGraphNodeGetType, (CUgraphNode hNode, CUgraphNodeType *type), hNode, type)

CU_HOOK_DRIVER_FUNC(cuGraphGetNodes_intercepted, cuGraphGetNodes, (CUgraph hGraph, CUgraphNode *nodes, size_t *numNodes), hGraph, nodes, numNodes)

CU_HOOK_DRIVER_FUNC(cuGraphGetRootNodes_intercepted, cuGraphGetRootNodes, (CUgraph hGraph, CUgraphNode *rootNodes, size_t *numRootNodes), hGraph, rootNodes, numRootNodes)

CU_HOOK_DRIVER_FUNC(cuGraphGetEdges_intercepted, cuGraphGetEdges, (CUgraph hGraph, CUgraphNode *from, CUgraphNode *to, size_t *numEdges), hGraph, from, to, numEdges)

CU_HOOK_DRIVER_FUNC(cuGraphNodeGetDependencies_intercepted, cuGraphNodeGetDependencies, (CUgraphNode hNode, CUgraphNode *dependencies, size_t *numDependencies), hNode, dependencies, numDependencies)

CU_HOOK_DRIVER_FUNC(cuGraphNodeGetDependentNodes_intercepted, cuGraphNodeGetDependentNodes, (CUgraphNode hNode, CUgraphNode *dependentNodes, size_t *numDependentNodes), hNode, dependentNodes, numDependentNodes)

CU_HOOK_DRIVER_FUNC(cuGraphAddDependencies_intercepted, cuGraphAddDependencies, (CUgraph hGraph, const CUgraphNode *from, const CUgraphNode *to, size_t numDependencies), hGraph, from, to, numDependencies)

CU_HOOK_DRIVER_FUNC(cuGraphRemoveDependencies_intercepted, cuGraphRemoveDependencies, (CUgraph hGraph, const CUgraphNode *from, const CUgraphNode *to, size_t numDependencies), hGraph, from, to, numDependencies)

CU_HOOK_DRIVER_FUNC(cuGraphDestroyNode_intercepted, cuGraphDestroyNode, (CUgraphNode hNode), hNode)

CU_HOOK_DRIVER_FUNC(cuGraphInstantiate_intercepted, cuGraphInstantiate, (CUgraphExec *phGraphExec, CUgraph hGraph, CUgraphNode *phErrorNode, char *logBuffer, size_t bufferSize), phGraphExec, hGraph, phErrorNode, logBuffer, bufferSize)

CU_HOOK_DRIVER_FUNC(cuGraphUpload_intercepted, cuGraphUpload, (CUgraphExec hGraph, CUstream hStream), hGraph, hStream)

CU_HOOK_DRIVER_FUNC(cuGraphLaunch_intercepted, cuGraphLaunch, (CUgraphExec hGraph, CUstream hStream), hGraph, hStream)

CU_HOOK_DRIVER_FUNC(cuGraphExecDestroy_intercepted, cuGraphExecDestroy, (CUgraphExec hGraphExec), hGraphExec)

CU_HOOK_DRIVER_FUNC(cuGraphDestroy_intercepted, cuGraphDestroy, (CUgraph hGraph), hGraph)

CU_HOOK_DRIVER_FUNC(cuStreamBeginCapture_intercepted, cuStreamBeginCapture, (CUstream hStream), hStream)

CU_HOOK_DRIVER_FUNC(cuStreamBeginCaptureToGraph_intercepted, cuStreamBeginCaptureToGraph, (CUstream hStream, CUgraph hGraph, const CUgraphNode *dependencies, const CUgraphEdgeData *dependencyData, size_t numDependencies, CUstreamCaptureMode mode), hStream, hGraph, dependencies, dependencyData, numDependencies, mode)

CU_HOOK_DRIVER_FUNC(cuStreamEndCapture_intercepted, cuStreamEndCapture, (CUstream hStream, CUgraph *phGraph), hStream, phGraph)

CU_HOOK_DRIVER_FUNC(cuStreamIsCapturing_intercepted, cuStreamIsCapturing, (CUstream hStream, CUstreamCaptureStatus *captureStatus), hStream, captureStatus)

CU_HOOK_DRIVER_FUNC(cuStreamGetCaptureInfo_intercepted, cuStreamGetCaptureInfo, (CUstream hStream, CUstreamCaptureStatus *captureStatus_out, cuuint64_t *id_out), hStream, captureStatus_out, id_out)

CU_HOOK_DRIVER_FUNC(cuStreamUpdateCaptureDependencies_intercepted, cuStreamUpdateCaptureDependencies, (CUstream hStream, CUgraphNode *dependencies, size_t numDependencies, unsigned int flags), hStream, dependencies, numDependencies, flags)

CU_HOOK_DRIVER_FUNC(cuGraphExecKernelNodeSetParams_intercepted, cuGraphExecKernelNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_KERNEL_NODE_PARAMS_v1 *nodeParams), hGraphExec, hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphExecMemcpyNodeSetParams_intercepted, cuGraphExecMemcpyNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_MEMCPY3D *copyParams, CUcontext ctx), hGraphExec, hNode, copyParams, ctx)

CU_HOOK_DRIVER_FUNC(cuGraphExecMemsetNodeSetParams_intercepted, cuGraphExecMemsetNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_MEMSET_NODE_PARAMS *memsetParams, CUcontext ctx), hGraphExec, hNode, memsetParams, ctx)

CU_HOOK_DRIVER_FUNC(cuGraphExecHostNodeSetParams_intercepted, cuGraphExecHostNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_HOST_NODE_PARAMS *nodeParams), hGraphExec, hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphExecChildGraphNodeSetParams_intercepted, cuGraphExecChildGraphNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, CUgraph childGraph), hGraphExec, hNode, childGraph)

CU_HOOK_DRIVER_FUNC(cuGraphExecEventRecordNodeSetEvent_intercepted, cuGraphExecEventRecordNodeSetEvent, (CUgraphExec hGraphExec, CUgraphNode hNode, CUevent event), hGraphExec, hNode, event)

CU_HOOK_DRIVER_FUNC(cuGraphExecEventWaitNodeSetEvent_intercepted, cuGraphExecEventWaitNodeSetEvent, (CUgraphExec hGraphExec, CUgraphNode hNode, CUevent event), hGraphExec, hNode, event)

CU_HOOK_DRIVER_FUNC(cuThreadExchangeStreamCaptureMode_intercepted, cuThreadExchangeStreamCaptureMode, (CUstreamCaptureMode *mode), mode)

CU_HOOK_DRIVER_FUNC(cuGraphExecUpdate_intercepted, cuGraphExecUpdate, (CUgraphExec hGraphExec, CUgraph hGraph, CUgraphNode *hErrorNode_out, CUgraphExecUpdateResult *updateResult_out), hGraphExec, hGraph, hErrorNode_out, updateResult_out)

CU_HOOK_DRIVER_FUNC(cuGraphKernelNodeCopyAttributes_intercepted, cuGraphKernelNodeCopyAttributes, (CUgraphNode dst, CUgraphNode src), dst, src)

CU_HOOK_DRIVER_FUNC(cuGraphDebugDotPrint_intercepted, cuGraphDebugDotPrint, (CUgraph hGraph, const char *path, unsigned int flags), hGraph, path, flags)

CU_HOOK_DRIVER_FUNC(cuUserObjectRetain_intercepted, cuUserObjectRetain, (CUuserObject object, unsigned int count), object, count)

CU_HOOK_DRIVER_FUNC(cuUserObjectRelease_intercepted, cuUserObjectRelease, (CUuserObject object, unsigned int count), object, count)

CU_HOOK_DRIVER_FUNC(cuGraphRetainUserObject_intercepted, cuGraphRetainUserObject, (CUgraph graph, CUuserObject object, unsigned int count, unsigned int flags), graph, object, count, flags)

CU_HOOK_DRIVER_FUNC(cuGraphReleaseUserObject_intercepted, cuGraphReleaseUserObject, (CUgraph graph, CUuserObject object, unsigned int count), graph, object, count)

CU_HOOK_DRIVER_FUNC(cuGraphNodeSetEnabled_intercepted, cuGraphNodeSetEnabled, (CUgraphExec hGraphExec, CUgraphNode hNode, unsigned int isEnabled), hGraphExec, hNode, isEnabled)

CU_HOOK_DRIVER_FUNC(cuGraphNodeGetEnabled_intercepted, cuGraphNodeGetEnabled, (CUgraphExec hGraphExec, CUgraphNode hNode, unsigned int *isEnabled), hGraphExec, hNode, isEnabled)

CU_HOOK_DRIVER_FUNC(cuGraphInstantiateWithParams_intercepted, cuGraphInstantiateWithParams, (CUgraphExec *phGraphExec, CUgraph hGraph, CUDA_GRAPH_INSTANTIATE_PARAMS *instantiateParams), phGraphExec, hGraph, instantiateParams)

CU_HOOK_DRIVER_FUNC(cuGraphExecGetFlags_intercepted, cuGraphExecGetFlags, (CUgraphExec hGraphExec, cuuint64_t *flags), hGraphExec, flags)

CU_HOOK_DRIVER_FUNC(cuGraphAddNode_intercepted, cuGraphAddNode, (CUgraphNode *phGraphNode, CUgraph hGraph, const CUgraphNode *dependencies, size_t numDependencies, CUgraphNodeParams *nodeParams), phGraphNode, hGraph, dependencies, numDependencies, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphNodeSetParams_intercepted, cuGraphNodeSetParams, (CUgraphNode hNode, CUgraphNodeParams *nodeParams), hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphExecNodeSetParams_intercepted, cuGraphExecNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, CUgraphNodeParams *nodeParams), hGraphExec, hNode, nodeParams)

CU_HOOK_DRIVER_FUNC(cuGraphConditionalHandleCreate_intercepted, cuGraphConditionalHandleCreate, (CUgraphConditionalHandle *pHandle_out, CUgraph hGraph, CUcontext ctx, unsigned int defaultLaunchValue, unsigned int flags), pHandle_out, hGraph, ctx, defaultLaunchValue, flags)

CU_HOOK_DRIVER_FUNC(cuDeviceRegisterAsyncNotification_intercepted, cuDeviceRegisterAsyncNotification, (CUdevice device, CUasyncCallback callbackFunc, void *userData, CUasyncCallbackHandle *callback), device, callbackFunc, userData, callback)

CU_HOOK_DRIVER_FUNC(cuDeviceUnregisterAsyncNotification_intercepted, cuDeviceUnregisterAsyncNotification, (CUdevice device, CUasyncCallbackHandle callback), device, callback)
#pragma endregion

CU_HOOK_DRIVER_FUNC(cuLibraryLoadData_intercepted, cuLibraryLoadData,
  ( CUlibrary* library, const void* code, CUjit_option* jitOptions, void** jitOptionsValues, unsigned int  numJitOptions, CUlibraryOption* libraryOptions, void** libraryOptionValues, unsigned int  numLibraryOptions ),
  library, code, jitOptions, jitOptionsValues, numJitOptions, libraryOptions, libraryOptionValues, numLibraryOptions)

CU_HOOK_DRIVER_FUNC(cuLibraryLoadFromFile_intercepted, cuLibraryLoadFromFile,
  (CUlibrary* library, const char* fileName, CUjit_option* jitOptions, void** jitOptionsValues, unsigned int numJitOptions, CUlibraryOption* libraryOptions, void** libraryOptionValues, unsigned int numLibraryOptions),
  library, fileName, jitOptions, jitOptionsValues, numJitOptions, libraryOptions, libraryOptionValues, numLibraryOptions)

CU_HOOK_DRIVER_FUNC(cuLinkAddData_intercepted, cuLinkAddData,
  (CUlinkState state, CUjitInputType type, void* data, size_t size, const char* name, unsigned int numOptions, CUjit_option* options, void** optionValues),
  state, type, data, size, name, numOptions, options, optionValues)

CU_HOOK_DRIVER_FUNC(cuLinkAddFile_intercepted, cuLinkAddFile,
  (CUlinkState state, CUjitInputType type, const char* path, unsigned int numOptions, CUjit_option* options, void** optionValues),
  state, type, path, numOptions, options, optionValues)


CU_HOOK_DRIVER_FUNC(cuLinkComplete_intercepted, cuLinkComplete,
  (CUlinkState state, void** cubinOut, size_t* sizeOut),
  state, cubinOut, sizeOut)


CU_HOOK_DRIVER_FUNC(cuLinkDestroy_intercepted, cuLinkDestroy,
  (CUlinkState state),
  state)

CU_HOOK_DRIVER_FUNC(cuMemPoolImportFromShareableHandle_intercepted, cuMemPoolImportFromShareableHandle,
  (CUmemoryPool* pool_out, void* handle, CUmemAllocationHandleType handleType, unsigned long long flags),
  pool_out, handle, handleType, flags)

CU_HOOK_DRIVER_FUNC(cuGLCtxCreate_intercepted, cuGLCtxCreate,
  (CUcontext* pCtx, unsigned int Flags, CUdevice device),
  pCtx, Flags, device)

//todo check the usage
CU_HOOK_DRIVER_FUNC(cuGLInit_intercepted, cuGLInit, (),)

// CU_HOOK_DRIVER_FUNC(cuGLGetDevices_intercepted, cuGLGetDevices,
//   (unsigned int* pCudaDeviceCount, CUdevice* pCudaDevices, unsigned int cudaDeviceCount, CUGLDeviceList deviceList),
//   pCudaDeviceCount, pCudaDevices, cudaDeviceCount, deviceList)

// CU_HOOK_DRIVER_FUNC(cuGLRegisterBufferObject_intercepted, cuGLRegisterBufferObject,
//   (GLuint buffer),
//   buffer)

// CU_HOOK_DRIVER_FUNC(cuGLMapBufferObject_intercepted, cuGLMapBufferObject,
//   (CUdeviceptr *dptr, size_t *size, GLuint buffer),
//   dptr, size, buffer)

// CU_HOOK_DRIVER_FUNC(cuGLMapBufferObjectAsync_intercepted, cuGLMapBufferObjectAsync,
//   (CUdeviceptr *dptr, size_t *size, GLuint buffer, CUstream hStream),
//   dptr, size, buffer, hStream)

// CU_HOOK_DRIVER_FUNC(cuGLUnmapBufferObject_intercepted, cuGLUnmapBufferObject,
//   (GLuint buffer),
//   buffer)

// CU_HOOK_DRIVER_FUNC(cuGLUnmapBufferObjectAsync_intercepted, cuGLUnmapBufferObjectAsync,
//   (GLuint buffer, CUstream hStream),
//   buffer, hStream)

// CU_HOOK_DRIVER_FUNC(cuGLUnregisterBufferObject_intercepted, cuGLUnregisterBufferObject,
//   (GLuint buffer),
//   buffer)

// CU_HOOK_DRIVER_FUNC(cuGLSetBufferObjectMapFlags_intercepted, cuGLSetBufferObjectMapFlags,
//   (GLuint buffer, unsigned int Flags),
//   buffer, Flags)

// CU_HOOK_DRIVER_FUNC(cuGraphicsGLRegisterImage_intercepted, cuGraphicsGLRegisterImage,
//   (CUgraphicsResource *pCudaResource, GLuint image, GLenum target, unsigned int flags),
//   pCudaResource, image, target, flags)

// CU_HOOK_DRIVER_FUNC(cuGraphicsGLRegisterBuffer_intercepted, cuGraphicsGLRegisterBuffer,
//   (CUgraphicsResource *pCudaResource, GLuint buffer, unsigned int flags),
//   pCudaResource, buffer, flags)

#pragma region egl functions
// CU_HOOK_DRIVER_FUNC(cuGraphicsEGLRegisterImage_intercepted, cuGraphicsEGLRegisterImage,
//   (CUgraphicsResource* pCudaResource, EGLImageKHR image, unsigned int  flags),
//   pCudaResource, image, flags)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamConsumerConnect_intercepted, cuEGLStreamConsumerConnect,
//   (CUeglStreamConnection *conn, CUeglStream stream),
//   conn, stream)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamConsumerDisconnect_intercepted, cuEGLStreamConsumerDisconnect,
//   (CUeglStreamConnection *conn),
//   conn)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamConsumerAcquireFrame_intercepted, cuEGLStreamConsumerAcquireFrame,
//   (CUeglStreamConnection* conn, CUgraphicsResource* pCudaResource, CUstream* pStream, unsigned int  timeout),
//   conn, pCudaResource, pStream, timeout)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamConsumerReleaseFrame_intercepted, cuEGLStreamConsumerReleaseFrame,
//   (CUeglStreamConnection* conn, CUgraphicsResource pCudaResource, CUstream* pStream ),
//   conn, pCudaResource, pStream)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamProducerConnect_intercepted, cuEGLStreamProducerConnect,
//   (CUeglStreamConnection* conn, EGLStreamKHR stream, EGLint width, EGLint height),
//   conn, stream, width, height)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamProducerDisconnect_intercepted, cuEGLStreamProducerDisconnect,
//   (CUeglStreamConnection *conn),
//   conn)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamProducerPresentFrame_intercepted, cuEGLStreamProducerPresentFrame,
//   (CUeglStreamConnection* conn, CUeglFrame eglframe, CUstream* pStream),
//   conn, eglframe, pStream)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamProducerReturnFrame_intercepted, cuEGLStreamProducerReturnFrame,
//   (CUeglStreamConnection* conn, CUeglFrame* eglframe, CUstream* pStream),
//   conn, eglframe, pStream)

// CU_HOOK_DRIVER_FUNC(cuGraphicsResourceGetMappedEglFrame_intercepted, cuGraphicsResourceGetMappedEglFrame,
//   (CUeglFrame* eglFrame, CUgraphicsResource resource, unsigned int  index, unsigned int  mipLevel),
//   eglFrame, resource, index, mipLevel)

// CU_HOOK_DRIVER_FUNC(cuEGLStreamConsumerConnectWithFlags_intercepted, cuEGLStreamConsumerConnectWithFlags,
//   (CUeglStreamConnection* conn, EGLStreamKHR stream, unsigned int  flags),
//   conn, stream, flags)
#pragma endregion
// CU_HOOK_DRIVER_FUNC(cuProfilerInitialize_intercepted, cuProfilerInitialize,
//   (const char* configFile, const char* outputFile, CUoutput_mode outputMode),
//   configFile, outputFile, outputMode)

CU_HOOK_DRIVER_FUNC(cuProfilerStart_intercepted, cuProfilerStart,(), )

CU_HOOK_DRIVER_FUNC(cuProfilerStop_intercepted, cuProfilerStop,(), )

// CU_HOOK_DRIVER_FUNC(cuVDPAUGetDevice_intercepted, cuVDPAUGetDevice,
//   (CUdevice* pDevice, VdpDevice vdpDevice, VdpGetProcAddress* vdpGetProcAddress),
//   pDevice, vdpDevice, vdpGetProcAddress)

// CU_HOOK_DRIVER_FUNC(cuVDPAUCtxCreate_intercepted, cuVDPAUCtxCreate,
//   (CUcontext* pCtx, unsigned int flags, CUdevice device, VdpDevice vdpDevice, VdpGetProcAddress* vdpGetProcAddress),
//   pCtx, flags, device, vdpDevice, vdpGetProcAddress)

// CU_HOOK_DRIVER_FUNC(cuGraphicsVDPAURegisterVideoSurface_intercepted, cuGraphicsVDPAURegisterVideoSurface,
//   (CUgraphicsResource *pCudaResource, VDPVideoSurface surface, unsigned int flags),
//   pCudaResource, surface, flags)

// CU_HOOK_DRIVER_FUNC(cuGraphicsVDPAURegisterOutputSurface_intercepted, cuGraphicsVDPAURegisterOutputSurface,
//   (CUgraphicsResource *pCudaResource, VDPVideoSurface surface, unsigned int flags),
//   pCudaResource, surface, flags)

CU_HOOK_DRIVER_FUNC(cuGraphInstantiateWithFlags_intercepted, cuGraphInstantiateWithFlags,
  (CUgraphExec* phGraphExec, CUgraph hGraph, unsigned long long flags),
  phGraphExec, hGraph, flags)

CU_HOOK_DRIVER_FUNC(cuGraphKernelNodeGetAttribute_intercepted, cuGraphKernelNodeGetAttribute,
  (CUgraphNode hNode, CUkernelNodeAttrID attr, CUkernelNodeAttrValue* value_out),
  hNode, attr, value_out)

CU_HOOK_DRIVER_FUNC(cuGraphKernelNodeSetAttribute_intercepted, cuGraphKernelNodeSetAttribute,
  (CUgraphNode hNode, CUkernelNodeAttrID attr, const CUkernelNodeAttrValue* value),
  hNode, attr, value)

CU_HOOK_DRIVER_FUNC(cuUserObjectCreate_intercepted, cuUserObjectCreate,
  (CUuserObject* object_out, void* ptr, CUhostFn destroy, unsigned int  initialRefcount, unsigned int  flags),
  object_out, ptr, destroy, initialRefcount, flags)

// CU_HOOK_DRIVER_FUNC(cuGraphInstantiateWithParams_ptsz_intercepted, cuGraphInstantiateWithParams_ptsz,
//   (CUgraph *phGraph, CUgraph graph, const CUgraphInstantiateParams *params),
//   phGraph, graph, params)
//==================================================================================================================
/* Intercept the `dlsym` function, which is used by `libcudart.so` to link to `libcuda.so` */
void *dlsym(void *handle, const char *symbol) {
  printf("[dlsym] %s\n", symbol);
  // for CUDA func
  if(strcmp(symbol, SYMBOL_TO_STR(cuGetProcAddress)) == 0){
    printf("intercepted: %s\n", symbol);
    return (void *) &getProcAddressBySymbol;
  }
  TRY_DLSYM("cuGetProcAddress", getProcAddressBySymbol)
  TRY_DLSYM("cuInit", cuInit)
  
  TRY_DLSYM("cuDeviceGet", cuDeviceGet)
  
  TRY_DLSYM("cuDeviceGetCount", cuDeviceGetCount)
  
  TRY_DLSYM("cuDeviceGetName", cuDeviceGetName)
  
  TRY_DLSYM("cuDeviceTotalMem", cuDeviceTotalMem_v2)
  
  TRY_DLSYM("cuDeviceGetAttribute", cuDeviceGetAttribute)
  
  TRY_DLSYM("cuDeviceGetP2PAttribute", cuDeviceGetP2PAttribute_intercepted)
  
  TRY_DLSYM("cuDriverGetVersion", cuDriverGetVersion)
  
  TRY_DLSYM("cuDeviceGetByPCIBusId", cuDeviceGetByPCIBusId_intercepted)
  
  TRY_DLSYM("cuDeviceGetPCIBusId", cuDeviceGetPCIBusId_intercepted)
  
  TRY_DLSYM("cuDeviceGetUuid", cuDeviceGetUuid)
  
  TRY_DLSYM("cuDeviceGetTexture1DLinearMaxWidth", cuDeviceGetTexture1DLinearMaxWidth_intercepted)
  
  TRY_DLSYM("cuDeviceGetDefaultMemPool", cuDeviceGetDefaultMemPool_intercepted)
  
  TRY_DLSYM("cuDeviceSetMemPool", cuDeviceSetMemPool_intercepted)
  
  TRY_DLSYM("cuDeviceGetMemPool", cuDeviceGetMemPool_intercepted)
  
  TRY_DLSYM("cuFlushGPUDirectRDMAWrites", cuFlushGPUDirectRDMAWrites_intercepted)
  
  TRY_DLSYM("cuDevicePrimaryCtxRetain", cuDevicePrimaryCtxRetain)
  
  TRY_DLSYM("cuDevicePrimaryCtxRelease", cuDevicePrimaryCtxRelease)
  
  TRY_DLSYM("cuDevicePrimaryCtxSetFlags", cuDevicePrimaryCtxSetFlags_intercepted)
  
  TRY_DLSYM("cuDevicePrimaryCtxGetState", cuDevicePrimaryCtxGetState_intercepted)
  
  TRY_DLSYM("cuDevicePrimaryCtxReset", cuDevicePrimaryCtxReset_intercepted)
  
  TRY_DLSYM("cuCtxCreate", cuCtxCreate)
  
  TRY_DLSYM("cuCtxGetFlags", cuCtxGetFlags_intercepted)
  
  TRY_DLSYM("cuCtxSetCurrent", cuCtxSetCurrent)
  
  TRY_DLSYM("cuCtxGetCurrent", cuCtxGetCurrent)
  
  TRY_DLSYM("cuCtxDetach", cuCtxDetach_intercepted)
  
  TRY_DLSYM("cuCtxGetApiVersion", cuCtxGetApiVersion_intercepted)
  
  TRY_DLSYM("cuCtxGetDevice", cuCtxGetDevice)
  
  TRY_DLSYM("cuCtxGetLimit", cuCtxGetLimit_intercepted)
  
  TRY_DLSYM("cuCtxSetLimit", cuCtxSetLimit_intercepted)
  
  TRY_DLSYM("cuCtxGetCacheConfig", cuCtxGetCacheConfig_intercepted)
  
  TRY_DLSYM("cuCtxSetCacheConfig", cuCtxSetCacheConfig_intercepted)
  
  TRY_DLSYM("cuCtxGetSharedMemConfig", cuCtxGetSharedMemConfig_intercepted)
  
  TRY_DLSYM("cuCtxGetStreamPriorityRange", cuCtxGetStreamPriorityRange)
  
  TRY_DLSYM("cuCtxSetSharedMemConfig", cuCtxSetSharedMemConfig_intercepted)
  
  TRY_DLSYM("cuCtxSynchronize", cuCtxSynchronize_intercepted)
  
  TRY_DLSYM("cuCtxResetPersistingL2Cache", cuCtxResetPersistingL2Cache_intercepted)
  
  TRY_DLSYM("cuCtxPopCurrent", cuCtxPopCurrent)
  
  TRY_DLSYM("cuCtxPushCurrent", cuCtxPushCurrent)
  
  TRY_DLSYM("cuModuleLoad", cuModuleLoad)
  
  TRY_DLSYM("cuModuleLoadData", cuModuleLoadData)
  
  TRY_DLSYM("cuModuleLoadFatBinary", cuModuleLoadFatBinary_intercepted)
  
  TRY_DLSYM("cuModuleUnload", cuModuleUnload)
  
  TRY_DLSYM("cuModuleGetFunction", cuModuleGetFunction)
  
  TRY_DLSYM("cuModuleGetGlobal", cuModuleGetGlobal)
  
  TRY_DLSYM("cuModuleGetTexRef", cuModuleGetTexRef_intercepted)
  
  TRY_DLSYM("cuModuleGetSurfRef", cuModuleGetSurfRef_intercepted)
  
  TRY_DLSYM("cuModuleGetLoadingMode", cuModuleGetLoadingMode)
  
  TRY_DLSYM("cuLibraryUnload", cuLibraryUnload)
  
  TRY_DLSYM("cuLibraryGetKernel", cuLibraryGetKernel_intercepted)
  
  TRY_DLSYM("cuLibraryGetModule", cuLibraryGetModule)
  
  TRY_DLSYM("cuKernelGetFunction", cuKernelGetFunction_intercepted)
  
  TRY_DLSYM("cuLibraryGetGlobal", cuLibraryGetGlobal_intercepted)
  
  TRY_DLSYM("cuLibraryGetManaged", cuLibraryGetManaged_intercepted)
  
  TRY_DLSYM("cuLibraryGetUnifiedFunction", cuLibraryGetUnifiedFunction_intercepted)
  
  TRY_DLSYM("cuLibraryGetKernelCount", cuLibraryGetKernelCount_intercepted)
  
  TRY_DLSYM("cuLibraryEnumerateKernels", cuLibraryEnumerateKernels_intercepted)
  
  TRY_DLSYM("cuKernelGetAttribute", cuKernelGetAttribute_intercepted)
  
  TRY_DLSYM("cuKernelSetAttribute", cuKernelSetAttribute_intercepted)
  
  TRY_DLSYM("cuKernelSetCacheConfig", cuKernelSetCacheConfig_intercepted)
  
  TRY_DLSYM("cuKernelGetName", cuKernelGetName_intercepted)
  
  TRY_DLSYM("cuKernelGetParamInfo", cuKernelGetParamInfo_intercepted)
  
  TRY_DLSYM("cuLinkCreate", cuLinkCreate_intercepted)
  
  TRY_DLSYM("cuMemGetInfo", cuMemGetInfo_intercepted)
  
  TRY_DLSYM("cuMemAllocManaged", cuMemAllocManaged_intercepted)
  
  TRY_DLSYM("cuMemAlloc", cuMemAlloc)
  
  TRY_DLSYM("cuMemAllocPitch", cuMemAllocPitch_intercepted)
  
  TRY_DLSYM("cuMemFree", cuMemFree)
  
  TRY_DLSYM("cuMemGetAddressRange", cuMemGetAddressRange_intercepted)
  
  TRY_DLSYM("cuMemFreeHost", cuMemFreeHost_intercepted)
  
  TRY_DLSYM("cuMemHostAlloc", cuMemHostAlloc)
  
  TRY_DLSYM("cuMemHostGetDevicePointer", cuMemHostGetDevicePointer)
  
  TRY_DLSYM("cuMemHostGetFlags", cuMemHostGetFlags_intercepted)
  
  TRY_DLSYM("cuMemHostRegister", cuMemHostRegister_intercepted)
  
  TRY_DLSYM("cuMemHostUnregister", cuMemHostUnregister_intercepted)
  
  TRY_DLSYM("cuPointerGetAttribute", cuPointerGetAttribute_intercepted)
  
  TRY_DLSYM("cuPointerGetAttributes", cuPointerGetAttributes_intercepted)
  
  TRY_DLSYM("cuMemAllocAsync", cuMemAllocAsync_intercepted)
  
  TRY_DLSYM("cuMemAllocFromPoolAsync", cuMemAllocFromPoolAsync_intercepted)
  
  TRY_DLSYM("cuMemFreeAsync", cuMemFreeAsync_intercepted)
  
  TRY_DLSYM("cuMemPoolTrimTo", cuMemPoolTrimTo_intercepted)
  
  TRY_DLSYM("cuMemPoolSetAttribute", cuMemPoolSetAttribute_intercepted)
  
  TRY_DLSYM("cuMemPoolGetAttribute", cuMemPoolGetAttribute_intercepted)
  
  TRY_DLSYM("cuMemPoolSetAccess", cuMemPoolSetAccess_intercepted)
  
  TRY_DLSYM("cuMemPoolGetAccess", cuMemPoolGetAccess_intercepted)
  
  TRY_DLSYM("cuMemPoolCreate", cuMemPoolCreate_intercepted)
  
  TRY_DLSYM("cuMemPoolDestroy", cuMemPoolDestroy_intercepted)
  
  TRY_DLSYM("cuMemPoolExportToShareableHandle", cuMemPoolExportToShareableHandle_intercepted)
  
  TRY_DLSYM("cuMemPoolExportPointer", cuMemPoolExportPointer_intercepted)
  
  TRY_DLSYM("cuMemPoolImportPointer", cuMemPoolImportPointer_intercepted)
  
  TRY_DLSYM("cuMemcpy", cuMemcpy_intercepted)
  
  TRY_DLSYM("cuMemcpyAsync", cuMemcpyAsync_intercepted)
  
  TRY_DLSYM("cuMemcpyPeer", cuMemcpyPeer_intercepted)
  
  TRY_DLSYM("cuMemcpyPeerAsync", cuMemcpyPeerAsync_intercepted)
  
  TRY_DLSYM("cuMemcpyHtoD", cuMemcpyHtoD)
  
  // TRY_DLSYM("cuMemcpyHtoDAsync", cuMemcpyHtoDAsync)
  
  TRY_DLSYM("cuMemcpyDtoH", cuMemcpyDtoH)
  
  TRY_DLSYM("cuMemcpyDtoHAsync", cuMemcpyDtoHAsync_intercepted)
  
  TRY_DLSYM("cuMemcpyDtoD", cuMemcpyDtoD_intercepted)
  
  TRY_DLSYM("cuMemcpyDtoDAsync", cuMemcpyDtoDAsync_intercepted)
  
  TRY_DLSYM("cuMemcpy2DUnaligned", cuMemcpy2DUnaligned_intercepted)
  
  TRY_DLSYM("cuMemcpy2DAsync", cuMemcpy2DAsync_intercepted)
  
  TRY_DLSYM("cuMemcpy3D", cuMemcpy3D_intercepted)
  
  TRY_DLSYM("cuMemcpy3DAsync", cuMemcpy3DAsync_intercepted)
  
  TRY_DLSYM("cuMemcpy3DPeer", cuMemcpy3DPeer_intercepted)
  
  TRY_DLSYM("cuMemcpy3DPeerAsync", cuMemcpy3DPeerAsync_intercepted)
  
  TRY_DLSYM("cuMemsetD8", cuMemsetD8_intercepted)
  
  TRY_DLSYM("cuMemsetD8Async", cuMemsetD8Async)
  
  TRY_DLSYM("cuMemsetD2D8", cuMemsetD2D8_intercepted)
  
  TRY_DLSYM("cuMemsetD2D8Async", cuMemsetD2D8Async_intercepted)
  
  TRY_DLSYM("cuFuncSetCacheConfig", cuFuncSetCacheConfig_intercepted)
  
  TRY_DLSYM("cuFuncSetSharedMemConfig", cuFuncSetSharedMemConfig_intercepted)
  
  TRY_DLSYM("cuFuncGetAttribute", cuFuncGetAttribute_intercepted)
  
  TRY_DLSYM("cuFuncSetAttribute", cuFuncSetAttribute_intercepted)
  
  TRY_DLSYM("cuFuncGetName", cuFuncGetName_intercepted)
  
  TRY_DLSYM("cuFuncGetParamInfo", cuFuncGetParamInfo_intercepted)
  
  TRY_DLSYM("cuArrayCreate", cuArrayCreate_intercepted)
  
  TRY_DLSYM("cuArrayGetDescriptor", cuArrayGetDescriptor_intercepted)
  
  TRY_DLSYM("cuArrayGetSparseProperties", cuArrayGetSparseProperties_intercepted)
  
  TRY_DLSYM("cuArrayGetPlane", cuArrayGetPlane_intercepted)
  
  TRY_DLSYM("cuArray3DCreate", cuArray3DCreate_intercepted)
  
  TRY_DLSYM("cuArray3DGetDescriptor", cuArray3DGetDescriptor_intercepted)
  
  TRY_DLSYM("cuArrayDestroy", cuArrayDestroy_intercepted)
  
  TRY_DLSYM("cuMipmappedArrayCreate", cuMipmappedArrayCreate_intercepted)
  
  TRY_DLSYM("cuMipmappedArrayGetLevel", cuMipmappedArrayGetLevel_intercepted)
  
  TRY_DLSYM("cuMipmappedArrayGetSparseProperties", cuMipmappedArrayGetSparseProperties_intercepted)
  
  TRY_DLSYM("cuMipmappedArrayDestroy", cuMipmappedArrayDestroy_intercepted)
  
  TRY_DLSYM("cuArrayGetMemoryRequirements", cuArrayGetMemoryRequirements_intercepted)
  
  TRY_DLSYM("cuMipmappedArrayGetMemoryRequirements", cuMipmappedArrayGetMemoryRequirements_intercepted)
  
  TRY_DLSYM("cuTexObjectCreate", cuTexObjectCreate_intercepted)
  
  TRY_DLSYM("cuTexObjectDestroy", cuTexObjectDestroy_intercepted)
  
  TRY_DLSYM("cuTexObjectGetResourceDesc", cuTexObjectGetResourceDesc_intercepted)
  
  TRY_DLSYM("cuTexObjectGetTextureDesc", cuTexObjectGetTextureDesc_intercepted)
  
  TRY_DLSYM("cuTexObjectGetResourceViewDesc", cuTexObjectGetResourceViewDesc_intercepted)
  
  TRY_DLSYM("cuSurfObjectCreate", cuSurfObjectCreate_intercepted)
  
  TRY_DLSYM("cuSurfObjectDestroy", cuSurfObjectDestroy_intercepted)
  
  TRY_DLSYM("cuSurfObjectGetResourceDesc", cuSurfObjectGetResourceDesc_intercepted)
  
  TRY_DLSYM("cuImportExternalMemory", cuImportExternalMemory_intercepted)
  
  TRY_DLSYM("cuExternalMemoryGetMappedBuffer", cuExternalMemoryGetMappedBuffer_intercepted)
  
  TRY_DLSYM("cuExternalMemoryGetMappedMipmappedArray", cuExternalMemoryGetMappedMipmappedArray_intercepted)
  
  TRY_DLSYM("cuDestroyExternalMemory", cuDestroyExternalMemory_intercepted)
  
  TRY_DLSYM("cuImportExternalSemaphore", cuImportExternalSemaphore_intercepted)
  
  TRY_DLSYM("cuSignalExternalSemaphoresAsync", cuSignalExternalSemaphoresAsync_intercepted)
  
  TRY_DLSYM("cuWaitExternalSemaphoresAsync", cuWaitExternalSemaphoresAsync_intercepted)
  
  TRY_DLSYM("cuDestroyExternalSemaphore", cuDestroyExternalSemaphore_intercepted)
  
  TRY_DLSYM("cuDeviceGetNvSciSyncAttributes", cuDeviceGetNvSciSyncAttributes_intercepted)
  
  TRY_DLSYM("cuLaunchKernel", cuLaunchKernel)
  
  TRY_DLSYM("cuLaunchCooperativeKernel", cuLaunchCooperativeKernel_intercepted)
  
  TRY_DLSYM("cuLaunchCooperativeKernelMultiDevice", cuLaunchCooperativeKernelMultiDevice_intercepted)
  
  TRY_DLSYM("cuLaunchHostFunc", cuLaunchHostFunc_intercepted)
  
  TRY_DLSYM("cuLaunchKernelEx", cuLaunchKernelEx_intercepted)
  
  TRY_DLSYM("cuEventCreate", cuEventCreate)
  
  TRY_DLSYM("cuEventRecord", cuEventRecord)
  
  TRY_DLSYM("cuEventRecordWithFlags", cuEventRecordWithFlags_intercepted)
  
  TRY_DLSYM("cuEventQuery", cuEventQuery_intercepted)
  
  TRY_DLSYM("cuEventSynchronize", cuEventSynchronize_intercepted)
  
  TRY_DLSYM("cuEventDestroy", cuEventDestroy_intercepted)
  
  TRY_DLSYM("cuEventElapsedTime", cuEventElapsedTime_intercepted)
  
  TRY_DLSYM("cuStreamWaitValue32", cuStreamWaitValue32_intercepted)
  
  TRY_DLSYM("cuStreamWriteValue32", cuStreamWriteValue32_intercepted)
  
  TRY_DLSYM("cuStreamWaitValue64", cuStreamWaitValue64_intercepted)
  
  TRY_DLSYM("cuStreamWriteValue64", cuStreamWriteValue64_intercepted)
  
  TRY_DLSYM("cuStreamBatchMemOp", cuStreamBatchMemOp_intercepted)
  
  TRY_DLSYM("cuStreamCreate", cuStreamCreate)
  
  TRY_DLSYM("cuStreamCreateWithPriority", cuStreamCreateWithPriority_intercepted)
  
  TRY_DLSYM("cuStreamGetPriority", cuStreamGetPriority)
  
  TRY_DLSYM("cuStreamGetFlags", cuStreamGetFlags_intercepted)
  
  TRY_DLSYM("cuStreamGetCtx", cuStreamGetCtx_intercepted)
  
  TRY_DLSYM("cuStreamGetId", cuStreamGetId_intercepted)
  
  TRY_DLSYM("cuStreamDestroy", cuStreamDestroy_intercepted)
  
  TRY_DLSYM("cuStreamWaitEvent", cuStreamWaitEvent_intercepted)
  
  TRY_DLSYM("cuStreamAddCallback", cuStreamAddCallback_intercepted)
  
  TRY_DLSYM("cuStreamSynchronize", cuStreamSynchronize)
  
  TRY_DLSYM("cuStreamQuery", cuStreamQuery_intercepted)
  
  TRY_DLSYM("cuStreamAttachMemAsync", cuStreamAttachMemAsync_intercepted)
  
  TRY_DLSYM("cuStreamCopyAttributes", cuStreamCopyAttributes_intercepted)
  
  TRY_DLSYM("cuStreamGetAttribute", cuStreamGetAttribute_intercepted)
  
  TRY_DLSYM("cuStreamSetAttribute", cuStreamSetAttribute_intercepted)
  
  TRY_DLSYM("cuDeviceCanAccessPeer", cuDeviceCanAccessPeer_intercepted)
  
  TRY_DLSYM("cuCtxEnablePeerAccess", cuCtxEnablePeerAccess_intercepted)
  
  TRY_DLSYM("cuCtxDisablePeerAccess", cuCtxDisablePeerAccess_intercepted)
  
  TRY_DLSYM("cuIpcGetEventHandle", cuIpcGetEventHandle_intercepted)
  
  TRY_DLSYM("cuIpcOpenEventHandle", cuIpcOpenEventHandle_intercepted)
  
  TRY_DLSYM("cuIpcGetMemHandle", cuIpcGetMemHandle_intercepted)
  
  TRY_DLSYM("cuIpcOpenMemHandle", cuIpcOpenMemHandle_intercepted)
  
  TRY_DLSYM("cuIpcCloseMemHandle", cuIpcCloseMemHandle_intercepted)
  
  TRY_DLSYM("cuGraphicsUnregisterResource", cuGraphicsUnregisterResource_intercepted)
  
  TRY_DLSYM("cuGraphicsMapResources", cuGraphicsMapResources_intercepted)
  
  TRY_DLSYM("cuGraphicsUnmapResources", cuGraphicsUnmapResources_intercepted)
  
  TRY_DLSYM("cuGraphicsResourceSetMapFlags", cuGraphicsResourceSetMapFlags_intercepted)
  
  TRY_DLSYM("cuGraphicsSubResourceGetMappedArray", cuGraphicsSubResourceGetMappedArray_intercepted)
  
  TRY_DLSYM("cuGraphicsResourceGetMappedMipmappedArray", cuGraphicsResourceGetMappedMipmappedArray_intercepted)
  
  TRY_DLSYM("cuGraphicsResourceGetMappedPointer", cuGraphicsResourceGetMappedPointer_intercepted)
  
  TRY_DLSYM("cuGetExportTable", cuGetExportTable)
  
  TRY_DLSYM("cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags", cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags)
  
  TRY_DLSYM("cuOccupancyAvailableDynamicSMemPerBlock", cuOccupancyAvailableDynamicSMemPerBlock_intercepted)
  
  TRY_DLSYM("cuOccupancyMaxPotentialClusterSize", cuOccupancyMaxPotentialClusterSize_intercepted)
  
  TRY_DLSYM("cuOccupancyMaxActiveClusters", cuOccupancyMaxActiveClusters_intercepted)
  
  TRY_DLSYM("cuMemAdvise", cuMemAdvise_intercepted)
  
  TRY_DLSYM("cuMemPrefetchAsync", cuMemPrefetchAsync_intercepted)
  
  TRY_DLSYM("cuMemRangeGetAttribute", cuMemRangeGetAttribute_intercepted)
  
  TRY_DLSYM("cuMemRangeGetAttributes", cuMemRangeGetAttributes_intercepted)
  
  TRY_DLSYM("cuGetErrorString", cuGetErrorString_intercepted)
  
  TRY_DLSYM("cuGetErrorName", cuGetErrorName_intercepted)
  
  TRY_DLSYM("cuGraphCreate", cuGraphCreate_intercepted)
  
  TRY_DLSYM("cuGraphAddKernelNode", cuGraphAddKernelNode_intercepted)
  
  TRY_DLSYM("cuGraphKernelNodeGetParams", cuGraphKernelNodeGetParams_intercepted)
  
  TRY_DLSYM("cuGraphKernelNodeSetParams", cuGraphKernelNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphAddMemcpyNode", cuGraphAddMemcpyNode_intercepted)
  
  TRY_DLSYM("cuGraphMemcpyNodeGetParams", cuGraphMemcpyNodeGetParams_intercepted)
  
  TRY_DLSYM("cuGraphMemcpyNodeSetParams", cuGraphMemcpyNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphAddMemsetNode", cuGraphAddMemsetNode_intercepted)
  
  TRY_DLSYM("cuGraphMemsetNodeGetParams", cuGraphMemsetNodeGetParams_intercepted)
  
  TRY_DLSYM("cuGraphMemsetNodeSetParams", cuGraphMemsetNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphAddHostNode", cuGraphAddHostNode_intercepted)
  
  TRY_DLSYM("cuGraphHostNodeGetParams", cuGraphHostNodeGetParams_intercepted)
  
  TRY_DLSYM("cuGraphHostNodeSetParams", cuGraphHostNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphAddChildGraphNode", cuGraphAddChildGraphNode_intercepted)
  
  TRY_DLSYM("cuGraphChildGraphNodeGetGraph", cuGraphChildGraphNodeGetGraph_intercepted)
  
  TRY_DLSYM("cuGraphAddEmptyNode", cuGraphAddEmptyNode_intercepted)
  
  TRY_DLSYM("cuGraphAddEventRecordNode", cuGraphAddEventRecordNode_intercepted)
  
  TRY_DLSYM("cuGraphEventRecordNodeGetEvent", cuGraphEventRecordNodeGetEvent_intercepted)
  
  TRY_DLSYM("cuGraphEventRecordNodeSetEvent", cuGraphEventRecordNodeSetEvent_intercepted)
  
  TRY_DLSYM("cuGraphAddEventWaitNode", cuGraphAddEventWaitNode_intercepted)
  
  TRY_DLSYM("cuGraphEventWaitNodeGetEvent", cuGraphEventWaitNodeGetEvent_intercepted)
  
  TRY_DLSYM("cuGraphEventWaitNodeSetEvent", cuGraphEventWaitNodeSetEvent_intercepted)
  
  TRY_DLSYM("cuGraphAddExternalSemaphoresSignalNode", cuGraphAddExternalSemaphoresSignalNode_intercepted)
  
  TRY_DLSYM("cuGraphExternalSemaphoresSignalNodeGetParams", cuGraphExternalSemaphoresSignalNodeGetParams_intercepted)
  
  TRY_DLSYM("cuGraphExternalSemaphoresSignalNodeSetParams", cuGraphExternalSemaphoresSignalNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphAddExternalSemaphoresWaitNode", cuGraphAddExternalSemaphoresWaitNode_intercepted)
  
  TRY_DLSYM("cuGraphExternalSemaphoresWaitNodeGetParams", cuGraphExternalSemaphoresWaitNodeGetParams_intercepted)
  
  TRY_DLSYM("cuGraphExternalSemaphoresWaitNodeSetParams", cuGraphExternalSemaphoresWaitNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphExecExternalSemaphoresSignalNodeSetParams", cuGraphExecExternalSemaphoresSignalNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphExecExternalSemaphoresWaitNodeSetParams", cuGraphExecExternalSemaphoresWaitNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphAddMemAllocNode", cuGraphAddMemAllocNode_intercepted)
  
  TRY_DLSYM("cuGraphMemAllocNodeGetParams", cuGraphMemAllocNodeGetParams_intercepted)
  
  TRY_DLSYM("cuGraphAddMemFreeNode", cuGraphAddMemFreeNode_intercepted)
  
  TRY_DLSYM("cuGraphMemFreeNodeGetParams", cuGraphMemFreeNodeGetParams_intercepted)
  
  TRY_DLSYM("cuDeviceGraphMemTrim", cuDeviceGraphMemTrim_intercepted)
  
  TRY_DLSYM("cuDeviceGetGraphMemAttribute", cuDeviceGetGraphMemAttribute_intercepted)
  
  TRY_DLSYM("cuDeviceSetGraphMemAttribute", cuDeviceSetGraphMemAttribute_intercepted)
  
  TRY_DLSYM("cuGraphClone", cuGraphClone_intercepted)
  
  TRY_DLSYM("cuGraphNodeFindInClone", cuGraphNodeFindInClone_intercepted)
  
  TRY_DLSYM("cuGraphNodeGetType", cuGraphNodeGetType_intercepted)
  
  TRY_DLSYM("cuGraphGetNodes", cuGraphGetNodes_intercepted)
  
  TRY_DLSYM("cuGraphGetRootNodes", cuGraphGetRootNodes_intercepted)
  
  TRY_DLSYM("cuGraphGetEdges", cuGraphGetEdges_intercepted)
  
  TRY_DLSYM("cuGraphNodeGetDependencies", cuGraphNodeGetDependencies_intercepted)
  
  TRY_DLSYM("cuGraphNodeGetDependentNodes", cuGraphNodeGetDependentNodes_intercepted)
  
  TRY_DLSYM("cuGraphAddDependencies", cuGraphAddDependencies_intercepted)
  
  TRY_DLSYM("cuGraphRemoveDependencies", cuGraphRemoveDependencies_intercepted)
  
  TRY_DLSYM("cuGraphDestroyNode", cuGraphDestroyNode_intercepted)
  
  TRY_DLSYM("cuGraphInstantiate", cuGraphInstantiate_intercepted)
  
  TRY_DLSYM("cuGraphUpload", cuGraphUpload_intercepted)
  
  TRY_DLSYM("cuGraphLaunch", cuGraphLaunch_intercepted)
  
  TRY_DLSYM("cuGraphExecDestroy", cuGraphExecDestroy_intercepted)
  
  TRY_DLSYM("cuGraphDestroy", cuGraphDestroy_intercepted)
  
  TRY_DLSYM("cuStreamBeginCapture", cuStreamBeginCapture_intercepted)
  
  TRY_DLSYM("cuStreamBeginCaptureToGraph", cuStreamBeginCaptureToGraph_intercepted)
  
  TRY_DLSYM("cuStreamEndCapture", cuStreamEndCapture_intercepted)
  
  TRY_DLSYM("cuStreamIsCapturing", cuStreamIsCapturing)
  
  TRY_DLSYM("cuStreamGetCaptureInfo", cuStreamGetCaptureInfo_intercepted)
  
  TRY_DLSYM("cuStreamUpdateCaptureDependencies", cuStreamUpdateCaptureDependencies_intercepted)
  
  TRY_DLSYM("cuGraphExecKernelNodeSetParams", cuGraphExecKernelNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphExecMemcpyNodeSetParams", cuGraphExecMemcpyNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphExecMemsetNodeSetParams", cuGraphExecMemsetNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphExecHostNodeSetParams", cuGraphExecHostNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphExecChildGraphNodeSetParams", cuGraphExecChildGraphNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphExecEventRecordNodeSetEvent", cuGraphExecEventRecordNodeSetEvent_intercepted)
  
  TRY_DLSYM("cuGraphExecEventWaitNodeSetEvent", cuGraphExecEventWaitNodeSetEvent_intercepted)
  
  TRY_DLSYM("cuThreadExchangeStreamCaptureMode", cuThreadExchangeStreamCaptureMode_intercepted)
  
  TRY_DLSYM("cuGraphExecUpdate", cuGraphExecUpdate_intercepted)
  
  TRY_DLSYM("cuGraphKernelNodeCopyAttributes", cuGraphKernelNodeCopyAttributes_intercepted)
  
  TRY_DLSYM("cuGraphDebugDotPrint", cuGraphDebugDotPrint_intercepted)
  
  TRY_DLSYM("cuUserObjectRetain", cuUserObjectRetain_intercepted)
  
  TRY_DLSYM("cuUserObjectRelease", cuUserObjectRelease_intercepted)
  
  TRY_DLSYM("cuGraphRetainUserObject", cuGraphRetainUserObject_intercepted)
  
  TRY_DLSYM("cuGraphReleaseUserObject", cuGraphReleaseUserObject_intercepted)
  
  TRY_DLSYM("cuGraphNodeSetEnabled", cuGraphNodeSetEnabled_intercepted)
  
  TRY_DLSYM("cuGraphNodeGetEnabled", cuGraphNodeGetEnabled_intercepted)
  
  TRY_DLSYM("cuGraphInstantiateWithParams", cuGraphInstantiateWithParams_intercepted)
  
  TRY_DLSYM("cuGraphExecGetFlags", cuGraphExecGetFlags_intercepted)
  
  TRY_DLSYM("cuGraphAddNode", cuGraphAddNode_intercepted)
  
  TRY_DLSYM("cuGraphNodeSetParams", cuGraphNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphExecNodeSetParams", cuGraphExecNodeSetParams_intercepted)
  
  TRY_DLSYM("cuGraphConditionalHandleCreate", cuGraphConditionalHandleCreate_intercepted)
  
  TRY_DLSYM("cuDeviceRegisterAsyncNotification", cuDeviceRegisterAsyncNotification_intercepted)
  
  TRY_DLSYM("cuDeviceUnregisterAsyncNotification", cuDeviceUnregisterAsyncNotification_intercepted)
  
  TRY_DLSYM("cuLibraryLoadData", cuLibraryLoadData_intercepted)
  TRY_DLSYM("cuLibraryLoadFromFile", cuLibraryLoadFromFile_intercepted)
  TRY_DLSYM("cuLinkAddData", cuLinkAddData_intercepted)
  TRY_DLSYM("cuLinkAddFile", cuLinkAddFile_intercepted)
  TRY_DLSYM("cuLinkComplete", cuLinkComplete_intercepted)
  TRY_DLSYM("cuLinkDestroy", cuLinkDestroy_intercepted)
  TRY_DLSYM("cuMemPoolImportFromShareableHandle", cuMemPoolImportFromShareableHandle_intercepted)
  TRY_DLSYM("cuGLCtxCreate", cuGLCtxCreate_intercepted)
  TRY_DLSYM("cuGLInit", cuGLInit_intercepted)
  // NO_INTERCEPT("cuGLGetDevices")
  // NO_INTERCEPT("cuGLRegisterBufferObject")
  // NO_INTERCEPT("cuGLMapBufferObject")
  // NO_INTERCEPT("cuGLMapBufferObjectAsync")
  // NO_INTERCEPT("cuGLUnmapBufferObject")
  // NO_INTERCEPT("cuGLUnmapBufferObjectAsync")
  // NO_INTERCEPT("cuGLUnregisterBufferObject")
  // NO_INTERCEPT("cuGLSetBufferObjectMapFlags")
  // NO_INTERCEPT("cuGraphicsGLRegisterImage")
  // NO_INTERCEPT("cuGraphicsGLRegisterBuffer")
  // NO_INTERCEPT("cuGraphicsEGLRegisterImage")
  // NO_INTERCEPT("cuEGLStreamConsumerConnect")
  // NO_INTERCEPT("cuEGLStreamConsumerDisconnect")
  // NO_INTERCEPT("cuEGLStreamConsumerAcquireFrame")
  // NO_INTERCEPT("cuEGLStreamConsumerReleaseFrame")
  // NO_INTERCEPT("cuEGLStreamProducerConnect")
  // NO_INTERCEPT("cuEGLStreamProducerDisconnect")
  // NO_INTERCEPT("cuEGLStreamProducerPresentFrame")
  // NO_INTERCEPT("cuEGLStreamProducerReturnFrame")
  // NO_INTERCEPT("cuGraphicsResourceGetMappedEglFrame")
  // NO_INTERCEPT("cuEGLStreamConsumerConnectWithFlags")
  // NO_INTERCEPT("cuProfilerInitialize")
  TRY_DLSYM("cuProfilerStart", cuProfilerStart_intercepted)
  TRY_DLSYM("cuProfilerStop", cuProfilerStop_intercepted)
  // NO_INTERCEPT("cuVDPAUGetDevice")
  // NO_INTERCEPT("cuVDPAUCtxCreate")
  // NO_INTERCEPT("cuGraphicsVDPAURegisterVideoSurface")
  // NO_INTERCEPT("cuGraphicsVDPAURegisterOutputSurface")
  TRY_DLSYM("cuGraphInstantiateWithFlags", cuGraphInstantiateWithFlags_intercepted)
  TRY_DLSYM("cuGraphKernelNodeGetAttribute", cuGraphKernelNodeGetAttribute_intercepted)
  TRY_DLSYM("cuGraphKernelNodeSetAttribute", cuGraphKernelNodeSetAttribute_intercepted)
  TRY_DLSYM("cuUserObjectCreate", cuUserObjectCreate_intercepted)
  TRY_DLSYM("cuOccupancyMaxPotentialBlockSize", cuOccupancyMaxPotentialBlockSize)
  // NO_INTERCEPT("cuGraphInstantiateWithParams_ptsz")
  void *result = real_dlsym(handle, symbol);
  assert(result != nullptr);
  // for all other func
  return result;
}
//==================================================================================================================

/* Interception version for `cuGetProcAddress` and all needed CUDA funcs */
extern "C" CUresult getProcAddressBySymbol(const char* symbol, void** pfn, int cudaVersion, cuuint64_t flags,
  CUdriverProcAddressQueryResult* symbolStatus) {
// printf("Intercepted cuGetProcAddress: symbol=%s\n", symbol);
assert(symbol != nullptr);
if(strcmp(symbol, "cuGetProcAddress") == 0){
  *pfn = (void *) &getProcAddressBySymbol;
  return CUDA_SUCCESS;
}
TRY_INTERCEPT("cuGetProcAddress", getProcAddressBySymbol)
TRY_INTERCEPT("cuInit", cuInit)

TRY_INTERCEPT("cuDeviceGet", cuDeviceGet)

TRY_INTERCEPT("cuDeviceGetCount", cuDeviceGetCount)

TRY_INTERCEPT("cuDeviceGetName", cuDeviceGetName)

TRY_INTERCEPT("cuDeviceTotalMem", cuDeviceTotalMem_v2)

TRY_INTERCEPT("cuDeviceGetAttribute", cuDeviceGetAttribute)

TRY_INTERCEPT("cuDeviceGetP2PAttribute", cuDeviceGetP2PAttribute_intercepted)

TRY_INTERCEPT("cuDriverGetVersion", cuDriverGetVersion)

TRY_INTERCEPT("cuDeviceGetByPCIBusId", cuDeviceGetByPCIBusId_intercepted)

TRY_INTERCEPT("cuDeviceGetPCIBusId", cuDeviceGetPCIBusId_intercepted)

TRY_INTERCEPT("cuDeviceGetUuid", cuDeviceGetUuid)

TRY_INTERCEPT("cuDeviceGetTexture1DLinearMaxWidth", cuDeviceGetTexture1DLinearMaxWidth_intercepted)

TRY_INTERCEPT("cuDeviceGetDefaultMemPool", cuDeviceGetDefaultMemPool_intercepted)

TRY_INTERCEPT("cuDeviceSetMemPool", cuDeviceSetMemPool_intercepted)

TRY_INTERCEPT("cuDeviceGetMemPool", cuDeviceGetMemPool_intercepted)

TRY_INTERCEPT("cuFlushGPUDirectRDMAWrites", cuFlushGPUDirectRDMAWrites_intercepted)

TRY_INTERCEPT("cuDevicePrimaryCtxRetain", cuDevicePrimaryCtxRetain)

TRY_INTERCEPT("cuDevicePrimaryCtxRelease", cuDevicePrimaryCtxRelease)

TRY_INTERCEPT("cuDevicePrimaryCtxSetFlags", cuDevicePrimaryCtxSetFlags_intercepted)

TRY_INTERCEPT("cuDevicePrimaryCtxGetState", cuDevicePrimaryCtxGetState_intercepted)

TRY_INTERCEPT("cuDevicePrimaryCtxReset", cuDevicePrimaryCtxReset_intercepted)

TRY_INTERCEPT("cuCtxCreate", cuCtxCreate)

TRY_INTERCEPT("cuCtxGetFlags", cuCtxGetFlags_intercepted)

TRY_INTERCEPT("cuCtxSetCurrent", cuCtxSetCurrent)

TRY_INTERCEPT("cuCtxGetCurrent", cuCtxGetCurrent)

TRY_INTERCEPT("cuCtxDetach", cuCtxDetach_intercepted)

TRY_INTERCEPT("cuCtxGetApiVersion", cuCtxGetApiVersion_intercepted)

TRY_INTERCEPT("cuCtxGetDevice", cuCtxGetDevice)

TRY_INTERCEPT("cuCtxGetLimit", cuCtxGetLimit_intercepted)

TRY_INTERCEPT("cuCtxSetLimit", cuCtxSetLimit_intercepted)

TRY_INTERCEPT("cuCtxGetCacheConfig", cuCtxGetCacheConfig_intercepted)

TRY_INTERCEPT("cuCtxSetCacheConfig", cuCtxSetCacheConfig_intercepted)

TRY_INTERCEPT("cuCtxGetSharedMemConfig", cuCtxGetSharedMemConfig_intercepted)

TRY_INTERCEPT("cuCtxGetStreamPriorityRange", cuCtxGetStreamPriorityRange)

TRY_INTERCEPT("cuCtxSetSharedMemConfig", cuCtxSetSharedMemConfig_intercepted)

TRY_INTERCEPT("cuCtxSynchronize", cuCtxSynchronize_intercepted)

TRY_INTERCEPT("cuCtxResetPersistingL2Cache", cuCtxResetPersistingL2Cache_intercepted)

TRY_INTERCEPT("cuCtxPopCurrent", cuCtxPopCurrent)

TRY_INTERCEPT("cuCtxPushCurrent", cuCtxPushCurrent)

TRY_INTERCEPT("cuModuleLoad", cuModuleLoad)

TRY_INTERCEPT("cuModuleLoadData", cuModuleLoadData)

TRY_INTERCEPT("cuModuleLoadFatBinary", cuModuleLoadFatBinary_intercepted)

TRY_INTERCEPT("cuModuleUnload", cuModuleUnload)

TRY_INTERCEPT("cuModuleGetFunction", cuModuleGetFunction)

TRY_INTERCEPT("cuModuleGetGlobal", cuModuleGetGlobal)

TRY_INTERCEPT("cuModuleGetTexRef", cuModuleGetTexRef_intercepted)

TRY_INTERCEPT("cuModuleGetSurfRef", cuModuleGetSurfRef_intercepted)

TRY_INTERCEPT("cuModuleGetLoadingMode", cuModuleGetLoadingMode)

TRY_INTERCEPT("cuLibraryUnload", cuLibraryUnload)

TRY_INTERCEPT("cuLibraryGetKernel", cuLibraryGetKernel_intercepted)

TRY_INTERCEPT("cuLibraryGetModule", cuLibraryGetModule)

TRY_INTERCEPT("cuKernelGetFunction", cuKernelGetFunction_intercepted)

TRY_INTERCEPT("cuLibraryGetGlobal", cuLibraryGetGlobal_intercepted)

TRY_INTERCEPT("cuLibraryGetManaged", cuLibraryGetManaged_intercepted)

TRY_INTERCEPT("cuLibraryGetUnifiedFunction", cuLibraryGetUnifiedFunction_intercepted)

TRY_INTERCEPT("cuLibraryGetKernelCount", cuLibraryGetKernelCount_intercepted)

TRY_INTERCEPT("cuLibraryEnumerateKernels", cuLibraryEnumerateKernels_intercepted)

TRY_INTERCEPT("cuKernelGetAttribute", cuKernelGetAttribute_intercepted)

TRY_INTERCEPT("cuKernelSetAttribute", cuKernelSetAttribute_intercepted)

TRY_INTERCEPT("cuKernelSetCacheConfig", cuKernelSetCacheConfig_intercepted)

TRY_INTERCEPT("cuKernelGetName", cuKernelGetName_intercepted)

TRY_INTERCEPT("cuKernelGetParamInfo", cuKernelGetParamInfo_intercepted)

TRY_INTERCEPT("cuLinkCreate", cuLinkCreate_intercepted)

TRY_INTERCEPT("cuMemGetInfo", cuMemGetInfo_intercepted)

TRY_INTERCEPT("cuMemAllocManaged", cuMemAllocManaged_intercepted)

TRY_INTERCEPT("cuMemAlloc", cuMemAlloc)

TRY_INTERCEPT("cuMemAllocPitch", cuMemAllocPitch_intercepted)

TRY_INTERCEPT("cuMemFree", cuMemFree)

TRY_INTERCEPT("cuMemGetAddressRange", cuMemGetAddressRange_intercepted)

TRY_INTERCEPT("cuMemFreeHost", cuMemFreeHost_intercepted)

TRY_INTERCEPT("cuMemHostAlloc", cuMemHostAlloc)

TRY_INTERCEPT("cuMemHostGetDevicePointer", cuMemHostGetDevicePointer)

TRY_INTERCEPT("cuMemHostGetFlags", cuMemHostGetFlags_intercepted)

TRY_INTERCEPT("cuMemHostRegister", cuMemHostRegister_intercepted)

TRY_INTERCEPT("cuMemHostUnregister", cuMemHostUnregister_intercepted)

TRY_INTERCEPT("cuPointerGetAttribute", cuPointerGetAttribute_intercepted)

TRY_INTERCEPT("cuPointerGetAttributes", cuPointerGetAttributes_intercepted)

TRY_INTERCEPT("cuMemAllocAsync", cuMemAllocAsync_intercepted)

TRY_INTERCEPT("cuMemAllocFromPoolAsync", cuMemAllocFromPoolAsync_intercepted)

TRY_INTERCEPT("cuMemFreeAsync", cuMemFreeAsync_intercepted)

TRY_INTERCEPT("cuMemPoolTrimTo", cuMemPoolTrimTo_intercepted)

TRY_INTERCEPT("cuMemPoolSetAttribute", cuMemPoolSetAttribute_intercepted)

TRY_INTERCEPT("cuMemPoolGetAttribute", cuMemPoolGetAttribute_intercepted)

TRY_INTERCEPT("cuMemPoolSetAccess", cuMemPoolSetAccess_intercepted)

TRY_INTERCEPT("cuMemPoolGetAccess", cuMemPoolGetAccess_intercepted)

TRY_INTERCEPT("cuMemPoolCreate", cuMemPoolCreate_intercepted)

TRY_INTERCEPT("cuMemPoolDestroy", cuMemPoolDestroy_intercepted)

TRY_INTERCEPT("cuMemPoolExportToShareableHandle", cuMemPoolExportToShareableHandle_intercepted)

TRY_INTERCEPT("cuMemPoolExportPointer", cuMemPoolExportPointer_intercepted)

TRY_INTERCEPT("cuMemPoolImportPointer", cuMemPoolImportPointer_intercepted)

TRY_INTERCEPT("cuMemcpy", cuMemcpy_intercepted)

TRY_INTERCEPT("cuMemcpyAsync", cuMemcpyAsync_intercepted)

TRY_INTERCEPT("cuMemcpyPeer", cuMemcpyPeer_intercepted)

TRY_INTERCEPT("cuMemcpyPeerAsync", cuMemcpyPeerAsync_intercepted)

TRY_INTERCEPT("cuMemcpyHtoD", cuMemcpyHtoD)

// TRY_INTERCEPT("cuMemcpyHtoDAsync", cuMemcpyHtoDAsync)

TRY_INTERCEPT("cuMemcpyDtoH", cuMemcpyDtoH)

TRY_INTERCEPT("cuMemcpyDtoHAsync", cuMemcpyDtoHAsync_intercepted)

TRY_INTERCEPT("cuMemcpyDtoD", cuMemcpyDtoD_intercepted)

TRY_INTERCEPT("cuMemcpyDtoDAsync", cuMemcpyDtoDAsync_intercepted)

TRY_INTERCEPT("cuMemcpy2DUnaligned", cuMemcpy2DUnaligned_intercepted)

TRY_INTERCEPT("cuMemcpy2DAsync", cuMemcpy2DAsync_intercepted)

TRY_INTERCEPT("cuMemcpy3D", cuMemcpy3D_intercepted)

TRY_INTERCEPT("cuMemcpy3DAsync", cuMemcpy3DAsync_intercepted)

TRY_INTERCEPT("cuMemcpy3DPeer", cuMemcpy3DPeer_intercepted)

TRY_INTERCEPT("cuMemcpy3DPeerAsync", cuMemcpy3DPeerAsync_intercepted)

TRY_INTERCEPT("cuMemsetD8", cuMemsetD8_intercepted)

TRY_INTERCEPT("cuMemsetD8Async", cuMemsetD8Async)

TRY_INTERCEPT("cuMemsetD2D8", cuMemsetD2D8_intercepted)

TRY_INTERCEPT("cuMemsetD2D8Async", cuMemsetD2D8Async_intercepted)

TRY_INTERCEPT("cuFuncSetCacheConfig", cuFuncSetCacheConfig_intercepted)

TRY_INTERCEPT("cuFuncSetSharedMemConfig", cuFuncSetSharedMemConfig_intercepted)

TRY_INTERCEPT("cuFuncGetAttribute", cuFuncGetAttribute_intercepted)

TRY_INTERCEPT("cuFuncSetAttribute", cuFuncSetAttribute_intercepted)

TRY_INTERCEPT("cuFuncGetName", cuFuncGetName_intercepted)

TRY_INTERCEPT("cuFuncGetParamInfo", cuFuncGetParamInfo_intercepted)

TRY_INTERCEPT("cuArrayCreate", cuArrayCreate_intercepted)

TRY_INTERCEPT("cuArrayGetDescriptor", cuArrayGetDescriptor_intercepted)

TRY_INTERCEPT("cuArrayGetSparseProperties", cuArrayGetSparseProperties_intercepted)

TRY_INTERCEPT("cuArrayGetPlane", cuArrayGetPlane_intercepted)

TRY_INTERCEPT("cuArray3DCreate", cuArray3DCreate_intercepted)

TRY_INTERCEPT("cuArray3DGetDescriptor", cuArray3DGetDescriptor_intercepted)

TRY_INTERCEPT("cuArrayDestroy", cuArrayDestroy_intercepted)

TRY_INTERCEPT("cuMipmappedArrayCreate", cuMipmappedArrayCreate_intercepted)

TRY_INTERCEPT("cuMipmappedArrayGetLevel", cuMipmappedArrayGetLevel_intercepted)

TRY_INTERCEPT("cuMipmappedArrayGetSparseProperties", cuMipmappedArrayGetSparseProperties_intercepted)

TRY_INTERCEPT("cuMipmappedArrayDestroy", cuMipmappedArrayDestroy_intercepted)

TRY_INTERCEPT("cuArrayGetMemoryRequirements", cuArrayGetMemoryRequirements_intercepted)

TRY_INTERCEPT("cuMipmappedArrayGetMemoryRequirements", cuMipmappedArrayGetMemoryRequirements_intercepted)

TRY_INTERCEPT("cuTexObjectCreate", cuTexObjectCreate_intercepted)

TRY_INTERCEPT("cuTexObjectDestroy", cuTexObjectDestroy_intercepted)

TRY_INTERCEPT("cuTexObjectGetResourceDesc", cuTexObjectGetResourceDesc_intercepted)

TRY_INTERCEPT("cuTexObjectGetTextureDesc", cuTexObjectGetTextureDesc_intercepted)

TRY_INTERCEPT("cuTexObjectGetResourceViewDesc", cuTexObjectGetResourceViewDesc_intercepted)

TRY_INTERCEPT("cuSurfObjectCreate", cuSurfObjectCreate_intercepted)

TRY_INTERCEPT("cuSurfObjectDestroy", cuSurfObjectDestroy_intercepted)

TRY_INTERCEPT("cuSurfObjectGetResourceDesc", cuSurfObjectGetResourceDesc_intercepted)

TRY_INTERCEPT("cuImportExternalMemory", cuImportExternalMemory_intercepted)

TRY_INTERCEPT("cuExternalMemoryGetMappedBuffer", cuExternalMemoryGetMappedBuffer_intercepted)

TRY_INTERCEPT("cuExternalMemoryGetMappedMipmappedArray", cuExternalMemoryGetMappedMipmappedArray_intercepted)

TRY_INTERCEPT("cuDestroyExternalMemory", cuDestroyExternalMemory_intercepted)

TRY_INTERCEPT("cuImportExternalSemaphore", cuImportExternalSemaphore_intercepted)

TRY_INTERCEPT("cuSignalExternalSemaphoresAsync", cuSignalExternalSemaphoresAsync_intercepted)

TRY_INTERCEPT("cuWaitExternalSemaphoresAsync", cuWaitExternalSemaphoresAsync_intercepted)

TRY_INTERCEPT("cuDestroyExternalSemaphore", cuDestroyExternalSemaphore_intercepted)

TRY_INTERCEPT("cuDeviceGetNvSciSyncAttributes", cuDeviceGetNvSciSyncAttributes_intercepted)

TRY_INTERCEPT("cuLaunchKernel", cuLaunchKernel)

TRY_INTERCEPT("cuLaunchCooperativeKernel", cuLaunchCooperativeKernel_intercepted)

TRY_INTERCEPT("cuLaunchCooperativeKernelMultiDevice", cuLaunchCooperativeKernelMultiDevice_intercepted)

TRY_INTERCEPT("cuLaunchHostFunc", cuLaunchHostFunc_intercepted)

TRY_INTERCEPT("cuLaunchKernelEx", cuLaunchKernelEx_intercepted)

TRY_INTERCEPT("cuEventCreate", cuEventCreate)

TRY_INTERCEPT("cuEventRecord", cuEventRecord)

TRY_INTERCEPT("cuEventRecordWithFlags", cuEventRecordWithFlags_intercepted)

TRY_INTERCEPT("cuEventQuery", cuEventQuery_intercepted)

TRY_INTERCEPT("cuEventSynchronize", cuEventSynchronize_intercepted)

TRY_INTERCEPT("cuEventDestroy", cuEventDestroy_intercepted)

TRY_INTERCEPT("cuEventElapsedTime", cuEventElapsedTime_intercepted)

TRY_INTERCEPT("cuStreamWaitValue32", cuStreamWaitValue32_intercepted)

TRY_INTERCEPT("cuStreamWriteValue32", cuStreamWriteValue32_intercepted)

TRY_INTERCEPT("cuStreamWaitValue64", cuStreamWaitValue64_intercepted)

TRY_INTERCEPT("cuStreamWriteValue64", cuStreamWriteValue64_intercepted)

TRY_INTERCEPT("cuStreamBatchMemOp", cuStreamBatchMemOp_intercepted)

TRY_INTERCEPT("cuStreamCreate", cuStreamCreate)

TRY_INTERCEPT("cuStreamCreateWithPriority", cuStreamCreateWithPriority_intercepted)

TRY_INTERCEPT("cuStreamGetPriority", cuStreamGetPriority)

TRY_INTERCEPT("cuStreamGetFlags", cuStreamGetFlags_intercepted)

TRY_INTERCEPT("cuStreamGetCtx", cuStreamGetCtx_intercepted)

TRY_INTERCEPT("cuStreamGetId", cuStreamGetId_intercepted)

TRY_INTERCEPT("cuStreamDestroy", cuStreamDestroy_intercepted)

TRY_INTERCEPT("cuStreamWaitEvent", cuStreamWaitEvent_intercepted)

TRY_INTERCEPT("cuStreamAddCallback", cuStreamAddCallback_intercepted)

TRY_INTERCEPT("cuStreamSynchronize", cuStreamSynchronize)

TRY_INTERCEPT("cuStreamQuery", cuStreamQuery_intercepted)

TRY_INTERCEPT("cuStreamAttachMemAsync", cuStreamAttachMemAsync_intercepted)

TRY_INTERCEPT("cuStreamCopyAttributes", cuStreamCopyAttributes_intercepted)

TRY_INTERCEPT("cuStreamGetAttribute", cuStreamGetAttribute_intercepted)

TRY_INTERCEPT("cuStreamSetAttribute", cuStreamSetAttribute_intercepted)

TRY_INTERCEPT("cuDeviceCanAccessPeer", cuDeviceCanAccessPeer_intercepted)

TRY_INTERCEPT("cuCtxEnablePeerAccess", cuCtxEnablePeerAccess_intercepted)

TRY_INTERCEPT("cuCtxDisablePeerAccess", cuCtxDisablePeerAccess_intercepted)

TRY_INTERCEPT("cuIpcGetEventHandle", cuIpcGetEventHandle_intercepted)

TRY_INTERCEPT("cuIpcOpenEventHandle", cuIpcOpenEventHandle_intercepted)

TRY_INTERCEPT("cuIpcGetMemHandle", cuIpcGetMemHandle_intercepted)

TRY_INTERCEPT("cuIpcOpenMemHandle", cuIpcOpenMemHandle_intercepted)

TRY_INTERCEPT("cuIpcCloseMemHandle", cuIpcCloseMemHandle_intercepted)

TRY_INTERCEPT("cuGraphicsUnregisterResource", cuGraphicsUnregisterResource_intercepted)

TRY_INTERCEPT("cuGraphicsMapResources", cuGraphicsMapResources_intercepted)

TRY_INTERCEPT("cuGraphicsUnmapResources", cuGraphicsUnmapResources_intercepted)

TRY_INTERCEPT("cuGraphicsResourceSetMapFlags", cuGraphicsResourceSetMapFlags_intercepted)

TRY_INTERCEPT("cuGraphicsSubResourceGetMappedArray", cuGraphicsSubResourceGetMappedArray_intercepted)

TRY_INTERCEPT("cuGraphicsResourceGetMappedMipmappedArray", cuGraphicsResourceGetMappedMipmappedArray_intercepted)

TRY_INTERCEPT("cuGraphicsResourceGetMappedPointer", cuGraphicsResourceGetMappedPointer_intercepted)

TRY_INTERCEPT("cuGetExportTable", cuGetExportTable)

TRY_INTERCEPT("cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags", cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags)

TRY_INTERCEPT("cuOccupancyAvailableDynamicSMemPerBlock", cuOccupancyAvailableDynamicSMemPerBlock_intercepted)

TRY_INTERCEPT("cuOccupancyMaxPotentialClusterSize", cuOccupancyMaxPotentialClusterSize_intercepted)

TRY_INTERCEPT("cuOccupancyMaxActiveClusters", cuOccupancyMaxActiveClusters_intercepted)

TRY_INTERCEPT("cuMemAdvise", cuMemAdvise_intercepted)

TRY_INTERCEPT("cuMemPrefetchAsync", cuMemPrefetchAsync_intercepted)

TRY_INTERCEPT("cuMemRangeGetAttribute", cuMemRangeGetAttribute_intercepted)

TRY_INTERCEPT("cuMemRangeGetAttributes", cuMemRangeGetAttributes_intercepted)

TRY_INTERCEPT("cuGetErrorString", cuGetErrorString_intercepted)

TRY_INTERCEPT("cuGetErrorName", cuGetErrorName_intercepted)

TRY_INTERCEPT("cuGraphCreate", cuGraphCreate_intercepted)

TRY_INTERCEPT("cuGraphAddKernelNode", cuGraphAddKernelNode_intercepted)

TRY_INTERCEPT("cuGraphKernelNodeGetParams", cuGraphKernelNodeGetParams_intercepted)

TRY_INTERCEPT("cuGraphKernelNodeSetParams", cuGraphKernelNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphAddMemcpyNode", cuGraphAddMemcpyNode_intercepted)

TRY_INTERCEPT("cuGraphMemcpyNodeGetParams", cuGraphMemcpyNodeGetParams_intercepted)

TRY_INTERCEPT("cuGraphMemcpyNodeSetParams", cuGraphMemcpyNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphAddMemsetNode", cuGraphAddMemsetNode_intercepted)

TRY_INTERCEPT("cuGraphMemsetNodeGetParams", cuGraphMemsetNodeGetParams_intercepted)

TRY_INTERCEPT("cuGraphMemsetNodeSetParams", cuGraphMemsetNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphAddHostNode", cuGraphAddHostNode_intercepted)

TRY_INTERCEPT("cuGraphHostNodeGetParams", cuGraphHostNodeGetParams_intercepted)

TRY_INTERCEPT("cuGraphHostNodeSetParams", cuGraphHostNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphAddChildGraphNode", cuGraphAddChildGraphNode_intercepted)

TRY_INTERCEPT("cuGraphChildGraphNodeGetGraph", cuGraphChildGraphNodeGetGraph_intercepted)

TRY_INTERCEPT("cuGraphAddEmptyNode", cuGraphAddEmptyNode_intercepted)

TRY_INTERCEPT("cuGraphAddEventRecordNode", cuGraphAddEventRecordNode_intercepted)

TRY_INTERCEPT("cuGraphEventRecordNodeGetEvent", cuGraphEventRecordNodeGetEvent_intercepted)

TRY_INTERCEPT("cuGraphEventRecordNodeSetEvent", cuGraphEventRecordNodeSetEvent_intercepted)

TRY_INTERCEPT("cuGraphAddEventWaitNode", cuGraphAddEventWaitNode_intercepted)

TRY_INTERCEPT("cuGraphEventWaitNodeGetEvent", cuGraphEventWaitNodeGetEvent_intercepted)

TRY_INTERCEPT("cuGraphEventWaitNodeSetEvent", cuGraphEventWaitNodeSetEvent_intercepted)

TRY_INTERCEPT("cuGraphAddExternalSemaphoresSignalNode", cuGraphAddExternalSemaphoresSignalNode_intercepted)

TRY_INTERCEPT("cuGraphExternalSemaphoresSignalNodeGetParams", cuGraphExternalSemaphoresSignalNodeGetParams_intercepted)

TRY_INTERCEPT("cuGraphExternalSemaphoresSignalNodeSetParams", cuGraphExternalSemaphoresSignalNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphAddExternalSemaphoresWaitNode", cuGraphAddExternalSemaphoresWaitNode_intercepted)

TRY_INTERCEPT("cuGraphExternalSemaphoresWaitNodeGetParams", cuGraphExternalSemaphoresWaitNodeGetParams_intercepted)

TRY_INTERCEPT("cuGraphExternalSemaphoresWaitNodeSetParams", cuGraphExternalSemaphoresWaitNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphExecExternalSemaphoresSignalNodeSetParams", cuGraphExecExternalSemaphoresSignalNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphExecExternalSemaphoresWaitNodeSetParams", cuGraphExecExternalSemaphoresWaitNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphAddMemAllocNode", cuGraphAddMemAllocNode_intercepted)

TRY_INTERCEPT("cuGraphMemAllocNodeGetParams", cuGraphMemAllocNodeGetParams_intercepted)

TRY_INTERCEPT("cuGraphAddMemFreeNode", cuGraphAddMemFreeNode_intercepted)

TRY_INTERCEPT("cuGraphMemFreeNodeGetParams", cuGraphMemFreeNodeGetParams_intercepted)

TRY_INTERCEPT("cuDeviceGraphMemTrim", cuDeviceGraphMemTrim_intercepted)

TRY_INTERCEPT("cuDeviceGetGraphMemAttribute", cuDeviceGetGraphMemAttribute_intercepted)

TRY_INTERCEPT("cuDeviceSetGraphMemAttribute", cuDeviceSetGraphMemAttribute_intercepted)

TRY_INTERCEPT("cuGraphClone", cuGraphClone_intercepted)

TRY_INTERCEPT("cuGraphNodeFindInClone", cuGraphNodeFindInClone_intercepted)

TRY_INTERCEPT("cuGraphNodeGetType", cuGraphNodeGetType_intercepted)

TRY_INTERCEPT("cuGraphGetNodes", cuGraphGetNodes_intercepted)

TRY_INTERCEPT("cuGraphGetRootNodes", cuGraphGetRootNodes_intercepted)

TRY_INTERCEPT("cuGraphGetEdges", cuGraphGetEdges_intercepted)

TRY_INTERCEPT("cuGraphNodeGetDependencies", cuGraphNodeGetDependencies_intercepted)

TRY_INTERCEPT("cuGraphNodeGetDependentNodes", cuGraphNodeGetDependentNodes_intercepted)

TRY_INTERCEPT("cuGraphAddDependencies", cuGraphAddDependencies_intercepted)

TRY_INTERCEPT("cuGraphRemoveDependencies", cuGraphRemoveDependencies_intercepted)

TRY_INTERCEPT("cuGraphDestroyNode", cuGraphDestroyNode_intercepted)

TRY_INTERCEPT("cuGraphInstantiate", cuGraphInstantiate_intercepted)

TRY_INTERCEPT("cuGraphUpload", cuGraphUpload_intercepted)

TRY_INTERCEPT("cuGraphLaunch", cuGraphLaunch_intercepted)

TRY_INTERCEPT("cuGraphExecDestroy", cuGraphExecDestroy_intercepted)

TRY_INTERCEPT("cuGraphDestroy", cuGraphDestroy_intercepted)

TRY_INTERCEPT("cuStreamBeginCapture", cuStreamBeginCapture_intercepted)

TRY_INTERCEPT("cuStreamBeginCaptureToGraph", cuStreamBeginCaptureToGraph_intercepted)

TRY_INTERCEPT("cuStreamEndCapture", cuStreamEndCapture_intercepted)

TRY_INTERCEPT("cuStreamIsCapturing", cuStreamIsCapturing)

TRY_INTERCEPT("cuStreamGetCaptureInfo", cuStreamGetCaptureInfo_intercepted)

TRY_INTERCEPT("cuStreamUpdateCaptureDependencies", cuStreamUpdateCaptureDependencies_intercepted)

TRY_INTERCEPT("cuGraphExecKernelNodeSetParams", cuGraphExecKernelNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphExecMemcpyNodeSetParams", cuGraphExecMemcpyNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphExecMemsetNodeSetParams", cuGraphExecMemsetNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphExecHostNodeSetParams", cuGraphExecHostNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphExecChildGraphNodeSetParams", cuGraphExecChildGraphNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphExecEventRecordNodeSetEvent", cuGraphExecEventRecordNodeSetEvent_intercepted)

TRY_INTERCEPT("cuGraphExecEventWaitNodeSetEvent", cuGraphExecEventWaitNodeSetEvent_intercepted)

TRY_INTERCEPT("cuThreadExchangeStreamCaptureMode", cuThreadExchangeStreamCaptureMode_intercepted)

TRY_INTERCEPT("cuGraphExecUpdate", cuGraphExecUpdate_intercepted)

TRY_INTERCEPT("cuGraphKernelNodeCopyAttributes", cuGraphKernelNodeCopyAttributes_intercepted)

TRY_INTERCEPT("cuGraphDebugDotPrint", cuGraphDebugDotPrint_intercepted)

TRY_INTERCEPT("cuUserObjectRetain", cuUserObjectRetain_intercepted)

TRY_INTERCEPT("cuUserObjectRelease", cuUserObjectRelease_intercepted)

TRY_INTERCEPT("cuGraphRetainUserObject", cuGraphRetainUserObject_intercepted)

TRY_INTERCEPT("cuGraphReleaseUserObject", cuGraphReleaseUserObject_intercepted)

TRY_INTERCEPT("cuGraphNodeSetEnabled", cuGraphNodeSetEnabled_intercepted)

TRY_INTERCEPT("cuGraphNodeGetEnabled", cuGraphNodeGetEnabled_intercepted)

TRY_INTERCEPT("cuGraphInstantiateWithParams", cuGraphInstantiateWithParams_intercepted)

TRY_INTERCEPT("cuGraphExecGetFlags", cuGraphExecGetFlags_intercepted)

TRY_INTERCEPT("cuGraphAddNode", cuGraphAddNode_intercepted)

TRY_INTERCEPT("cuGraphNodeSetParams", cuGraphNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphExecNodeSetParams", cuGraphExecNodeSetParams_intercepted)

TRY_INTERCEPT("cuGraphConditionalHandleCreate", cuGraphConditionalHandleCreate_intercepted)

TRY_INTERCEPT("cuDeviceRegisterAsyncNotification", cuDeviceRegisterAsyncNotification_intercepted)

TRY_INTERCEPT("cuDeviceUnregisterAsyncNotification", cuDeviceUnregisterAsyncNotification_intercepted)

TRY_INTERCEPT("cuLibraryLoadData", cuLibraryLoadData_intercepted)
TRY_INTERCEPT("cuLibraryLoadFromFile", cuLibraryLoadFromFile_intercepted)
TRY_INTERCEPT("cuLinkAddData", cuLinkAddData_intercepted)
TRY_INTERCEPT("cuLinkAddFile", cuLinkAddFile_intercepted)
TRY_INTERCEPT("cuLinkComplete", cuLinkComplete_intercepted)
TRY_INTERCEPT("cuLinkDestroy", cuLinkDestroy_intercepted)
TRY_INTERCEPT("cuMemPoolImportFromShareableHandle", cuMemPoolImportFromShareableHandle_intercepted)
TRY_INTERCEPT("cuGLCtxCreate", cuGLCtxCreate_intercepted)
TRY_INTERCEPT("cuGLInit", cuGLInit_intercepted)
// NO_INTERCEPT("cuGLGetDevices")
// NO_INTERCEPT("cuGLRegisterBufferObject")
// NO_INTERCEPT("cuGLMapBufferObject")
// NO_INTERCEPT("cuGLMapBufferObjectAsync")
// NO_INTERCEPT("cuGLUnmapBufferObject")
// NO_INTERCEPT("cuGLUnmapBufferObjectAsync")
// NO_INTERCEPT("cuGLUnregisterBufferObject")
// NO_INTERCEPT("cuGLSetBufferObjectMapFlags")
// NO_INTERCEPT("cuGraphicsGLRegisterImage")
// NO_INTERCEPT("cuGraphicsGLRegisterBuffer")
// NO_INTERCEPT("cuGraphicsEGLRegisterImage")
// NO_INTERCEPT("cuEGLStreamConsumerConnect")
// NO_INTERCEPT("cuEGLStreamConsumerDisconnect")
// NO_INTERCEPT("cuEGLStreamConsumerAcquireFrame")
// NO_INTERCEPT("cuEGLStreamConsumerReleaseFrame")
// NO_INTERCEPT("cuEGLStreamProducerConnect")
// NO_INTERCEPT("cuEGLStreamProducerDisconnect")
// NO_INTERCEPT("cuEGLStreamProducerPresentFrame")
// NO_INTERCEPT("cuEGLStreamProducerReturnFrame")
// NO_INTERCEPT("cuGraphicsResourceGetMappedEglFrame")
// NO_INTERCEPT("cuEGLStreamConsumerConnectWithFlags")
// NO_INTERCEPT("cuProfilerInitialize")
TRY_INTERCEPT("cuProfilerStart", cuProfilerStart_intercepted)
TRY_INTERCEPT("cuProfilerStop", cuProfilerStop_intercepted)
// NO_INTERCEPT("cuVDPAUGetDevice")
// NO_INTERCEPT("cuVDPAUCtxCreate")
// NO_INTERCEPT("cuGraphicsVDPAURegisterVideoSurface")
// NO_INTERCEPT("cuGraphicsVDPAURegisterOutputSurface")
TRY_INTERCEPT("cuGraphInstantiateWithFlags", cuGraphInstantiateWithFlags_intercepted)
TRY_INTERCEPT("cuGraphKernelNodeGetAttribute", cuGraphKernelNodeGetAttribute_intercepted)
TRY_INTERCEPT("cuGraphKernelNodeSetAttribute", cuGraphKernelNodeSetAttribute_intercepted)
TRY_INTERCEPT("cuUserObjectCreate", cuUserObjectCreate_intercepted)
TRY_INTERCEPT("cuOccupancyMaxPotentialBlockSize", cuOccupancyMaxPotentialBlockSize)
// NO_INTERCEPT("cuGraphInstantiateWithParams_ptsz")

// If no need to intercept, call the corresponding CUDA function directly
CUresult result = cuGetProcAddress_v2(symbol, pfn, cudaVersion, flags, symbolStatus);
return result;
}