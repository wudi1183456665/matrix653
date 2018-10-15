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
** 文   件   名: sched.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 19 日
**
** 描        述: 微内核调度器.

** BUG:
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** 函数名称: __schedIpiLowCandCpu
** 功能描述: 向需要调度的 CPU 发送核间中断(该 CPU 需要满足以下两点: 1. Cand 无效, 2. Cand 优先级最低)
** 输　入  : ulCpuIdCur       当前 CPU ID
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
** 注  意  : 当另一个核产生了调度器卷绕, 并且没有在处理 sched 核间中断, 
             则向候选任务优先级最低的 CPU 发送核间中断.
*********************************************************************************************************/
static VOID  __schedIpiLowCandCpu (ULONG  ulCpuIdCur)
{
    INT             i;
    INT             iCpuIdSend;
    UINT32          uiPrioLow = 0;
    UINT32          uiPrioCand;
    PMX_PHYS_CPU    pcpu;
    
    MX_CPU_FOREACH_ACTIVE_EXCEPT (i, ulCpuIdCur) {                      /*  遍历 CPU 检查是否需要调度   */
        pcpu = MX_CPU_GET(i);
        if (MX_CAND_ROT(pcpu) &&
            ((MX_CPU_GET_IPI_PEND(i) & MX_IPI_SCHED_MSK) == 0)) {
            uiPrioCand = MX_CAND_VCPU(pcpu)->VCPU_uiPriority;
            if (MX_PRIO_IS_HIGH(uiPrioLow, uiPrioCand)) {
                uiPrioLow  = uiPrioCand;
                iCpuIdSend = i;
            }
        }
    }
    if (uiPrioLow) {
        _SmpIpiSend(iCpuIdSend, MX_IPI_SCHED, 0, MX_TRUE);                /*  产生核间中断                */
    }
}
/*********************************************************************************************************
** 函数名称: schedSwap
** 功能描述: 将本核的当前线程赋值为已经选好的最高优先级线程
** 输　入  : pcpuCur   当前物理 CPU
** 输　出  : NONE
** 全局变量: 
** 调用模块: 在 ARCH 层的任务切换函数中使用: 1.保存旧线程上下文, 2.调用此函数, 3.恢复新线程上下文
*********************************************************************************************************/
VOID  schedSwap (PMX_PHYS_CPU  pcpuCur)
{
    PMX_VCPU  pvcpuOld = pcpuCur->CPU_pvcpuCur;
    PMX_VCPU  pvcpuNew = pcpuCur->CPU_pvcpuHigh;

    pcpuCur->CPU_pvcpuCur = pvcpuNew;                                   /*  切换任务                    */

    //bspDebugMsg("schedSwap() old stack = 0x%X, new stack = 0x%X\n", pvcpuOld->VCPU_pstkKStackNow, pvcpuNew->VCPU_pstkKStackNow);

    if (pvcpuOld->VCPU_pprtnBelong != pvcpuNew->VCPU_pprtnBelong) {
        prtnMakeCur(pvcpuNew->VCPU_pprtnBelong);
    }
    kernelUnlockSched(pvcpuOld);                                        /*  解锁内核 spinlock           */
}
/*********************************************************************************************************
** 函数名称: schedule
** 功能描述: 选取候选表中的线程运行. 中断退出时, 会调用此调度函数 (进入内核状态并关中断被调用)
** 输　入  : NONE
** 输　出  : 线程上下文返回值
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_ERROR  schedule (VOID)
{
    ULONG           ulCpuId;
    PMX_PHYS_CPU    pcpuCur;
    PMX_VCPU        pvcpuCur;
    PMX_VCPU        pvcpuCand;
    MX_ERROR        iRetVal = ERROR_NONE;

    ulCpuId   = MX_CPU_GET_CUR_ID();                                    /*  当前 CPU ID                 */
    pcpuCur   = MX_CPU_GET_CUR();                                       /*  当前 CPU 控制块             */
    pvcpuCur  = pcpuCur->CPU_pvcpuCur;                                  /*  当前 vCPU 控制块            */

    //if (pvcpuCur == 0xC00BE000) bspDebugMsg(".");
    //if (archMpCur() != 0) { bspDebugMsg("schedule()\n"); } while (1);
    //if (pvcpuCur->VCPU_ulLockCounter == 0) bspDebugMsg("[%d][%p]\n", archMpCur(), pvcpuCur);

    pvcpuCand = _CandTableVcpuGet(pcpuCur);                             /*  获得需要运行的 vCPU         */
    //bspDebugMsg("CPU%d: Cur = %X, Cand = %X\n", ulCpuId, pvcpuCur, pvcpuCand);
    //bspDebugMsg("schedule() Cur stk = 0x%X, Cand stk = 0x%X\n", pvcpuCur->VCPU_pstkKStackNow, pvcpuCand->VCPU_pstkKStackNow);
    //bspDebugMsg("schedule() Cand cpsr = 0x%X\n", ((ARCH_KERN_CTX *)(pvcpuCand->VCPU_pstkKStackNow))->KCTX_uiCpsr);
    //bspDebugMsg("schedule() Cand pc   = 0x%X\n", ((ARCH_KERN_CTX *)(pvcpuCand->VCPU_pstkKStackNow))->KCTX_uiPc);
    //bspDebugMsg("schedule() Cand fp   = 0x%X\n", ((ARCH_KERN_CTX *)(pvcpuCand->VCPU_pstkKStackNow))->KCTX_uiFp);
    if (pvcpuCand != pvcpuCur) {                                        /*  如果与当前运行的不同, 切换  */
        pcpuCur->CPU_bIsIntSwtich = MX_FALSE;                           /*  非中断调度                  */
        pcpuCur->CPU_pvcpuHigh    = pvcpuCand;


        /*
         *  VCPU CTX SAVE();
         *  schedSwap();
         *  VCPU CTX LOAD();
         */
        archVcpuCtxSwitch(pcpuCur);                                     /*  ARCH 层的任务切换           */
        kernelLockIgnIrq();                                             /*  内核自旋锁重新加锁          */
#if 0
        if (pvcpuCur->VCPU_uiIrqPend) {
            pvcpuCur->VCPU_pfuncIsr(pvcpuCur->VCPU_uiIrqPend,
                                    pvcpuCur->VCPU_puctxSP);
            pvcpuCur->VCPU_uiIrqPend = 0;
        }
#endif
    } else {                                                            /*  本核没发生切换, 通知其它核  */
        //bspDebugMsg("schedule++++++++++++++\n");
        __schedIpiLowCandCpu(ulCpuId);
        return  (iRetVal);
    }

    pvcpuCur = MX_VCPU_GET_CUR();                                       /*  获得新的当前 vCPU           */

    iRetVal = pvcpuCur->VCPU_iSchedRet;                                 /*  获得调度器信号的返回值      */
    pvcpuCur->VCPU_iSchedRet = ERROR_NONE;                              /*  清空                        */

    return  (iRetVal);
}
/*********************************************************************************************************
** 函数名称: schedTick
** 功能描述: 时间片处理 (tick 中断服务程序中被调用, 进入内核且关闭中断状态)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  schedTick (VOID)
{
    REGISTER PMX_PHYS_CPU   pcpu;
    REGISTER PMX_VCPU       pvcpu;
             UINT32         uiRdyPrio;
             UINT32         uiCurPrio;
             INT            i;
             INTREG         iregInterLevel;
    iregInterLevel = kernelEnterAndDisIrq();                            /*  进入内核同时关闭中断        */

    _WakeupTick(1);                                                     /*  更新唤醒表                  */

#if 1
    /*
     *  置优先级卷绕标志，支持同优先级轮转调度
     */
    MX_CPU_FOREACH_ACTIVE (i) {
        pcpu      = MX_CPU_GET(i);
        pvcpu     = pcpu->CPU_pvcpuCur;
        uiCurPrio = pvcpu->VCPU_uiPriority;
        if (pvcpu->VCPU_usSchedCounter == 0) {                          /*  当前线程时间片已经耗尽      */
            if (MX_CAND_ROT(pcpu) == MX_FALSE) {                        /*  轮转标志未置                */
                if (_ReadyTableGet(pcpu, &uiRdyPrio) &&
                    MX_PRIO_IS_HIGH_OR_EQU(uiRdyPrio, uiCurPrio)) {     /*  就绪未运行任务的最高优先级  */

                    MX_CAND_ROT(pcpu) = MX_TRUE;                        /*  下次调度时检查轮转          */
                }
            }
        } else {
            pvcpu->VCPU_usSchedCounter--;
        }
    }
#else
    __asm__ __volatile__ ("NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t");
#endif

    kernelExitAndEnIrq(iregInterLevel);                                 /*  退出内核同时打开中断        */
}
/*********************************************************************************************************
** 函数名称: _SchedYield
** 功能描述: 指定线程主动让出 CPU 使用权, (同优先级内) (此函数被调用时已进入内核且中断已经关闭)
** 输　入  : pvcpu          正在运行的线程
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  schedYield (PMX_VCPU  pvcpu)
{
    REGISTER PMX_PHYS_CPU   pcpu;
             UINT32         uiPriority;
             INTREG         iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();                            /*  进入内核同时关闭中断        */

    if (MX_VCPU_IS_RUNNING(pvcpu)) {                                    /*  必须正在执行                */
        pcpu = MX_CPU_GET(pvcpu->VCPU_ulCpuIdRun);
        if (_ReadyTableGet(pcpu, &uiPriority) &&                        /*  就绪未运行任务的最高优先级  */
            MX_PRIO_IS_HIGH_OR_EQU(uiPriority,
                                   pvcpu->VCPU_uiPriority)) {
            pvcpu->VCPU_usSchedCounter = 0;                             /*  没收剩余时间片              */
            MX_CAND_ROT(MX_CPU_GET(pvcpu->VCPU_ulCpuIdRun)) = MX_TRUE;  /*  下次调度时检查轮转          */
        }
    }

    kernelExitAndEnIrq(iregInterLevel);                                 /*  退出内核同时打开中断        */ 
}
/*********************************************************************************************************
** 函数名称: schedActiveCpu
** 功能描述: 激活 CPU, 使其进入调度器管理范畴
** 输　入  : pcpu      CPU 结构体
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  schedActiveCpu (PMX_PHYS_CPU  pcpu)
{
    pcpu->CPU_ulStatus |= MX_CPU_STATUS_ACTIVE;

    KN_SMP_MB(); 

    _CandTableUpdate(pcpu);                                             /*  更新候选线程                */
    pcpu->CPU_pvcpuCur  = MX_CAND_VCPU(pcpu);
    pcpu->CPU_pvcpuHigh = MX_CAND_VCPU(pcpu);
    bspDebugMsg("-------- CPU%d Start vCPU %X ", pcpu->CPU_ulCpuId, pcpu->CPU_pvcpuCur);
    bspDebugMsg("-- Belong to %X ------ \n", pcpu->CPU_pvcpuCur->VCPU_pprtnBelong);

    prtnMakeCur(pcpu->CPU_pvcpuCur->VCPU_pprtnBelong);

#if 0
    ARCH_KERN_CTX  *pregctx = pcpu->CPU_pvcpuCur->VCPU_pstkKStackNow;
    bspDebugMsg("  sp : %X\n", pregctx);
    bspDebugMsg("  Fp : %X\n", pregctx->KCTX_uiFp);
    bspDebugMsg("Cpsr : %X\n", pregctx->KCTX_uiCpsr);
    bspDebugMsg("  Pc : %X\n", pregctx->KCTX_uiPc);
#endif

    KN_SMP_MB(); 

    bspCpuUpDone();                                                     /*  本核初始化完成, 并等待其它核*/
}
/*********************************************************************************************************
** 函数名称: __schedIdle
** 功能描述: Idle 线程
** 输　入  : pvArg      CPU ID
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static PVOID  __schedIdle (PVOID  pvArg)
{
    //ULONG  ulCpuId = (ULONG)pvArg;
    
    for (;;) {
        //__MX_VCPU_IDLE_HOOK(ulCpuId);
        //bspDebugMsg("Idle %d\n", ulCpuId);
        //bspDelayUs(1000000);
        //dumpHook();
        (*(volatile int *)(0x02020098)); /* 可行 */
     }

    return  (MX_NULL);
}
/*********************************************************************************************************
** 函数名称: schedInit
** 功能描述: 内核对象模块初始化
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  schedInit (VOID)
{
    INT             i;
    PMX_PHYS_CPU    pcpu;
    MX_VCPU_ATTR    vcpuattr;
    PMX_VCPU        pvcpuIdle;

    MX_CPU_FOREACH (i) {
        pcpu = MX_CPU_GET(i);

        pcpu->CPU_ulCpuId = (ULONG)i;                                   /*  设置 CPU ID                 */

        /*
         *  创建 Idle 线程
         */
        vcpuattr.VCPUATTR_uiPriority = MX_PRIO_LOWEST;
        vcpuattr.VCPUATTR_pvArg      = (PVOID)i;
        vcpuattr.VCPUATTR_bIsUser    = MX_FALSE;

        pvcpuIdle = vcpuCreate(__schedIdle, &vcpuattr);                 /*  构造空闲线程 VCPU 结构体    */

        vcpuBindPartition(pvcpuIdle, _K_pprtnKernel);                   /*  归属于内核进程              */
        vcpuSetAffinity(pvcpuIdle, i);                                  /*  亲和于物理 CPU              */
        vcpuStart(pvcpuIdle);
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
