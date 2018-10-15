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
** ��   ��   ��: ppcAssert.c
**
** ��   ��   ��: Li.Yanqi
**
** �ļ���������: 2018 �� 06 �� 29 ��
**
** ��        ��: .
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** ��������:archPartitionCreate
** ��������:��������
** �䡡��  :
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archPartitionCreate (VOID)
{

}
/*********************************************************************************************************
** ��������: archVcpuCtxCreate
** ��������: VCPU �ں������ĵĴ���
** �䡡��  :  pfuncStart     ��ں�����ַ
**           pvArg          ��ں�������
**           pstkTop        ��ǰջ��(ָ�����һ����ЧԪ��)
**           ulOpt          ѡ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PMX_STACK	archVcpuCtxCreate (VOID_FUNCPTR  pfuncshell,
							   PMX_STACK	 pstkTop,
							   ULONG		 ulOpt)
{
	ARCH_KERN_CTX		*pregctx;
	INTREG			 	 uiMsr;
//	PPC_PARAM			*pParam;

	uiMsr = ppcGetMSR();												/*  ��õ�ǰMSR��ֵ 			*/
#define ARCH_PPC_MSR_VEC			0x02000000							/*  Bit 6 of MSR				*/
    uiMsr |=  ARCH_PPC_MSR_EE;                                 		    /*  ʹ���ж�                    */
    uiMsr &= ~ARCH_PPC_MSR_PR;                                      	/*  ��Ȩģʽ                    */
    uiMsr &= ~ARCH_PPC_MSR_FP;                				            /*  ���� FPU                    */
    uiMsr &= ~ARCH_PPC_MSR_VEC;                                     	/*  ���� ALTIVEC                */
#undef ARCH_PPC_MSR_VEC
    /*  ��ʹ�������ڴ�ʱ��ʹ��MMU */
    uiMsr |=  ARCH_PPC_MSR_IR | ARCH_PPC_MSR_DR;						/*	ʹ�� MMU					*/
    /*  ��ʹ�������ڴ�ʱ������MMU */
//  uiMsr &= ~(ARCH_PPC_MSR_IR | ARCH_PPC_MSR_DR);                  	/*  ���� MMU                    */
    pstkTop = (PMX_STACK)ROUND_DOWN(pstkTop, ARCH_STK_ALIGN_SIZE);		/*	��ջָ������ 8 �ֽڶ���		*/

    pregctx = (ARCH_KERN_CTX *)((PCHAR)pstkTop - sizeof(ARCH_KERN_CTX));

#if 1	/*������ں�������*/
    pregctx->KCTX_uiFp		= 0;				/* ???������ */
    pregctx->KCTX_uiPc		= (ARCH_REG_T)pfuncshell;

#else
    pregctx->UCTX_uiR0		= 0;
    pregctx->UCTX_uiR0   = 0;
    pregctx->UCTX_uiSp   = (ARCH_REG_T)pfpctx;
    pregctx->UCTX_uiR2   = 2;
    pregctx->UCTX_uiR3   = (ARCH_REG_T)pvArg;                            /*  R3 ���ڲ�������             */
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
    pregctx->UCTX_uiPc   = (ARCH_REG_T)pfuncTask;                        /*  RFI ���صĵ�ַ              */
    pregctx->UCTX_uiMsr  = uiMsr;                                        /*  RFI ��� MSR                */
    pregctx->UCTX_uiCtr  = 0;
    pregctx->UCTX_uiXer  = 0;
    pregctx->UCTX_uiCr   = 0;
#endif
    return ((PMX_STACK)pregctx);
}
/*********************************************************************************************************
** ��������: archVcpuCtxPrint
** ��������: Vcpu �����Ĵ�ӡ
** �䡡��  : pstkTop
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archVcpuCtxPrint(PMX_STACK  pstkTop)
{

}
