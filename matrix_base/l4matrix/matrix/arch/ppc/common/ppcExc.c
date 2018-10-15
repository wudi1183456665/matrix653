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
** 文   件   名: ppcExc.c
**
** 创   建   人: Li.Yanqi
**
** 文件创建日期: 2018 年 07 月 06 日
**
** 描        述: Power PC 体系架构异常处理
*********************************************************************************************************/

#include <Matrix.h>
/*********************************************************************************************************
  定义 Decrementer 相关全局变量
*********************************************************************************************************/
static ULONG    _G_ulDecVector[MX_CFG_MAX_PROCESSORS];
static BOOL     _G_bDecPreemptive[MX_CFG_MAX_PROCESSORS];
static UINT32   _G_uiDecValue[MX_CFG_MAX_PROCESSORS];
static BOOL     _G_bDecInited[MX_CFG_MAX_PROCESSORS];
/*********************************************************************************************************
** 函数名称: archIntHandle
** 功能描述: bspIntHandle 需要调用此函数处理中断 (关闭中断情况被调用)
** 输　入  : ulVec         中断向量
**           puctxSP       用户上下文结构体指针
** 输　出  : NONE
** 全局变量:
** 调用模块: bspIntHandle()
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archIntHandle (ULONG	ulVec, PARCH_USER_CTX puctxSP)
{
	MX_VCPU_GET_CUR()->VCPU_puctxSP = puctxSP;

	irqIsr(ulVec, puctxSP);

	MX_VCPU_GET_CUR()->VCPU_puctxSP = MX_NULL;
}
/*********************************************************************************************************
** 函数名称: archDataStorageExceptionHandle
** 功能描述: 数据存储异常处理
** 输　入  :  puctxSP       用户上下文结构体指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archDataStorageExceptionHandle (PARCH_USER_CTX puctxSP)
{
    dump("\n\033[31m****** [%d] Data Storage Exception : 0x%08X *****\033[0m\n", archMpCur(), puctxSP);
    dumpUctx(puctxSP);
    while (1);
}
/*********************************************************************************************************
** 函数名称: archInstructionStorageExceptionHandle
** 功能描述: 数据存储异常处理
** 输　入  :  puctxSP       用户上下文结构体指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archInstructionStorageExceptionHandle (PARCH_USER_CTX puctxSP)
{
	dump("\n\033[31m****** [%d] Instruction Storage Exception : 0x%08X *****\033[0m\n", archMpCur(), puctxSP);
	dumpUctx(puctxSP);
	while (1);
}
/*********************************************************************************************************
** 函数名称: archAlignmentExceptionHandle
** 功能描述: 数据存储异常处理
** 输　入  :  puctxSP       用户上下文结构体指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archAlignmentExceptionHandle (PARCH_USER_CTX puctxSP)
{
	dump("\n\033[31m****** [%d] Alignment Exception : 0x%08X *****\033[0m\n", archMpCur(), puctxSP);
	dumpUctx(puctxSP);
	while (1);
}
/*********************************************************************************************************
** 函数名称: archProgramExceptionHandle
** 功能描述: 数据存储异常处理
** 输　入  :  puctxSP       用户上下文结构体指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archProgramExceptionHandle (PARCH_USER_CTX puctxSP)
{
	dump("\n\033[31m****** [%d] Program Exception : 0x%08X *****\033[0m\n", archMpCur(), puctxSP);
	dumpUctx(puctxSP);
	while (1);
}
/*********************************************************************************************************
** 函数名称: archSystemCallExceptionHandle
** 功能描述: 数据存储异常处理
** 输　入  :  puctxSP       用户上下文结构体指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archSystemCallExceptionHandle (PARCH_USER_CTX puctxSP)
{
	dump("\n\033[31m****** [%d] System Call Exception : 0x%08X *****\033[0m\n", archMpCur(), puctxSP);
	dumpUctx(puctxSP);
	while (1);
}
/*********************************************************************************************************
** 函数名称: archTraceHandle
** 功能描述: 数据存储异常处理
** 输　入  :  puctxSP       用户上下文结构体指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archTraceHandle (PARCH_USER_CTX puctxSP)
{
	dump("\n\033[31m****** [%d] Trace Handle : 0x%08X *****\033[0m\n", archMpCur(), puctxSP);
	dumpUctx(puctxSP);
	while (1);
}
/*********************************************************************************************************
** 函数名称: archMachineCheckExceptionHandle
** 功能描述: 机器检查异常处理
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archMachineCheckExceptionHandle (PARCH_USER_CTX puctxSP)
{
	dump("\n\033[31m****** [%d] Machine Check Exception : 0x%08X *****\033[0m\n", archMpCur(), puctxSP);
	dumpUctx(puctxSP);
	while (1);
}


/*********************************************************************************************************
** 函数名称: archDecrementerInterruptHandle
** 功能描述: Decrementer 中断处理
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 此函数退出时必须为中断关闭状态.
*********************************************************************************************************/
VOID  archDecrementerInterruptHandle (PARCH_USER_CTX puctxSP)
{
	dump("\n\033[31m****** [%d] Machine Check Exception : 0x%08X *****\033[0m\n", archMpCur(), puctxSP);
	dumpUctx(puctxSP);
	while (1);
}
/*********************************************************************************************************
** 函数名称: archDecrementerInit
** 功能描述: 初始化 Decrementer
** 输　入  : ulVector          Decrementer 中断向量
**           bPreemptive       是否可抢占
**           uiDecValue        Decrementer 值
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 不能在多任务环境启动前调用该函数.
*********************************************************************************************************/
VOID  archDecrementerInit (ULONG    ulVector,
                           BOOL     bPreemptive,
                           UINT32   uiDecValue)
{
    ULONG   ulCPUId = archMpCur();

    _G_ulDecVector[ulCPUId]    = ulVector;
    _G_bDecPreemptive[ulCPUId] = bPreemptive;
    _G_uiDecValue[ulCPUId]     = uiDecValue;

    ppcSetDEC(uiDecValue);

    _G_bDecInited[ulCPUId] = MX_TRUE;
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
