#include "interception.h"
#include <stdio.h>
#include <cstring>
#include <unordered_map>

#define STRINGIFY(x) #x

VirtualGPU vgpu((1 << 20) * sizeof(float));

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
void *dlsym(void *handle, const char *symbol) {
  // for CUDA func
  if(strcmp(symbol, STRINGIFY(cuGetProcAddress)) == 0){
    printf("intercepted: %s\n", symbol);
    // return (void *) &getProcAddressBySymbol;
  }

  // for all other func
  return (real_dlsym(handle, symbol));
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

  // If no need to intercept, call the corresponding CUDA function directly
  CUresult result = cuGetProcAddress_v2(symbol, pfn, cudaVersion, flags, symbolStatus);

  return result;
}
//==================================================================================================================
/* Macro used to generate hooks to CUDA driver functions (that need to be intercepted) */
#define CU_HOOK_DRIVER_FUNC(symbol, params, ...) \
  extern "C" CUresult CUDAAPI symbol params {   \
    using symbol##handler = CUresult CUDAAPI (params);  \
    static auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, STRINGIFY(symbol)); \
    printf("Intercepted: %s\n", STRINGIFY(symbol)); \
    //// client.CallCudaFunction(STRINGIFY(symbol)); \
    CUresult result = real_func(__VA_ARGS__); \
    return result;  \
  }
//==================================================================================================================
//currently no work to do, CUDA will only be initialized on the host
extern "C" CUresult CUDAAPI cuInit(unsigned int Flags){ return CUDA_SUCCESS; };
extern "C" CUresult CUDAAPI cuDeviceGet(CUdevice* device, int ordinal){ return CUDA_SUCCESS; };
extern "C" CUresult CUDAAPI cuCtxCreate(CUcontext* pctx, unsigned int flags, CUdevice dev){ return CUDA_SUCCESS; };
//currently no work to do, context is managed by the host
extern "C" CUresult CUDAAPI cuCtxDestroy(CUcontext ctx){ return CUDA_SUCCESS; };

//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemAlloc(CUdeviceptr* dptr, size_t bytesize){
    static std::vector<std::string> string_args;
    static std::vector<std::uint64_t> scalar_args(2, 0);

    scalar_args[1] = bytesize;
    ResponseMessage &response = client.CallCudaFunction(__func__, string_args, scalar_args);
    *dptr = response.scalar();
    
    return (CUresult) response.curesult();
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemcpyHtoD(CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount){
    vgpu.to_device(srcHost, ByteCount);
    static std::vector<std::string> string_args;
    static std::vector<std::uint64_t> scalar_args(3, 0);

    scalar_args[0] = dstDevice;
    scalar_args[2] = ByteCount;

    ResponseMessage &response = client.CallCudaFunction(__func__, string_args, scalar_args);

    return (CUresult) response.curesult();
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuModuleLoad(CUmodule* cu_module, const char* fname){
    static std::vector<std::string> string_args(1);
    static std::vector<std::uint64_t> scalar_args;

    string_args[0] = fname;
    ResponseMessage &response = client.CallCudaFunction(__func__, string_args, scalar_args);
    *cu_module = (CUmodule) response.scalar();

    return (CUresult) response.curesult();
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemcpyDtoH(void* dstHost, CUdeviceptr srcDevice, size_t ByteCount){
    static std::vector<std::string> string_args;
    static std::vector<std::uint64_t> scalar_args(3, 0);

    scalar_args[1] = srcDevice;
    scalar_args[2] = ByteCount;

    ResponseMessage &response = client.CallCudaFunction(__func__, string_args, scalar_args);

    vgpu.from_device(dstHost, ByteCount);
    return (CUresult) response.curesult();
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuMemFree(CUdeviceptr dptr){
    static std::vector<std::string> string_args;
    static std::vector<std::uint64_t> scalar_args(1);

    scalar_args[0] = dptr;
    ResponseMessage &response = client.CallCudaFunction(__func__, string_args, scalar_args);
    return (CUresult) response.curesult();
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuModuleUnload(CUmodule hmod){
    static std::vector<std::string> string_args;
    static std::vector<std::uint64_t> scalar_args(1);

    scalar_args[0] = reinterpret_cast<std::uint64_t>(hmod);
    ResponseMessage &response = client.CallCudaFunction(__func__, string_args, scalar_args);
    return (CUresult) response.curesult();
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuModuleGetFunction(CUfunction* hfunc, CUmodule hmod, const char* name){
    static std::vector<std::string> string_args(1);
    static std::vector<std::uint64_t> scalar_args(1);

    string_args[0] = name;
    scalar_args[0] = reinterpret_cast<std::uint64_t>(hmod);
    ResponseMessage &response = client.CallCudaFunction(__func__, string_args, scalar_args);
    *hfunc = (CUfunction) response.scalar();
    hashfunc[*hfunc] = name;

    return (CUresult) response.curesult();
}
//==================================================================================================================
//FIXME how to deal with the `extra`? where is it used? =
extern "C" CUresult CUDAAPI cuLaunchKernel(CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ,
                                           unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ,
                                           unsigned int sharedMemBytes, CUstream hStream,
                                           void** kernelParams, void** extra){
    static std::vector<std::string> string_args;
    //currently, the `extra` is not included
    std::vector<std::uint64_t> scalar_args(9);

    scalar_args[0] = reinterpret_cast<std::uint64_t>(f);
    scalar_args[1] = gridDimX; scalar_args[2] = gridDimY; scalar_args[3] = gridDimZ;
    scalar_args[4] = blockDimX; scalar_args[5] = blockDimY; scalar_args[6] = blockDimZ;
    scalar_args[7] = sharedMemBytes; scalar_args[8] = 0;  //TODO deal with the CUstream

    /******************************************
     *    Set parameters for the cuda func    *
     ******************************************/
    if(!hashfunc.count(f)) std::cout << "cuFunc " << f << " not found!\n" << std::endl;
    std::vector<std::string> &param_type = func_proto.get_params(hashfunc[f]);
    int param_count = param_type.size();
    printf("the function launched has the name %s, with %d params\n", hashfunc[f].c_str(), param_count);

    for(int i = 0; i < param_count; i++){
        //// if(param_type[i] == ".u32") scalar_args.push_back(*reinterpret_cast<uint32_t*>(kernelParams[i]));
        //// if(param_type[i] == ".u64") scalar_args.push_back(*reinterpret_cast<uint64_t*>(kernelParams[i]));
        scalar_args.push_back(*reinterpret_cast<std::uint64_t*>(kernelParams[i]));
    }
    

    ResponseMessage &response = client.CallCudaFunction(__func__, string_args, scalar_args);

    return (CUresult) response.curesult();
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