#include "interception.h"
#include <stdio.h>
#include <cstring>
#include <unordered_map>
#include <unistd.h>

//==================================================================================================================
/* Get the real `dlsym` handler in `libdl` */
//FIXME better compatibility for all sys
static void *real_dlsym(void *handle, const char *symbol) {
  static fnDlsym internal_dlsym =
    (fnDlsym) dlvsym(dlopen("libdl.so.2", RTLD_LAZY), "dlsym", "GLIBC_2.34");
  return (*internal_dlsym)(handle, symbol);
}
//==================================================================================================================

//==================================================================================================================
#define TRY_INTERCEPT(name, handler) \
  if(strcmp(symbol, name) == 0){ \
    *pfn = (void*) &handler; \
    return CUDA_SUCCESS;\
  }
//==================================================================================================================

//==================================================================================================================
/* Macro used to generate hooks to CUDA driver functions (that need to be intercepted) */
#define CU_HOOK_DRIVER_FUNC(symbol, params, ...) \
  extern "C" CUresult CUDAAPI symbol params {   \
    using symbol##handler = CUresult CUDAAPI (params);  \
    static auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, SYMBOL_TO_STR(symbol)); \
    printf("Intercepted: %s\n", SYMBOL_TO_STR(symbol)); \
    CUresult result = real_func(__VA_ARGS__); \
    return result;  \
  }
//==================================================================================================================
/* Macro used to intercept and then generate corresponding remoting API for CUDA driver functions. */
#define CU_HOOK_REMOTE(symbol, params, ...) \
  extern "C" CUresult CUDAAPI symbol params{  \
    client.CallCudaFunction(__func__, __VA_ARGS__); \
    return client.wait_recv(__VA_ARGS__); \
  }
//==================================================================================================================
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
CU_HOOK_REMOTE((cuDeviceTotalMem), (unsigned int *bytes, CUdevice dev), bytes, dev)
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
extern "C" CUresult CUDAAPI cuGetExportTable(const void **ppExportTable, const CUuuid *pExportTableId){
    client.CallCudaFunction(__func__, *pExportTableId);
    CUresult result = client.wait_recv(ppExportTable);

    return result;
}
//==================================================================================================================
//todo where is the data in srchost located?
extern "C" CUresult CUDAAPI cuMemcpyHtoDAsync(CUdeviceptr dstDevice, const void *srcHost, unsigned int ByteCount, CUstream hStream){
    pinned_memory.sync(srcHost);
    client.CallCudaFunction(__func__, dstDevice, srcHost, ByteCount, hStream);
    CUresult result = client.wait_recv();

    return result;
}
//==================================================================================================================
//todo 
// extern "C" CUresult CUDAAPI cuGetExportTable(const void **ppExportTable, const CUuuid *pExportTableId){
//   client.CallCudaFunction(__func__, ppExportTable, pExportTableId);
//   CUresult result = client.wait_recv();
//   *priority = client.get_scalar_result();

//   return result;
// }
//==================================================================================================================
CU_HOOK_REMOTE((cuStreamIsCapturing), (CUstream hStream, CUstreamCaptureStatus *captureStatus), hStream, captureStatus)
CU_HOOK_REMOTE((cuStreamGetPriority), (CUstream hStream, int *priority), hStream, priority)
CU_HOOK_REMOTE((cuMemAlloc), (CUdeviceptr* dptr, size_t bytesize), dptr, bytesize)
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemHostAlloc(void **pp, size_t bytesize, unsigned int Flags){
    //! [todo] there would be waste for the allocated memory on the microvm
    std::cout << "cumemhostalloc invoked" << std::endl;
    *pp = pinned_memory.register_pinned_memory(bytesize);
    client.CallCudaFunction(__func__, *pp, bytesize, Flags);
    CUresult result = client.wait_recv();
    return result;
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemcpyHtoD(CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount){
    if(pinned_memory.is_pinned(srcHost)) pinned_memory.sync(srcHost), std::cout << "the ptr is pinned at " << srcHost << std::endl;
    else client.to_device(srcHost, ByteCount);
    client.CallCudaFunction(__func__, dstDevice, srcHost, ByteCount);
    CUresult result = client.wait_recv();
    return result;
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemcpyDtoH(void* dstHost, CUdeviceptr srcDevice, size_t ByteCount){
    client.CallCudaFunction(__func__, dstHost, srcDevice, ByteCount);
    CUresult result = client.wait_recv();
    if(pinned_memory.is_pinned(dstHost)) pinned_memory.remap();
    else client.from_device(dstHost, ByteCount);
    return result;
}
//==================================================================================================================
//* methods needed to process the const char*
extern "C" CUresult CUDAAPI cuModuleLoad(CUmodule* cu_module, const char* fname){
    client.CallCudaFunction(__func__, cu_module, fname);
    CUresult result = client.wait_recv();
    *cu_module = (CUmodule) client.get_scalar_result();
    return result;
}
//==================================================================================================================
//* methods needed to process the const char*
extern "C" CUresult CUDAAPI cuModuleGetFunction(CUfunction* hfunc, CUmodule hmod, const char* name){
    client.CallCudaFunction(__func__, hfunc, hmod, name);
    CUresult result = client.wait_recv();
    *hfunc = (CUfunction) client.get_scalar_result();
    hashfunc[*hfunc] = name;
    return result;
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

    return result;
}
//==================================================================================================================
/* Interception version for `cuGetProcAddress` and all needed CUDA funcs */
extern "C" CUresult getProcAddressBySymbol(const char* symbol, void** pfn, int cudaVersion, cuuint64_t flags,
                                            CUdriverProcAddressQueryResult* symbolStatus) {
  // printf("Intercepted cuGetProcAddress: symbol=%s\n", symbol);
  if(strcmp(symbol, "cuGetProcAddress") == 0){
  *pfn = (void *) &getProcAddressBySymbol;
  return CUDA_SUCCESS;
  }
  TRY_INTERCEPT("cuInit", cuInit)
  TRY_INTERCEPT("cuDriverGetVersion", cuDriverGetVersion)
  TRY_INTERCEPT("cuGetExportTable", cuGetExportTable)
  TRY_INTERCEPT("cuModuleGetLoadingMode", cuModuleGetLoadingMode)
  TRY_INTERCEPT("cuDeviceGetCount", cuDeviceGetCount)
  TRY_INTERCEPT("cuDeviceGet", cuDeviceGet)
  TRY_INTERCEPT("cuDeviceGetName", cuDeviceGetName)
  TRY_INTERCEPT("cuDeviceTotalMem", cuDeviceTotalMem)
  TRY_INTERCEPT("cuDeviceGetAttribute", cuDeviceGetAttribute)
  TRY_INTERCEPT("cuDeviceGetUuid", cuDeviceGetUuid)
  TRY_INTERCEPT("cuCtxGetDevice", cuCtxGetDevice)
  TRY_INTERCEPT("cuCtxGetCurrent", cuCtxGetCurrent)
  TRY_INTERCEPT("cuCtxSetCurrent", cuCtxSetCurrent)
  TRY_INTERCEPT("cuDevicePrimaryCtxRetain", cuDevicePrimaryCtxRetain)
  TRY_INTERCEPT("cuCtxGetStreamPriorityRange", cuCtxGetStreamPriorityRange)
  TRY_INTERCEPT("cuStreamIsCapturing", cuStreamIsCapturing)
  TRY_INTERCEPT("cuMemAlloc", cuMemAlloc)
  TRY_INTERCEPT("cuMemcpyHtoDAsync", cuMemcpyHtoDAsync)
  TRY_INTERCEPT("cuStreamSynchronize", cuStreamSynchronize)
  TRY_INTERCEPT("cuStreamCreate", cuStreamCreate)
  TRY_INTERCEPT("cuMemsetD8Async", cuMemsetD8Async)
  TRY_INTERCEPT("cuEventCreate", cuEventCreate)
  TRY_INTERCEPT("cuMemHostAlloc", cuMemHostAlloc)
  TRY_INTERCEPT("cuMemHostGetDevicePointer", cuMemHostGetDevicePointer)
  TRY_INTERCEPT("cuMemFree", cuMemFree)
  TRY_INTERCEPT("cuEventRecord", cuEventRecord)
  TRY_INTERCEPT("cuStreamGetPriority", cuStreamGetPriority)
  TRY_INTERCEPT("cuCtxPushCurrent", cuCtxPushCurrent)
  TRY_INTERCEPT("cuLibraryGetModule", cuLibraryGetModule)
  TRY_INTERCEPT("cuCtxPopCurrent", cuCtxPopCurrent)
  TRY_INTERCEPT("cuModuleGetFunction", cuModuleGetFunction)
  TRY_INTERCEPT("cuLaunchKernel", cuLaunchKernel)
  TRY_INTERCEPT("cuModuleGetGlobal", cuModuleGetGlobal)
  TRY_INTERCEPT("cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags", cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags)
  TRY_INTERCEPT("cuLibraryUnload", cuLibraryUnload)
  TRY_INTERCEPT("cuDevicePrimaryCtxRelease", cuDevicePrimaryCtxRelease)

  // If no need to intercept, call the corresponding CUDA function directly
  CUresult result = cuGetProcAddress_v2(symbol, pfn, cudaVersion, flags, symbolStatus);

  return result;
}
//==================================================================================================================
/* Intercept the `dlsym` function, which is used by `libcudart.so` to link to `libcuda.so` */
void *dlsym(void *handle, const char *symbol) {
  // for CUDA func
  if(strcmp(symbol, SYMBOL_TO_STR(cuGetProcAddress)) == 0){
    printf("intercepted: %s\n", symbol);
    return (void *) &getProcAddressBySymbol;
  }

  // for all other func
  return (real_dlsym(handle, symbol));
}
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