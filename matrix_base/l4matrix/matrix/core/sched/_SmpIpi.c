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
** 文   件   名: _SmpIpi.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 20 日
**
** 描        述: CPU 核间中断.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
  IPI LOCK
*********************************************************************************************************/
#define __SMP_IPI_LOCK(pcpu, bIntLock)                                  \
        if (bIntLock) {                                                 \
            spinLockIgnIrq(&pcpu->CPU_slIpi);                           \
        } else {                                                        \
            spinLockAndDisIrq(&pcpu->CPU_slIpi, &iregInterLevel);       \
        }
#define __SMP_IPI_UNLOCK(pcpu, bIntLock)                                \
        if (bIntLock) {                                                 \
            spinUnlockIgnIrq(&pcpu->CPU_slIpi);                         \
        } else {                                                        \
            spinUnlockAndEnIrq(&pcpu->CPU_slIpi, iregInterLevel);       \
        }
#define __SMP_IPI_IRQ_LOCK(bIntLock)                                    \
        if (bIntLock == MX_FALSE) {                                     \
            iregInterLevel = archIntDisable();                          \
        }
#define __SMP_IPI_IRQ_UNLOCK(bIntLock)                                  \
        if (bIntLock == MX_FALSE) {                                     \
            archIntEnable(iregInterLevel);                              \
        }
/*********************************************************************************************************
** 函数名称: _SmpIpiSend
** 功能描述: 发送一个除自定义以外的核间中断给指定的 CPU. (如果不等待则外部可不锁定当前 CPU)
** 输　入  : ulCpuId       CPU ID
**           ulIpiType     核间中断类型 (除自定义类型中断以外)
**           iWait         是否等待处理结束 (MX_IPI_SCHED 绝不允许等待, 否则会死锁)
**           bIntLock      外部是否关中断了.
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
** 注  意  : 通知调度 (MX_IPI_SCHED) 与通知 CPU 停止 (MX_IPI_DOWN) 不需要等待结束.
*********************************************************************************************************/
VOID  _SmpIpiSend (ULONG  ulCpuId, ULONG  ulIpiType, INT  iWait, BOOL  bIntLock)
{
    INTREG         iregInterLevel;
    PMX_PHYS_CPU   pcpuDst = MX_CPU_GET(ulCpuId);
    PMX_PHYS_CPU   pcpuCur = MX_CPU_GET_CUR();
    ULONG          ulMask  = (ULONG)(1 << ulIpiType);
    
    if (!MX_CPU_IS_ACTIVE(pcpuDst)) {                                   /*  CPU 必须被激活              */
        return;
    }
    
    __SMP_IPI_LOCK(pcpuDst, bIntLock);                                  /*  锁定目标 CPU IPI            */
    MX_CPU_ADD_IPI_PEND(ulCpuId, ulMask);                               /*  添加 PEND 位                */
    archMpInt(ulCpuId);
    __SMP_IPI_UNLOCK(pcpuDst, bIntLock);                                /*  解锁目标 CPU IPI            */
    
    if (iWait) {
        while (MX_CPU_GET_IPI_PEND(ulCpuId) & ulMask) {                 /*  等待结束                    */
            __SMP_IPI_IRQ_LOCK(bIntLock);
            _SmpIpiTryProc(pcpuCur);                                    /*  尝试执行其他核发来的 IPI    */
            __SMP_IPI_IRQ_UNLOCK(bIntLock);
            archSpinDelay();
        }
    }
}
/*********************************************************************************************************
** 函数名称: _SmpIpiSendOther
** 功能描述: 发送一个除自定义以外的核间中断给所有其他 CPU, (外部必须锁定当前 CPU 调度)
** 输　入  : ulIpiType     核间中断类型 (除自定义类型中断以外)
**           iWait         是否等待处理结束 (MX_IPI_SCHED 绝不允许等待, 否则会死锁)
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _SmpIpiSendOther (ULONG  ulIpiType, INT  iWait)
{
    ULONG   i;
    ULONG   ulCpuId;
    
    ulCpuId = MX_CPU_GET_CUR_ID();
    
    KN_SMP_WMB();
    MX_CPU_FOREACH_EXCEPT (i, ulCpuId) {
        _SmpIpiSend(i, ulIpiType, iWait, MX_FALSE);
    }
}
/*********************************************************************************************************
** 函数名称: __smpIpiCall
** 功能描述: 发送一个自定义核间中断给指定的 CPU. (如果不等待则外部可不锁定当前 CPU)
** 输　入  : ulCpuId       CPU ID
**           pipim         核间中断参数
** 输　出  : 调用返回值
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static INT  __smpIpiCall (ULONG  ulCpuId, PMX_IPI_MSG  pipim)
{
    INTREG         iregInterLevel;
    PMX_PHYS_CPU   pcpuDst = MX_CPU_GET(ulCpuId);
    PMX_PHYS_CPU   pcpuCur = MX_CPU_GET_CUR();
    
    if (!MX_CPU_IS_ACTIVE(pcpuDst)) {                                   /*  CPU 必须被激活              */
        return  (ERROR_NONE);
    }
    
    __SMP_IPI_LOCK(pcpuDst, MX_FALSE);                                  /*  锁定目标 CPU IPI            */
    _List_Ring_Add_Last(&pipim->IPIM_ringManage, &pcpuDst->CPU_pringMsg);
    pcpuDst->CPU_uiMsgCnt++;
    MX_CPU_ADD_IPI_PEND(ulCpuId, MX_IPI_CALL_MSK);
    archMpInt(ulCpuId);
    __SMP_IPI_UNLOCK(pcpuDst, MX_FALSE);                                /*  解锁目标 CPU IPI            */
    
    while (pipim->IPIM_iWait) {                                         /*  等待结束                    */
        __SMP_IPI_IRQ_LOCK(MX_FALSE);
        _SmpIpiTryProc(pcpuCur);
        __SMP_IPI_IRQ_UNLOCK(MX_FALSE);
        archSpinDelay();
    }
    
    return  (pipim->IPIM_iRet);
}
/*********************************************************************************************************
** 函数名称: __smpIpiCallOther
** 功能描述: 发送一个自定义核间中断给其他所有 CPU. (外部必须锁定当前 CPU 调度)
** 输　入  : pipim         核间中断参数
** 输　出  : NONE (无法确定返回值)
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __smpIpiCallOther (PMX_IPI_MSG  pipim)
{
    ULONG   i;
    ULONG   ulCpuId;
    INT     iWaitSave = pipim->IPIM_iWait;
    
    ulCpuId = MX_CPU_GET_CUR_ID();
    
    KN_SMP_WMB();
    MX_CPU_FOREACH_EXCEPT (i, ulCpuId) {
        __smpIpiCall(i, pipim);
        
        KN_SMP_MB();
        pipim->IPIM_iWait = iWaitSave;
        KN_SMP_WMB();
    }
}
/*********************************************************************************************************
** 函数名称: _SmpCallFunc
** 功能描述: 利用核间中断让指定的 CPU 运行指定的函数. (外部必须锁定当前 CPU 调度)
** 输　入  : ulCpuId       CPU ID
**           pfunc         同步执行函数 (被调用函数内部不允许有锁内核操作, 否则可能产生死锁)
**           pvArg         同步参数
**           pfuncAsync    异步执行函数 (被调用函数内部不允许有锁内核操作, 否则可能产生死锁)
**           pvAsync       异步执行参数
**           iOpt          选项 IPIM_OPT_NORMAL / IPIM_OPT_NOKERN
** 输　出  : 调用返回值
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
INT  _SmpCallFunc (ULONG         ulCpuId,
                   INT_FUNCPTR   pfunc,
                   PVOID         pvArg,
                   VOID_FUNCPTR  pfuncAsync,
                   PVOID         pvAsync,
                   INT           iOpt)
{
    MX_IPI_MSG  ipim;
    
    ipim.IPIM_pfuncCall      = pfunc;
    ipim.IPIM_pvArg          = pvArg;
    ipim.IPIM_pfuncAsyncCall = pfuncAsync;
    ipim.IPIM_pvAsyncArg     = pvAsync;
    ipim.IPIM_iRet           = -1;
    ipim.IPIM_iOption        = iOpt;
    ipim.IPIM_iWait          = 1;
    
    return  (__smpIpiCall(ulCpuId, &ipim));
}
/*********************************************************************************************************
** 函数名称: _SmpCallFuncAllOther
** 功能描述: 利用核间中断让指定的 CPU 运行指定的函数. (外部必须锁定当前 CPU 调度)
** 输　入  : pfunc         同步执行函数 (被调用函数内部不允许有锁内核操作, 否则可能产生死锁)
**           pvArg         同步参数
**           pfuncAsync    异步执行函数 (被调用函数内部不允许有锁内核操作, 否则可能产生死锁)
**           pvAsync       异步执行参数
**           iOpt          选项 IPIM_OPT_NORMAL / IPIM_OPT_NOKERN
** 输　出  : NONE (无法确定返回值)
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _SmpCallFuncAllOther (INT_FUNCPTR   pfunc,
                            PVOID         pvArg,
                            VOID_FUNCPTR  pfuncAsync,
                            PVOID         pvAsync,
                            INT           iOpt)
{
    MX_IPI_MSG  ipim;
    
    ipim.IPIM_pfuncCall      = pfunc;
    ipim.IPIM_pvArg          = pvArg;
    ipim.IPIM_pfuncAsyncCall = pfuncAsync;
    ipim.IPIM_pvAsyncArg     = pvAsync;
    ipim.IPIM_iRet           = -1;
    ipim.IPIM_iOption        = iOpt;
    ipim.IPIM_iWait          = 1;
    
    __smpIpiCallOther(&ipim);
}
/*********************************************************************************************************
** 函数名称: __smpProcMsg
** 功能描述: 处理核间中断调用函数
** 输　入  : pcpuCur       当前 CPU
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __smpProcMsg (PMX_PHYS_CPU  pcpuCur)
{
    UINT            i, uiCnt;
    PMX_IPI_MSG     pipim;
    PMX_LIST_RING   pringTemp;
    PMX_LIST_RING   pringDelete;
    
    spinLockIgnIrq(&pcpuCur->CPU_slIpi);                                /*  锁定 CPU                    */
    
    pringTemp = pcpuCur->CPU_pringMsg;
    uiCnt     = pcpuCur->CPU_uiMsgCnt;
    
    for (i = 0; i < uiCnt; i++) {
        pipim = _LIST_ENTRY(pringTemp, MX_IPI_MSG, IPIM_ringManage);
        
        pringDelete = pringTemp;
        pringTemp   = _list_ring_get_next(pringTemp);
        _List_Ring_Del(pringDelete, &pcpuCur->CPU_pringMsg);            /*  删除一个节点                */
        pcpuCur->CPU_uiMsgCnt--;
        
        if (pipim->IPIM_pfuncCall) {
            pipim->IPIM_iRet = pipim->IPIM_pfuncCall(pipim->IPIM_pvArg);/*  执行同步调用                */
        }
        
        KN_SMP_MB();
        pipim->IPIM_iWait = 0;                                          /*  调用结束                    */
        KN_SMP_WMB();
        
        if (pipim->IPIM_pfuncAsyncCall) {
            pipim->IPIM_pfuncAsyncCall(pipim->IPIM_pvAsyncArg);         /*  执行异步调用                */
        }
    }
    
    KN_SMP_MB();
    if (pcpuCur->CPU_pringMsg == MX_NULL) {
        MX_CPU_CLR_IPI_PEND2(pcpuCur, MX_IPI_CALL_MSK);                 /*  清除                        */
    }
    
    spinUnlockIgnIrq(&pcpuCur->CPU_slIpi);                              /*  解锁 CPU                    */
}
/*********************************************************************************************************
** 函数名称: _SmpProcIpi
** 功能描述: 处理核间中断 (这里不处理调度器消息)
** 输　入  : pcpuCur       当前 CPU
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _SmpIpiProc (PMX_PHYS_CPU  pcpuCur)
{
    INTREG  iregInterLevel;

    pcpuCur->CPU_iIpiCnt++;                                             /*  核间中断数量 ++             */

#if 1
    //kernelEnter();
    if (MX_CPU_GET_IPI_PEND2(pcpuCur) & MX_IPI_IRQ_MSK) {
#if 0
        pcpuCur->CPU_pvcpuCur->VCPU_pfuncIsr(pcpuCur->CPU_pvcpuCur->VCPU_uiIrqPend,
                                             pcpuCur->CPU_pvcpuCur->VCPU_puctxSP);
        pcpuCur->CPU_pvcpuCur->VCPU_uiIrqPend = 0;
#endif
        //bspDebugMsg("-%d)", archMpCur());
        vcpuDoPendIsr(pcpuCur->CPU_pvcpuCur);
    }
    //kernelExit();
#endif

    if (MX_CPU_GET_IPI_PEND2(pcpuCur) & MX_IPI_SCHED_MSK) {             /*  调度核间中断                */
        MX_CPU_CLR_SCHED_IPI_PEND(pcpuCur);
    }

    if (MX_CPU_GET_IPI_PEND2(pcpuCur) & MX_IPI_CALL_MSK) {              /*  自定义调用 ?                */
        iregInterLevel = archIntDisable();
        __smpProcMsg(pcpuCur);
        archIntEnable(iregInterLevel);
    }
}
/*********************************************************************************************************
** 函数名称: _SmpIpiTryProc
** 功能描述: 尝试处理核间中断 (必须在关中断情况下调用, 这里仅仅尝试执行 call 函数)
** 输　入  : pcpuCur       当前 CPU
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _SmpIpiTryProc (PMX_PHYS_CPU  pcpuCur)
{
    if (MX_CPU_GET_IPI_PEND2(pcpuCur) & MX_IPI_CALL_MSK) {              /*  自定义调用 ?                */
        __smpProcMsg(pcpuCur);
    }
}
/*********************************************************************************************************
** 函数名称: _SmpIpiUpdate
** 功能描述: 产生一个 IPI
** 输　入  : pcpuCur   CPU 控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _SmpIpiUpdate (PMX_PHYS_CPU  pcpu)
{
    if (!MX_CPU_IS_ACTIVE(pcpu)) {                                      /*  CPU 必须被激活              */
        return;
    }

    archMpInt(pcpu->CPU_ulCpuId);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

