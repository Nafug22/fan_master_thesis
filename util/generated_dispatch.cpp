TRY_INTERCEPT("cuInit", cuInit_intercepted)

TRY_INTERCEPT("cuDeviceGet", cuDeviceGet_intercepted)

TRY_INTERCEPT("cuDeviceGetCount", cuDeviceGetCount_intercepted)

TRY_INTERCEPT("cuDeviceGetName", cuDeviceGetName_intercepted)

TRY_INTERCEPT("cuDeviceTotalMem", cuDeviceTotalMem_intercepted)

TRY_INTERCEPT("cuDeviceGetAttribute", cuDeviceGetAttribute_intercepted)

TRY_INTERCEPT("cuDeviceGetP2PAttribute", cuDeviceGetP2PAttribute_intercepted)

TRY_INTERCEPT("cuDriverGetVersion", cuDriverGetVersion_intercepted)

TRY_INTERCEPT("cuDeviceGetByPCIBusId", cuDeviceGetByPCIBusId_intercepted)

TRY_INTERCEPT("cuDeviceGetPCIBusId", cuDeviceGetPCIBusId_intercepted)

TRY_INTERCEPT("cuDeviceGetUuid", cuDeviceGetUuid_intercepted)

TRY_INTERCEPT("cuDeviceGetTexture1DLinearMaxWidth", cuDeviceGetTexture1DLinearMaxWidth_intercepted)

TRY_INTERCEPT("cuDeviceGetDefaultMemPool", cuDeviceGetDefaultMemPool_intercepted)

TRY_INTERCEPT("cuDeviceSetMemPool", cuDeviceSetMemPool_intercepted)

TRY_INTERCEPT("cuDeviceGetMemPool", cuDeviceGetMemPool_intercepted)

TRY_INTERCEPT("cuFlushGPUDirectRDMAWrites", cuFlushGPUDirectRDMAWrites_intercepted)

TRY_INTERCEPT("cuDevicePrimaryCtxRetain", cuDevicePrimaryCtxRetain_intercepted)

TRY_INTERCEPT("cuDevicePrimaryCtxRelease", cuDevicePrimaryCtxRelease_intercepted)

TRY_INTERCEPT("cuDevicePrimaryCtxSetFlags", cuDevicePrimaryCtxSetFlags_intercepted)

TRY_INTERCEPT("cuDevicePrimaryCtxGetState", cuDevicePrimaryCtxGetState_intercepted)

TRY_INTERCEPT("cuDevicePrimaryCtxReset", cuDevicePrimaryCtxReset_intercepted)

TRY_INTERCEPT("cuCtxCreate", cuCtxCreate_intercepted)

TRY_INTERCEPT("cuCtxGetFlags", cuCtxGetFlags_intercepted)

TRY_INTERCEPT("cuCtxSetCurrent", cuCtxSetCurrent_intercepted)

TRY_INTERCEPT("cuCtxGetCurrent", cuCtxGetCurrent_intercepted)

TRY_INTERCEPT("cuCtxDetach", cuCtxDetach_intercepted)

TRY_INTERCEPT("cuCtxGetApiVersion", cuCtxGetApiVersion_intercepted)

TRY_INTERCEPT("cuCtxGetDevice", cuCtxGetDevice_intercepted)

TRY_INTERCEPT("cuCtxGetLimit", cuCtxGetLimit_intercepted)

TRY_INTERCEPT("cuCtxSetLimit", cuCtxSetLimit_intercepted)

TRY_INTERCEPT("cuCtxGetCacheConfig", cuCtxGetCacheConfig_intercepted)

TRY_INTERCEPT("cuCtxSetCacheConfig", cuCtxSetCacheConfig_intercepted)

TRY_INTERCEPT("cuCtxGetSharedMemConfig", cuCtxGetSharedMemConfig_intercepted)

TRY_INTERCEPT("cuCtxGetStreamPriorityRange", cuCtxGetStreamPriorityRange_intercepted)

TRY_INTERCEPT("cuCtxSetSharedMemConfig", cuCtxSetSharedMemConfig_intercepted)

TRY_INTERCEPT("cuCtxSynchronize", cuCtxSynchronize_intercepted)

TRY_INTERCEPT("cuCtxResetPersistingL2Cache", cuCtxResetPersistingL2Cache_intercepted)

TRY_INTERCEPT("cuCtxPopCurrent", cuCtxPopCurrent_intercepted)

TRY_INTERCEPT("cuCtxPushCurrent", cuCtxPushCurrent_intercepted)

TRY_INTERCEPT("cuModuleLoad", cuModuleLoad_intercepted)

TRY_INTERCEPT("cuModuleLoadData", cuModuleLoadData_intercepted)

TRY_INTERCEPT("cuModuleLoadFatBinary", cuModuleLoadFatBinary_intercepted)

TRY_INTERCEPT("cuModuleUnload", cuModuleUnload_intercepted)

TRY_INTERCEPT("cuModuleGetFunction", cuModuleGetFunction_intercepted)

TRY_INTERCEPT("cuModuleGetGlobal", cuModuleGetGlobal_intercepted)

TRY_INTERCEPT("cuModuleGetTexRef", cuModuleGetTexRef_intercepted)

TRY_INTERCEPT("cuModuleGetSurfRef", cuModuleGetSurfRef_intercepted)

TRY_INTERCEPT("cuModuleGetLoadingMode", cuModuleGetLoadingMode_intercepted)

TRY_INTERCEPT("cuLibraryUnload", cuLibraryUnload_intercepted)

TRY_INTERCEPT("cuLibraryGetKernel", cuLibraryGetKernel_intercepted)

TRY_INTERCEPT("cuLibraryGetModule", cuLibraryGetModule_intercepted)

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

TRY_INTERCEPT("cuMemAlloc", cuMemAlloc_intercepted)

TRY_INTERCEPT("cuMemAllocPitch", cuMemAllocPitch_intercepted)

TRY_INTERCEPT("cuMemFree", cuMemFree_intercepted)

TRY_INTERCEPT("cuMemGetAddressRange", cuMemGetAddressRange_intercepted)

TRY_INTERCEPT("cuMemFreeHost", cuMemFreeHost_intercepted)

TRY_INTERCEPT("cuMemHostAlloc", cuMemHostAlloc_intercepted)

TRY_INTERCEPT("cuMemHostGetDevicePointer", cuMemHostGetDevicePointer_intercepted)

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

TRY_INTERCEPT("cuMemcpyHtoD", cuMemcpyHtoD_intercepted)

TRY_INTERCEPT("cuMemcpyHtoDAsync", cuMemcpyHtoDAsync_intercepted)

TRY_INTERCEPT("cuMemcpyDtoH", cuMemcpyDtoH_intercepted)

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

TRY_INTERCEPT("cuMemsetD8Async", cuMemsetD8Async_intercepted)

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

TRY_INTERCEPT("cuLaunchKernel", cuLaunchKernel_intercepted)

TRY_INTERCEPT("cuLaunchCooperativeKernel", cuLaunchCooperativeKernel_intercepted)

TRY_INTERCEPT("cuLaunchCooperativeKernelMultiDevice", cuLaunchCooperativeKernelMultiDevice_intercepted)

TRY_INTERCEPT("cuLaunchHostFunc", cuLaunchHostFunc_intercepted)

TRY_INTERCEPT("cuLaunchKernelEx", cuLaunchKernelEx_intercepted)

TRY_INTERCEPT("cuEventCreate", cuEventCreate_intercepted)

TRY_INTERCEPT("cuEventRecord", cuEventRecord_intercepted)

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

TRY_INTERCEPT("cuStreamCreate", cuStreamCreate_intercepted)

TRY_INTERCEPT("cuStreamCreateWithPriority", cuStreamCreateWithPriority_intercepted)

TRY_INTERCEPT("cuStreamGetPriority", cuStreamGetPriority_intercepted)

TRY_INTERCEPT("cuStreamGetFlags", cuStreamGetFlags_intercepted)

TRY_INTERCEPT("cuStreamGetCtx", cuStreamGetCtx_intercepted)

TRY_INTERCEPT("cuStreamGetId", cuStreamGetId_intercepted)

TRY_INTERCEPT("cuStreamDestroy", cuStreamDestroy_intercepted)

TRY_INTERCEPT("cuStreamWaitEvent", cuStreamWaitEvent_intercepted)

TRY_INTERCEPT("cuStreamAddCallback", cuStreamAddCallback_intercepted)

TRY_INTERCEPT("cuStreamSynchronize", cuStreamSynchronize_intercepted)

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

TRY_INTERCEPT("cuGetExportTable", cuGetExportTable_intercepted)

TRY_INTERCEPT("cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags", cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags_intercepted)

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

TRY_INTERCEPT("cuStreamIsCapturing", cuStreamIsCapturing_intercepted)

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
