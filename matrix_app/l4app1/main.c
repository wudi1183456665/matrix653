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
** 文   件   名: main.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 02 月 28 日
**
** 描        述: 应用进程.
*********************************************************************************************************/
#include <Matrix.h>
#include <libc.h>
#include <api.h>
/*********************************************************************************************************
  Tick 中断号
*********************************************************************************************************/
#define IRQ_NUM_TICK    27
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
UINT32          _G_uiStack[1024];
ARCH_USER_CTX   _G_uctxThread[2];
UINT32          _G_uiThreadCur;
UINT32          _G_uiTick;
UINT32          _G_puctxPC;
/*********************************************************************************************************
  函数声明
*********************************************************************************************************/
VOID  thread1(VOID);
VOID  _isr(UINT32  uiIrqNum, PARCH_USER_CTX  puctxSP);
VOID  delayMs(ULONG  ulMs);
VOID  context_switch(PARCH_USER_CTX  puctxSP);
/*********************************************************************************************************
** 函数名称: l_main
** 功能描述: 进程 C 入口
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  l_main (VOID)
{
    //INT    i = 10;
    ARCH_REG_T  uiMsr;

    _G_uiThreadCur = 0;
    _G_uiTick      = 0;


#define ARCH_PPC_MSR_VEC			0x02000000							/*  Bit 6 of MSR				*/
    uiMsr  = 0; //archGetCpsr();                                            /*  获得当前 CPSR 寄存器        */
    uiMsr |=  ARCH_PPC_MSR_EE;                                 		    /*  使能中断                    */
    uiMsr &= ~ARCH_PPC_MSR_PR;                                      	/*  特权模式                    */
    uiMsr &= ~ARCH_PPC_MSR_FP;                				            /*  禁能 FPU                    */
    //uiMsr &= ~ARCH_PPC_MSR_VEC;                                     	/*  禁能 ALTIVEC                */


    _G_uctxThread[1].UCTX_uiSrr1 = uiMsr;                        /*Msr寄存器*/
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
    _G_uctxThread[1].UCTX_uiR1   = (ARCH_REG_T)&_G_uiStack[1023];  /*SP指针*/
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
** 函数名称: _isr
** 功能描述: 中断入口
** 输　入  : uiIrqNum       中断号
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: delayMs
** 功能描述: 延迟毫秒
** 输  入  : ulMs     毫秒数
** 输  出  : NONE
** 全局变量:
** 调用模块:
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
    puctxDst->UCTX_uiSrr1 = puctxSrc->UCTX_uiSrr1;        /*Msr寄存器*/
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

