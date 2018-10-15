/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: core.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 1 ��
**
** ��        ��: Ӧ������ʱ��������
*********************************************************************************************************/
#include "t_sys.h"
/*********************************************************************************************************
  exec
*********************************************************************************************************/
/*
 *  ��ȡ�û�ջ�ṹ
 */
PT_VCPU_STK  T_vcpuGetCurStk ()
{
    PVOID   pvSp       = T_execGetCurSp();
    ULONG   ulStkSize  = T__USE_STK_SIZE;

    return  ((PT_VCPU_STK)(((addr_t)pvSp) & ~(ulStkSize - 1)));
}
/*
 *  ��ȡ�û���Ϣ�ṹ
 */
PMX_IPC_MSG  T_vcpuGetCurMsg ()
{
    return  ((PMX_IPC_MSG)(T_vcpuGetCurStk()->addrMsg));
}
/*
 *  ��ʼ��ջ�ṹ
 *  NOTE: ��ʵ���л���, Ҫ�ڽ��̿ռ��ڲ��ܷ��ʶ�ջ
 */
VOID  T_vcpuStkInit (PT_VCPU_STK  pStk, PMX_IPC_MSG  pMsg)
{
    IPC_ASSERT(T_MEM_ALIGN((addr_t)pStk, 12));
    IPC_ASSERT(T_MEM_ALIGN((addr_t)pMsg, 12));

    pStk->addrMsg    = (addr_t)pMsg;
#if 1                                                                   /*  ջ��͵�ַ����              */
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
     * ���� IPC_MSG, VCPU_STACK
     * ���� VCPU
     * SYSCALL
     * ������
     */
    PT_VCPU      pVcpu   = T_memObjAlloc(TMEM_VCPU);
    PT_VCPU_STK  pStk    = T_memObjAlloc(TMEM_USE_STK);

    PMX_IPC_MSG  pIpcMsg = T_vcpuGetCurMsg();

    MX_SC_RESULT_T  error;

    T_TITLE(T_vcpuCreate);
    IPC_ASSERT(pIpcMsg != MX_NULL);

    pVcpu->pIpcMsg      = T_memObjAlloc(TMEM_IPC_MSG);                  /*  TODO: �û��㲻��ӳ�������ڴ�*/

    T_vcpuStkInit(pStk, pVcpu->pIpcMsg);
    pVcpu->addrStack    = pStk->addrStkTop;                             /*  Ŀǰ����ջ����ַ            */

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
