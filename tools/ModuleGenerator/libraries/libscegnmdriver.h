#pragma once

#include "library_common.h" 

namespace Libraries::GnmDriver{

int PS4_SYSV_ABI sceGnmAddEqEvent();
int PS4_SYSV_ABI sceGnmAreSubmitsAllowed();
int PS4_SYSV_ABI sceGnmBeginWorkload();
int PS4_SYSV_ABI sceGnmComputeWaitOnAddress();
int PS4_SYSV_ABI sceGnmComputeWaitSemaphore();
int PS4_SYSV_ABI sceGnmCreateWorkloadStream();
int PS4_SYSV_ABI sceGnmDebuggerGetAddressWatch();
int PS4_SYSV_ABI sceGnmDebuggerHaltWavefront();
int PS4_SYSV_ABI sceGnmDebuggerReadGds();
int PS4_SYSV_ABI sceGnmDebuggerReadSqIndirectRegister();
int PS4_SYSV_ABI sceGnmDebuggerResumeWavefront();
int PS4_SYSV_ABI sceGnmDebuggerResumeWavefrontCreation();
int PS4_SYSV_ABI sceGnmDebuggerSetAddressWatch();
int PS4_SYSV_ABI sceGnmDebuggerWriteGds();
int PS4_SYSV_ABI sceGnmDebuggerWriteSqIndirectRegister();
int PS4_SYSV_ABI sceGnmDebugHardwareStatus();
int PS4_SYSV_ABI sceGnmDeleteEqEvent();
int PS4_SYSV_ABI sceGnmDestroyWorkloadStream();
int PS4_SYSV_ABI sceGnmDingDong();
int PS4_SYSV_ABI sceGnmDingDongForWorkload();
int PS4_SYSV_ABI sceGnmDisableMipStatsReport();
int PS4_SYSV_ABI sceGnmDispatchDirect();
int PS4_SYSV_ABI sceGnmDispatchIndirect();
int PS4_SYSV_ABI sceGnmDispatchIndirectOnMec();
int PS4_SYSV_ABI sceGnmDispatchInitDefaultHardwareState();
int PS4_SYSV_ABI sceGnmDrawIndex();
int PS4_SYSV_ABI sceGnmDrawIndexAuto();
int PS4_SYSV_ABI sceGnmDrawIndexIndirect();
int PS4_SYSV_ABI sceGnmDrawIndexIndirectCountMulti();
int PS4_SYSV_ABI sceGnmDrawIndexIndirectMulti();
int PS4_SYSV_ABI sceGnmDrawIndexMultiInstanced();
int PS4_SYSV_ABI sceGnmDrawIndexOffset();
int PS4_SYSV_ABI sceGnmDrawIndirect();
int PS4_SYSV_ABI sceGnmDrawIndirectCountMulti();
int PS4_SYSV_ABI sceGnmDrawIndirectMulti();
int PS4_SYSV_ABI sceGnmDrawInitDefaultHardwareState();
int PS4_SYSV_ABI sceGnmDrawInitDefaultHardwareState175();
int PS4_SYSV_ABI sceGnmDrawInitDefaultHardwareState200();
int PS4_SYSV_ABI sceGnmDrawInitDefaultHardwareState350();
int PS4_SYSV_ABI sceGnmDrawInitToDefaultContextState();
int PS4_SYSV_ABI sceGnmDrawInitToDefaultContextState400();
int PS4_SYSV_ABI sceGnmDrawOpaqueAuto();
int PS4_SYSV_ABI sceGnmDriverCaptureInProgress();
int PS4_SYSV_ABI sceGnmDriverInternalRetrieveGnmInterface();
int PS4_SYSV_ABI sceGnmDriverInternalRetrieveGnmInterfaceForGpuDebugger();
int PS4_SYSV_ABI sceGnmDriverInternalRetrieveGnmInterfaceForGpuException();
int PS4_SYSV_ABI sceGnmDriverInternalRetrieveGnmInterfaceForHDRScopes();
int PS4_SYSV_ABI sceGnmDriverInternalRetrieveGnmInterfaceForReplay();
int PS4_SYSV_ABI sceGnmDriverInternalRetrieveGnmInterfaceForResourceRegistration();
int PS4_SYSV_ABI sceGnmDriverInternalRetrieveGnmInterfaceForValidation();
int PS4_SYSV_ABI sceGnmDriverInternalVirtualQuery();
int PS4_SYSV_ABI sceGnmDriverTraceInProgress();
int PS4_SYSV_ABI sceGnmDriverTriggerCapture();
int PS4_SYSV_ABI sceGnmEndWorkload();
int PS4_SYSV_ABI sceGnmFindResourcesPublic();
int PS4_SYSV_ABI sceGnmFlushGarlic();
int PS4_SYSV_ABI sceGnmGetCoredumpAddress();
int PS4_SYSV_ABI sceGnmGetCoredumpMode();
int PS4_SYSV_ABI sceGnmGetCoredumpProtectionFaultTimestamp();
int PS4_SYSV_ABI sceGnmGetDbgGcHandle();
int PS4_SYSV_ABI sceGnmGetDebugTimestamp();
int PS4_SYSV_ABI sceGnmGetEqEventType();
int PS4_SYSV_ABI sceGnmGetEqTimeStamp();
int PS4_SYSV_ABI sceGnmGetGpuBlockStatus();
int PS4_SYSV_ABI sceGnmGetGpuCoreClockFrequency();
int PS4_SYSV_ABI sceGnmGetGpuInfoStatus();
int PS4_SYSV_ABI sceGnmGetLastWaitedAddress();
int PS4_SYSV_ABI sceGnmGetNumTcaUnits();
int PS4_SYSV_ABI sceGnmGetOffChipTessellationBufferSize();
int PS4_SYSV_ABI sceGnmGetOwnerName();
int PS4_SYSV_ABI sceGnmGetPhysicalCounterFromVirtualized();
int PS4_SYSV_ABI sceGnmGetProtectionFaultTimeStamp();
int PS4_SYSV_ABI sceGnmGetResourceBaseAddressAndSizeInBytes();
int PS4_SYSV_ABI sceGnmGetResourceName();
int PS4_SYSV_ABI sceGnmGetResourceShaderGuid();
int PS4_SYSV_ABI sceGnmGetResourceType();
int PS4_SYSV_ABI sceGnmGetResourceUserData();
int PS4_SYSV_ABI sceGnmGetShaderProgramBaseAddress();
int PS4_SYSV_ABI sceGnmGetShaderStatus();
int PS4_SYSV_ABI sceGnmGetTheTessellationFactorRingBufferBaseAddress();
int PS4_SYSV_ABI sceGnmGpuPaDebugEnter();
int PS4_SYSV_ABI sceGnmGpuPaDebugLeave();
int PS4_SYSV_ABI sceGnmInsertDingDongMarker();
int PS4_SYSV_ABI sceGnmInsertPopMarker();
int PS4_SYSV_ABI sceGnmInsertPushColorMarker();
int PS4_SYSV_ABI sceGnmInsertPushMarker();
int PS4_SYSV_ABI sceGnmInsertSetColorMarker();
int PS4_SYSV_ABI sceGnmInsertSetMarker();
int PS4_SYSV_ABI sceGnmInsertThreadTraceMarker();
int PS4_SYSV_ABI sceGnmInsertWaitFlipDone();
int PS4_SYSV_ABI sceGnmIsCoredumpValid();
int PS4_SYSV_ABI sceGnmIsUserPaEnabled();
int PS4_SYSV_ABI sceGnmLogicalCuIndexToPhysicalCuIndex();
int PS4_SYSV_ABI sceGnmLogicalCuMaskToPhysicalCuMask();
int PS4_SYSV_ABI sceGnmLogicalTcaUnitToPhysical();
int PS4_SYSV_ABI sceGnmMapComputeQueue();
int PS4_SYSV_ABI sceGnmMapComputeQueueWithPriority();
int PS4_SYSV_ABI sceGnmPaDisableFlipCallbacks();
int PS4_SYSV_ABI sceGnmPaEnableFlipCallbacks();
int PS4_SYSV_ABI sceGnmPaHeartbeat();
int PS4_SYSV_ABI sceGnmQueryResourceRegistrationUserMemoryRequirements();
int PS4_SYSV_ABI sceGnmRaiseUserExceptionEvent();
int PS4_SYSV_ABI sceGnmRegisterGdsResource();
int PS4_SYSV_ABI sceGnmRegisterGnmLiveCallbackConfig();
int PS4_SYSV_ABI sceGnmRegisterOwner();
int PS4_SYSV_ABI sceGnmRegisterResource();
int PS4_SYSV_ABI sceGnmRequestFlipAndSubmitDone();
int PS4_SYSV_ABI sceGnmRequestFlipAndSubmitDoneForWorkload();
int PS4_SYSV_ABI sceGnmRequestMipStatsReportAndReset();
int PS4_SYSV_ABI sceGnmResetVgtControl();
int PS4_SYSV_ABI sceGnmSdmaClose();
int PS4_SYSV_ABI sceGnmSdmaConstFill();
int PS4_SYSV_ABI sceGnmSdmaCopyLinear();
int PS4_SYSV_ABI sceGnmSdmaCopyTiled();
int PS4_SYSV_ABI sceGnmSdmaCopyWindow();
int PS4_SYSV_ABI sceGnmSdmaFlush();
int PS4_SYSV_ABI sceGnmSdmaGetMinCmdSize();
int PS4_SYSV_ABI sceGnmSdmaOpen();
int PS4_SYSV_ABI sceGnmSetCsShader();
int PS4_SYSV_ABI sceGnmSetCsShaderWithModifier();
int PS4_SYSV_ABI sceGnmSetEmbeddedPsShader();
int PS4_SYSV_ABI sceGnmSetEmbeddedVsShader();
int PS4_SYSV_ABI sceGnmSetEsShader();
int PS4_SYSV_ABI sceGnmSetGsRingSizes();
int PS4_SYSV_ABI sceGnmSetGsShader();
int PS4_SYSV_ABI sceGnmSetHsShader();
int PS4_SYSV_ABI sceGnmSetLsShader();
int PS4_SYSV_ABI sceGnmSetPsShader();
int PS4_SYSV_ABI sceGnmSetPsShader350();
int PS4_SYSV_ABI sceGnmSetResourceRegistrationUserMemory();
int PS4_SYSV_ABI sceGnmSetResourceUserData();
int PS4_SYSV_ABI sceGnmSetSpiEnableSqCounters();
int PS4_SYSV_ABI sceGnmSetSpiEnableSqCountersForUnitInstance();
int PS4_SYSV_ABI sceGnmSetupMipStatsReport();
int PS4_SYSV_ABI sceGnmSetVgtControl();
int PS4_SYSV_ABI sceGnmSetVsShader();
int PS4_SYSV_ABI sceGnmSetWaveLimitMultiplier();
int PS4_SYSV_ABI sceGnmSetWaveLimitMultipliers();
int PS4_SYSV_ABI sceGnmSpmEndSpm();
int PS4_SYSV_ABI sceGnmSpmInit();
int PS4_SYSV_ABI sceGnmSpmInit2();
int PS4_SYSV_ABI sceGnmSpmSetDelay();
int PS4_SYSV_ABI sceGnmSpmSetMuxRam();
int PS4_SYSV_ABI sceGnmSpmSetMuxRam2();
int PS4_SYSV_ABI sceGnmSpmSetSelectCounter();
int PS4_SYSV_ABI sceGnmSpmSetSpmSelects();
int PS4_SYSV_ABI sceGnmSpmSetSpmSelects2();
int PS4_SYSV_ABI sceGnmSpmStartSpm();
int PS4_SYSV_ABI sceGnmSqttFini();
int PS4_SYSV_ABI sceGnmSqttFinishTrace();
int PS4_SYSV_ABI sceGnmSqttGetBcInfo();
int PS4_SYSV_ABI sceGnmSqttGetGpuClocks();
int PS4_SYSV_ABI sceGnmSqttGetHiWater();
int PS4_SYSV_ABI sceGnmSqttGetStatus();
int PS4_SYSV_ABI sceGnmSqttGetTraceCounter();
int PS4_SYSV_ABI sceGnmSqttGetTraceWptr();
int PS4_SYSV_ABI sceGnmSqttGetWrapCounts();
int PS4_SYSV_ABI sceGnmSqttGetWrapCounts2();
int PS4_SYSV_ABI sceGnmSqttGetWritebackLabels();
int PS4_SYSV_ABI sceGnmSqttInit();
int PS4_SYSV_ABI sceGnmSqttSelectMode();
int PS4_SYSV_ABI sceGnmSqttSelectTarget();
int PS4_SYSV_ABI sceGnmSqttSelectTokens();
int PS4_SYSV_ABI sceGnmSqttSetCuPerfMask();
int PS4_SYSV_ABI sceGnmSqttSetDceEventWrite();
int PS4_SYSV_ABI sceGnmSqttSetHiWater();
int PS4_SYSV_ABI sceGnmSqttSetTraceBuffer2();
int PS4_SYSV_ABI sceGnmSqttSetTraceBuffers();
int PS4_SYSV_ABI sceGnmSqttSetUserData();
int PS4_SYSV_ABI sceGnmSqttSetUserdataTimer();
int PS4_SYSV_ABI sceGnmSqttStartTrace();
int PS4_SYSV_ABI sceGnmSqttStopTrace();
int PS4_SYSV_ABI sceGnmSqttSwitchTraceBuffer();
int PS4_SYSV_ABI sceGnmSqttSwitchTraceBuffer2();
int PS4_SYSV_ABI sceGnmSqttWaitForEvent();
int PS4_SYSV_ABI sceGnmSubmitAndFlipCommandBuffers();
int PS4_SYSV_ABI sceGnmSubmitAndFlipCommandBuffersForWorkload();
int PS4_SYSV_ABI sceGnmSubmitCommandBuffers();
int PS4_SYSV_ABI sceGnmSubmitCommandBuffersForWorkload();
int PS4_SYSV_ABI sceGnmSubmitDone();
int PS4_SYSV_ABI sceGnmUnmapComputeQueue();
int PS4_SYSV_ABI sceGnmUnregisterAllResourcesForOwner();
int PS4_SYSV_ABI sceGnmUnregisterOwnerAndResources();
int PS4_SYSV_ABI sceGnmUnregisterResource();
int PS4_SYSV_ABI sceGnmUpdateGsShader();
int PS4_SYSV_ABI sceGnmUpdateHsShader();
int PS4_SYSV_ABI sceGnmUpdatePsShader();
int PS4_SYSV_ABI sceGnmUpdatePsShader350();
int PS4_SYSV_ABI sceGnmUpdateVsShader();
int PS4_SYSV_ABI sceGnmValidateCommandBuffers();
int PS4_SYSV_ABI sceGnmValidateDisableDiagnostics();
int PS4_SYSV_ABI sceGnmValidateDisableDiagnostics2();
int PS4_SYSV_ABI sceGnmValidateDispatchCommandBuffers();
int PS4_SYSV_ABI sceGnmValidateDrawCommandBuffers();
int PS4_SYSV_ABI sceGnmValidateGetDiagnosticInfo();
int PS4_SYSV_ABI sceGnmValidateGetDiagnostics();
int PS4_SYSV_ABI sceGnmValidateGetVersion();
int PS4_SYSV_ABI sceGnmValidateOnSubmitEnabled();
int PS4_SYSV_ABI sceGnmValidateResetState();
int PS4_SYSV_ABI sceGnmValidationRegisterMemoryCheckCallback();
int PS4_SYSV_ABI sceRazorCaptureCommandBuffersOnlyImmediate();
int PS4_SYSV_ABI sceRazorCaptureCommandBuffersOnlySinceLastFlip();
int PS4_SYSV_ABI sceRazorCaptureImmediate();
int PS4_SYSV_ABI sceRazorCaptureSinceLastFlip();
int PS4_SYSV_ABI sceRazorIsLoaded();
int PS4_SYSV_ABI Func_Bj0GWi1jWcM();
int PS4_SYSV_ABI Func_DKusr7JYQp0();
int PS4_SYSV_ABI Func_FQzzNvwumaM();
int PS4_SYSV_ABI Func_F8pof57lLUk();
int PS4_SYSV_ABI Func_GHC4n3Wca0U();
int PS4_SYSV_ABI Func_JvkCnvaKlV4();
int PS4_SYSV_ABI Func_MB49u6sJLbA();
int PS4_SYSV_ABI Func_MLr+Fyrxf+8();
int PS4_SYSV_ABI Func_Pmo+ggPZUxc();
int PS4_SYSV_ABI Func_QP7vDGU0xDQ();
int PS4_SYSV_ABI Func_QWuQed5Mus4();
int PS4_SYSV_ABI Func_R3TYO7Tdv5o();
int PS4_SYSV_ABI Func_UGePHM7rmgA();
int PS4_SYSV_ABI Func_VKLsX6TGJBM();
int PS4_SYSV_ABI Func_WpxSyDE4rms();
int PS4_SYSV_ABI Func_XSIZOjHqEUI();
int PS4_SYSV_ABI Func_clo23rtglI0();
int PS4_SYSV_ABI Func_gCGlAvphubs();
int PS4_SYSV_ABI Func_nQAv4PpA8OY();
int PS4_SYSV_ABI Func_nSl-NqcCi3E();
int PS4_SYSV_ABI Func_otfsenvPebM();
int PS4_SYSV_ABI Func_qhKjy4mQhUo();
int PS4_SYSV_ABI Func_rcjdwAUCC8Y();
int PS4_SYSV_ABI Func_sKhoi2ectC0();
int PS4_SYSV_ABI Func_tIkCC1FXpf8();
int PS4_SYSV_ABI Func_ut57TBmRQN0();
int PS4_SYSV_ABI Func_0VEbnc-7Pdk();
int PS4_SYSV_ABI Func_1TRGZJsC5Y4();
int PS4_SYSV_ABI Func_2Lbo4o4e8KM();
int PS4_SYSV_ABI Func_2T1zOhnddFQ();
int PS4_SYSV_ABI Func_3plUQ7wqgxc();
int PS4_SYSV_ABI Func_326VKBUMI-8();
int PS4_SYSV_ABI Func_7LTGukH+M1A();
int PS4_SYSV_ABI sceGnmDebugModuleReset();
int PS4_SYSV_ABI sceGnmDebugReset();
int PS4_SYSV_ABI Func_xMMot887QXE();
int PS4_SYSV_ABI sceGnmDrawInitToDefaultContextStateInternalCommand();
int PS4_SYSV_ABI sceGnmDrawInitToDefaultContextStateInternalSize();
int PS4_SYSV_ABI sceGnmFindResources();
int PS4_SYSV_ABI sceGnmGetResourceRegistrationBuffers();
int PS4_SYSV_ABI sceGnmRegisterOwnerForSystem();
int PS4_SYSV_ABI Func_HEOIaxbuVTA();
int PS4_SYSV_ABI Func_gQNwGezNDgE();
int PS4_SYSV_ABI Func_v7QcBXR48L8();
int PS4_SYSV_ABI Func_5R1E24FRI4w();
int PS4_SYSV_ABI Func_+RaJBCVJZVM();

void RegisterlibSceGnmDriver(Loader::SymbolsResolver * sym);
}