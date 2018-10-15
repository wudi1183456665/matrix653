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
** ��   ��   ��: ipc.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 03 �� 02 ��
**
** ��        ��: IPC ��� API �ӿ�.
*********************************************************************************************************/
#include <Matrix.h>
#include "abi.h"
/*********************************************************************************************************
** ��������: MXI_ipcCall
** ��������: IPC call
** �䡡��  : objTag        recver ����
**           ullTimeout    ��ʱʱ��
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_ipcCall (MX_OBJECT_ID  objTag, SC_TIME  ullTimeout)
{
    ULONG   ulArg0      = 0;
    ULONG   ulArg2      = 0;
    ULONG   ulArg3      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IPC);
    ABI_OPCODE_SET(&ulArg0, IPC_OP_SEND | IPC_OP_RECV);
    ABI_SCTIME_SET(ulArg2, ulArg3, ullTimeout);

    MX_Syscall41(ulArg0, (ULONG)objTag, ulArg2, ulArg3, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}

/*********************************************************************************************************
** ��������: MXI_ipcSend
** ��������: IPC send
** �䡡��  : objTag        recver ����
**           ullTimeout    ��ʱʱ��
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_ipcSend (MX_OBJECT_ID  objTag, SC_TIME  ullTimeout)
{
    ULONG   ulArg0      = 0;
    ULONG   ulArg2      = 0;
    ULONG   ulArg3      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IPC);
    ABI_OPCODE_SET(&ulArg0, IPC_OP_SEND);
    ABI_SCTIME_SET(ulArg2, ulArg3, ullTimeout);

    MX_Syscall41(ulArg0, (ULONG)objTag, ulArg2, ulArg3, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_ipcOpenWait
** ��������: IPC open wait
** �䡡��  : pobjSender    ���Ͷ���
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_ipcOpenWait (MX_OBJECT_ID  *pobjSender)
{
    ULONG   ulArg0      = 0;
    ULONG   ulObjId     = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IPC);
    ABI_OPCODE_SET(&ulArg0, IPC_OP_RECV | IPC_OP_OPEN);

    MX_Syscall42(ulArg0, 0, 0, 0, &ulArg0, &ulObjId);

    *pobjSender = (MX_OBJECT_ID)ulObjId;
    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_ipcReplyWait
** ��������: IPC reply wait
** �䡡��  : pobjSender    �µķ��Ͷ���
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_ipcReplyWait (MX_OBJECT_ID  *pobjSender)
{
    ULONG   ulArg0      = 0;
    ULONG   ulObjId     = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IPC);
    ABI_OPCODE_SET(&ulArg0, (IPC_OP_SEND | IPC_OP_RECV |
                             IPC_OP_OPEN | IPC_OP_REPLY));

    MX_Syscall42(ulArg0, 0, 0, 0, &ulArg0, &ulObjId);

    *pobjSender = (MX_OBJECT_ID)ulObjId;
    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** ��������: MXI_ipcClose
** ��������: �رշ���
** �䡡��  :
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_ipcClose (VOID)
{
    ULONG   ulArg0      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_IPC);
    ABI_OPCODE_SET(&ulArg0, IPC_OP_CLOSE);

    MX_Syscall41(ulArg0, 0, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

