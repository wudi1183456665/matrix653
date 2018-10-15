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
** 文   件   名: kernel.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 03 日
**
** 描        述: 这是系统内核状态控制.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** 函数名称: __disIrqAndLockKernel
** 功能描述: 关闭中断, 并且内核自旋锁加锁
** 输　入  : piregInterLevel   中断锁定信息
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __disIrqAndLockKernel (INTREG  *piregInterLevel)
{
    PMX_PHYS_CPU     pcpuCur;
    INT              iRet;

    for (;;) {
        *piregInterLevel = archIntDisable();
        
        iRet = archSpinTryLock(&_K_klKernel.KERN_slLock); 
        if (iRet != MX_SPIN_OK) {
            _SmpIpiTryProc(MX_CPU_GET_CUR());                           /*  尝试执行 IPI                */
            archIntEnable(*piregInterLevel);
            archSpinDelay();
            //bspDebugMsg("x");
            
        } else {
            //bspDebugMsg("+");
            pcpuCur = MX_CPU_GET_CUR();
            pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter++;                /*  锁定任务在当前 CPU          */
            KN_SMP_MB();
            break;
        }
    }
}
/*********************************************************************************************************
** 函数名称: __unlockKernelAndEnIrq
** 功能描述: 内核自旋锁解锁, 并打开中断
** 输　入  : iregInterLevel    中断锁定信息
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __unlockKernelAndEnIrq (INTREG  iregInterLevel)
{
    PMX_PHYS_CPU    pcpuCur;
    INT             iRet;

    KN_SMP_MB();
    iRet = archSpinUnlock(&_K_klKernel.KERN_slLock);                    /*  解锁内核自旋锁              */
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);

    //bspDebugMsg("-");
    
    pcpuCur = MX_CPU_GET_CUR();
    //bspDebugMsg("[%p]ulLockCounter--\n", pcpuCur->CPU_pvcpuCur);
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                        /*  解除任务锁定                */

    archIntEnable(iregInterLevel);                                      /*  开启中断                    */
}
/*********************************************************************************************************
** 函数名称: kernelEnterAndDisIrq
** 功能描述: 进入内核状态, 进入后保持中断关闭状态
** 输　入  : NONE
** 输　出  : 中断寄存器
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
INTREG  kernelEnterAndDisIrq (VOID)
{
    INTREG         iregInterLevel;
    PMX_PHYS_CPU   pcpuCur;
    
    __disIrqAndLockKernel(&iregInterLevel);                             /*  关闭中断, 并锁内核 spinlock */

    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_iKernelCounter++;                                      /*  本 CPU 进入内核状态         */
    KN_SMP_WMB();                                                       /*  等待以上操作完成            */

    return  (iregInterLevel);                                           /*  打开中断                    */
}
/*********************************************************************************************************
** 函数名称: kernelExitAndEnIrq
** 功能描述: 退出内核状态
** 输　入  : iregInterLevel     中断寄存器
** 输　出  : 调度器返回值
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
INT  kernelExitAndEnIrq (INTREG  iregInterLevel)
{
    PMX_PHYS_CPU    pcpuCur;
    INT             iRetVal;
    
    pcpuCur = MX_CPU_GET_CUR();
    if (pcpuCur->CPU_iKernelCounter) {
        pcpuCur->CPU_iKernelCounter--;                                  /*  本 CPU 退出内核状态         */
        KN_SMP_WMB();                                                   /*  等待以上操作完成            */

        if (pcpuCur->CPU_iKernelCounter == 0) {

            iRetVal = schedule();                                       /*  尝试调度                    */
            __unlockKernelAndEnIrq(iregInterLevel);                     /*  解锁内核 spinlock 并打开中断*/
            
            return  (iRetVal);
        }
    }
    
    __unlockKernelAndEnIrq(iregInterLevel);                             /*  解锁内核 spinlock 并打开中断*/
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: kernelEnter
** 功能描述: 进入内核状态, 进入后将中断打开
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  kernelEnter (VOID)
{
    INTREG         iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();
    archIntEnable(iregInterLevel);                                      /*  打开中断                    */
}
/*********************************************************************************************************
** 函数名称: kernelExit
** 功能描述: 退出内核状态
** 输　入  : NONE
** 输　出  : 调度器返回值
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
INT  kernelExit (VOID)
{
    INTREG         iregInterLevel;

    iregInterLevel = archIntDisable();                                  /*  关闭中断                    */
    return  (kernelExitAndEnIrq(iregInterLevel));
}
/*********************************************************************************************************
** 函数名称: kernelUnlockSched
** 功能描述: 内核 SMP 调度器切换完成后专用释放函数 (关中断状态下被调用)
** 输　入  : pvcpuOwner     锁的持有者
** 输　出  : NONE
** 全局变量: 
** 调用模块: schedSwap()
*********************************************************************************************************/
VOID  kernelUnlockSched (PMX_VCPU  pvcpuOwner)
{
    INT            iRet;

    KN_SMP_MB();
    iRet = archSpinUnlock(&_K_klKernel.KERN_slLock);
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);

    pvcpuOwner->VCPU_ulLockCounter--;                                   /*  解除锁定任务在当前 CPU      */
}
/*********************************************************************************************************
** 函数名称: kernelLockIgnIrq
** 功能描述: 内核自旋锁加锁操作, 忽略中断锁定 (必须在中断关闭的状态下被调用)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: irqExit(), schedule()
*********************************************************************************************************/
 VOID  kernelLockIgnIrq (VOID)
{
    PMX_PHYS_CPU     pcpuCur;
    INT              iRet;

    pcpuCur  = MX_CPU_GET_CUR();

    iRet     = archSpinTryLock(&_K_klKernel.KERN_slLock);               /*  首次尝试锁定内核自旋锁      */
    while (iRet != MX_SPIN_OK) {
        _SmpIpiTryProc(MX_CPU_GET_CUR());                               /*  尝试执行 IPI                */

        iRet = archSpinTryLock(&_K_klKernel.KERN_slLock);               /*  再次尝试锁定内核自旋锁      */
    }

    //bspDebugMsg("[%p]ulLockCounter++\n", pcpuCur->CPU_pvcpuCur);
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter++;                        /*  锁定任务在当前 CPU          */
    KN_SMP_MB();
}
/*********************************************************************************************************
** 函数名称: kernelUnlockIgnIrq
** 功能描述: 内核自旋锁解锁操作, 忽略中断锁定 (必须在中断关闭的状态下被调用)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: irqExit()
*********************************************************************************************************/
VOID  kernelUnlockIgnIrq (VOID)
{
    PMX_PHYS_CPU   pcpuCur;
    INT            iRet;
    
    KN_SMP_MB();
    iRet = archSpinUnlock(&_K_klKernel.KERN_slLock);
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);

    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                        /*  解除锁定任务在当前 CPU      */
}
/*********************************************************************************************************
** 函数名称: kernelIsEnter
** 功能描述: 是否进入了进入内核状态
** 输　入  : NONE
** 输　出  : 在内核状态 MX_TRUE, 否则 MX_FALSE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
BOOL  kernelIsEnter (VOID)
{
    INTREG          iregInterLevel;
    PMX_PHYS_CPU    pcpuCur;
    INT             iKernelCounter;
    
    iregInterLevel = archIntDisable();                                  /*  关闭中断                    */
    
    pcpuCur        = MX_CPU_GET_CUR();
    iKernelCounter = pcpuCur->CPU_iKernelCounter;                       /*  获取内核状态                */
    
    archIntEnable(iregInterLevel);                                      /*  打开中断                    */
    
    if (iKernelCounter) {
        return  (MX_TRUE);
    } else {
        return  (MX_FALSE);
    }
}
/*********************************************************************************************************
** 函数名称: kernelSched
** 功能描述: 内核调度
** 输　入  : NONE
** 输　出  : 调度器返回值
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_ERROR  kernelSched (VOID)
{
    INTREG          iregInterLevel;
    MX_ERROR        ulRetVal;
    
    iregInterLevel = kernelEnterAndDisIrq();                            /*  锁内核 spinlock 并关闭中断  */
    ulRetVal = schedule();                                              /*  尝试调度                    */
    kernelExitAndEnIrq(iregInterLevel);                                 /*  解锁内核 spinlock 并打开中断*/
    
    return  (ulRetVal);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
