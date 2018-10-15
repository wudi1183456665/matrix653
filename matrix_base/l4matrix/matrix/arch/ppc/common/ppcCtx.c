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
** 文   件   名: ppcAssert.c
**
** 创   建   人: Li.Yanqi
**
** 文件创建日期: 2018 年 06 月 29 日
**
** 描        述: .
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** 函数名称:archPartitionCreate
** 功能描述:分区创建
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  archPartitionCreate (VOID)
{

}
/*********************************************************************************************************
** 函数名称: archVcpuCtxCreate
** 功能描述: VCPU 内核上下文的创建
** 输　入  :  pfuncStart     入口函数地址
**           pvArg          入口函数参数
**           pstkTop        当前栈顶(指向最后一个有效元素)
**           ulOpt          选项
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
PMX_STACK	archVcpuCtxCreate (VOID_FUNCPTR  pfuncshell,
							   PMX_STACK	 pstkTop,
							   ULONG		 ulOpt)
{
	ARCH_KERN_CTX		*pregctx;
	INTREG			 	 uiMsr;
//	PPC_PARAM			*pParam;

	uiMsr = ppcGetMSR();												/*  获得当前MSR的值 			*/
#define ARCH_PPC_MSR_VEC			0x02000000							/*  Bit 6 of MSR				*/
    uiMsr |=  ARCH_PPC_MSR_EE;                                 		    /*  使能中断                    */
    uiMsr &= ~ARCH_PPC_MSR_PR;                                      	/*  特权模式                    */
    uiMsr &= ~ARCH_PPC_MSR_FP;                				            /*  禁能 FPU                    */
    uiMsr &= ~ARCH_PPC_MSR_VEC;                                     	/*  禁能 ALTIVEC                */
#undef ARCH_PPC_MSR_VEC
    /*  当使用虚拟内存时。使能MMU */
    uiMsr |=  ARCH_PPC_MSR_IR | ARCH_PPC_MSR_DR;						/*	使能 MMU					*/
    /*  不使用虚拟内存时，禁能MMU */
//  uiMsr &= ~(ARCH_PPC_MSR_IR | ARCH_PPC_MSR_DR);                  	/*  禁能 MMU                    */
    pstkTop = (PMX_STACK)ROUND_DOWN(pstkTop, ARCH_STK_ALIGN_SIZE);		/*	堆栈指针向下 8 字节对齐		*/

    pregctx = (ARCH_KERN_CTX *)((PCHAR)pstkTop - sizeof(ARCH_KERN_CTX));

#if 1	/*精简版内核上下文*/
    pregctx->KCTX_uiFp		= 0;				/* ???有问题 */
    pregctx->KCTX_uiPc		= (ARCH_REG_T)pfuncshell;

#else
    pregctx->UCTX_uiR0		= 0;
    pregctx->UCTX_uiR0   = 0;
    pregctx->UCTX_uiSp   = (ARCH_REG_T)pfpctx;
    pregctx->UCTX_uiR2   = 2;
    pregctx->UCTX_uiR3   = (ARCH_REG_T)pvArg;                            /*  R3 用于参数传递             */
    pregctx->UCTX_uiR4   = 4;
    pregctx->UCTX_uiR5   = 5;
    pregctx->UCTX_uiR6   = 6;
    pregctx->UCTX_uiR7   = 7;
    pregctx->UCTX_uiR8   = 8;
    pregctx->UCTX_uiR9   = 9;
    pregctx->UCTX_uiR10  = 10;
    pregctx->UCTX_uiR11  = 11;
    pregctx->UCTX_uiR12  = 12;
    pregctx->UCTX_uiR13  = 13;
    pregctx->UCTX_uiR14  = 14;
    pregctx->UCTX_uiR15  = 15;
    pregctx->UCTX_uiR16  = 16;
    pregctx->UCTX_uiR17  = 17;
    pregctx->UCTX_uiR18  = 18;
    pregctx->UCTX_uiR19  = 19;
    pregctx->UCTX_uiR20  = 20;
    pregctx->UCTX_uiR21  = 21;
    pregctx->UCTX_uiR22  = 22;
    pregctx->UCTX_uiR23  = 23;
    pregctx->UCTX_uiR24  = 24;
    pregctx->UCTX_uiR25  = 25;
    pregctx->UCTX_uiR26  = 26;
    pregctx->UCTX_uiR27  = 27;
    pregctx->UCTX_uiR28  = 28;
    pregctx->UCTX_uiR29  = 29;
    pregctx->UCTX_uiR30  = 30;
    pregctx->UCTX_uiFp   = pfpctx->FP_uiFp;

    pregctx->UCTX_uiLr   = (ARCH_REG_T)pfuncTask;
    pregctx->UCTX_uiPc   = (ARCH_REG_T)pfuncTask;                        /*  RFI 返回的地址              */
    pregctx->UCTX_uiMsr  = uiMsr;                                        /*  RFI 后的 MSR                */
    pregctx->UCTX_uiCtr  = 0;
    pregctx->UCTX_uiXer  = 0;
    pregctx->UCTX_uiCr   = 0;
#endif
    return ((PMX_STACK)pregctx);
}
/*********************************************************************************************************
** 函数名称: archVcpuCtxPrint
** 功能描述: Vcpu 上下文打印
** 输　入  : pstkTop
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  archVcpuCtxPrint(PMX_STACK  pstkTop)
{

}
