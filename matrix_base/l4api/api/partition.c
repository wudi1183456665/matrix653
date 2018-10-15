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
** 文   件   名: partition.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 04 月 19 日
**
** 描        述: Partition 相关 API 接口.
*********************************************************************************************************/
#include <Matrix.h>
#include "abi.h"
/*********************************************************************************************************
** 函数名称: MXI_prtnRestart
** 功能描述: 重启分区
** 输　入  : objPrtn       分区 ID
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  MXI_prtnRestart (MX_OBJECT_ID  objPrtn)
{
    ULONG    ulArg0 = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_PRTN);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_PRTN_RESTART);

    MX_Syscall10(ulArg0);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/


