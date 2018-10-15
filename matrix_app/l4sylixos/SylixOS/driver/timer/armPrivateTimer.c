/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: armPrivateTimer.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2013 年 12 月 13 日
**
** 描        述: 私有定时器.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "timer/armPrivateTimer.h"
#include "arch/arm/common/cp15/armCp15.h"
/*********************************************************************************************************
  私有定时器寄存器偏移
*********************************************************************************************************/
#define PRIVATE_TIMER_REGS_BASE_OFFSET      (0x600)     /*  私有定时器寄存器偏移                        */
/*********************************************************************************************************
  私有定时器寄存器
*********************************************************************************************************/
typedef struct {
    volatile UINT32     uiLoad;                         /*  Private Timer Load Register.                */
    volatile UINT32     uiCounter;                      /*  Private Timer Counter Register.             */
    volatile UINT32     uiControl;                      /*  Private Timer Control Register.             */
    volatile UINT32     uiIntStatus;                    /*  Private Timer Interrupt Status Register.    */
} PRIVATE_TIMER_REGS;
/*********************************************************************************************************
  PRIVATE_TIMER_REGS.uiControl 寄存器位域常量
*********************************************************************************************************/
#define BIT_OFFS_CTLR_TIMERENABLE           (0)
#define BTI_MASK_CTLR_TIMERENABLE           (0x1ul << BIT_OFFS_CTLR_TIMERENABLE)

#define BIT_OFFS_CTLR_AUTORELOAD            (1)
#define BTI_MASK_CTLR_AUTORELOAD            (0x1ul << BIT_OFFS_CTLR_AUTORELOAD)

#define BIT_OFFS_CTLR_IRQENABLE             (2)
#define BTI_MASK_CTLR_IRQENABLE             (0x1ul << BIT_OFFS_CTLR_IRQENABLE)

#define BIT_OFFS_CTLR_PRESCALER             (8)
#define BTI_MASK_CTLR_PRESCALER             (0xFFul << BIT_OFFS_CTLR_PRESCALER)
/*********************************************************************************************************
** 函数名称: armPrivateTimerGet
** 功能描述: 获得私有定时器
** 输　入  : NONE
** 输　出  : 私有定时器寄存器基址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE PRIVATE_TIMER_REGS  *armPrivateTimerGet (VOID)
{
    REGISTER addr_t  ulBase = armPrivatePeriphBaseGet() + PRIVATE_TIMER_REGS_BASE_OFFSET;

    return  ((PRIVATE_TIMER_REGS *)ulBase);
}
/*********************************************************************************************************
** 函数名称: armPrivateTimerInit
** 功能描述: 初始化私有定时器
** 输　入  : bAutoReload           到期后是否自动重装
**           uiCounter             计数器值
**           uiPrescaler           预分频数值
**           bIrqEnable            是否使能 IRQ 中断
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armPrivateTimerInit (BOOL    bAutoReload,
                           UINT32  uiCounter,
                           UINT32  uiPrescaler,
                           BOOL    bIrqEnable)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer    = armPrivateTimerGet();
    REGISTER UINT32               uiControl = 0;

    write32(0, (addr_t)&pTimer->uiControl);

    if (bAutoReload) {
        uiControl |=  BTI_MASK_CTLR_AUTORELOAD;
    }

    write32(uiCounter, (addr_t)&pTimer->uiLoad);
    write32(uiCounter, (addr_t)&pTimer->uiCounter);

    if (bIrqEnable) {
        uiControl |=  BTI_MASK_CTLR_IRQENABLE;
    }

    uiControl |= uiPrescaler << BIT_OFFS_CTLR_PRESCALER;

    write32(uiControl, (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** 函数名称: armPrivateTimerIntClear
** 功能描述: 清除私有定时器中断
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armPrivateTimerIntClear (VOID)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer = armPrivateTimerGet();

    write32(1, (addr_t)&pTimer->uiIntStatus);
}
/*********************************************************************************************************
** 函数名称: armPrivateTimerStart
** 功能描述: 启动私有定时器
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armPrivateTimerStart (VOID)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer = armPrivateTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) | BTI_MASK_CTLR_TIMERENABLE,
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** 函数名称: armPrivateTimerStop
** 功能描述: 停止私有定时器
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armPrivateTimerStop (VOID)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer = armPrivateTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) & (~BTI_MASK_CTLR_TIMERENABLE),
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** 函数名称: armPrivateTimerCounterGet
** 功能描述: 获得私有定时器计数器值
** 输　入  : NONE
** 输　出  : 计数器值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT32  armPrivateTimerCounterGet (VOID)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer = armPrivateTimerGet();

    return  (read32((addr_t)&pTimer->uiCounter));
}
/*********************************************************************************************************
** 函数名称: armPrivateTimerCounterSet
** 功能描述: 设置私有定时器计数器值
** 输　入  : uiCounter         计数器值
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armPrivateTimerCounterSet (UINT32  uiCounter)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer    = armPrivateTimerGet();
    REGISTER UINT32               uiControl = read32((addr_t)&pTimer->uiControl);

    write32(read32((addr_t)&pTimer->uiControl) & (~BTI_MASK_CTLR_TIMERENABLE),
            (addr_t)&pTimer->uiControl);

    write32(uiCounter, (addr_t)&pTimer->uiLoad);
    write32(uiCounter, (addr_t)&pTimer->uiCounter);

    write32(uiControl, (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
