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
** 文   件   名: debug.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 03 月 03 日
**
** 描        述: Irq 相关 API 接口.
*********************************************************************************************************/
#include <Matrix.h>
#include "abi.h"
/*********************************************************************************************************
** 函数名称: MXI_irqRegister
** 功能描述: 注册中断服务函数
** 输　入  : pfuncIsr       中断服务函数
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: MXI_irqAttach
** 功能描述: 申请中断号
** 输　入  : uiIrqNum       中断号
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: MXI_irqEnable
** 功能描述: 使能硬件中断
** 输　入  : uiIrqNum       中断号
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: MXI_irqDisable
** 功能描述: 使能硬件中断
** 输　入  : uiIrqNum       中断号
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: MXI_swiRegister
** 功能描述: 注册系统调用服务函数
** 输　入  : uiSwiNum       系统调用号
**           pfuncSwi       系统调用服务函数
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
