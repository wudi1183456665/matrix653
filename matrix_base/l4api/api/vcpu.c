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
** ��   ��   ��: vcpu.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 03 �� 02 ��
**
** ��        ��: VCPU ��� API �ӿ�.
*********************************************************************************************************/
#include <Matrix.h>
#include "abi.h"
/*********************************************************************************************************
** ��������: MXI_vcpuCreate
** ��������: ���� VCPU
** �䡡��  : pVcpu   ϵͳ���õ�ǰ VCPU
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T MXI_vcpuCreate (PMX_IPC_MSG         pipcMsg,
                            MX_OBJECT_ID        objPt,
                            MX_OBJECT_ID       *pobjVcpu,
                            PVCPU_START_ROUTINE pRount,
                            PVOID               pvArg,
                            UINT32              uiPriority,
                            addr_t              addrStack,
                            addr_t              addrMsgTag)
{
    /*
     * syscall ����˵����
     *     [IN]
     *          ARG1    - UINT32    - ���� OBJ
     *          ARG2    - ULONG     - �߳���ڵ�ַ
     *          ARG3    - ULONG     - �߳���ڲ���
     *          IPC_MSG - UINT32    - �߳����ȼ�
     *                  - addr_t    - �û� IPC_MSG ��ַ
     *                  - addr_t    _ �û���ջ��ַ(ջ��)
     *     [OUT]
     *          ARG1    - UINT32    - �´����߳� OBJ
     */
    ULONG   ulArg0      = 0;
    ULONG   ulOobjVcpu  = 0;

    //T_DEBUG(": prio=%d, pMsg=0x%x", uiPriority, addrMsgTag);

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_CREATE);
#if 0
    ipcMsgWriteOpen(pipcMsg);
    ipcMsgUlWrite(pipcMsg, uiPriority);
    ipcMsgUlWrite(pipcMsg, (ULONG)addrMsgTag);
    ipcMsgUlWrite(pipcMsg, (ULONG)addrStack);
    ipcMsgFlush(pipcMsg);
#endif

    MX_Syscall42(ulArg0, (ULONG)objPt, (ULONG)pRount, (ULONG)pvArg,
                &ulArg0, &ulOobjVcpu);

    *pobjVcpu = (MX_OBJECT_ID)ulOobjVcpu;
    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_vcpuStart
** ��������: ���� VCPU
** �䡡��  : pVcpu   ϵͳ���õ�ǰ VCPU
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T MXI_vcpuStart (MX_OBJECT_ID  objVcpu)
{
    /*
     * syscall ����˵����
     *     [IN]
     *         ARG1    - UINT32    - �߳� OBJ
     */
    ULONG   ulArg0      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_START);

    MX_Syscall41(ulArg0, (ULONG)objVcpu, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_vcpuSleep
** ��������: ���� VCPU
** �䡡��  : pVcpu   ϵͳ���õ�ǰ VCPU
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_vcpuSleep (SC_TIME  scTime)
{
    /*
     * syscall ����˵����
     *     [IN]
     *          ARG2,ARG3     - TIMEOUT    - ����ʱ��
     */
    ULONG   ulArg0      = 0;
    ULONG   ulArg2      = 0;
    ULONG   ulArg3      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_SLEEP);
    ABI_SCTIME_SET(ulArg2, ulArg3, scTime);

    MX_Syscall41(ulArg0, 0, ulArg2, ulArg3, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_vcpuWakeup
** ��������: �����߳�
** �䡡��  : objVcpu
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_vcpuWakeup (MX_OBJECT_ID  objVcpu)
{
    /*
     * syscall ����˵����
     *     [IN]
     *          ARG1    - MX_OBJECT_ID   - �߳�ID
     */
    ULONG   ulArg0      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_WAKEUP);

    MX_Syscall41(ulArg0, (ULONG)objVcpu, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_vcpuDisableInt
** ��������: ���ܵ�ǰ VCPU �����ж�
** �䡡��  : NONE
** �䡡��  : ��ǰ�ж�״̬
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INTREG  MXI_vcpuDisableInt (VOID)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_DIS_INT);

    MX_Syscall42(ulArg0, 0, 0, 0, &ulArg0, &ulArg1);

    return  (ulArg1);
}
/*********************************************************************************************************
** ��������: MXI_vcpuEnableInt
** ��������: ʹ��(�ָ�)��ǰ VCPU �����ж�
** �䡡��  : iregInterLevel  ����ʱ�����״̬
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  MXI_vcpuEnableInt (INTREG  iregInterLevel)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = (ULONG)iregInterLevel;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_EN_INT);

    MX_Syscall41(ulArg0, ulArg1, 0, 0, &ulArg0);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/


