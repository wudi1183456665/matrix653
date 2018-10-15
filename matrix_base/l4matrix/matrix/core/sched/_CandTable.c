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
** 文   件   名: _CandTable.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 19 日
**
** 描        述: 候选表操作.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** 函数名称: __candTableClear
** 功能描述: 将候选表中的候选线程放入就绪表
** 输　入  : pcpu      CPU 结构
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __candTableClear (PMX_PHYS_CPU  pcpu)
{
    REGISTER PMX_VCPU  pvcpu;
    
    pvcpu               = MX_CAND_VCPU(pcpu);
    pvcpu->VCPU_bIsCand = MX_FALSE;                                     /*  清本线程候选标识            */
    MX_CAND_VCPU(pcpu)  = MX_NULL;                                      /*  退出候选表                  */
    //bspDebugMsg(T_COL_YELLOW"       [%d]  CandDel          = %p\033[0m\n", archMpCur(), pvcpu);
    _ReadyTableAdd(pvcpu);                                              /*  重新加入就绪表              */
}
/*********************************************************************************************************
** 函数名称: __candTableFill
** 功能描述: 从就绪表中选择一个最该执行的线程放入候选表
** 输　入  : pcpu          CPU 结构
**           ppcbbmap      优先级位图
**           uiPriority    需要运行任务的优先级
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __candTableFill (PMX_PHYS_CPU  pcpu, PMX_READY  pready, UINT32  uiPriority)
{
    REGISTER PMX_VCPU   pvcpu;
    
    pvcpu = _ReadyTableSeek(pready, uiPriority);                        /*  确定可以候选运行的线程      */
    
    //bspDebugMsg(T_COL_GREEN"       [%d]  CandNew          = %p\033[0m\n", archMpCur(), pvcpu);

    MX_CAND_VCPU(pcpu)     = pvcpu;                                     /*  进入候选表                  */
    pvcpu->VCPU_ulCpuIdRun = pcpu->CPU_ulCpuId;                         /*  记录 CPU 号                 */
    pvcpu->VCPU_bIsCand    = MX_TRUE;                                   /*  置候选标识                  */
    _ReadyTableDel(pvcpu);                                              /*  从就绪表中删除              */
}
/*********************************************************************************************************
** 函数名称: _CandTableUpdate
** 功能描述: 尝试将最高优先级就绪任务装入候选表
** 输　入  : pcpu      CPU 结构
** 输　出  : NONE
** 全局变量: 
** 调用模块: schedActiveCpu(), _CandTableVcpuGet()
*********************************************************************************************************/
VOID  _CandTableUpdate (PMX_PHYS_CPU  pcpu)
{
             UINT32        uiPriority;
    REGISTER PMX_VCPU      pvcpuCand;
             PMX_READY     preadyHigh;                                  /*  包含最高优先级线程的就绪表  */
             BOOL          bNeedFlush = MX_FALSE;

    preadyHigh = _ReadyTableGet(pcpu, &uiPriority);                     /*  获取含最高优先级线程就绪表  */
    pvcpuCand  = MX_CAND_VCPU(pcpu);                                    /*  获取当前候选线程            */

    if (pvcpuCand == MX_NULL) {                                         /*  当前没有候选线程            */
        __candTableFill(pcpu, preadyHigh, uiPriority);                  /*  就绪表线程 --> 候选表       */
        goto  __update_done;
    }
    
    if (preadyHigh == MX_NULL) {                                        /*  一个就绪线程也没有          */
        goto  __update_done;
    }
    
    if (pvcpuCand->VCPU_usSchedCounter == 0) {                          /*  已经没有时间片了            */
        if (MX_PRIO_IS_HIGH_OR_EQU(uiPriority, 
                                   pvcpuCand->VCPU_uiPriority)) {       /*  是否需要轮转                */
            bNeedFlush = MX_TRUE;
        }
    } else {
        if (MX_PRIO_IS_HIGH(uiPriority, 
                            pvcpuCand->VCPU_uiPriority)) {
            bNeedFlush = MX_TRUE;
        }
    }
    
    if (bNeedFlush) {                                                   /*  存在更需要运行的线程        */
        __candTableClear(pcpu);                                         /*  候选表线程 --> 就绪表       */
        __candTableFill(pcpu, preadyHigh, uiPriority);                  /*  就绪表线程 --> 候选表       */
    }
    
__update_done:
    MX_CAND_ROT(pcpu) = MX_FALSE;                                       /*  清除优先级卷绕标志          */
}
/*********************************************************************************************************
** 函数名称: _CandTableTryAdd
** 功能描述: 试图将一个就绪线程装入候选线程表 (只有候选表是空时才会装入, 否则仅置优先级卷绕标志)
** 输　入  : pvcpu         就绪的线程
** 输　出  : 是否加入了候选运行表
** 全局变量: 
** 调用模块: _WakeupTick(), vcpuWakeup()
** 注  意  : 当线程被唤醒时, 先尝试将其装入候选表, 如果不行再放到就绪表
*********************************************************************************************************/
BOOL  _CandTableTryAdd (PMX_VCPU  pvcpu)
{
    REGISTER PMX_PHYS_CPU   pcpu;
    REGISTER PMX_VCPU       pvcpuCand;
    REGISTER ULONG          i;

    if (pvcpu->VCPU_bCpuAffinity) {                                     /*  亲和运行 CPU                */
        pcpu = MX_CPU_GET(pvcpu->VCPU_ulCpuIdAffinity);
        if (!MX_CPU_IS_ACTIVE(pcpu)) {
            goto    __can_not_cand;
        }
        
        pvcpuCand = MX_CAND_VCPU(pcpu);
        if (pvcpuCand == MX_NULL) {                                     /*  候选表为空                  */
            while (1) bspDebugMsg("_CandTableTryAdd(0)");;
            MX_CAND_VCPU(pcpu)     = pvcpu;
            pvcpu->VCPU_ulCpuIdRun = pvcpu->VCPU_ulCpuIdAffinity;       /*  记录 CPU 号                 */
            pvcpu->VCPU_bIsCand    = MX_TRUE;                           /*  进入候选运行表              */
            return  (MX_TRUE);
            
        } else if (MX_PRIO_IS_HIGH(pvcpu->VCPU_uiPriority, 
                                   pvcpuCand->VCPU_uiPriority)) {       /*  优先级高于当前候选线程      */
            MX_CAND_ROT(pcpu) = MX_TRUE;                                /*  产生优先级卷绕              */
        }
    
    } else {                                                            /*  所有 CPU 均可运行此任务     */
        //if (pvcpu == 0xC00C0000) bspDebugMsg("try :");
        MX_CPU_FOREACH_ACTIVE (i) {                                     /*  遍历激活状态的 CPU          */
            pcpu      = MX_CPU_GET(i);
            pvcpuCand = MX_CAND_VCPU(pcpu);
            if (pvcpuCand == MX_NULL) {                                 /*  候选表为空                  */
                while (1) bspDebugMsg("_CandTableTryAdd()");;
                MX_CAND_VCPU(pcpu)     = pvcpu;
                pvcpu->VCPU_ulCpuIdRun = i;                             /*  记录 CPU 号                 */
                pvcpu->VCPU_bIsCand    = MX_TRUE;                       /*  进入候选运行表              */
                return  (MX_TRUE);
            
            } else if (MX_PRIO_IS_HIGH(pvcpu->VCPU_uiPriority, 
                                       pvcpuCand->VCPU_uiPriority)) {   /*  优先级高于当前候选线程      */
                MX_CAND_ROT(pcpu) = MX_TRUE;                            /*  产生优先级卷绕              */
            }
            //if (pvcpu == 0xC00C0000) bspDebugMsg(" %d,%d,%p", MX_CAND_ROT(pcpu), pcpu->CPU_ulIpiPend, pvcpuCand);
        }
        //if (pvcpu == 0xC00C0000) bspDebugMsg("\n");
    }

__can_not_cand:
    return  (MX_FALSE);                                                 /*  无法加入候选运行表          */
}
/*********************************************************************************************************
** 函数名称: _CandTableTryDel
** 功能描述: 试图将一个不再就绪线程从候选线程表中删除, 如果删除成功, 从就绪表中再填充一个
** 输　入  : pvcpu         不再就绪的线程
** 输　出  : 是否从候选运行表中删除
** 全局变量: 
** 调用模块: vcpuSleep(), vcpuSuspend()
** 注  意  : 当线程不再就绪时, 先判断其当前是否在候选表中, 如果不在, 再从就绪表中删除
*********************************************************************************************************/
BOOL  _CandTableTryDel (PMX_VCPU  pvcpu)
{
    REGISTER PMX_PHYS_CPU  pcpu = MX_CPU_GET(pvcpu->VCPU_ulCpuIdRun);
             PMX_READY     preadyHigh;
             UINT32        uiPriority;

    if (MX_CAND_VCPU(pcpu) == pvcpu) {
        //bspDebugMsg(T_COL_YELLOW"       [%d] *CandDel          = %p\033[0m\n", archMpCur(), pvcpu);
        pvcpu->VCPU_bIsCand = MX_FALSE;
        preadyHigh          = _ReadyTableGet(pcpu, &uiPriority);        /*  获取含最高优先级线程就绪表  */
        __candTableFill(pcpu, preadyHigh, uiPriority);                  /*  候选表中填充一个其它线程    */
        MX_CAND_ROT(pcpu)   = MX_FALSE;                                 /*  清除优先级卷绕标志          */
        return  (MX_TRUE);
    } else {
        return  (MX_FALSE);
    }
}
/*********************************************************************************************************
** 函数名称: _CandTableVcpuGet
** 功能描述: 获取当前 CPU 需要被运行的线程控制块
** 输　入  : pcpuCur   当前物理 CPU
** 输　出  : NONE
** 全局变量: 
** 调用模块: schedule()
*********************************************************************************************************/
PMX_VCPU  _CandTableVcpuGet (PMX_PHYS_CPU  pcpuCur)
{
    /*
     *  以下三种情况下不进行任务切换:
     *  1. 中断处理中; (退出中断时统一调度)
     *  2. 进入了内核状态; (退出内核时统一调度)
     *  3. 当前线程除了占用内核锁之外, 还锁定了其它 spinlock; (不允许被调度出去)
     */
    if (pcpuCur->CPU_ulInterNesting ||
        pcpuCur->CPU_iKernelCounter ||
        pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter > 1) {
        //bspDebugMsg("       [%d][%p]%d, %d, %d\n", archMpCur(), pcpuCur->CPU_pvcpuCur, pcpuCur->CPU_ulInterNesting, pcpuCur->CPU_iKernelCounter, pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter);
        return  (pcpuCur->CPU_pvcpuCur);                                /*  返回当前线程                */

    } else {
        //bspDebugMsg("   [%d][%p]%d, %d, %d\n", archMpCur(), pcpuCur->CPU_pvcpuCur, pcpuCur->CPU_ulInterNesting, pcpuCur->CPU_iKernelCounter, pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter);
        if (MX_CAND_ROT(pcpuCur)) {                                     /*  产生优先级卷绕              */
            //bspDebugMsg("%d.", archMpCur());
            //MX_CPU_CLR_SCHED_IPI_PEND(pcpuCur);
            _CandTableUpdate(pcpuCur);
        }
        return  (MX_CAND_VCPU(pcpuCur));                                /*  返回最高优先级线程          */
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

