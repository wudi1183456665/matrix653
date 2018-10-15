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
** ��   ��   ��: debug.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 03 �� 03 ��
**
** ��        ��: Irq ��� API �ӿ�.
*********************************************************************************************************/
#include <Matrix.h>
#include "abi.h"
/*********************************************************************************************************
** ��������: MXI_irqRegister
** ��������: ע���жϷ�����
** �䡡��  : pfuncIsr       �жϷ�����
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_irqRegister (IRQ_FUNCPTR  pfuncIsr)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = (ULONG)pfuncIsr;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IRQ);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_IRQ_REGISTER);

    MX_Syscall41(ulArg0, ulArg1, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_irqAttach
** ��������: �����жϺ�
** �䡡��  : uiIrqNum       �жϺ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_irqAttach (UINT32  uiIrqNum)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = (ULONG)uiIrqNum;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IRQ);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_IRQ_ATTACH);

    MX_Syscall41(ulArg0, ulArg1, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_irqEnable
** ��������: ʹ��Ӳ���ж�
** �䡡��  : uiIrqNum       �жϺ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_irqEnable (UINT32  uiIrqNum)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = (ULONG)uiIrqNum;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IRQ);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_IRQ_ENABLE);

    MX_Syscall41(ulArg0, ulArg1, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_irqDisable
** ��������: ʹ��Ӳ���ж�
** �䡡��  : uiIrqNum       �жϺ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_irqDisable (UINT32  uiIrqNum)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = (ULONG)uiIrqNum;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IRQ);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_IRQ_DISABLE);

    MX_Syscall41(ulArg0, ulArg1, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_swiRegister
** ��������: ע��ϵͳ���÷�����
** �䡡��  : uiSwiNum       ϵͳ���ú�
**           pfuncSwi       ϵͳ���÷�����
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_swiRegister (UINT32  uiSwiNum, IRQ_FUNCPTR  pfuncSwi)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = (ULONG)uiSwiNum;
    ULONG    ulArg2 = (ULONG)pfuncSwi;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IRQ);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_SWI_REGISTER);

    MX_Syscall41(ulArg0, ulArg1, ulArg2, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
