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
** 文   件   名: ipc.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 03 月 02 日
**
** 描        述: IPC 相关 API 接口.
*********************************************************************************************************/
#include <Matrix.h>
#include "abi.h"
/*********************************************************************************************************
** 函数名称: MXI_ipcCall
** 功能描述: IPC call
** 输　入  : objTag        recver 对象
**           ullTimeout    超时时长
** 输　出  :
** 全局变量:
** 调用模块:
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
** 函数名称: MXI_ipcSend
** 功能描述: IPC send
** 输　入  : objTag        recver 对象
**           ullTimeout    超时时长
** 输　出  :
** 全局变量:
** 调用模块:
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
** 函数名称: MXI_ipcOpenWait
** 功能描述: IPC open wait
** 输　入  : pobjSender    发送对象
** 输　出  :
** 全局变量:
** 调用模块:
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
** 函数名称: MXI_ipcReplyWait
** 功能描述: IPC reply wait
** 输　入  : pobjSender    新的发送对象
** 输　出  :
** 全局变量:
** 调用模块:
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
** 函数名称: MXI_ipcClose
** 功能描述: 关闭服务
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
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

