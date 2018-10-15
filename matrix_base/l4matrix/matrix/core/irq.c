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
** 文   件   名: irq.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 19 日
**
** 描        述: 中断管理.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
  抽象中断号到 VCPU 的转换表
*********************************************************************************************************/
static PMX_VCPU    _G_pvcpuVecMap[MX_CFG_MAX_IRQS];
static PMX_VCPU    _G_pvcpuTickAt[MX_CFG_MAX_VCPUS];
//static spinlock_t  _G_slVecMapLock;
/*********************************************************************************************************
** 函数名称: __irqVecToVcpu
** 功能描述: 查找抽象中断号对应的 VCPU 结构体
** 输　入  : ulVec           抽象中断号
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static PMX_VCPU  __irqVecToVcpu (ULONG  ulVec)
{
    return  (_G_pvcpuVecMap[ulVec]);
}
/*********************************************************************************************************
** 函数名称: irqRegister
** 功能描述: 注册当前 VCPU 的中断服务函数
** 输　入  : pfuncIsr        中断服务函数指针
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  irqRegister (IRQ_FUNCPTR  pfuncIsr)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();

    //bspDebugMsg("irqRegister(%p)\n", pfuncIsr);
    pvcpuCur->VCPU_pfuncIsr = pfuncIsr;
}
/*********************************************************************************************************
** 函数名称: irqAttach
** 功能描述: 申请抽象中断号
** 输　入  : ulVec           抽象中断号
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_ERROR  irqAttach (ULONG  ulVec)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();
    INT         i;

    //bspDebugMsg("irqAttach(%d)\n", ulVec);
    if (ulVec != MX_CFG_TICK_VEC) {
        _G_pvcpuVecMap[ulVec] = pvcpuCur;

    } else {
        for (i = 0; i < MX_CFG_MAX_VCPUS; i++) {
            if (_G_pvcpuTickAt[i] == pvcpuCur) {
                return (ERROR_NONE);
            } else if (_G_pvcpuTickAt[i] == NULL) {
                _G_pvcpuTickAt[i] = pvcpuCur;
                return (ERROR_NONE);
            }
        }
        return (ERROR_VCPU_FULL);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: irqDisAttach
** 功能描述: 收回线程申请的所有抽象中断
** 输　入  : pvcpu          线程控制块 
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  irqDisAttach (PMX_VCPU  pvcpu)
{
    INT         i;

    for (i = 0; i < MX_CFG_MAX_IRQS; i++) {
        if (_G_pvcpuVecMap[i] == pvcpu) {
            _G_pvcpuVecMap[i] = MX_NULL;
        }
    }

    for (i = 0; i < MX_CFG_MAX_VCPUS; i++) {
        if (_G_pvcpuTickAt[i] == pvcpu) {
            _G_pvcpuTickAt[i] = MX_NULL;
        }
    }
} 
/*********************************************************************************************************
** 函数名称: irqEnable
** 功能描述: 设置硬件使能中断
** 输　入  : ulVec           抽象中断号    
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  irqEnable (ULONG  ulVec)
{
    if (ulVec != MX_CFG_TICK_VEC) {
        //bspDebugMsg("irqEnable(%d)\n", ulVec);
        bspIntVectorEnable(ulVec);
    }
}
/*********************************************************************************************************
** 函数名称: irqDisable
** 功能描述: 设置硬件禁能中断
** 输　入  : ulVec           抽象中断号    
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  irqDisable (ULONG  ulVec)
{
    if (ulVec != MX_CFG_TICK_VEC) {
        bspIntVectorDisable(ulVec);
    }
}
/*********************************************************************************************************
** 函数名称: irqIpiVecSet
** 功能描述: 设置 CPU 的核间中断逻辑中断号
** 输　入  : ulCpuId         CPU ID
**           ulVec           核间中断逻辑中断号
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  irqIpiVecSet (ULONG  ulCpuId, ULONG  ulVec)
{
    PMX_PHYS_CPU    pcpu;

    if (ulCpuId < MX_CFG_MAX_PROCESSORS) {
        pcpu = MX_CPU_GET(ulCpuId);
        pcpu->CPU_ulIpiVec = ulVec;
    }
}
/*********************************************************************************************************
** 函数名称: irqIsr
** 功能描述: 中断总服务
** 输　入  : ulVec           抽象中断号, 有效范围: 0 - 255 (arch 层函数需要保证此参数正确)
** 输　出  : 中断返回值
** 全局变量: 
** 调用模块: archIntHandle()
*********************************************************************************************************/
MX_ERROR  irqIsr (ULONG  ulVec, PARCH_USER_CTX  puctxSP)
{
    PMX_PHYS_CPU        pcpu;
    PMX_VCPU            pvcpu;
    INT                 i;
    
    //dump("-%d-", ulVec);

    pcpu = MX_CPU_GET_CUR();                                            /*  中断处理程序中, 不会改变 CPU*/
    
    //__MX_CPU_INT_ENTER_HOOK(ulVec, pcpu->CPU_ulInterNesting);
    if (pcpu->CPU_ulIpiVec == ulVec) {                                  /*  核间中断                    */
        _SmpIpiProc(pcpu);                                              /*  处理核间中断                */
        //dump("CPU %d Proc Ipi Vector %d\n", pcpu->CPU_ulCpuId, ulVec);

    } else {
        //spinLock(&_G_slVecMapLock);                                   /*  锁住 spinlock               */
        //kernelEnter();

        if (ulVec == MX_CFG_TICK_VEC) {
            for (i = 0; i < MX_CFG_MAX_VCPUS; i++) {
                pvcpu = _G_pvcpuTickAt[i];
                if (pvcpu != MX_NULL) {                                 /*  本中断已被分配给某 VCPU     */
                    vcpuOccurIrq(pvcpu, ulVec);
                } else {
                    break;
                }
            }
        } else {
            pvcpu = __irqVecToVcpu(ulVec);
            if (pvcpu != MX_NULL) {                                     /*  本中断已被分配给某 VCPU     */
                vcpuOccurIrq(pvcpu, ulVec);
            }
        }

        //kernelExit();
        //spinUnlock(&_G_slVecMapLock);                                 /*  解锁 spinlock               */
    }
    //__MX_CPU_INT_EXIT_HOOK(ulVec, pcpu->CPU_ulInterNesting);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: irqEnter
** 功能描述: 内核中断入口函数 (在关中断的情况下被调用)
** 输　入  : NONE
** 输　出  : 中断层数
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
ULONG  irqEnter (VOID)
{
    PMX_PHYS_CPU  pcpu;

    pcpu = MX_CPU_GET_CUR();
    //bspDebugMsg("[%d]irqEnter() pcpu = %p\n", archMpCur(), pcpu);
    if (pcpu->CPU_ulInterNesting != MX_CFG_MAX_INTER_NESTING) {
        pcpu->CPU_ulInterNesting++;                                     /*  系统中断嵌套层数++          */
    }
    
    KN_SMP_WMB();                                                       /*  等待以上操作完成            */

    return  (pcpu->CPU_ulInterNesting);
}
/*********************************************************************************************************
** 函数名称: irqExit
** 功能描述: 内核中断出口函数 (在关中断的情况下被调用)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  irqExit (VOID)
{
    PMX_PHYS_CPU  pcpu;

    pcpu = MX_CPU_GET_CUR();
    //bspDebugMsg("[%d]irqExit()  pcpu = %p\n", archMpCur(), pcpu);
    //bspDebugMsg("           Nesting = %p\n", pcpu->CPU_ulInterNesting);

    if (pcpu->CPU_ulInterNesting) {                                     /*  系统中断嵌套层数--          */
        pcpu->CPU_ulInterNesting--;
    }

    KN_SMP_WMB();                                                       /*  等待以上操作完成            */

    if (pcpu->CPU_ulInterNesting) {                                     /*  查看系统是否在中断嵌套中    */
        return;
    }

    kernelLockIgnIrq();
    schedule(); //scheduleInt();
    kernelUnlockIgnIrq();
}
/*********************************************************************************************************
** 函数名称: swiRegister
** 功能描述: 注册当前 VCPU 的软中断服务函数
** 输　入  : pfuncSwi        软中断服务函数指针
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  swiRegister (UINT32  uiSwiNum, IRQ_FUNCPTR  pfuncSwi)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();

    pvcpuCur->VCPU_pfuncSwi  = pfuncSwi;
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
