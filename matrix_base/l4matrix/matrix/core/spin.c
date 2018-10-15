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
** 文   件   名: spin.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 27 日
**
** 描        述: 多 CPU 系统自旋锁.
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** 函数名称: spinInit
** 功能描述: 自旋锁初始化
** 输　入  : psl           自旋锁
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  spinInit (spinlock_t  *psl)
{
    archSpinInit(psl);
    KN_SMP_WMB();
}
/*********************************************************************************************************
** 函数名称: spinTryLock
** 功能描述: 自旋锁尝试加锁操作
** 输　入  : psl           自旋锁
** 输　出  : MX_TRUE 加锁正常 MX_FALSE 加锁失败
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
BOOL  spinTryLock (spinlock_t  *psl)
{
    INTREG          iregInterLevel;
    PMX_PHYS_CPU    pcpuCur;
    INT             iRet;
    
    iregInterLevel = archIntDisable();
    
    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter++;                        /*  锁定任务在当前 CPU          */
    
    iRet = archSpinTryLock(psl);
    KN_SMP_MB();
    
    if (iRet != MX_SPIN_OK) {
        pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                    /*  解锁失败, 解除任务锁定      */
    }
    
    archIntEnable(iregInterLevel);
    
    return  ((iRet == MX_SPIN_OK) ? (MX_TRUE) : (MX_FALSE));
}
/*********************************************************************************************************
** 函数名称: spinLockIgnIrq
** 功能描述: 自旋锁加锁操作, 忽略中断锁定 (必须在中断关闭的状态下被调用)
** 输　入  : psl           自旋锁
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  spinLockIgnIrq (spinlock_t *psl)
{
    PMX_PHYS_CPU    pcpuCur;

    archSpinLock(psl);                                                  /*  ARCH 层保证锁定成功         */

    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter++;                        /*  锁定任务在当前 CPU          */
    KN_SMP_MB();
}
/*********************************************************************************************************
** 函数名称: spinLockAndIrq
** 功能描述: 自旋锁加锁操作, 连同锁定中断
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  spinLockAndDisIrq (spinlock_t *psl, INTREG  *piregInterLevel)
{
    *piregInterLevel = archIntDisable();

    spinLockIgnIrq(psl);
}
/*********************************************************************************************************
** 函数名称: spinLock
** 功能描述: 自旋锁加锁操作
** 输　入  : psl           自旋锁
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  spinLock (spinlock_t  *psl)
{
    INTREG          iregInterLevel;

    spinLockAndDisIrq(psl, &iregInterLevel);                            /*  自旋锁加锁并关中断          */
    
    archIntEnable(iregInterLevel);                                      /*  开中断                      */
}
/*********************************************************************************************************
** 函数名称: spinUnlockIgnIrq
** 功能描述: 自旋锁解锁操作, 忽略中断锁定 (必须在中断关闭的状态下被调用)
** 输　入  : psl           自旋锁
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  spinUnlockIgnIrq (spinlock_t *psl)
{
    PMX_PHYS_CPU    pcpuCur;
    INT             iRet;
    
    KN_SMP_MB();
    iRet = archSpinUnlock(psl);
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);
    
    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                        /*  解除任务锁定                */
}
/*********************************************************************************************************
** 函数名称: spinUnlockIrq
** 功能描述: 自旋锁解锁操作, 连同解锁中断
** 输　入  : psl               自旋锁
**           iregInterLevel    中断锁定信息
** 输　出  : 调度器返回值
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_ERROR  spinUnlockAndEnIrq (spinlock_t *psl, INTREG  iregInterLevel)
{
    PMX_PHYS_CPU    pcpuCur;
    BOOL            bTrySched = MX_FALSE;
    INT             iRet;
    
    KN_SMP_MB();
    iRet = archSpinUnlock(psl);
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);
    
    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                        /*  解除任务锁定                */
    if (pcpuCur->CPU_ulInterNesting == 0 &&
        pcpuCur->CPU_iKernelCounter == 0 &&
        pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter == 0 &&
        MX_CAND_ROT(pcpuCur)) {

        bTrySched = MX_TRUE;                                            /*  需要尝试调度                */
    }
    
    archIntEnable(iregInterLevel);
    
    if (bTrySched) {
        //K_WARNING("spinUnlockAndEnIrq: KERN SCHED");
        return  (kernelSched());
    
    } else {
        return  (ERROR_NONE);
    }
}
/*********************************************************************************************************
** 函数名称: spinUnlock
** 功能描述: 自旋锁解锁操作
** 输　入  : psl           自旋锁
** 输　出  : 调度器返回值
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_ERROR  spinUnlock (spinlock_t  *psl)
{
    INTREG          iregInterLevel;
    
    iregInterLevel = archIntDisable();
    
    return  (spinUnlockAndEnIrq(psl, iregInterLevel));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

