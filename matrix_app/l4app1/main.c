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
** ��   ��   ��: main.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 02 �� 28 ��
**
** ��        ��: Ӧ�ý���.
*********************************************************************************************************/
#include <Matrix.h>
#include <libc.h>
#include <api.h>
/*********************************************************************************************************
  Tick �жϺ�
*********************************************************************************************************/
#define IRQ_NUM_TICK    27
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
UINT32          _G_uiStack[1024];
ARCH_USER_CTX   _G_uctxThread[2];
UINT32          _G_uiThreadCur;
UINT32          _G_uiTick;
UINT32          _G_puctxPC;
/*********************************************************************************************************
  ��������
*********************************************************************************************************/
VOID  thread1(VOID);
VOID  _isr(UINT32  uiIrqNum, PARCH_USER_CTX  puctxSP);
VOID  delayMs(ULONG  ulMs);
VOID  context_switch(PARCH_USER_CTX  puctxSP);
/*********************************************************************************************************
** ��������: l_main
** ��������: ���� C ���
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  l_main (VOID)
{
    //INT    i = 10;
    ARCH_REG_T  uiMsr;

    _G_uiThreadCur = 0;
    _G_uiTick      = 0;


#define ARCH_PPC_MSR_VEC			0x02000000							/*  Bit 6 of MSR				*/
    uiMsr  = 0; //archGetCpsr();                                            /*  ��õ�ǰ CPSR �Ĵ���        */
    uiMsr |=  ARCH_PPC_MSR_EE;                                 		    /*  ʹ���ж�                    */
    uiMsr &= ~ARCH_PPC_MSR_PR;                                      	/*  ��Ȩģʽ                    */
    uiMsr &= ~ARCH_PPC_MSR_FP;                				            /*  ���� FPU                    */
    //uiMsr &= ~ARCH_PPC_MSR_VEC;                                     	/*  ���� ALTIVEC                */


    _G_uctxThread[1].UCTX_uiSrr1 = uiMsr;                        /*Msr�Ĵ���*/
    _G_uctxThread[1].UCTX_uiR0   = 0x00000000;
    _G_uctxThread[1].UCTX_uiR1   = 0x01010101;
    _G_uctxThread[1].UCTX_uiR2   = 0x02020202;
    _G_uctxThread[1].UCTX_uiR3   = 0x03030303;
    _G_uctxThread[1].UCTX_uiR4   = 0x04040404;
    _G_uctxThread[1].UCTX_uiR5   = 0x05050505;
    _G_uctxThread[1].UCTX_uiR6   = 0x06060606;
    _G_uctxThread[1].UCTX_uiR7   = 0x07070707;
    _G_uctxThread[1].UCTX_uiR8   = 0x08080808;
    _G_uctxThread[1].UCTX_uiR9   = 0x09090909;
    _G_uctxThread[1].UCTX_uiR10  = 0x10101010;
    _G_uctxThread[1].UCTX_uiR11  = 0x00000000;
    _G_uctxThread[1].UCTX_uiR12  = 0x12121212;
    _G_uctxThread[1].UCTX_uiR1   = (ARCH_REG_T)&_G_uiStack[1023];  /*SPָ��*/
    _G_uctxThread[1].UCTX_uiR14  = (ARCH_REG_T)thread1;
    _G_uctxThread[1].UCTX_uiR15  = (ARCH_REG_T)thread1;

    MXI_irqRegister(_isr);
    MXI_irqAttach(IRQ_NUM_TICK);
    //lib_printf("_start = 0x%08X, thread1 = 0x%08X\n", _start, thread1);

    while (1) {
        lib_printf("This is App1, PC = 0x%08X, The tick counter is %d\n", _G_puctxPC, _G_uiTick);
        MXI_debugWrite(23);
        //API_vcpuSleep(i);
        delayMs(2000);
    }
}
VOID  thread1 (VOID)
{
    UINT   i = 0;

    while (1) {
        lib_printf("This is App1, Thread1 -- %d\n", i++);
        delayMs(1000);
    }
}
/*********************************************************************************************************
** ��������: _isr
** ��������: �ж����
** �䡡��  : uiIrqNum       �жϺ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _isr (UINT32  uiIrqNum, PARCH_USER_CTX  puctxSP)
{
    switch (uiIrqNum) {

    case IRQ_NUM_TICK:
        _G_uiTick++;
        _G_puctxPC = puctxSP->UCTX_uiR15;
        context_switch(puctxSP);
        break;

    default:
        break;
    }
}
/*********************************************************************************************************
** ��������: delayMs
** ��������: �ӳٺ���
** ��  ��  : ulMs     ������
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  delayMs (ULONG  ulMs)
{
    volatile UINT  i;

    while (ulMs) {
        ulMs--;
        for (i = 0; i < 8065; i++) {
        }
    }
}
VOID  uctxCopy (PARCH_USER_CTX  puctxDst, PARCH_USER_CTX  puctxSrc)
{
    puctxDst->UCTX_uiSrr1 = puctxSrc->UCTX_uiSrr1;        /*Msr�Ĵ���*/
    puctxDst->UCTX_uiR0   = puctxSrc->UCTX_uiR0;
    puctxDst->UCTX_uiR1   = puctxSrc->UCTX_uiR1;
    puctxDst->UCTX_uiR2   = puctxSrc->UCTX_uiR2;
    puctxDst->UCTX_uiR3   = puctxSrc->UCTX_uiR3;
    puctxDst->UCTX_uiR4   = puctxSrc->UCTX_uiR4;
    puctxDst->UCTX_uiR5   = puctxSrc->UCTX_uiR5;
    puctxDst->UCTX_uiR6   = puctxSrc->UCTX_uiR6;
    puctxDst->UCTX_uiR7   = puctxSrc->UCTX_uiR7;
    puctxDst->UCTX_uiR8   = puctxSrc->UCTX_uiR8;
    puctxDst->UCTX_uiR9   = puctxSrc->UCTX_uiR9;
    puctxDst->UCTX_uiR10  = puctxSrc->UCTX_uiR10;
    puctxDst->UCTX_uiR11  = puctxSrc->UCTX_uiR11;
    puctxDst->UCTX_uiR12  = puctxSrc->UCTX_uiR12;
    puctxDst->UCTX_uiR1   = puctxSrc->UCTX_uiR1;
    puctxDst->UCTX_uiR14  = puctxSrc->UCTX_uiR14;
    puctxDst->UCTX_uiR15  = puctxSrc->UCTX_uiR15;
}
VOID  context_switch (PARCH_USER_CTX  puctxSP)
{
    uctxCopy(&_G_uctxThread[_G_uiThreadCur], puctxSP);
    _G_uiThreadCur = 1 - _G_uiThreadCur;
    uctxCopy(puctxSP, &_G_uctxThread[_G_uiThreadCur]);
}
VOID  tcb_swap (PVOID  pcpuCur)
{
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

