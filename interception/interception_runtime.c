// cuda_intercept.c
#define _GNU_SOURCE
#include <stdio.h>
#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <dlfcn.h>
#define INTERCEPT_RT(func_name, args_decl, ...) \
cudaError_t func_name args_decl { \
    typedef cudaError_t handle_type args_decl; \
    static handle_type *real_##func_name = NULL; \
    if (!real_##func_name) { \
        real_##func_name = dlsym(RTLD_NEXT, #func_name); \
        if (!real_##func_name) { \
            fprintf(stderr, "Failed to find real %s\n", #func_name); \
            return cudaErrorUnknown; \
        } \
    } \
    fprintf(stderr, "[Intercepted] %s\n", #func_name); \
    return real_##func_name(__VA_ARGS__); \
}

// INTERCEPT_RT(cudaMalloc, (void **devPtr, size_t size), devPtr, size)

// cudaError_t cudaMalloc(void **devPtr, size_t size) {
//     static cudaError_t (*real_cudaMalloc)(void **, size_t) = NULL;
//     if (!real_cudaMalloc) {
//         real_cudaMalloc = dlsym(RTLD_NEXT, "cudaMalloc");
//         if (!real_cudaMalloc) {
//             fprintf(stderr, "Failed to find real cudaMalloc\n");
//             return cudaErrorUnknown;
//         }
//     }

//     fprintf(stderr, "[Intercepted] cudaMalloc: size = %zu bytes\n", size);
//     return real_cudaMalloc(devPtr, size);
// }
INTERCEPT_RT(cudaDeviceReset, (void), )
INTERCEPT_RT(cudaDeviceSynchronize, (void), )
INTERCEPT_RT(cudaDeviceSetLimit, (enum cudaLimit limit, size_t value), limit, value)
INTERCEPT_RT(cudaDeviceGetLimit, (size_t *pValue, enum cudaLimit limit), pValue, limit)
INTERCEPT_RT(cudaDeviceGetTexture1DLinearMaxWidth, (size_t *maxWidthInElements, const struct cudaChannelFormatDesc *fmtDesc, int device), maxWidthInElements, fmtDesc, device)
INTERCEPT_RT(cudaDeviceGetCacheConfig, (enum cudaFuncCache *pCacheConfig), pCacheConfig)
INTERCEPT_RT(cudaDeviceGetStreamPriorityRange, (int *leastPriority, int *greatestPriority), leastPriority, greatestPriority)
INTERCEPT_RT(cudaDeviceSetCacheConfig, (enum cudaFuncCache cacheConfig), cacheConfig)
INTERCEPT_RT(cudaDeviceGetByPCIBusId, (int *device, const char *pciBusId), device, pciBusId)
INTERCEPT_RT(cudaDeviceGetPCIBusId, (char *pciBusId, int len, int device), pciBusId, len, device)
INTERCEPT_RT(cudaIpcGetEventHandle, (cudaIpcEventHandle_t *handle, cudaEvent_t event), handle, event)
INTERCEPT_RT(cudaIpcOpenEventHandle, (cudaEvent_t *event, cudaIpcEventHandle_t handle), event, handle)
INTERCEPT_RT(cudaIpcGetMemHandle, (cudaIpcMemHandle_t *handle, void *devPtr), handle, devPtr)
INTERCEPT_RT(cudaIpcOpenMemHandle, (void **devPtr, cudaIpcMemHandle_t handle, unsigned int flags), devPtr, handle, flags)
INTERCEPT_RT(cudaIpcCloseMemHandle, (void *devPtr), devPtr)
INTERCEPT_RT(cudaDeviceFlushGPUDirectRDMAWrites, (enum cudaFlushGPUDirectRDMAWritesTarget target, enum cudaFlushGPUDirectRDMAWritesScope scope), target, scope)
INTERCEPT_RT(cudaDeviceRegisterAsyncNotification, (int device, cudaAsyncCallback callbackFunc, void* userData, cudaAsyncCallbackHandle_t* callback), device, callbackFunc, userData, callback)
INTERCEPT_RT(cudaDeviceUnregisterAsyncNotification, (int device, cudaAsyncCallbackHandle_t callback), device, callback)
INTERCEPT_RT(cudaGetLastError, (void), )
INTERCEPT_RT(cudaPeekAtLastError, (void), )
INTERCEPT_RT(cudaGetDeviceCount, (int *count), count)
INTERCEPT_RT(cudaGetDeviceProperties, (struct cudaDeviceProp *prop, int device), prop, device)
INTERCEPT_RT(cudaDeviceGetAttribute, (int *value, enum cudaDeviceAttr attr, int device), value, attr, device)
INTERCEPT_RT(cudaDeviceGetDefaultMemPool, (cudaMemPool_t *memPool, int device), memPool, device)
INTERCEPT_RT(cudaDeviceSetMemPool, (int device, cudaMemPool_t memPool), device, memPool)
INTERCEPT_RT(cudaDeviceGetMemPool, (cudaMemPool_t *memPool, int device), memPool, device)
INTERCEPT_RT(cudaDeviceGetNvSciSyncAttributes, (void *nvSciSyncAttrList, int device, int flags), nvSciSyncAttrList, device, flags)
INTERCEPT_RT(cudaDeviceGetP2PAttribute, (int *value, enum cudaDeviceP2PAttr attr, int srcDevice, int dstDevice), value, attr, srcDevice, dstDevice)
INTERCEPT_RT(cudaChooseDevice, (int *device, const struct cudaDeviceProp *prop), device, prop)
INTERCEPT_RT(cudaInitDevice, (int device, unsigned int deviceFlags, unsigned int flags), device, deviceFlags, flags)
INTERCEPT_RT(cudaSetDevice, (int device), device)
INTERCEPT_RT(cudaGetDevice, (int *device), device)
INTERCEPT_RT(cudaSetValidDevices, (int *device_arr, int len), device_arr, len)
INTERCEPT_RT(cudaSetDeviceFlags, (unsigned int flags), flags)
INTERCEPT_RT(cudaGetDeviceFlags, (unsigned int *flags), flags)
INTERCEPT_RT(cudaStreamCreate, (cudaStream_t *pStream), pStream)
INTERCEPT_RT(cudaStreamCreateWithFlags, (cudaStream_t *pStream, unsigned int flags), pStream, flags)
INTERCEPT_RT(cudaStreamCreateWithPriority, (cudaStream_t *pStream, unsigned int flags, int priority), pStream, flags, priority)
INTERCEPT_RT(cudaStreamGetPriority, (cudaStream_t hStream, int *priority), hStream, priority)
INTERCEPT_RT(cudaStreamGetFlags, (cudaStream_t hStream, unsigned int *flags), hStream, flags)
INTERCEPT_RT(cudaStreamGetId, (cudaStream_t hStream, unsigned long long *streamId), hStream, streamId)
INTERCEPT_RT(cudaCtxResetPersistingL2Cache, (void), )
// INTERCEPT_RT(cudaStreamCopyAttributes, (cudaStream_t dst, cudaStream_t src), dst, src)
INTERCEPT_RT(cudaStreamDestroy, (cudaStream_t stream), stream)
INTERCEPT_RT(cudaStreamSynchronize, (cudaStream_t stream), stream)
INTERCEPT_RT(cudaStreamQuery, (cudaStream_t stream), stream)
INTERCEPT_RT(cudaStreamBeginCapture, (cudaStream_t stream, enum cudaStreamCaptureMode mode), stream, mode)
INTERCEPT_RT(cudaStreamBeginCaptureToGraph, (cudaStream_t stream, cudaGraph_t graph, const cudaGraphNode_t *dependencies, const cudaGraphEdgeData *dependencyData, size_t numDependencies, enum cudaStreamCaptureMode mode), stream, graph, dependencies, dependencyData, numDependencies, mode)
INTERCEPT_RT(cudaThreadExchangeStreamCaptureMode, (enum cudaStreamCaptureMode *mode), mode)
INTERCEPT_RT(cudaStreamEndCapture, (cudaStream_t stream, cudaGraph_t *pGraph), stream, pGraph)
INTERCEPT_RT(cudaStreamIsCapturing, (cudaStream_t stream, enum cudaStreamCaptureStatus *pCaptureStatus), stream, pCaptureStatus)
INTERCEPT_RT(cudaEventCreate, (cudaEvent_t *event), event)
INTERCEPT_RT(cudaEventCreateWithFlags, (cudaEvent_t *event, unsigned int flags), event, flags)
INTERCEPT_RT(cudaEventQuery, (cudaEvent_t event), event)
INTERCEPT_RT(cudaEventSynchronize, (cudaEvent_t event), event)
INTERCEPT_RT(cudaEventDestroy, (cudaEvent_t event), event)
INTERCEPT_RT(cudaEventElapsedTime, (float *ms, cudaEvent_t start, cudaEvent_t end), ms, start, end)
INTERCEPT_RT(cudaImportExternalMemory, (cudaExternalMemory_t *extMem_out, const struct cudaExternalMemoryHandleDesc *memHandleDesc), extMem_out, memHandleDesc)
INTERCEPT_RT(cudaExternalMemoryGetMappedBuffer, (void **devPtr, cudaExternalMemory_t extMem, const struct cudaExternalMemoryBufferDesc *bufferDesc), devPtr, extMem, bufferDesc)
INTERCEPT_RT(cudaExternalMemoryGetMappedMipmappedArray, (cudaMipmappedArray_t *mipmap, cudaExternalMemory_t extMem, const struct cudaExternalMemoryMipmappedArrayDesc *mipmapDesc), mipmap, extMem, mipmapDesc)
INTERCEPT_RT(cudaDestroyExternalMemory, (cudaExternalMemory_t extMem), extMem)
INTERCEPT_RT(cudaImportExternalSemaphore, (cudaExternalSemaphore_t *extSem_out, const struct cudaExternalSemaphoreHandleDesc *semHandleDesc), extSem_out, semHandleDesc)
INTERCEPT_RT(cudaDestroyExternalSemaphore, (cudaExternalSemaphore_t extSem), extSem)
INTERCEPT_RT(cudaLaunchKernel, (const void *func, dim3 gridDim, dim3 blockDim, void **args, size_t sharedMem, cudaStream_t stream), func, gridDim, blockDim, args, sharedMem, stream)
INTERCEPT_RT(cudaLaunchKernelExC, (const cudaLaunchConfig_t *config, const void *func, void **args), config, func, args)
INTERCEPT_RT(cudaLaunchCooperativeKernel, (const void *func, dim3 gridDim, dim3 blockDim, void **args, size_t sharedMem, cudaStream_t stream), func, gridDim, blockDim, args, sharedMem, stream)
INTERCEPT_RT(cudaFuncSetCacheConfig, (const void *func, enum cudaFuncCache cacheConfig), func, cacheConfig)
INTERCEPT_RT(cudaFuncGetAttributes, (struct cudaFuncAttributes *attr, const void *func), attr, func)
INTERCEPT_RT(cudaFuncSetAttribute, (const void *func, enum cudaFuncAttribute attr, int value), func, attr, value)
INTERCEPT_RT(cudaFuncGetName, (const char **name, const void *func), name, func)
INTERCEPT_RT(cudaFuncGetParamInfo, (const void *func, size_t paramIndex, size_t *paramOffset, size_t *paramSize), func, paramIndex, paramOffset, paramSize)
INTERCEPT_RT(cudaLaunchHostFunc, (cudaStream_t stream, cudaHostFn_t fn, void *userData), stream, fn, userData)
INTERCEPT_RT(cudaOccupancyMaxActiveBlocksPerMultiprocessor, (int *numBlocks, const void *func, int blockSize, size_t dynamicSMemSize), numBlocks, func, blockSize, dynamicSMemSize)
INTERCEPT_RT(cudaOccupancyAvailableDynamicSMemPerBlock, (size_t *dynamicSmemSize, const void *func, int numBlocks, int blockSize), dynamicSmemSize, func, numBlocks, blockSize)
INTERCEPT_RT(cudaOccupancyMaxActiveBlocksPerMultiprocessorWithFlags, (int *numBlocks, const void *func, int blockSize, size_t dynamicSMemSize, unsigned int flags), numBlocks, func, blockSize, dynamicSMemSize, flags)
INTERCEPT_RT(cudaOccupancyMaxPotentialClusterSize, (int *clusterSize, const void *func, const cudaLaunchConfig_t *launchConfig), clusterSize, func, launchConfig)
INTERCEPT_RT(cudaOccupancyMaxActiveClusters, (int *numClusters, const void *func, const cudaLaunchConfig_t *launchConfig), numClusters, func, launchConfig)
// INTERCEPT_RT(cudaMallocManaged, (void **devPtr, size_t size, unsigned int flags = cudaMemAttachGlobal), devPtr, size, cudaMemAttachGlobal)
INTERCEPT_RT(cudaMallocManaged, (void **devPtr, size_t size, unsigned int flags), devPtr, size, flags)
INTERCEPT_RT(cudaMalloc, (void **devPtr, size_t size), devPtr, size)
INTERCEPT_RT(cudaMallocHost, (void **ptr, size_t size), ptr, size)
INTERCEPT_RT(cudaMallocPitch, (void **devPtr, size_t *pitch, size_t width, size_t height), devPtr, pitch, width, height)
INTERCEPT_RT(cudaFree, (void *devPtr), devPtr)
INTERCEPT_RT(cudaFreeHost, (void *ptr), ptr)
INTERCEPT_RT(cudaFreeArray, (cudaArray_t array), array)
INTERCEPT_RT(cudaFreeMipmappedArray, (cudaMipmappedArray_t mipmappedArray), mipmappedArray)
INTERCEPT_RT(cudaHostAlloc, (void **pHost, size_t size, unsigned int flags), pHost, size, flags)
INTERCEPT_RT(cudaHostRegister, (void *ptr, size_t size, unsigned int flags), ptr, size, flags)
INTERCEPT_RT(cudaHostUnregister, (void *ptr), ptr)
INTERCEPT_RT(cudaHostGetDevicePointer, (void **pDevice, void *pHost, unsigned int flags), pDevice, pHost, flags)
INTERCEPT_RT(cudaHostGetFlags, (unsigned int *pFlags, void *pHost), pFlags, pHost)
INTERCEPT_RT(cudaMalloc3D, (struct cudaPitchedPtr* pitchedDevPtr, struct cudaExtent extent), pitchedDevPtr, extent)
INTERCEPT_RT(cudaGetMipmappedArrayLevel, (cudaArray_t *levelArray, cudaMipmappedArray_const_t mipmappedArray, unsigned int level), levelArray, mipmappedArray, level)
INTERCEPT_RT(cudaMemcpy3D, (const struct cudaMemcpy3DParms *p), p)
INTERCEPT_RT(cudaMemcpy3DPeer, (const struct cudaMemcpy3DPeerParms *p), p)
INTERCEPT_RT(cudaMemGetInfo, (size_t *free, size_t *total), free, total)
INTERCEPT_RT(cudaArrayGetInfo, (struct cudaChannelFormatDesc *desc, struct cudaExtent *extent, unsigned int *flags, cudaArray_t array), desc, extent, flags, array)
INTERCEPT_RT(cudaArrayGetPlane, (cudaArray_t *pPlaneArray, cudaArray_t hArray, unsigned int planeIdx), pPlaneArray, hArray, planeIdx)
INTERCEPT_RT(cudaArrayGetMemoryRequirements, (struct cudaArrayMemoryRequirements  *memoryRequirements, cudaArray_t array, int device), memoryRequirements, array, device)
INTERCEPT_RT(cudaMipmappedArrayGetMemoryRequirements, (struct cudaArrayMemoryRequirements *memoryRequirements, cudaMipmappedArray_t mipmap, int device), memoryRequirements, mipmap, device)
INTERCEPT_RT(cudaArrayGetSparseProperties, (struct cudaArraySparseProperties *sparseProperties, cudaArray_t array), sparseProperties, array)
INTERCEPT_RT(cudaMipmappedArrayGetSparseProperties, (struct cudaArraySparseProperties *sparseProperties, cudaMipmappedArray_t mipmap), sparseProperties, mipmap)
INTERCEPT_RT(cudaMemcpy, (void *dst, const void *src, size_t count, enum cudaMemcpyKind kind), dst, src, count, kind)
INTERCEPT_RT(cudaMemcpyPeer, (void *dst, int dstDevice, const void *src, int srcDevice, size_t count), dst, dstDevice, src, srcDevice, count)
INTERCEPT_RT(cudaMemcpy2D, (void *dst, size_t dpitch, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind), dst, dpitch, src, spitch, width, height, kind)
INTERCEPT_RT(cudaMemcpy2DToArray, (cudaArray_t dst, size_t wOffset, size_t hOffset, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind), dst, wOffset, hOffset, src, spitch, width, height, kind)
INTERCEPT_RT(cudaMemcpy2DFromArray, (void *dst, size_t dpitch, cudaArray_const_t src, size_t wOffset, size_t hOffset, size_t width, size_t height, enum cudaMemcpyKind kind), dst, dpitch, src, wOffset, hOffset, width, height, kind)
INTERCEPT_RT(cudaMemset, (void *devPtr, int value, size_t count), devPtr, value, count)
INTERCEPT_RT(cudaMemset2D, (void *devPtr, size_t pitch, int value, size_t width, size_t height), devPtr, pitch, value, width, height)
INTERCEPT_RT(cudaMemset3D, (struct cudaPitchedPtr pitchedDevPtr, int value, struct cudaExtent extent), pitchedDevPtr, value, extent)
INTERCEPT_RT(cudaGetSymbolAddress, (void **devPtr, const void *symbol), devPtr, symbol)
INTERCEPT_RT(cudaGetSymbolSize, (size_t *size, const void *symbol), size, symbol)
INTERCEPT_RT(cudaMemAdvise, (const void *devPtr, size_t count, enum cudaMemoryAdvise advice, int device), devPtr, count, advice, device)
INTERCEPT_RT(cudaMemAdvise_v2, (const void *devPtr, size_t count, enum cudaMemoryAdvise advice, struct cudaMemLocation location), devPtr, count, advice, location)
INTERCEPT_RT(cudaMemRangeGetAttribute, (void *data, size_t dataSize, enum cudaMemRangeAttribute attribute, const void *devPtr, size_t count), data, dataSize, attribute, devPtr, count)
INTERCEPT_RT(cudaMemRangeGetAttributes, (void **data, size_t *dataSizes, enum cudaMemRangeAttribute *attributes, size_t numAttributes, const void *devPtr, size_t count), data, dataSizes, attributes, numAttributes, devPtr, count)
INTERCEPT_RT(cudaMallocAsync, (void **devPtr, size_t size, cudaStream_t hStream), devPtr, size, hStream)
INTERCEPT_RT(cudaFreeAsync, (void *devPtr, cudaStream_t hStream), devPtr, hStream)
INTERCEPT_RT(cudaMemPoolTrimTo, (cudaMemPool_t memPool, size_t minBytesToKeep), memPool, minBytesToKeep)
INTERCEPT_RT(cudaMemPoolSetAttribute, (cudaMemPool_t memPool, enum cudaMemPoolAttr attr, void *value), memPool, attr, value)
INTERCEPT_RT(cudaMemPoolGetAttribute, (cudaMemPool_t memPool, enum cudaMemPoolAttr attr, void *value), memPool, attr, value)
INTERCEPT_RT(cudaMemPoolSetAccess, (cudaMemPool_t memPool, const struct cudaMemAccessDesc *descList, size_t count), memPool, descList, count)
INTERCEPT_RT(cudaMemPoolGetAccess, (enum cudaMemAccessFlags *flags, cudaMemPool_t memPool, struct cudaMemLocation *location), flags, memPool, location)
INTERCEPT_RT(cudaMemPoolCreate, (cudaMemPool_t *memPool, const struct cudaMemPoolProps *poolProps), memPool, poolProps)
INTERCEPT_RT(cudaMemPoolDestroy, (cudaMemPool_t memPool), memPool)
INTERCEPT_RT(cudaMallocFromPoolAsync, (void **ptr, size_t size, cudaMemPool_t memPool, cudaStream_t stream), ptr, size, memPool, stream)
INTERCEPT_RT(cudaMemPoolExportPointer, (struct cudaMemPoolPtrExportData *exportData, void *ptr), exportData, ptr)
INTERCEPT_RT(cudaMemPoolImportPointer, (void **ptr, cudaMemPool_t memPool, struct cudaMemPoolPtrExportData *exportData), ptr, memPool, exportData)
INTERCEPT_RT(cudaPointerGetAttributes, (struct cudaPointerAttributes *attributes, const void *ptr), attributes, ptr)
INTERCEPT_RT(cudaDeviceCanAccessPeer, (int *canAccessPeer, int device, int peerDevice), canAccessPeer, device, peerDevice)
INTERCEPT_RT(cudaDeviceEnablePeerAccess, (int peerDevice, unsigned int flags), peerDevice, flags)
INTERCEPT_RT(cudaDeviceDisablePeerAccess, (int peerDevice), peerDevice)
INTERCEPT_RT(cudaGraphicsUnregisterResource, (cudaGraphicsResource_t resource), resource)
INTERCEPT_RT(cudaGraphicsResourceSetMapFlags, (cudaGraphicsResource_t resource, unsigned int flags), resource, flags)
INTERCEPT_RT(cudaGraphicsResourceGetMappedPointer, (void **devPtr, size_t *size, cudaGraphicsResource_t resource), devPtr, size, resource)
INTERCEPT_RT(cudaGraphicsSubResourceGetMappedArray, (cudaArray_t *array, cudaGraphicsResource_t resource, unsigned int arrayIndex, unsigned int mipLevel), array, resource, arrayIndex, mipLevel)
INTERCEPT_RT(cudaGraphicsResourceGetMappedMipmappedArray, (cudaMipmappedArray_t *mipmappedArray, cudaGraphicsResource_t resource), mipmappedArray, resource)
INTERCEPT_RT(cudaGetChannelDesc, (struct cudaChannelFormatDesc *desc, cudaArray_const_t array), desc, array)
INTERCEPT_RT(cudaCreateTextureObject, (cudaTextureObject_t *pTexObject, const struct cudaResourceDesc *pResDesc, const struct cudaTextureDesc *pTexDesc, const struct cudaResourceViewDesc *pResViewDesc), pTexObject, pResDesc, pTexDesc, pResViewDesc)
INTERCEPT_RT(cudaDestroyTextureObject, (cudaTextureObject_t texObject), texObject)
INTERCEPT_RT(cudaGetTextureObjectResourceDesc, (struct cudaResourceDesc *pResDesc, cudaTextureObject_t texObject), pResDesc, texObject)
INTERCEPT_RT(cudaGetTextureObjectTextureDesc, (struct cudaTextureDesc *pTexDesc, cudaTextureObject_t texObject), pTexDesc, texObject)
INTERCEPT_RT(cudaGetTextureObjectResourceViewDesc, (struct cudaResourceViewDesc *pResViewDesc, cudaTextureObject_t texObject), pResViewDesc, texObject)
INTERCEPT_RT(cudaCreateSurfaceObject, (cudaSurfaceObject_t *pSurfObject, const struct cudaResourceDesc *pResDesc), pSurfObject, pResDesc)
INTERCEPT_RT(cudaDestroySurfaceObject, (cudaSurfaceObject_t surfObject), surfObject)
INTERCEPT_RT(cudaGetSurfaceObjectResourceDesc, (struct cudaResourceDesc *pResDesc, cudaSurfaceObject_t surfObject), pResDesc, surfObject)
INTERCEPT_RT(cudaDriverGetVersion, (int *driverVersion), driverVersion)
INTERCEPT_RT(cudaRuntimeGetVersion, (int *runtimeVersion), runtimeVersion)
INTERCEPT_RT(cudaGraphCreate, (cudaGraph_t *pGraph, unsigned int flags), pGraph, flags)
INTERCEPT_RT(cudaGraphAddKernelNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, const struct cudaKernelNodeParams *pNodeParams), pGraphNode, graph, pDependencies, numDependencies, pNodeParams)
INTERCEPT_RT(cudaGraphKernelNodeGetParams, (cudaGraphNode_t node, struct cudaKernelNodeParams *pNodeParams), node, pNodeParams)
INTERCEPT_RT(cudaGraphKernelNodeSetParams, (cudaGraphNode_t node, const struct cudaKernelNodeParams *pNodeParams), node, pNodeParams)
INTERCEPT_RT(cudaGraphAddMemcpyNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, const struct cudaMemcpy3DParms *pCopyParams), pGraphNode, graph, pDependencies, numDependencies, pCopyParams)
INTERCEPT_RT(cudaGraphMemcpyNodeGetParams, (cudaGraphNode_t node, struct cudaMemcpy3DParms *pNodeParams), node, pNodeParams)
INTERCEPT_RT(cudaGraphMemcpyNodeSetParams, (cudaGraphNode_t node, const struct cudaMemcpy3DParms *pNodeParams), node, pNodeParams)
INTERCEPT_RT(cudaGraphAddMemsetNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, const struct cudaMemsetParams *pMemsetParams), pGraphNode, graph, pDependencies, numDependencies, pMemsetParams)
INTERCEPT_RT(cudaGraphMemsetNodeGetParams, (cudaGraphNode_t node, struct cudaMemsetParams *pNodeParams), node, pNodeParams)
INTERCEPT_RT(cudaGraphMemsetNodeSetParams, (cudaGraphNode_t node, const struct cudaMemsetParams *pNodeParams), node, pNodeParams)
INTERCEPT_RT(cudaGraphAddHostNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, const struct cudaHostNodeParams *pNodeParams), pGraphNode, graph, pDependencies, numDependencies, pNodeParams)
INTERCEPT_RT(cudaGraphHostNodeGetParams, (cudaGraphNode_t node, struct cudaHostNodeParams *pNodeParams), node, pNodeParams)
INTERCEPT_RT(cudaGraphHostNodeSetParams, (cudaGraphNode_t node, const struct cudaHostNodeParams *pNodeParams), node, pNodeParams)
INTERCEPT_RT(cudaGraphAddChildGraphNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, cudaGraph_t childGraph), pGraphNode, graph, pDependencies, numDependencies, childGraph)
INTERCEPT_RT(cudaGraphChildGraphNodeGetGraph, (cudaGraphNode_t node, cudaGraph_t *pGraph), node, pGraph)
INTERCEPT_RT(cudaGraphAddEmptyNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies), pGraphNode, graph, pDependencies, numDependencies)
INTERCEPT_RT(cudaGraphAddEventRecordNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, cudaEvent_t event), pGraphNode, graph, pDependencies, numDependencies, event)
INTERCEPT_RT(cudaGraphEventRecordNodeGetEvent, (cudaGraphNode_t node, cudaEvent_t *event_out), node, event_out)
INTERCEPT_RT(cudaGraphEventRecordNodeSetEvent, (cudaGraphNode_t node, cudaEvent_t event), node, event)
INTERCEPT_RT(cudaGraphAddEventWaitNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, cudaEvent_t event), pGraphNode, graph, pDependencies, numDependencies, event)
INTERCEPT_RT(cudaGraphEventWaitNodeGetEvent, (cudaGraphNode_t node, cudaEvent_t *event_out), node, event_out)
INTERCEPT_RT(cudaGraphEventWaitNodeSetEvent, (cudaGraphNode_t node, cudaEvent_t event), node, event)
INTERCEPT_RT(cudaGraphAddExternalSemaphoresSignalNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, const struct cudaExternalSemaphoreSignalNodeParams *nodeParams), pGraphNode, graph, pDependencies, numDependencies, nodeParams)
INTERCEPT_RT(cudaGraphExternalSemaphoresSignalNodeGetParams, (cudaGraphNode_t hNode, struct cudaExternalSemaphoreSignalNodeParams *params_out), hNode, params_out)
INTERCEPT_RT(cudaGraphExternalSemaphoresSignalNodeSetParams, (cudaGraphNode_t hNode, const struct cudaExternalSemaphoreSignalNodeParams *nodeParams), hNode, nodeParams)
INTERCEPT_RT(cudaGraphAddExternalSemaphoresWaitNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, const struct cudaExternalSemaphoreWaitNodeParams *nodeParams), pGraphNode, graph, pDependencies, numDependencies, nodeParams)
INTERCEPT_RT(cudaGraphExternalSemaphoresWaitNodeGetParams, (cudaGraphNode_t hNode, struct cudaExternalSemaphoreWaitNodeParams *params_out), hNode, params_out)
INTERCEPT_RT(cudaGraphExternalSemaphoresWaitNodeSetParams, (cudaGraphNode_t hNode, const struct cudaExternalSemaphoreWaitNodeParams *nodeParams), hNode, nodeParams)
INTERCEPT_RT(cudaGraphAddMemAllocNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, struct cudaMemAllocNodeParams *nodeParams), pGraphNode, graph, pDependencies, numDependencies, nodeParams)
INTERCEPT_RT(cudaGraphMemAllocNodeGetParams, (cudaGraphNode_t node, struct cudaMemAllocNodeParams *params_out), node, params_out)
INTERCEPT_RT(cudaGraphAddMemFreeNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, void *dptr), pGraphNode, graph, pDependencies, numDependencies, dptr)
INTERCEPT_RT(cudaGraphMemFreeNodeGetParams, (cudaGraphNode_t node, void *dptr_out), node, dptr_out)
INTERCEPT_RT(cudaDeviceGraphMemTrim, (int device), device)
INTERCEPT_RT(cudaDeviceGetGraphMemAttribute, (int device, enum cudaGraphMemAttributeType attr, void* value), device, attr, value)
INTERCEPT_RT(cudaDeviceSetGraphMemAttribute, (int device, enum cudaGraphMemAttributeType attr, void* value), device, attr, value)
INTERCEPT_RT(cudaGraphClone, (cudaGraph_t *pGraphClone, cudaGraph_t originalGraph), pGraphClone, originalGraph)
INTERCEPT_RT(cudaGraphNodeFindInClone, (cudaGraphNode_t *pNode, cudaGraphNode_t originalNode, cudaGraph_t clonedGraph), pNode, originalNode, clonedGraph)
INTERCEPT_RT(cudaGraphNodeGetType, (cudaGraphNode_t node, enum cudaGraphNodeType *pType), node, pType)
INTERCEPT_RT(cudaGraphGetNodes, (cudaGraph_t graph, cudaGraphNode_t *nodes, size_t *numNodes), graph, nodes, numNodes)
INTERCEPT_RT(cudaGraphGetRootNodes, (cudaGraph_t graph, cudaGraphNode_t *pRootNodes, size_t *pNumRootNodes), graph, pRootNodes, pNumRootNodes)
INTERCEPT_RT(cudaGraphGetEdges, (cudaGraph_t graph, cudaGraphNode_t *from, cudaGraphNode_t *to, size_t *numEdges), graph, from, to, numEdges)
INTERCEPT_RT(cudaGraphGetEdges_v2, (cudaGraph_t graph, cudaGraphNode_t *from, cudaGraphNode_t *to, cudaGraphEdgeData *edgeData, size_t *numEdges), graph, from, to, edgeData, numEdges)
INTERCEPT_RT(cudaGraphNodeGetDependencies, (cudaGraphNode_t node, cudaGraphNode_t *pDependencies, size_t *pNumDependencies), node, pDependencies, pNumDependencies)
INTERCEPT_RT(cudaGraphNodeGetDependencies_v2, (cudaGraphNode_t node, cudaGraphNode_t *pDependencies, cudaGraphEdgeData *edgeData, size_t *pNumDependencies), node, pDependencies, edgeData, pNumDependencies)
INTERCEPT_RT(cudaGraphNodeGetDependentNodes, (cudaGraphNode_t node, cudaGraphNode_t *pDependentNodes, size_t *pNumDependentNodes), node, pDependentNodes, pNumDependentNodes)
INTERCEPT_RT(cudaGraphNodeGetDependentNodes_v2, (cudaGraphNode_t node, cudaGraphNode_t *pDependentNodes, cudaGraphEdgeData *edgeData, size_t *pNumDependentNodes), node, pDependentNodes, edgeData, pNumDependentNodes)
INTERCEPT_RT(cudaGraphAddDependencies, (cudaGraph_t graph, const cudaGraphNode_t *from, const cudaGraphNode_t *to, size_t numDependencies), graph, from, to, numDependencies)
INTERCEPT_RT(cudaGraphAddDependencies_v2, (cudaGraph_t graph, const cudaGraphNode_t *from, const cudaGraphNode_t *to, const cudaGraphEdgeData *edgeData, size_t numDependencies), graph, from, to, edgeData, numDependencies)
INTERCEPT_RT(cudaGraphRemoveDependencies, (cudaGraph_t graph, const cudaGraphNode_t *from, const cudaGraphNode_t *to, size_t numDependencies), graph, from, to, numDependencies)
INTERCEPT_RT(cudaGraphRemoveDependencies_v2, (cudaGraph_t graph, const cudaGraphNode_t *from, const cudaGraphNode_t *to, const cudaGraphEdgeData *edgeData, size_t numDependencies), graph, from, to, edgeData, numDependencies)
INTERCEPT_RT(cudaGraphDestroyNode, (cudaGraphNode_t node), node)
INTERCEPT_RT(cudaGraphInstantiateWithParams, (cudaGraphExec_t *pGraphExec, cudaGraph_t graph, cudaGraphInstantiateParams *instantiateParams), pGraphExec, graph, instantiateParams)
INTERCEPT_RT(cudaGraphExecGetFlags, (cudaGraphExec_t graphExec, unsigned long long *flags), graphExec, flags)
INTERCEPT_RT(cudaGraphExecKernelNodeSetParams, (cudaGraphExec_t hGraphExec, cudaGraphNode_t node, const struct cudaKernelNodeParams *pNodeParams), hGraphExec, node, pNodeParams)
INTERCEPT_RT(cudaGraphExecMemcpyNodeSetParams, (cudaGraphExec_t hGraphExec, cudaGraphNode_t node, const struct cudaMemcpy3DParms *pNodeParams), hGraphExec, node, pNodeParams)
INTERCEPT_RT(cudaGraphExecMemsetNodeSetParams, (cudaGraphExec_t hGraphExec, cudaGraphNode_t node, const struct cudaMemsetParams *pNodeParams), hGraphExec, node, pNodeParams)
INTERCEPT_RT(cudaGraphExecHostNodeSetParams, (cudaGraphExec_t hGraphExec, cudaGraphNode_t node, const struct cudaHostNodeParams *pNodeParams), hGraphExec, node, pNodeParams)
INTERCEPT_RT(cudaGraphExecChildGraphNodeSetParams, (cudaGraphExec_t hGraphExec, cudaGraphNode_t node, cudaGraph_t childGraph), hGraphExec, node, childGraph)
INTERCEPT_RT(cudaGraphExecEventRecordNodeSetEvent, (cudaGraphExec_t hGraphExec, cudaGraphNode_t hNode, cudaEvent_t event), hGraphExec, hNode, event)
INTERCEPT_RT(cudaGraphExecEventWaitNodeSetEvent, (cudaGraphExec_t hGraphExec, cudaGraphNode_t hNode, cudaEvent_t event), hGraphExec, hNode, event)
INTERCEPT_RT(cudaGraphExecExternalSemaphoresSignalNodeSetParams, (cudaGraphExec_t hGraphExec, cudaGraphNode_t hNode, const struct cudaExternalSemaphoreSignalNodeParams *nodeParams), hGraphExec, hNode, nodeParams)
INTERCEPT_RT(cudaGraphExecExternalSemaphoresWaitNodeSetParams, (cudaGraphExec_t hGraphExec, cudaGraphNode_t hNode, const struct cudaExternalSemaphoreWaitNodeParams *nodeParams), hGraphExec, hNode, nodeParams)
INTERCEPT_RT(cudaGraphNodeSetEnabled, (cudaGraphExec_t hGraphExec, cudaGraphNode_t hNode, unsigned int isEnabled), hGraphExec, hNode, isEnabled)
INTERCEPT_RT(cudaGraphNodeGetEnabled, (cudaGraphExec_t hGraphExec, cudaGraphNode_t hNode, unsigned int *isEnabled), hGraphExec, hNode, isEnabled)
INTERCEPT_RT(cudaGraphExecUpdate, (cudaGraphExec_t hGraphExec, cudaGraph_t hGraph, cudaGraphExecUpdateResultInfo *resultInfo), hGraphExec, hGraph, resultInfo)
INTERCEPT_RT(cudaGraphUpload, (cudaGraphExec_t graphExec, cudaStream_t stream), graphExec, stream)
INTERCEPT_RT(cudaGraphLaunch, (cudaGraphExec_t graphExec, cudaStream_t stream), graphExec, stream)
INTERCEPT_RT(cudaGraphExecDestroy, (cudaGraphExec_t graphExec), graphExec)
INTERCEPT_RT(cudaGraphDestroy, (cudaGraph_t graph), graph)
INTERCEPT_RT(cudaGraphDebugDotPrint, (cudaGraph_t graph, const char *path, unsigned int flags), graph, path, flags)
INTERCEPT_RT(cudaUserObjectCreate, (cudaUserObject_t *object_out, void *ptr, cudaHostFn_t destroy, unsigned int initialRefcount, unsigned int flags), object_out, ptr, destroy, initialRefcount, flags)
INTERCEPT_RT(cudaGraphAddNode, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, size_t numDependencies, struct cudaGraphNodeParams *nodeParams), pGraphNode, graph, pDependencies, numDependencies, nodeParams)
INTERCEPT_RT(cudaGraphAddNode_v2, (cudaGraphNode_t *pGraphNode, cudaGraph_t graph, const cudaGraphNode_t *pDependencies, const cudaGraphEdgeData *dependencyData, size_t numDependencies, struct cudaGraphNodeParams *nodeParams), pGraphNode, graph, pDependencies, dependencyData, numDependencies, nodeParams)
INTERCEPT_RT(cudaGraphNodeSetParams, (cudaGraphNode_t node, struct cudaGraphNodeParams *nodeParams), node, nodeParams)
INTERCEPT_RT(cudaGraphExecNodeSetParams, (cudaGraphExec_t graphExec, cudaGraphNode_t node, struct cudaGraphNodeParams *nodeParams), graphExec, node, nodeParams)
// INTERCEPT_RT(cudaGetDriverEntryPoint, (const char *symbol, void **funcPtr, unsigned long long flags, enum cudaDriverEntryPointQueryResult *driverStatus = NULL), symbol, funcPtr, flags, NULL)
INTERCEPT_RT(cudaGetDriverEntryPoint, (const char *symbol, void **funcPtr, unsigned long long flags, enum cudaDriverEntryPointQueryResult *driverStatus), symbol, funcPtr, flags, driverStatus)
// INTERCEPT_RT(cudaGetDriverEntryPointByVersion, (const char *symbol, void **funcPtr, unsigned int cudaVersion, unsigned long long flags, enum cudaDriverEntryPointQueryResult *driverStatus = NULL), symbol, funcPtr, cudaVersion, flags, NULL)
INTERCEPT_RT(cudaGetDriverEntryPointByVersion, (const char *symbol, void **funcPtr, unsigned int cudaVersion, unsigned long long flags, enum cudaDriverEntryPointQueryResult *driverStatus), symbol, funcPtr, cudaVersion, flags, driverStatus)
INTERCEPT_RT(cudaGetExportTable, (const void **ppExportTable, const cudaUUID_t *pExportTableId), ppExportTable, pExportTableId)
INTERCEPT_RT(cudaGetKernel, (cudaKernel_t *kernelPtr, const void *entryFuncAddr), kernelPtr, entryFuncAddr)
INTERCEPT_RT(cudaMemcpyToArray, (cudaArray_t dst, size_t wOffset, size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind), dst, wOffset, hOffset, src, count, kind)
INTERCEPT_RT(cudaMemcpyFromArray, (void *dst, cudaArray_const_t src, size_t wOffset, size_t hOffset, size_t count, enum cudaMemcpyKind kind), dst, src, wOffset, hOffset, count, kind)
INTERCEPT_RT(cudaStreamWaitEvent, (cudaStream_t stream, cudaEvent_t event, unsigned int flags), stream, event, flags)
INTERCEPT_RT(cudaStreamAddCallback, (cudaStream_t stream, cudaStreamCallback_t callback, void *userData, unsigned int flags), stream, callback, userData, flags)
INTERCEPT_RT(cudaStreamAttachMemAsync, (cudaStream_t stream, void *devPtr, size_t length, unsigned int flags), stream, devPtr, length, flags)
INTERCEPT_RT(cudaMemPrefetchAsync, (const void *devPtr, size_t count, int dstDevice, cudaStream_t stream), devPtr, count, dstDevice, stream)
INTERCEPT_RT(cudaMemPrefetchAsync_v2, (const void *devPtr, size_t count, struct cudaMemLocation location, unsigned int flags, cudaStream_t stream), devPtr, count, location, flags, stream)
// INTERCEPT_RT(cudaStreamGetCaptureInfo, (cudaStream_t stream, enum cudaStreamCaptureStatus *captureStatus_out, unsigned long long *id_out), stream, captureStatus_out, id_out)
INTERCEPT_RT(cudaStreamGetCaptureInfo_ptsz, (cudaStream_t stream, enum cudaStreamCaptureStatus *captureStatus_out, unsigned long long *id_out), stream, captureStatus_out, id_out)
INTERCEPT_RT(cudaStreamCopyAttributes, (cudaStream_t dstStream, cudaStream_t srcStream), dstStream, srcStream)
INTERCEPT_RT(cudaStreamGetAttribute, (cudaStream_t stream, cudaStreamAttrID attr, cudaStreamAttrValue *value), stream, attr, value)
INTERCEPT_RT(cudaStreamSetAttribute, (cudaStream_t stream, cudaStreamAttrID attr, const cudaStreamAttrValue *param), stream, attr, param)

// INTERCEPT_RT(cudaKernelSetAttributeForDevice,
//   (cudaKernel_t kernel, cudaFuncAttribute attr, int value, int device),
//   kernel, attr, value, device)

INTERCEPT_RT(cudaLibraryEnumerateKernels,
  (cudaKernel_t* kernels, unsigned int numKernels, cudaLibrary_t lib),
  kernels, numKernels, lib)

INTERCEPT_RT(cudaLibraryGetGlobal,
  (void** dptr, size_t* bytes, cudaLibrary_t library, const char* name),
  dptr, bytes, library, name)

INTERCEPT_RT(cudaLibraryGetKernel,
  (cudaKernel_t* pKernel, cudaLibrary_t library, const char* name),
  pKernel, library, name)

INTERCEPT_RT(cudaLibraryGetKernelCount,
  (unsigned int* count, cudaLibrary_t lib),
  count, lib)

INTERCEPT_RT(cudaLibraryGetManaged,
  (void** dptr, size_t* bytes, cudaLibrary_t library, const char* name),
  dptr, bytes, library, name)

INTERCEPT_RT(cudaLibraryGetUnifiedFunction,
  (void** fptr, cudaLibrary_t library, const char* symbol),
  fptr, library, symbol)

INTERCEPT_RT(cudaLibraryLoadData,
  (cudaLibrary_t* library, const void* code,
   cudaJitOption** jitOptions, void** jitOptionsValues, unsigned int numJitOptions,
   cudaLibraryOption** libraryOptions, void** libraryOptionValues, unsigned int numLibraryOptions),
  library, code, jitOptions, jitOptionsValues, numJitOptions, libraryOptions, libraryOptionValues, numLibraryOptions)

INTERCEPT_RT(cudaLibraryLoadFromFile,
  (cudaLibrary_t* library, const char* fileName,
   cudaJitOption** jitOptions, void** jitOptionsValues, unsigned int numJitOptions,
   cudaLibraryOption** libraryOptions, void** libraryOptionValues, unsigned int numLibraryOptions),
  library, fileName, jitOptions, jitOptionsValues, numJitOptions, libraryOptions, libraryOptionValues, numLibraryOptions)

INTERCEPT_RT(cudaLibraryUnload,
  (cudaLibrary_t library),
  library)
