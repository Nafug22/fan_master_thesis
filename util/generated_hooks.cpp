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

CU_HOOK_DRIVER_FUNC(cuCtxSynchronize_intercepted, cuCtxSynchronize, (void), )

CU_HOOK_DRIVER_FUNC(cuCtxResetPersistingL2Cache_intercepted, cuCtxResetPersistingL2Cache, (void), )

CU_HOOK_DRIVER_FUNC(cuCtxPopCurrent_intercepted, cuCtxPopCurrent, (CUcontext *pctx), pctx)

CU_HOOK_DRIVER_FUNC(cuCtxPushCurrent_intercepted, cuCtxPushCurrent, (CUcontext ctx), ctx)

CU_HOOK_DRIVER_FUNC(cuModuleLoad_intercepted, cuModuleLoad, (CUmodule *module, const char *fname), module, fname)

CU_HOOK_DRIVER_FUNC(cuModuleLoadData_intercepted, cuModuleLoadData, (CUmodule *module, const void *image), module, image)

CU_HOOK_DRIVER_FUNC(cuModuleLoadFatBinary_intercepted, cuModuleLoadFatBinary, (CUmodule *module, const void *fatCubin), module, fatCubin)

CU_HOOK_DRIVER_FUNC(cuModuleUnload_intercepted, cuModuleUnload, (CUmodule hmod), hmod)

CU_HOOK_DRIVER_FUNC(cuModuleGetFunction_intercepted, cuModuleGetFunction, (CUfunction *hfunc, CUmodule hmod, const char *name), hfunc, hmod, name)

CU_HOOK_DRIVER_FUNC(cuModuleGetGlobal_intercepted, cuModuleGetGlobal, (CUdeviceptr_v1 *dptr, unsigned int *bytes, CUmodule hmod, const char *name), dptr, bytes, hmod, name)

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

CU_HOOK_DRIVER_FUNC(cuMemAlloc_intercepted, cuMemAlloc, (CUdeviceptr_v1 *dptr, unsigned int bytesize), dptr, bytesize)

CU_HOOK_DRIVER_FUNC(cuMemAllocPitch_intercepted, cuMemAllocPitch, (CUdeviceptr_v1 *dptr, unsigned int *pPitch, unsigned int WidthInBytes, unsigned int Height, unsigned int ElementSizeBytes), dptr, pPitch, WidthInBytes, Height, ElementSizeBytes)

CU_HOOK_DRIVER_FUNC(cuMemFree_intercepted, cuMemFree, (CUdeviceptr_v1 dptr), dptr)

CU_HOOK_DRIVER_FUNC(cuMemGetAddressRange_intercepted, cuMemGetAddressRange, (CUdeviceptr_v1 *pbase, unsigned int *psize, CUdeviceptr_v1 dptr), pbase, psize, dptr)

CU_HOOK_DRIVER_FUNC(cuMemFreeHost_intercepted, cuMemFreeHost, (void *p), p)

CU_HOOK_DRIVER_FUNC(cuMemHostAlloc_intercepted, cuMemHostAlloc, (void **pp, size_t bytesize, unsigned int Flags), pp, bytesize, Flags)

CU_HOOK_DRIVER_FUNC(cuMemHostGetDevicePointer_intercepted, cuMemHostGetDevicePointer, (CUdeviceptr_v1 *pdptr, void *p, unsigned int Flags), pdptr, p, Flags)

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

CU_HOOK_DRIVER_FUNC(cuMemcpyHtoD_intercepted, cuMemcpyHtoD, (CUdeviceptr_v1 dstDevice, const void *srcHost, unsigned int ByteCount), dstDevice, srcHost, ByteCount)

CU_HOOK_DRIVER_FUNC(cuMemcpyHtoDAsync_intercepted, cuMemcpyHtoDAsync, (CUdeviceptr_v1 dstDevice, const void *srcHost, unsigned int ByteCount, CUstream hStream), dstDevice, srcHost, ByteCount, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpyDtoH_intercepted, cuMemcpyDtoH, (void *dstHost, CUdeviceptr_v1 srcDevice, unsigned int ByteCount), dstHost, srcDevice, ByteCount)

CU_HOOK_DRIVER_FUNC(cuMemcpyDtoHAsync_intercepted, cuMemcpyDtoHAsync, (void *dstHost, CUdeviceptr_v1 srcDevice, unsigned int ByteCount, CUstream hStream), dstHost, srcDevice, ByteCount, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpyDtoD_intercepted, cuMemcpyDtoD, (CUdeviceptr_v1 dstDevice, CUdeviceptr_v1 srcDevice, unsigned int ByteCount), dstDevice, srcDevice, ByteCount)

CU_HOOK_DRIVER_FUNC(cuMemcpyDtoDAsync_intercepted, cuMemcpyDtoDAsync, (CUdeviceptr_v1 dstDevice, CUdeviceptr_v1 srcDevice, unsigned int ByteCount, CUstream hStream), dstDevice, srcDevice, ByteCount, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpy2DUnaligned_intercepted, cuMemcpy2DUnaligned, (const CUDA_MEMCPY2D_v1 *pCopy), pCopy)

CU_HOOK_DRIVER_FUNC(cuMemcpy2DAsync_intercepted, cuMemcpy2DAsync, (const CUDA_MEMCPY2D_v1 *pCopy, CUstream hStream), pCopy, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpy3D_intercepted, cuMemcpy3D, (const CUDA_MEMCPY3D_v1 *pCopy), pCopy)

CU_HOOK_DRIVER_FUNC(cuMemcpy3DAsync_intercepted, cuMemcpy3DAsync, (const CUDA_MEMCPY3D_v1 *pCopy, CUstream hStream), pCopy, hStream)

CU_HOOK_DRIVER_FUNC(cuMemcpy3DPeer_intercepted, cuMemcpy3DPeer, (const CUDA_MEMCPY3D_PEER *pCopy), pCopy)

CU_HOOK_DRIVER_FUNC(cuMemcpy3DPeerAsync_intercepted, cuMemcpy3DPeerAsync, (const CUDA_MEMCPY3D_PEER *pCopy, CUstream hStream), pCopy, hStream)

CU_HOOK_DRIVER_FUNC(cuMemsetD8_intercepted, cuMemsetD8, (CUdeviceptr_v1 dstDevice, unsigned char uc, unsigned int N), dstDevice, uc, N)

CU_HOOK_DRIVER_FUNC(cuMemsetD8Async_intercepted, cuMemsetD8Async, (CUdeviceptr dstDevice, unsigned char uc, size_t N, CUstream hStream), dstDevice, uc, N, hStream)

CU_HOOK_DRIVER_FUNC(cuMemsetD2D8_intercepted, cuMemsetD2D8, (CUdeviceptr_v1 dstDevice, unsigned int dstPitch, unsigned char uc, unsigned int Width, unsigned int Height), dstDevice, dstPitch, uc, Width, Height)

CU_HOOK_DRIVER_FUNC(cuMemsetD2D8Async_intercepted, cuMemsetD2D8Async, (CUdeviceptr dstDevice, size_t dstPitch, unsigned char uc, size_t Width, size_t Height, CUstream hStream), dstDevice, dstPitch, uc, Width, Height, hStream)

CU_HOOK_DRIVER_FUNC(cuFuncSetCacheConfig_intercepted, cuFuncSetCacheConfig, (CUfunction hfunc, CUfunc_cache config), hfunc, config)

CU_HOOK_DRIVER_FUNC(cuFuncSetSharedMemConfig_intercepted, cuFuncSetSharedMemConfig, (CUfunction hfunc, CUsharedconfig config), hfunc, config)

CU_HOOK_DRIVER_FUNC(cuFuncGetAttribute_intercepted, cuFuncGetAttribute, (int *pi, CUfunction_attribute attrib, CUfunction hfunc), pi, attrib, hfunc)

CU_HOOK_DRIVER_FUNC(cuFuncSetAttribute_intercepted, cuFuncSetAttribute, (CUfunction hfunc, CUfunction_attribute attrib, int value), hfunc, attrib, value)

CU_HOOK_DRIVER_FUNC(cuFuncGetName_intercepted, cuFuncGetName, (const char **name, CUfunction hfunc), name, hfunc)

CU_HOOK_DRIVER_FUNC(cuFuncGetParamInfo_intercepted, cuFuncGetParamInfo, (CUfunction func, size_t paramIndex, size_t *paramOffset, size_t *paramSize), func, paramIndex, paramOffset, paramSize)

CU_HOOK_DRIVER_FUNC(cuArrayCreate_intercepted, cuArrayCreate, (CUarray *pHandle, const CUDA_ARRAY_DESCRIPTOR_v1 *pAllocateArray), pHandle, pAllocateArray)

CU_HOOK_DRIVER_FUNC(cuArrayGetDescriptor_intercepted, cuArrayGetDescriptor, (CUDA_ARRAY_DESCRIPTOR_v1 *pArrayDescriptor, CUarray hArray), pArrayDescriptor, hArray)

CU_HOOK_DRIVER_FUNC(cuArrayGetSparseProperties_intercepted, cuArrayGetSparseProperties, (CUDA_ARRAY_SPARSE_PROPERTIES *sparseProperties, CUarray array), sparseProperties, array)

CU_HOOK_DRIVER_FUNC(cuArrayGetPlane_intercepted, cuArrayGetPlane, (CUarray *pPlaneArray, CUarray hArray, unsigned int planeIdx), pPlaneArray, hArray, planeIdx)

CU_HOOK_DRIVER_FUNC(cuArray3DCreate_intercepted, cuArray3DCreate, (CUarray *pHandle, const CUDA_ARRAY3D_DESCRIPTOR_v1 *pAllocateArray), pHandle, pAllocateArray)

CU_HOOK_DRIVER_FUNC(cuArray3DGetDescriptor_intercepted, cuArray3DGetDescriptor, (CUDA_ARRAY3D_DESCRIPTOR_v1 *pArrayDescriptor, CUarray hArray), pArrayDescriptor, hArray)

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

CU_HOOK_DRIVER_FUNC(cuGraphicsResourceGetMappedPointer_intercepted, cuGraphicsResourceGetMappedPointer, (CUdeviceptr_v1 *pDevPtr, unsigned int *pSize, CUgraphicsResource resource), pDevPtr, pSize, resource)

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
