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
** 文   件   名: k_cpu.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 06 日
**
** 描        述: CPU 信息描述.
*********************************************************************************************************/

#ifndef __K_CPU_H
#define __K_CPU_H

/*********************************************************************************************************
  CPU 结构 (要求 CPU ID 编号从 0 开始并由连续数字组成)
*********************************************************************************************************/
typedef struct {
    /*
     *  运行线程情况
     */
    volatile PMX_VCPU        CPU_pvcpuCur;                              /*  本物理 CPU 当前运行的 VCPU  */
    volatile PMX_VCPU        CPU_pvcpuHigh;                             /*  需要运行的高优先 VCPU       */

    /*
     *  当前发生调度的调度方式
     */
    BOOL                     CPU_bIsIntSwtich;                          /*  是否为中断调度              */

    /*
     *  内核锁定状态
     */
    volatile INT             CPU_iKernelCounter;                        /*  内核状态计数器              */

    /*
     *  候选运行结构
     */
    volatile MX_CAND         CPU_cand;                                  /*  候选运行的线程              */

    /*
     *  当前核就绪表(存放和当前核有亲和性的线程)
     */
    MX_READY                 CPU_ready;                                 /*  当前 CPU 就绪表             */

    /*
     *  核间中断处理函数链表
     */
    spinlock_t               CPU_slIpi;                                 /*  核间中断锁                  */
    PMX_LIST_RING            CPU_pringMsg;                              /*  自定义核间中断参数链        */
    volatile UINT            CPU_uiMsgCnt;                              /*  自定义核间中断数量          */
    
    ULONG                    CPU_ulIpiVec;                              /*  核间中断逻辑中断号          */
    INT_FUNCPTR              CPU_pfuncIpiClear;                         /*  核间中断清除函数            */
    PVOID                    CPU_pvIpiArg;                              /*  核间中断清除参数            */
    
    /*
     *  核间中断待处理标志, 这里最多有 ULONG 位数个核间中断类型, 和 CPU 硬件中断向量原理相同
     */
    INT64                    CPU_iIpiCnt;                               /*  核间中断次数                */
    volatile ULONG           CPU_ulIpiPend;                             /*  核间中断标志码              */
#define MX_IPI_NOP              0                                       /*  测试用核间中断向量          */
#define MX_IPI_SCHED            1                                       /*  调度请求                    */
#define MX_IPI_IRQ              2                                       /*  逻辑中断请求                */
#define MX_IPI_CALL             3                                       /*  自定义调用 (有参数可选等待) */


#define MX_IPI_NOP_MSK          (1 << MX_IPI_NOP)
#define MX_IPI_SCHED_MSK        (1 << MX_IPI_SCHED)
#define MX_IPI_IRQ_MSK          (1 << MX_IPI_IRQ)
#define MX_IPI_CALL_MSK         (1 << MX_IPI_CALL)

    /*
     *  CPU 基本信息
     */
             ULONG           CPU_ulCpuId;                               /*  CPU ID 号                   */
    volatile ULONG           CPU_ulStatus;                              /*  CPU 工作状态(目前只支持激活)*/
#define MX_CPU_STATUS_ACTIVE    0x80000000                              /*  CPU 激活                    */
#define MX_CPU_STATUS_INACTIVE  0x00000000                              /*  CPU 未激活                  */
    
    /*
     *  中断信息
     */
             PMX_STACK       CPU_pstkInterBase;                         /*  中断堆栈基址                */
    volatile ULONG           CPU_ulInterNesting;                        /*  中断嵌套计数器              */
    volatile ULONG           CPU_ulInterNestingMax;                     /*  中断嵌套最大值              */

    /*
     *  进程信息
     */
    volatile PMX_PRTN        CPU_pprtnCur;                              /*  本物理 CPU 当前运行的进程   */

} MX_PHYS_CPU;
typedef MX_PHYS_CPU         *PMX_PHYS_CPU;
/*********************************************************************************************************
  CPU 工作状态
*********************************************************************************************************/
#define MX_CPU_IS_ACTIVE(pcpu)          ((pcpu)->CPU_ulStatus & MX_CPU_STATUS_ACTIVE)
/*********************************************************************************************************
  当前 CPU 信息 MX_NCPUS 绝不能大于 MX_CFG_MAX_PROCESSORS
*********************************************************************************************************/
//ULONG   archMpCur(VOID);
//#define MX_CPU_GET_CUR_ID()             archMpCur()                     /*  获得当前 CPU ID             */
#define MX_NCPUS                        (_K_ulNCpus)
/*********************************************************************************************************
  CPU 表操作
*********************************************************************************************************/
extern  MX_PHYS_CPU   _K_cpuTable[];                                    /*  CPU 表                      */
//#define MX_CPU_GET_CUR()                (&_K_cpuTable[archMpCur()])/*  获得当前 CPU 结构        */
#define MX_CPU_GET(id)                  (&_K_cpuTable[(id)])            /*  获得指定 CPU 结构           */
/*********************************************************************************************************
  CPU 遍历
*********************************************************************************************************/
#define MX_CPU_FOREACH(i)                       \
        for (i = 0; i < MX_NCPUS; i++)
        
#define MX_CPU_FOREACH_ACTIVE(i)                \
        for (i = 0; i < MX_NCPUS; i++)          \
        if (MX_CPU_IS_ACTIVE(MX_CPU_GET(i)))
                                    
#define MX_CPU_FOREACH_EXCEPT(i, id)            \
        for (i = 0; i < MX_NCPUS; i++)          \
        if ((i) != (id))
        
#define MX_CPU_FOREACH_ACTIVE_EXCEPT(i, id)     \
        for (i = 0; i < MX_NCPUS; i++)          \
        if ((i) != (id))                        \
        if (MX_CPU_IS_ACTIVE(MX_CPU_GET(i)))

/*********************************************************************************************************
  CPU 核间中断
*********************************************************************************************************/
#define MX_CPU_ADD_IPI_PEND(id, ipi_msk)    \
        (_K_cpuTable[(id)].CPU_ulIpiPend |= (ipi_msk))                  /*  加入指定 CPU 核间中断 pend  */
#define MX_CPU_CLR_IPI_PEND(id, ipi_msk)    \
        (_K_cpuTable[(id)].CPU_ulIpiPend &= ~(ipi_msk))                 /*  清除指定 CPU 核间中断 pend  */
#define MX_CPU_GET_IPI_PEND(id)             \
        (_K_cpuTable[(id)].CPU_ulIpiPend)                               /*  获取指定 CPU 核间中断 pend  */
        
#define MX_CPU_ADD_IPI_PEND2(pcpu, ipi_msk)    \
        (pcpu->CPU_ulIpiPend |= (ipi_msk))                              /*  加入指定 CPU 核间中断 pend  */
#define MX_CPU_CLR_IPI_PEND2(pcpu, ipi_msk)    \
        (pcpu->CPU_ulIpiPend &= ~(ipi_msk))                             /*  清除指定 CPU 核间中断 pend  */
#define MX_CPU_GET_IPI_PEND2(pcpu)             \
        (pcpu->CPU_ulIpiPend)                                           /*  获取指定 CPU 核间中断 pend  */
/*********************************************************************************************************
  CPU 清除调度请求中断 (关中断情况下被调用)
*********************************************************************************************************/
#define MX_CPU_CLR_SCHED_IPI_PEND(pcpu)                             \
        do {                                                        \
            if (MX_CPU_GET_IPI_PEND2(pcpu) & MX_IPI_SCHED_MSK) {    \
                spinLockIgnIrq(&pcpu->CPU_slIpi);               \
                MX_CPU_CLR_IPI_PEND2(pcpu, MX_IPI_SCHED_MSK);       \
                spinUnlockIgnIrq(&pcpu->CPU_slIpi);             \
            }                                                       \
            archSpinNotify();                                       \
        } while (0)
/*********************************************************************************************************
  CPU 核间中断数量
*********************************************************************************************************/
#define MX_CPU_GET_IPI_CNT(id)          (_K_cpuTable[(id)].CPU_iIpiCnt)
/*********************************************************************************************************
  CPU 中断信息
*********************************************************************************************************/
#define MX_CPU_GET_NESTING(id)          (_K_cpuTable[(id)].CPU_ulInterNesting)
#define MX_CPU_GET_NESTING_MAX(id)      (_K_cpuTable[(id)].CPU_ulInterNestingMax)

ULONG  _CpuGetNesting(VOID);
ULONG  _CpuGetMaxNesting(VOID);
#define MX_CPU_GET_CUR_NESTING()        _CpuGetNesting()
#define MX_CPU_GET_CUR_NESTING_MAX()    _CpuGetMaxNesting()

#endif                                                                  /*  __K_CPU_H                   */
/*********************************************************************************************************
  END
*********************************************************************************************************/

