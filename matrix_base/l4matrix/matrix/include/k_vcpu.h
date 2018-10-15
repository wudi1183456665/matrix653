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
** 文   件   名: k_vcpu.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 11 月 28 日
**
** 描        述: 这是微内核线程类型定义文件.
*********************************************************************************************************/

#ifndef __K_VCPU_H
#define __K_VCPU_H

/*********************************************************************************************************
  线程入口函数类型定义
*********************************************************************************************************/
typedef PVOID           (*PVCPU_START_ROUTINE)(PVOID  pvArg);           /*  系统线程类型定义            */
/*********************************************************************************************************
  唤醒表节点 (差分时间)
*********************************************************************************************************/
typedef struct {
    MX_LIST_LINE          WUN_lineManage;                               /*  扫描链表                    */
    BOOL                  WUN_bInQ;                                     /*  是否在扫描链中              */
    ULONG                 WUN_ulCounter;                                /*  相对等待时间                */
} MX_WAKEUP_NODE;
typedef MX_WAKEUP_NODE   *PMX_WAKEUP_NODE;
/*********************************************************************************************************
  线程控制块
*********************************************************************************************************/
typedef struct __mx_vcpu {
    PMX_STACK             VCPU_pstkKStackNow;                           /*  线程当前堆栈指针            */
    PMX_STACK             VCPU_pstkUStackStart;                         /*  线程用户堆栈初始值          */
    UINT32                VCPU_uiPriority;                              /*  线程优先级                  */
    UINT32                VCPU_uiStatus;                                /*  线程当前状态                */

    PVCPU_START_ROUTINE   VCPU_pfuncStart;                              /*  线程入口函数                */
    PVOID                 VCPU_pvArg;                                   /*  线程入口参数                */
    BOOL                  VCPU_bIsUser;                                 /*  是否是用户线程              */

    MX_LIST_RING          VCPU_ringReady;                               /*  优先级控制块中环表节点      */
    MX_WAKEUP_NODE        VCPU_wunDelay;                                /*  等待线性表中节点            */
#define VCPU_ulDelay      VCPU_wunDelay.WUN_ulCounter

    BOOL                  VCPU_bCpuAffinity;                            /*  是否亲和运行 CPU            */
    ULONG                 VCPU_ulCpuIdAffinity;                         /*  亲和运行的 CPU ID           */

    volatile ULONG        VCPU_ulCpuIdRun;                              /*  正在运行的 CPU ID           */
    volatile BOOL         VCPU_bIsCand;                                 /*  是否在候选运行表中          */

    INT                   VCPU_iSchedRet;                               /*  调度器返回的值, signal      */
    UINT8                 VCPU_ucSchedPolicy;                           /*  调度策略                    */
    volatile ULONG        VCPU_ulLockCounter;                           /*  线程锁定计数器, 当本数值大于*/
                                                                        /*  1 时, 本线程不允许放弃 CPU  */

    UINT16                VCPU_usSchedSlice;                            /*  线程时间片保存值            */
    UINT16                VCPU_usSchedCounter;                          /*  线程当前剩余时间片          */

    PMX_PRTN              VCPU_pprtnBelong;                             /*  本线程所在的进程            */

    MX_SC_PARAM           VCPU_scparam;                                 /*  系统调用参数结构体          */
    MX_IPC                VCPU_ipc;                                     /*  IPC 控制块                  */

    volatile ULONG        VCPU_ulStatus;                                /*  VCPU 状态                   */
#define VCPU_STAT_DIS_INT 0x00000001                                    /*  关逻辑中断状态              */

#if 1
    UINT32                VCPU_uiIrqMask[8];                            /*  逻辑中断 Mask 位            */
    UINT32                VCPU_uiIrqPend[8];                            /*  逻辑中断 Pend 位            */
#else
    UINT32                VCPU_uiIrqPend;
#endif
    PARCH_USER_CTX        VCPU_puctxSP;                                 /*  中断发生时栈中的用户上下文  */
    IRQ_FUNCPTR           VCPU_pfuncIsr;                                /*  中断服务函数入口            */
    IRQ_FUNCPTR           VCPU_pfuncSwi;                                /*  系统调用函数入口            */

    MX_OBJECT_ID          VCPU_ulVcpuId;                                /*  本线程内核对象 ID           */

    BOOL                  VCPU_bIsUsed;                                 /*  本结构体是否正在使用        */
    //ARCH_REG_CTX        VCPU_regctx;
} MX_VCPU;
typedef MX_VCPU          *PMX_VCPU;
/*********************************************************************************************************
  线程属性块
*********************************************************************************************************/
typedef struct {
    PMX_STACK             VCPUATTR_pstkLowAddr;                         /*  全部堆栈区低内存起始地址    */
    ULONG                 VCPUATTR_stStackByteSize;                     /*  全部堆栈区大小(字节)        */
    UINT32                VCPUATTR_uiPriority;                          /*  线程优先级                  */
    ULONG                 VCPUATTR_ulOption;                            /*  任务选项                    */
    PVOID                 VCPUATTR_pvArg;                               /*  线程参数                    */
    BOOL                  VCPUATTR_bIsUser;                             /*  是否是用户线程              */
} MX_VCPU_ATTR;
typedef MX_VCPU_ATTR     *PMX_VCPU_ATTR;
/*********************************************************************************************************
  MX_VCPU 和内核堆栈的联合体
*********************************************************************************************************/
typedef union {
    MX_VCPU               VCPUKSTACK_vcpu;
    UINT8                 VCPUKSTACK_uiKernelStack[MX_CFG_KSTACK_SIZE];
} MX_VCPU_KSTACK;
/*********************************************************************************************************
  线程状态
*********************************************************************************************************/
#define  MX_VCPU_STATUS_READY         0x0000                            /*  任务就绪                    */
#define  MX_VCPU_STATUS_DELAY         0x0001                            /*  延迟                        */
#define  MX_VCPU_STATUS_IPC           0x0002                            /*  等待 IPC 通信               */
#define  MX_VCPU_STATUS_INIT          0x0040                            /*  初始化                      */
#define  MX_VCPU_STATUS_SUSPEND       0x0080                            /*  任务被挂起                  */

#define  MX_VCPU_IS_RUNNING(pvcpu)    (MX_CPU_GET(pvcpu->VCPU_ulCpuIdRun)->CPU_pvcpuCur == pvcpu)
#endif                                                                  /*  __K_VCPU_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/

