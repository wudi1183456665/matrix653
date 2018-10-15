/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                嵌入式 L4 微内核操作系统
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: k_api.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 18 日
**
** 描        述: 微内核各个子模块提供的接口.
*********************************************************************************************************/

#ifndef __K_API_H
#define __K_API_H

/*********************************************************************************************************
  init 子模块提供接口
*********************************************************************************************************/
VOID          initMatrix(CPCHAR  pcParam);
VOID          initSmpMatrix(VOID);
/*********************************************************************************************************
  ipc 子模块提供接口
*********************************************************************************************************/
VOID          ipcInit(PMX_IPC  pIpc);
BOOL          ipcKernSend(MX_OBJECT_ID  objTarget, BOOL  bSyn);
BOOL          ipcReadyTestSafe(PMX_IPC  pIpc);
BOOL          ipcReadyTryTestSetSafe(PMX_IPC  pIpc, BOOL  *pbIpcReady);
VOID          ipcTickTimeoutPro(PMX_IPC  pIpc);
VOID          ipcDoSyscall(PMX_VCPU  pvcpu);
/*********************************************************************************************************
  irq 子模块提供接口
*********************************************************************************************************/
VOID          irqRegister(IRQ_FUNCPTR  pfuncIsr);
MX_ERROR      irqAttach(ULONG  ulVec);
VOID          irqEnable(ULONG  ulVec);
VOID          irqDisable(ULONG  ulVec);
VOID          irqDisAttach(PMX_VCPU  pvcpu);
VOID          irqIpiVecSet(ULONG  ulCpuId, ULONG  ulVec);
ULONG         irqEnter(VOID);
MX_ERROR      irqIsr(ULONG  ulVec, PARCH_USER_CTX  puctxSP);
VOID          irqExit(VOID);
VOID          swiRegister(UINT32  uiSwiNum, IRQ_FUNCPTR  pfuncSwi);
/*********************************************************************************************************
  kobj 子模块提供接口
*********************************************************************************************************/
VOID          kobjInit(VOID);
MX_OBJECT_ID  kobjAlloc(UINT8  ucClass);
VOID          kobjFree(MX_OBJECT_ID  ulKobjId);
PVOID         kobjGet(MX_OBJECT_ID  ulKobjId);
BOOL          kobjCheck(MX_OBJECT_ID ulKobjId, UINT8 ucObjClass);
/*********************************************************************************************************
  vcpu 子模块提供接口
*********************************************************************************************************/
PMX_VCPU      vcpuCreate(PVCPU_START_ROUTINE  pfuncStart, PMX_VCPU_ATTR  pvcpuattr);
MX_ERROR      vcpuStart(PMX_VCPU  pvcpu);
MX_ERROR      vcpuSetIrqEntry(PMX_VCPU  pvcpu, VOID_FUNCPTR  pfuncIsr);
VOID          vcpuSuspend(VOID);
VOID          vcpuSleep(ULONG  ulTick);
VOID          vcpuWakeup(PMX_VCPU  pvcpu);
MX_ERROR      vcpuGetPriority(PMX_VCPU  pvcpu, UINT32  *puiPriority);
VOID          vcpuSetAffinity(PMX_VCPU  pvcpu, ULONG  ulCpuIdAffinity);
VOID          vcpuBindPartition(PMX_VCPU  pvcpu, PMX_PRTN  pprtnBelong);
VOID          vcpuBindIpcMsg(PMX_VCPU  pvcpu, PMX_IPC_MSG  pIpcMsg);
ULONG         vcpuDisableInt(VOID);
VOID          vcpuEnableInt(ULONG  ulStatus);
BOOL          vcpuIsIntEnable(PMX_VCPU  pvcpu);
VOID          vcpuSaveStatus(PARCH_USER_CTX  puctx);
VOID          vcpuRestoreStatus(PARCH_USER_CTX  puctx);
VOID          vcpuOccurIrq(PMX_VCPU  pvcpu, ULONG  ulVec);
VOID          vcpuDoPendIsr(PMX_VCPU  pvcpu);
PMX_VCPU      vcpuGetCur(VOID);
/*********************************************************************************************************
  sched 子模块提供接口
*********************************************************************************************************/
VOID          schedInit(VOID);
VOID          schedSwap(PMX_PHYS_CPU  pcpuCur);
MX_ERROR      schedule(VOID);
VOID          scheduleInt(VOID);
VOID          schedYield(PMX_VCPU  pvcpu);
VOID          schedTick(VOID);
VOID          schedActiveCpu(PMX_PHYS_CPU  pcpu);
/*********************************************************************************************************
  kernel 子模块提供接口
*********************************************************************************************************/
INTREG        kernelEnterAndDisIrq(VOID);
INT           kernelExitAndEnIrq(INTREG  iregInterLevel);
VOID          kernelEnter(VOID);
INT           kernelExit(VOID);
VOID          kernelLockIgnIrq(VOID);
VOID          kernelUnlockIgnIrq(VOID);
VOID          kernelUnlockSched(PMX_VCPU  pvcpuOwner);
BOOL          kernelIsEnter(VOID);
MX_ERROR      kernelSched(VOID);
/*********************************************************************************************************
  spin 子模块提供接口
*********************************************************************************************************/
VOID          spinInit(spinlock_t  *psl);
BOOL          spinTryLock(spinlock_t  *psl);
VOID          spinLock(spinlock_t  *psl);
MX_ERROR      spinUnlock(spinlock_t  *psl);
VOID          spinLockIgnIrq(spinlock_t *psl);
VOID          spinUnlockIgnIrq(spinlock_t *psl);
VOID          spinLockAndDisIrq(spinlock_t *psl, INTREG  *piregInterLevel);
MX_ERROR      spinUnlockAndEnIrq(spinlock_t *psl, INTREG  iregInterLevel);
/*********************************************************************************************************
  syscall 子模块提供接口
*********************************************************************************************************/
BOOL          scTimeToTicks(SC_TIME  ullTime, ULONG  *pulTicks);
VOID          scEntry(PARCH_USER_CTX  pscArg);
/*********************************************************************************************************
  partition 子模块提供接口
*********************************************************************************************************/
VOID          prtnInit(VOID);
PMX_PRTN      prtnKernCreate(addr_t  ulVirtAddr, addr_t  ulPhysAddr, ULONG  ulSize);
PMX_PRTN      prtnUserCreate(PMX_PRTN_INFO  ppiPrtn);
VOID          prtnMakeCur(PMX_PRTN  pprtn);
VOID          prtnRestart(PMX_PRTN  pprtn);
/*********************************************************************************************************
  heap 子模块提供接口
*********************************************************************************************************/
VOID          heapInit(VOID);
BOOL          heapAdd(PVOID  pvMemory, ULONG  ulSize);
PVOID         heapAlloc(ULONG  ulSize, UINT  uiType);
PVOID         heapAllocAlign(ULONG  ulSize, UINT  uiType, ULONG  ulAlign);
PVOID         heapAllocSegment(PVOID  pvStartAddr, ULONG  ulSize, UINT  uiType);
PVOID         heapFree(PVOID  pvStartAddr);
/*********************************************************************************************************
  dump 子模块提供接口
*********************************************************************************************************/
VOID          dump(CPCHAR  pcFormat, ... );
VOID          dumpUctx(const PARCH_USER_CTX  puctx);
VOID          dumpUctx1(const PARCH_USER_CTX  puctx);
VOID          dumpReady(const PMX_READY  pready);
VOID          dumpCpu(VOID);
VOID          dumpHeap(VOID);
VOID          dumpHook(VOID);

#endif                                                                  /*  __K_API_H                   */
/*********************************************************************************************************
  END
*********************************************************************************************************/

