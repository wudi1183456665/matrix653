/*
 * ppc_regs.h
 *
 *  Created on: Jun 29, 2018
 *      Author: Liyanqi
 */

#ifndef __PPC_REGS_H
#define __PPC_REGS_H

/*********************************************************************************************************
  寄存器类型
*********************************************************************************************************/

#define ARCH_GREG_NR            32                                      		/*  通用寄存器数目              */
#define ARCH_REG_SIZE           4                                       		/*  寄存器大小                  	*/


#if (!defined(__ASSEMBLY__)) && (!defined(ASSEMBLY))
typedef UINT        ARCH_REG_T;
/*********************************************************************************************************
  PPC开发版
*********************************************************************************************************/
//PPC
/*********************************************************************************************************
  用户模式上下文信息
*********************************************************************************************************/
typedef struct {
	ARCH_REG_T		UCTX_uiR0;
	ARCH_REG_T		UCTX_uiR1;
	ARCH_REG_T		UCTX_uiR2;
	ARCH_REG_T		UCTX_uiR3;
	ARCH_REG_T		UCTX_uiR4;
	ARCH_REG_T		UCTX_uiR5;
	ARCH_REG_T		UCTX_uiR6;
	ARCH_REG_T		UCTX_uiR7;
	ARCH_REG_T		UCTX_uiR8;
	ARCH_REG_T		UCTX_uiR9;
	ARCH_REG_T		UCTX_uiR10;
	ARCH_REG_T		UCTX_uiR11;
	ARCH_REG_T		UCTX_uiR12;
	ARCH_REG_T		UCTX_uiR13;
	ARCH_REG_T		UCTX_uiR14;
	ARCH_REG_T		UCTX_uiR15;
	ARCH_REG_T		UCTX_uiR16;
	ARCH_REG_T		UCTX_uiR17;
	ARCH_REG_T		UCTX_uiR18;
	ARCH_REG_T		UCTX_uiR19;
	ARCH_REG_T		UCTX_uiR20;
	ARCH_REG_T		UCTX_uiR21;
	ARCH_REG_T		UCTX_uiR22;
	ARCH_REG_T		UCTX_uiR23;
	ARCH_REG_T		UCTX_uiR24;
	ARCH_REG_T		UCTX_uiR25;
	ARCH_REG_T		UCTX_uiR26;
	ARCH_REG_T		UCTX_uiR27;
	ARCH_REG_T		UCTX_uiR28;
	ARCH_REG_T		UCTX_uiR29;
	ARCH_REG_T		UCTX_uiR30;
	ARCH_REG_T		UCTX_uiR31;
	ARCH_REG_T		UCTX_uiSrr0;
	ARCH_REG_T		UCTX_uiSrr1;
	ARCH_REG_T		UCTX_uiCtr;
	ARCH_REG_T		UCTX_uiXer;
	ARCH_REG_T		UCTX_uiCr;
	ARCH_REG_T		UCTX_uiLr;
} ARCH_USER_CTX;

typedef ARCH_USER_CTX	*PARCH_USER_CTX;

#if 1 /* 精简版内核上下文 */
/*********************************************************************************************************
  内核模式的线程上下文
*********************************************************************************************************/
typedef struct {
	ARCH_REG_T		KCTX_uiFp;
	ARCH_REG_T		KCTX_uiPc;
} ARCH_KERN_CTX;
#else
//内核模式存在问题，需要详细阅读寄存器说明，找到不同模式下的寄存器功能
typedef struct {
	ARCH_REG_T		KCTX_uiR0;
	ARCH_REG_T		KCTX_uiR1;
	ARCH_REG_T		KCTX_uiR2;
	ARCH_REG_T		KCTX_uiR3;
	ARCH_REG_T		KCTX_uiR4;
	ARCH_REG_T		KCTX_uiR5;
	ARCH_REG_T		KCTX_uiR6;
	ARCH_REG_T		KCTX_uiR7;
	ARCH_REG_T		KCTX_uiR8;
	ARCH_REG_T		KCTX_uiR9;
	ARCH_REG_T		KCTX_uiR10;
	ARCH_REG_T		KCTX_uiR11;
	ARCH_REG_T		KCTX_uiR12;
	//?????????????????????????????????????????????????????????????????????
};
#endif
#endif
/*********************************************************************************************************
  堆栈对齐要求
*********************************************************************************************************/
#define ARCH_STK_ALIGN_SIZE     8
#define ARCH_GREG_NR            32                                      /*  通用寄存器数目              */

#define ARCH_REG_CTX_WORD_SIZE  38                                      /*  寄存器上下文字数            */
#define ARCH_STK_MIN_WORD_SIZE  256                                     /*  堆栈最小字数                */

#define ARCH_REG_SIZE           4                                       /*  寄存器大小                  */
#define ARCH_REG_CTX_SIZE       (ARCH_REG_CTX_WORD_SIZE * ARCH_REG_SIZE)/*  寄存器上下文大小            */

#define ARCH_STK_ALIGN_SIZE     8                                       /*  堆栈对齐要求                */

#define ARCH_JMP_BUF_WORD_SIZE  44                                      /*  跳转缓冲字数(向后兼容)      */

/*********************************************************************************************************
  寄存器在 ARCH_REG_CTX 中的偏移量
*********************************************************************************************************/

#define XR(n)                   ((n) * ARCH_REG_SIZE)
#define XSRR0                   ((ARCH_GREG_NR + 0) * ARCH_REG_SIZE)
#define XSRR1                   ((ARCH_GREG_NR + 1) * ARCH_REG_SIZE)
#define XCTR                    ((ARCH_GREG_NR + 2) * ARCH_REG_SIZE)
#define XXER                    ((ARCH_GREG_NR + 3) * ARCH_REG_SIZE)
#define XCR                     ((ARCH_GREG_NR + 4) * ARCH_REG_SIZE)
#define XLR                     ((ARCH_GREG_NR + 5) * ARCH_REG_SIZE)
/*********************************************************************************************************
  用户上下文和系统调用参数之间的复制
*********************************************************************************************************/
#define ARCH_UCTX_COPYTO_SCPARAM(puctx, pscparam)				\
		do {													\
			(pscparam)->SCPARAM_ui32Arg0 = puctx->UCTX_uiR0;	\
			(pscparam)->SCPARAM_ui32Arg1 = puctx->UCTX_uiR1;	\
			(pscparam)->SCPARAM_ui32Arg2 = puctx->UCTX_uiR2;	\
			(pscparam)->SCPARAM_ui32Arg3 = puctx->UCTX_uiR3;	\
		} while (0)
#define ARCH_SCPARAM_COPYTO_UCTX(pscparam, puctx)				\
		do {													\
			puctx->UCTX_uiR0 = (pscparam)->SCPARAM_ui32Arg0;	\
			puctx->UCTX_uiR1 = (pscparam)->SCPARAM_ui32Arg1;	\
			puctx->UCTX_uiR2 = (pscparam)->SCPARAM_ui32Arg2;	\
			puctx->UCTX_uiR3 = (pscparam)->SCPARAM_ui32Arg3;	\
		} while (0)
/*********************************************************************************************************
  用户上下文中断开关
*********************************************************************************************************/

/*
 *  没有Cpsr寄存器。。要改。
 */


/*********************************************************************************************************
  Lower Machine State Register (MSR) mask
*********************************************************************************************************/
#define ARCH_PPC_INT_MASK(src, des) \
    RLWINM  des, src, 0, ARCH_PPC_MSR_BIT_EE + 1, ARCH_PPC_MSR_BIT_EE - 1

#define ARCH_PPC_MSR_BIT_EE         16              /*  MSR Ext. Intr. Enable bit - EE                  */
#define ARCH_PPC_MSR_BIT_PR         17              /*  MSR Privilege Level bit - PR                    */
#define ARCH_PPC_MSR_BIT_ME         19              /*  MSR Machine Check Enable bit - ME               */
#define ARCH_PPC_MSR_BIT_LE         31              /*  MSR Little Endian mode bit - LE                 */


#define ARCH_PPC_MSR_EE             0x8000          /*  External interrupt enable                       */
#define ARCH_PPC_MSR_PR             0x4000          /*  Privilege level                                 */
#define ARCH_PPC_MSR_FP             0x2000          /*  Floating-point available                        */
#define ARCH_PPC_MSR_ME             0x1000          /*  Machine check enable                            */
#define ARCH_PPC_MSR_FE0            0x0800          /*  Floating-point exception mode 0                 */
#define ARCH_PPC_MSR_SE             0x0400          /*  Single-step trace enable                        */
#define ARCH_PPC_MSR_BE             0x0200          /*  Branch trace enable                             */
#define ARCH_PPC_MSR_FE1            0x0100          /*  Floating-point exception mode 1                 */
#define ARCH_PPC_MSR_IP             0x0040          /*  Exception prefix                                */
#define ARCH_PPC_MSR_IR             0x0020          /*  Instruction address translation                 */
#define ARCH_PPC_MSR_DR             0x0010          /*  Data address translation                        */
#define ARCH_PPC_MSR_RI             0x0002          /*  Recoverable interrupt                           */
#define ARCH_PPC_MSR_LE             0x0001          /*  Little-endian mode                              */

#define ARCH_PPC_MSR_POWERUP        0x0040          /*  State of MSR at powerup                         */


#endif /* __PPC_REGS_H */
