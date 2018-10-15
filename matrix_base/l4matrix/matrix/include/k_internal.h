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
** 文   件   名: k_internal.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 04 日
**
** 描        述: 这是系统内部函数声明.
*********************************************************************************************************/

#ifndef  __K_INTERNAL_H
#define  __K_INTERNAL_H

/*********************************************************************************************************
  位图基本操作
*********************************************************************************************************/
VOID       _BitmapInit(PMX_BMAP  pbmap);
VOID       _BitmapAdd(PMX_BMAP   pbmap, UINT32  uiPriority);
VOID       _BitmapDel(PMX_BMAP   pbmap, UINT32  uiPriority);
UINT32     _BitmapHigh(PMX_BMAP  pbmap);
BOOL       _BitmapIsEmpty(PMX_BMAP  pbmap);
/*********************************************************************************************************
  候选表操作
*********************************************************************************************************/
VOID       _CandTableUpdate(PMX_PHYS_CPU  pcpuCur);
PMX_VCPU   _CandTableVcpuGet(PMX_PHYS_CPU  pcpuCur);
BOOL       _CandTableTryAdd(PMX_VCPU  pvcpu);
BOOL       _CandTableTryDel(PMX_VCPU  pvcpu);
/*********************************************************************************************************
  优先级控制块操作
*********************************************************************************************************/
PMX_RDYP   _RdyPrioGet(PMX_VCPU  pvcpu);
BOOL       _RdyPrioIsEmpty(PMX_RDYP  prdyp);
BOOL       _RdyPrioIsOne(PMX_RDYP  prdyp);
VOID       _RdyPrioAddVcpu(PMX_VCPU  pvcpu, PMX_RDYP  prdyp, BOOL  bIsHeader);
VOID       _RdyPrioDelVcpu(PMX_VCPU  pvcpu, PMX_RDYP  prdyp);
/*********************************************************************************************************
  就绪表操作
*********************************************************************************************************/
PMX_READY  _ReadyTableGet(PMX_PHYS_CPU  pcpu,  UINT32 *puiPriority);
VOID       _ReadyTableAdd(PMX_VCPU  pvcpu);
VOID       _ReadyTableDel(PMX_VCPU  pvcpu);
PMX_VCPU   _ReadyTableSeek(PMX_READY  pready, UINT32  uiPriority);

/*********************************************************************************************************
  唤醒表操作
*********************************************************************************************************/
VOID       _WakeupAdd(PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod);
VOID       _WakeupDel(PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod);
BOOL       _WakeupIsIn(PMX_WAKEUP_NODE pwnod);
VOID       _WakeupTime(PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod, ULONG  *pulLeft);
VOID       _WakeupTick(ULONG  ulCounter);
/*********************************************************************************************************
  SMP 核间中断
*********************************************************************************************************/
VOID       _SmpIpiSend(ULONG  ulCpuId, ULONG  ulIpiVec, INT  iWait, BOOL  bIntLock);
VOID       _SmpIpiSendOther(ULONG  ulIpiVec, INT  iWait);
INT        _SmpCallFunc(ULONG         ulCpuId, 
                        INT_FUNCPTR   pfunc, 
                        PVOID         pvArg,
                        VOID_FUNCPTR  pfuncAsync,
                        PVOID         pvAsync,
                        INT           iOpt);
VOID       _SmpCallFuncAllOther(INT_FUNCPTR   pfunc, 
                                PVOID         pvArg,
                                VOID_FUNCPTR  pfuncAsync,
                                PVOID         pvAsync,
                                INT           iOpt);
VOID       _SmpIpiProc(PMX_PHYS_CPU  pcpuCur);
VOID       _SmpIpiTryProc(PMX_PHYS_CPU  pcpuCur);
VOID       _SmpIpiUpdate(PMX_PHYS_CPU  pcpu);
/*********************************************************************************************************
  虚拟中断 Pend 位操作
*********************************************************************************************************/
VOID       _vIrqPendIni(PMX_VCPU  pvcpu);
VOID       _vIrqPendSet(PMX_VCPU  pvcpu, ULONG  ulVec);
VOID       _vIrqPendClr(PMX_VCPU  pvcpu, ULONG  ulVec);
BOOL       _vIrqPendGet(PMX_VCPU  pvcpu, ULONG  *pulVec);

#endif                                                                  /*  __K_INTERNAL_H              */
/*********************************************************************************************************
  END
*********************************************************************************************************/

