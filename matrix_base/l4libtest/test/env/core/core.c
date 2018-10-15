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
** 文   件   名: core.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: 应用运行时环境抽象
*********************************************************************************************************/
#include "t_sys.h"
/*********************************************************************************************************
  exec
*********************************************************************************************************/
/*
 *  获取用户栈结构
 */
PT_VCPU_STK  T_vcpuGetCurStk ()
{
    PVOID   pvSp       = T_execGetCurSp();
    ULONG   ulStkSize  = T__USE_STK_SIZE;

    return  ((PT_VCPU_STK)(((addr_t)pvSp) & ~(ulStkSize - 1)));
}
/*
 *  获取用户消息结构
 */
PMX_IPC_MSG  T_vcpuGetCurMsg ()
{
    return  ((PMX_IPC_MSG)(T_vcpuGetCurStk()->addrMsg));
}
/*
 *  初始化栈结构
 *  NOTE: 真实运行环境, 要在进程空间内才能访问堆栈
 */
VOID  T_vcpuStkInit (PT_VCPU_STK  pStk, PMX_IPC_MSG  pMsg)
{
    IPC_ASSERT(T_MEM_ALIGN((addr_t)pStk, 12));
    IPC_ASSERT(T_MEM_ALIGN((addr_t)pMsg, 12));

    pStk->addrMsg    = (addr_t)pMsg;
#if 1                                                                   /*  栈向低地址生长              */
    pStk->addrStkTop = ((addr_t)pStk) + T__USE_STK_SIZE;
#else
    pStk->addrStkTop = (addr_t)(pStk + 2);
#endif
}
/*********************************************************************************************************
  VCPU
*********************************************************************************************************/
PT_VCPU  T_vcpuCreate  (MX_OBJECT_ID        objPt,
                        PVCPU_START_ROUTINE pRount,
                        UINT32              uiPriority)
{
    /*
     * 分配 IPC_MSG, VCPU_STACK
     * 创建 VCPU
     * SYSCALL
     * 处理结果
     */
    PT_VCPU      pVcpu   = T_memObjAlloc(TMEM_VCPU);
    PT_VCPU_STK  pStk    = T_memObjAlloc(TMEM_USE_STK);

    PMX_IPC_MSG  pIpcMsg = T_vcpuGetCurMsg();

    MX_SC_RESULT_T  error;

    T_TITLE(T_vcpuCreate);
    IPC_ASSERT(pIpcMsg != MX_NULL);

    pVcpu->pIpcMsg      = T_memObjAlloc(TMEM_IPC_MSG);                  /*  TODO: 用户层不需映射物理内存*/

    T_vcpuStkInit(pStk, pVcpu->pIpcMsg);
    pVcpu->addrStack    = pStk->addrStkTop;                             /*  目前传递栈顶地址            */

    pVcpu->objPt        = objPt;
    pVcpu->pfuncStart   = pRount;
    pVcpu->uiPriority   = uiPriority;

    error = API_vcpuCreate(pIpcMsg,
                           objPt,
                           &pVcpu->objVcpu,
                           pVcpu->pfuncStart,
                           (PVOID)pVcpu,
                           pVcpu->uiPriority,
                           pVcpu->addrStack,
                           (addr_t)pVcpu->pIpcMsg);

    if(error != MX_SC_RESULT_OK) {
        T_ERROR(" error");
        return  (MX_NULL);
    }

    return  (pVcpu);
}

BOOL  T_vcpuStart (PT_VCPU pVcpu)
{
    MX_SC_RESULT_T  error;

    T_TITLE(T_vcpuStart);

    if (pVcpu == MX_NULL) {
        return (MX_FALSE);
    }

    error = API_vcpuStart(pVcpu->objVcpu);

    return  ((error == MX_SC_RESULT_OK) ? MX_TRUE : MX_FALSE);
}

VOID  T_vcpuSleep (SC_TIME time)
{
    MX_SC_RESULT_T  error = API_vcpuSleep(time);

    if (error != MX_SC_RESULT_OK) {
        T_ERROR(" error");
    }
}

VOID  T_vcpuWakeup (MX_OBJECT_ID objVcpu)
{
    MX_SC_RESULT_T  error = API_vcpuWakeup(objVcpu);

    if (error != MX_SC_RESULT_OK) {
        T_ERROR(" error");
    }
}

/*********************************************************************************************************
  IPC
*********************************************************************************************************/

/*********************************************************************************************************
  END
*********************************************************************************************************/
