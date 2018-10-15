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
** 文   件   名: vcpu.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 19 日
**
** 描        述: 线程管理.

** BUG:
2018.02.07 增加 vcpuSuspend.
2018.02.07 增加 vcpuWakeup.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** 函数名称: __vcpuEntryShell
** 功能描述: 所有线程的入口函数
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __vcpuEntryShell (VOID)
{
    PMX_VCPU                pvcpuCur;
    PVCPU_START_ROUTINE     pfuncStart;
    PVOID                   pvArg;
    BOOL                    bIsUser;
    PVOID                   pvReturnVal;

    pvcpuCur    = MX_VCPU_GET_CUR();
    pfuncStart  = pvcpuCur->VCPU_pfuncStart;
    pvArg       = pvcpuCur->VCPU_pvArg;
    bIsUser     = pvcpuCur->VCPU_bIsUser;

#if 0
    PMX_STACK               pstkUser;
    static INT              iUserIndex = 0;

    pstkUser    = pvcpuCur->VCPU_pstkUStackStart;
    if (bIsUser) {                                                      /*  用户线程                    */
        //kernelEnter();
        if (pstkUser == MX_NULL) {
            KN_ASSERT(iUserIndex < MX_CFG_MAX_USTACKS);                 /*  确保池中有可用用户堆栈      */
            pstkUser = &_K_stkUser[iUserIndex + 1][0];                  /*  分配一个默认用户堆栈        */
            iUserIndex++;
        }
        //kernelExit();

        __asm__ __volatile__ ("MSR  CPSR_c , #(0x10 | 0x40) \n\t"
                              "MOV  SP , %0 \n\t"
                              :
                              :"r"(pstkUser)
                              :"cc", "memory"
                              );
    }
    pvReturnVal = pfuncStart(pvArg);                                    /*  执行线程                    */
#else
    if (bIsUser) {                                                      /*  用户线程                    */
        archJumpToUser(pvArg, pfuncStart);
    } else {
        archJumpToKern(pvArg, pfuncStart);
    }
#endif

    (VOID)pvReturnVal;
}
/*********************************************************************************************************
** 函数名称: vcpuCreate
** 功能描述: 创建一个线程
** 输　入  : pfuncStart         线程入口地址
**           pvcpuattr          线程属性集合指针
** 输　出  : 线程控制块指针
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PMX_VCPU  vcpuCreate (PVCPU_START_ROUTINE  pfuncStart, PMX_VCPU_ATTR  pvcpuattr)
{
    MX_OBJECT_ID    ulVcpuId;
    PMX_VCPU        pvcpuNew;
    PMX_STACK       pstkTop;
    PMX_STACK       pstkNow;

    ulVcpuId = kobjAlloc(_KOBJ_VCPU);
    pvcpuNew = (PMX_VCPU)kobjGet(ulVcpuId);
    pstkTop  = (PMX_STACK)((UINT32)pvcpuNew + MX_CFG_KSTACK_SIZE);

    pstkNow  = archVcpuCtxCreate(__vcpuEntryShell, pstkTop, 0);

    pvcpuNew->VCPU_pstkKStackNow   = pstkNow;
    pvcpuNew->VCPU_pstkUStackStart = pvcpuattr->VCPUATTR_pstkLowAddr;   /*  NOTE: 传递用户栈(顶)地址    */
    pvcpuNew->VCPU_pfuncStart      = pfuncStart;
    pvcpuNew->VCPU_uiPriority      = pvcpuattr->VCPUATTR_uiPriority;
    pvcpuNew->VCPU_pvArg           = pvcpuattr->VCPUATTR_pvArg;
    pvcpuNew->VCPU_bIsUser         = pvcpuattr->VCPUATTR_bIsUser;
    pvcpuNew->VCPU_uiStatus        = MX_VCPU_STATUS_INIT;

    pvcpuNew->VCPU_bCpuAffinity    = MX_FALSE;
    pvcpuNew->VCPU_ulCpuIdAffinity = 0;
    pvcpuNew->VCPU_bIsCand         = MX_FALSE;

    pvcpuNew->VCPU_ulStatus        = VCPU_STAT_DIS_INT;
    //pvcpuNew->VCPU_uiIrqPend     = 0;
    _vIrqPendIni(pvcpuNew);
    pvcpuNew->VCPU_pfuncIsr        = MX_NULL;
    pvcpuNew->VCPU_pfuncSwi        = MX_NULL;

    pvcpuNew->VCPU_usSchedSlice    = MX_VCPU_SCHED_SLICE;
    pvcpuNew->VCPU_usSchedCounter  = MX_VCPU_SCHED_SLICE;

    pvcpuNew->VCPU_ulVcpuId        = ulVcpuId;

    ipcInit(&pvcpuNew->VCPU_ipc);

    bspDebugMsg("vcpuCreate() id:%X, ptr:%X, prio:%d\n", ulVcpuId, pvcpuNew, pvcpuNew->VCPU_uiPriority);

    return  (pvcpuNew);
}
/*********************************************************************************************************
** 函数名称: vcpuStart
** 功能描述: 启动一个线程
** 输　入  : pvcpu              线程控制块指针
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_ERROR  vcpuStart (PMX_VCPU  pvcpu)
{
    INTREG     iregInterLevel;

    if (pvcpu == MX_NULL) {
        return  (ERROR_KOBJ_NULL);
    }

    iregInterLevel = archIntDisable();
    if (pvcpu->VCPU_uiStatus & MX_VCPU_STATUS_INIT) {
        pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_INIT;

        if (pvcpu->VCPU_uiStatus == MX_VCPU_STATUS_READY) {
            _ReadyTableAdd(pvcpu);                                      /*  加入到就绪表                */
        }
    }
    archIntEnable(iregInterLevel);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: vcpuSetIrqEntry
** 功能描述: 设置线程的中断入口
** 输　入  : pvcpu              线程控制块指针
**           pfuncIsr           线程中断入口函数
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_ERROR  vcpuSetIrqEntry (PMX_VCPU  pvcpu, VOID_FUNCPTR  pfuncIsr)
{
    MX_ERROR   eReturn = ERROR_KOBJ_NULL;

    if (pvcpu) {
        pvcpu->VCPU_pfuncIsr = pfuncIsr;
        eReturn              = ERROR_NONE;
    }

    return  (eReturn);
}
/*********************************************************************************************************
** 函数名称: vcpuSleep
** 功能描述: 当前线程睡眠函数
** 输　入  : ulTick             睡眠的时间
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  vcpuSleep (ULONG  ulTick)
{
    PMX_VCPU        pvcpuCur;
    INTREG          iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();                            /*  进入内核同时关闭中断        */

    pvcpuCur = MX_VCPU_GET_CUR();                                       /*  当前 CPU 控制块             */

    if (!_CandTableTryDel(pvcpuCur)) {                                  /*  尝试从候选表中删除          */
        bspDebugMsg("vcpuSleep()");
        _ReadyTableDel(pvcpuCur);                                       /*  从就绪表中删除              */
    }

    pvcpuCur->VCPU_ulDelay   = ulTick;
    pvcpuCur->VCPU_uiStatus |= MX_VCPU_STATUS_DELAY;
    _WakeupAdd(&_K_wuDelay, &pvcpuCur->VCPU_wunDelay);

    //bspDebugMsg("Sleep : %p\n", pvcpuCur);

    kernelExitAndEnIrq(iregInterLevel);                                 /*  退出内核同时打开中断        */
}
/*********************************************************************************************************
** 函数名称: vcpuSuspend
** 功能描述: 线程暂停, 由用户程序调用 vcpuWakeup() 唤醒
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  vcpuSuspend (VOID)
{
    PMX_VCPU        pvcpuCur;
    INTREG          iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();                            /*  进入内核同时关闭中断        */

    pvcpuCur = MX_VCPU_GET_CUR();                                       /*  当前 CPU 控制块             */
    IPC_SET_IN_PROCESS(&(pvcpuCur->VCPU_ipc), MX_FALSE);                /*  普通阻塞                    */

    if (!_CandTableTryDel(pvcpuCur)) {                                  /*  尝试从候选表中删除          */
        bspDebugMsg("vcpuSuspend()"); //while(1);
        _ReadyTableDel(pvcpuCur);                                       /*  从就绪表中删除              */
    }

    pvcpuCur->VCPU_uiStatus |= MX_VCPU_STATUS_SUSPEND;

    kernelExitAndEnIrq(iregInterLevel);                                 /*  退出内核同时打开中断        */
}
/*********************************************************************************************************
** 函数名称: vcpuWakeup
** 功能描述: 唤醒线程
** 输　入  : pVcpu             待唤醒的线程
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  vcpuWakeup (PMX_VCPU  pvcpu)
{
    INTREG          iregInterLevel;

#if 1
    if (IPC_GET_IN_PROCESS(&pvcpu->VCPU_ipc)) {                         /*  IPC 唤醒模式: 加入就绪表    */
                                                                        /*  线程由 tick 移出唤醒表      */
        if (!ipcReadyTestSafe(&pvcpu->VCPU_ipc)) {
            /*
             *  IPC可重入性: 当一个服务线程向其它服务请求 IPC 处于等待
             *  状态时, 有线程向此服务发起 IPC 请求并尝试唤醒它, 不予唤醒
             *
             */
            K_NOTE("ipc not ready");
            return;
        }
    }

    iregInterLevel = kernelEnterAndDisIrq();                            /*  进入内核同时关闭中断        */

    pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_SUSPEND;
    pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_DELAY;
    /*
     *  IPC 状态且在唤醒表中 或者 非IPC 状态且非PENDING 状态
     */
    if (_WakeupIsIn(&pvcpu->VCPU_wunDelay)) {
        //K_DEBUG(" wk_del_vcpu=0x%x", pvcpu);
        _WakeupDel(&_K_wuDelay, &pvcpu->VCPU_wunDelay);                 /*  超时线程从唤醒表中移除      */
    }

    if (pvcpu->VCPU_uiStatus == MX_VCPU_STATUS_READY) {
        //K_DEBUG(" ready_vcpu=0x%x", pvcpu);
        if (!_CandTableTryAdd(pvcpu)) {                                 /*  尝试加入候选表              */
            _ReadyTableAdd(pvcpu);                                      /*  加入就绪表                  */
        }
    }

    kernelExitAndEnIrq(iregInterLevel);                                 /*  退出内核同时打开中断        */
#else
    iregInterLevel = kernelEnterAndDisIrq();                            /*  进入内核同时关闭中断        */

    pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_SUSPEND;

    if (pvcpu->VCPU_uiStatus == MX_VCPU_STATUS_READY) {
        if (!_CandTableTryAdd(pvcpu)) {                                 /*  尝试加入候选表              */
            _ReadyTableAdd(pvcpu);                                      /*  加入就绪表                  */
        }
    }

    kernelExitAndEnIrq(iregInterLevel);                                 /*  退出内核同时打开中断        */
#endif
}
/*********************************************************************************************************
** 函数名称: vcpuGetPriority
** 功能描述: 获得线程优先级
** 输　入  : pvcpu              线程控制块指针
**           puiPriority        优先级
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_ERROR  vcpuGetPriority (PMX_VCPU  pvcpu, UINT32  *puiPriority)
{
    MX_ERROR   eReturn = ERROR_KOBJ_NULL;

    if (pvcpu) {
        *puiPriority = pvcpu->VCPU_uiPriority;
        eReturn      = ERROR_NONE;
    }

    return  (eReturn);
}
/*********************************************************************************************************
** 函数名称: vcpuSetAffinity
** 功能描述: 将线程锁定到指定的 CPU 运行
** 输　入  : pvcpu              线程控制块指针
**           ulCpuIdAffinity    要绑定的 CPU (MX_CFG_MAX_PROCESSORS 时, 解除绑定)
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  vcpuSetAffinity (PMX_VCPU  pvcpu, ULONG  ulCpuIdAffinity)
{
    if (ulCpuIdAffinity == MX_CFG_MAX_PROCESSORS) {
        pvcpu->VCPU_bCpuAffinity    = MX_FALSE;
        pvcpu->VCPU_ulCpuIdAffinity = 0;
    } else {
        pvcpu->VCPU_bCpuAffinity    = MX_TRUE;
        pvcpu->VCPU_ulCpuIdAffinity = ulCpuIdAffinity;
    }
}
/*********************************************************************************************************
** 函数名称: vcpuSetUserStack
** 功能描述: 设置线程的用户堆栈起始地址
** 输　入  : pvcpu              线程控制块指针
**           pstkUser           用户堆栈栈底地址
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  vcpuSetUserStack (PMX_VCPU  pvcpu, PMX_STACK  pstkUser)
{
    pvcpu->VCPU_pstkUStackStart = pstkUser;
}
/*********************************************************************************************************
** 函数名称: vcpuBindPartition
** 功能描述: 设置线程所属的进程
** 输　入  : pvcpu              线程控制块指针
**           pprtnBelong        所属进程结构体指针
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  vcpuBindPartition (PMX_VCPU  pvcpu, PMX_PRTN  pprtnBelong)
{
    pvcpu->VCPU_pprtnBelong = pprtnBelong;
}
/*********************************************************************************************************
** 函数名称: vcpuBindIpcMsg
** 功能描述: 线程绑定用户消息, 与内核消息建立内存映射
** 输　入  : pvcpu              线程控制块指针
**           pIpcMsg            IPC 消息块指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  vcpuBindIpcMsg (PMX_VCPU  pvcpu, PMX_IPC_MSG  pIpcMsg)
{
    MX_IPC_MSG_GET(pvcpu) = pIpcMsg;
}
/*********************************************************************************************************
** 函数名称: vcpuDisableInt
** 功能描述: 屏蔽当前 VCPU 中断
** 输　入  : NONE
** 输　出  : 原 VCPU 状态
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
ULONG  vcpuDisableInt (VOID)
{
    PMX_VCPU         pvcpuCur = MX_VCPU_GET_CUR();
    ULONG            ulStatus;

    ulStatus = pvcpuCur->VCPU_ulStatus & VCPU_STAT_DIS_INT;

    pvcpuCur->VCPU_ulStatus |= VCPU_STAT_DIS_INT;
    //bspDebugMsg("(");

    return  (ulStatus);
}
/*********************************************************************************************************
** 函数名称: vcpuEnableInt
** 功能描述: 使能当前 VCPU 中断
** 输　入  : ulStatus   要恢复的 VCPU 状态
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  vcpuEnableInt (ULONG  ulStatus)
{
    PMX_VCPU         pvcpuCur = MX_VCPU_GET_CUR();

    if ((ulStatus & VCPU_STAT_DIS_INT) == 0) {
        pvcpuCur->VCPU_ulStatus &= ~VCPU_STAT_DIS_INT;
    }
}
/*********************************************************************************************************
** 函数名称: vcpuIsIntEnable
** 功能描述: VCPU 是否开中断
** 输　入  : pvcpu         线程控制块指针
** 输　出  : 当前处于开中断状态, 返回 MX_TRUE; 否则返回 MX_FALSE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
BOOL  vcpuIsIntEnable (PMX_VCPU  pvcpu)
{
    if (pvcpu->VCPU_ulStatus & VCPU_STAT_DIS_INT) {
        return  (MX_FALSE);
    } else {
        return  (MX_TRUE);
    }
}
/*********************************************************************************************************
** 函数名称: vcpuSaveStatus
** 功能描述: 将 VCPU 状态值存入 CPU 状态寄存器
** 输　入  : puctx      用户上下文指针
** 输　出  : NONE
** 全局变量: 
** 调用模块: 进入本函数时, UCTX 中的状态一定是开中断的, 因为其时硬件寄存器值(用户状态硬件中断一直开)
*********************************************************************************************************/
VOID  vcpuSaveStatus (PARCH_USER_CTX  puctx)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();

    if (pvcpuCur->VCPU_ulStatus & VCPU_STAT_DIS_INT) {
        ARCH_UCTX_DIS_IRQ(puctx);
    }
}
/*********************************************************************************************************
** 函数名称: vcpuRestoreStatus
** 功能描述: 根据用户上下文中的 CPU 状态寄存器值修改 VCPU 状态
** 输　入  : puctx      用户上下文指针
** 输　出  : NONE
** 全局变量: 
** 调用模块: 退出本函数后, UCTX 中的状态必须是开中断的. 注意该状态可能在进本函数前被用户服务函数替换
*********************************************************************************************************/
VOID  vcpuRestoreStatus (PARCH_USER_CTX  puctx)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();

    if (ARCH_UCTX_IS_DISIRQ(puctx)) {                                   /*  中断关                      */
        pvcpuCur->VCPU_ulStatus |= VCPU_STAT_DIS_INT;
        ARCH_UCTX_EN_IRQ(puctx);
        //bspDebugMsg("[");
    } else {
        pvcpuCur->VCPU_ulStatus &= ~VCPU_STAT_DIS_INT;
        //bspDebugMsg("]");
    }
}
/*********************************************************************************************************
** 函数名称: vcpuOccurIrq
** 功能描述: 当 VCPU 发生某个逻辑中断时的操作
** 输　入  : pvcpu         线程控制块指针
**           ulVec         逻辑中断号
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  vcpuOccurIrq (PMX_VCPU  pvcpu, ULONG  ulVec)
{
    _vIrqPendSet(pvcpu, ulVec);

    if (vcpuIsIntEnable(pvcpu)) {
        if (pvcpu == MX_VCPU_GET_CUR()) {
            vcpuDoPendIsr(pvcpu);

        } else if (MX_VCPU_IS_RUNNING(pvcpu)) {
            _SmpIpiSend(pvcpu->VCPU_ulCpuIdRun, MX_IPI_IRQ, 0, MX_TRUE);
        }
    } else {                                                            /*  VCPU 处在中断关状态         */
        irqDisable(ulVec);                                              /*  暂关该中断, 以免一直有中断  */
    }
}
/*********************************************************************************************************
** 函数名称: vcpuDoPendIsr
** 功能描述: 处理 VCPU 已经 Pend 的中断
** 输　入  : pvcpu         线程控制块指针(必须是当前 VCPU)
** 输　出  : NONE
** 全局变量: 
** 调用模块: 当 VCPU 开中断时
*********************************************************************************************************/
VOID  vcpuDoPendIsr (PMX_VCPU  pvcpu)
{
    ULONG  ulVec;

    if (pvcpu->VCPU_pfuncIsr) {
        while (_vIrqPendGet(pvcpu, &ulVec)) {
            vcpuSaveStatus(pvcpu->VCPU_puctxSP);

            pvcpu->VCPU_pfuncIsr(ulVec, pvcpu->VCPU_puctxSP);
            _vIrqPendClr(pvcpu, ulVec);
            irqEnable(ulVec);                                           /*  重新开该中断                */

            vcpuRestoreStatus(pvcpu->VCPU_puctxSP);

            //bspDebugMsg("%d", ulVec);
        }
    }
}
/*********************************************************************************************************
** 函数名称: vcpuGetCur
** 功能描述: 获取当前被中断线程 MX_VCPU 结构体指针
** 输　入  : NONE
** 输　出  : PMX_VCPU
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PMX_VCPU  vcpuGetCur (VOID)
{
    PMX_VCPU    pvcpuCur;

    pvcpuCur = MX_VCPU_GET_CUR();

    return  (pvcpuCur);
} 
/*********************************************************************************************************
  END
*********************************************************************************************************/

