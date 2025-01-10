#include "interception.h"
#include <stdio.h>
#include <cstring>

#define STRINGIFY(x) #x

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
    return (void *) &getProcAddressBySymbol;
  }

  // for all other func
  return (real_dlsym(handle, symbol));
}
//==================================================================================================================
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
//==================================================================================================================
/* Macro used to generate hooks to CUDA driver functions (that need to be intercepted) */
#define CU_HOOK_DRIVER_FUNC(symbol, params, ...) \
  extern "C" CUresult CUDAAPI symbol params {   \
    using symbol##handler = CUresult CUDAAPI (params);  \
    static auto real_func = (symbol##handler *) real_dlsym(RTLD_NEXT, STRINGIFY(symbol)); \
    printf("Intercepted: %s\n", STRINGIFY(symbol)); \
    client.CallCudaFunction(STRINGIFY(symbol)); \
    CUresult result = real_func(__VA_ARGS__); \
    return result;  \
  }
//==================================================================================================================
extern "C" CUresult CUDAAPI cuModuleGetFunction(CUfunction* hfunc, CUmodule hmod, const char* name){
    using cuModuleGetFunction_handler = CUresult CUDAAPI (*)(CUfunction* hfunc, CUmodule hmod, const char* name);
    static cuModuleGetFunction_handler real_cuModuleGetFunction = nullptr;

    // Lazy loading of the real cuModuleGetFunction
    if (!real_cuModuleGetFunction) {
      real_cuModuleGetFunction = (cuModuleGetFunction_handler)dlsym(RTLD_NEXT, "cuModuleGetFunction");
      if (!real_cuModuleGetFunction) {
        std::cerr << "Error: Unable to load the real cuModuleGetFunction function!" << std::endl;
        return CUDA_ERROR_UNKNOWN;
      }
    }

    CUresult result = real_cuModuleGetFunction(hfunc, hmod, name);
    hashfunc[*hfunc] = name;

    if (result != CUDA_SUCCESS) {
      std::cerr << "Error: cuModuleGetFunction failed with error code " << result << std::endl;
    }

    return result;
}
//==================================================================================================================
extern "C" CUresult CUDAAPI cuLaunchKernel(CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ,
                     unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ,
                     unsigned int sharedMemBytes, CUstream hStream,
                     void** kernelParams, void** extra)
{
    using cuLaunchKernel_handler = CUresult CUDAAPI (*)(CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ,
                     unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ,
                     unsigned int sharedMemBytes, CUstream hStream,
                     void** kernelParams, void** extra);
    static cuLaunchKernel_handler real_cuLaunchKernel = nullptr;

    // Lazy loading of the real cuLaunchKernel
    if (!real_cuLaunchKernel) {
        real_cuLaunchKernel = (cuLaunchKernel_handler)dlsym(RTLD_NEXT, "cuLaunchKernel");
        if (!real_cuLaunchKernel) {
            std::cerr << "Error: Unable to load the real cuLaunchKernel function!" << std::endl;
            return CUDA_ERROR_UNKNOWN;
        }
    }

    /**
     * HACK there should have been another preparing grpc call to get the number of func params.
     * todo host version needs modification to fit into grpc
     * 
     * basic implementation idea: gpu-related variables used as original; host related variables
     * needs transfer - cast - reconstruction to array
     */
    if(!hashfunc.count(f)) std::cout << "cuFunc " << f << " not found!\n" << std::endl;
    else {
        std::vector<std::string> &param_type = func_proto.get_params(hashfunc[f]);
        int param_count = param_type.size();
        printf("the function launched has the name %s, with %d params\n", hashfunc[f].c_str(), param_count);
    }

    CUresult result = real_cuLaunchKernel(f, gridDimX, gridDimY, gridDimZ, 
        blockDimX, blockDimY, blockDimZ, 
        sharedMemBytes, hStream, 
        kernelParams, extra
    );

    if (result != CUDA_SUCCESS) {
        std::cerr << "Error: cuLaunchKernel failed with error code " << result << std::endl;
    }

    return result;

}
//==================================================================================================================

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
CU_HOOK_DRIVER_FUNC(cuMemcpyDtoH,
                    (void* dstHost, CUdeviceptr srcDevice, size_t ByteCount),
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