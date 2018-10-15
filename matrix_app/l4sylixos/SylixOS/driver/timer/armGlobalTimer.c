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
** 文   件   名: armGlobalTimer.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2013 年 12 月 13 日
**
** 描        述: 全局定时器.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "timer/armGlobalTimer.h"
#include "arch/arm/common/cp15/armCp15.h"
/*********************************************************************************************************
  全局定时器寄存器偏移
*********************************************************************************************************/
#define GLOBAL_TIMER_REGS_BASE_OFFSET   (0x0200)        /*  全局定时器寄存器偏移                        */
/*********************************************************************************************************
  全局定时器寄存器
*********************************************************************************************************/
typedef struct {
    volatile UINT32     uiCounterLow;                   /*  Global Timer Counter Register(Lower).       */
    volatile UINT32     uiCounterHigh;                  /*  Global Timer Counter Register(Upper).       */
    volatile UINT32     uiControl;                      /*  Global Timer Control Register.              */
    volatile UINT32     uiIntStatus;                    /*  Global Timer Interrupt Status Register.     */
    volatile UINT32     uiComparatorLow;                /*  Comparator Value Registers(Lower).          */
    volatile UINT32     uiComparatorHigh;               /*  Comparator Value Registers(Upper).          */
    volatile UINT32     uiAutoIncrement;                /*  Auto-increment Register.                    */
} GLOBAL_TIMER_REGS;
/*********************************************************************************************************
  GLOBAL_TIMER_REGS.uiControl 寄存器位域常量
*********************************************************************************************************/
#define BIT_OFFS_CTLR_TIMERENABLE       (0)
#define BTI_MASK_CTLR_TIMERENABLE       (0x1ul << BIT_OFFS_CTLR_TIMERENABLE)

#define BIT_OFFS_CTLR_COMPENABLE        (1)
#define BTI_MASK_CTLR_COMPENABLE        (0x1ul << BIT_OFFS_CTLR_COMPENABLE)

#define BIT_OFFS_CTLR_IRQENABLE         (2)
#define BTI_MASK_CTLR_IRQENABLE         (0x1ul << BIT_OFFS_CTLR_IRQENABLE)

#define BIT_OFFS_CTLR_AUTOINCREMENT     (3)
#define BTI_MASK_CTLR_AUTOINCREMENT     (0x1ul << BIT_OFFS_CTLR_AUTOINCREMENT)

#define BIT_OFFS_CTLR_PRESCALER         (8)
#define BTI_MASK_CTLR_PRESCALER         (0xFFul << BIT_OFFS_CTLR_PRESCALER)
/*********************************************************************************************************
** 函数名称: armGlobalTimerGet
** 功能描述: 获得全局定时器
** 输　入  : NONE
** 输　出  : 全局定时器寄存器基址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE GLOBAL_TIMER_REGS  *armGlobalTimerGet (VOID)
{
    REGISTER addr_t  ulBase = armPrivatePeriphBaseGet() + GLOBAL_TIMER_REGS_BASE_OFFSET;

    return  ((GLOBAL_TIMER_REGS *)ulBase);
}
/*********************************************************************************************************
** 函数名称: armGlobalTimerInit
** 功能描述: 初始化全局定时器
** 输　入  : bAutoIncrement        到期后 Comparator 是否自增
**           uiIncrementValue      Comparator 增量
**           uiPrescaler           预分频数值
**           bIrqEnable            是否使能 IRQ 中断
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armGlobalTimerInit (BOOL    bAutoIncrement,
                          UINT32  uiIncrementValue,
                          UINT32  uiPrescaler,
                          BOOL    bIrqEnable)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer    = armGlobalTimerGet();
    REGISTER UINT32              uiControl = 0;

    write32(0, (addr_t)&pTimer->uiControl);

    if (bAutoIncrement) {
        uiControl |=  BTI_MASK_CTLR_AUTOINCREMENT;
        write32(uiIncrementValue, (addr_t)&pTimer->uiAutoIncrement);
    }

    if (bIrqEnable) {
        uiControl |=  BTI_MASK_CTLR_IRQENABLE;
    }

    uiControl |= BTI_MASK_CTLR_COMPENABLE;

    uiControl |= uiPrescaler << BIT_OFFS_CTLR_PRESCALER;

    write32(0, (addr_t)&pTimer->uiCounterLow);
    write32(0, (addr_t)&pTimer->uiCounterHigh);

    write32(0, (addr_t)&pTimer->uiComparatorLow);
    write32(0, (addr_t)&pTimer->uiComparatorHigh);

    write32(uiControl, (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** 函数名称: armGlobalTimerIntClear
** 功能描述: 清除全局定时器中断
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armGlobalTimerIntClear (VOID)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(1, (addr_t)&pTimer->uiIntStatus);
}
/*********************************************************************************************************
** 函数名称: armGlobalTimerStart
** 功能描述: 启动全局定时器
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armGlobalTimerStart (VOID)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) | BTI_MASK_CTLR_TIMERENABLE,
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** 函数名称: armGlobalTimerStop
** 功能描述: 停止全局定时器
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armGlobalTimerStop (VOID)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) & (~BTI_MASK_CTLR_TIMERENABLE),
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** 函数名称: armGlobalTimerCounterGet
** 功能描述: 获得全局定时器计数器值
** 输　入  : NONE
** 输　出  : 计数器值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT64  armGlobalTimerCounterGet (VOID)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();
    REGISTER UINT32  uiLow, uiHigh, uiOldHigh;
    REGISTER UINT64  ulCounter;

    uiOldHigh = read32((addr_t)&pTimer->uiCounterHigh);
    while (1) {
        uiLow  = read32((addr_t)&pTimer->uiCounterLow);
        uiHigh = read32((addr_t)&pTimer->uiCounterHigh);
        if (uiOldHigh == uiHigh) {
            break;
        } else {
            uiOldHigh = uiHigh;
        }
    }

    ulCounter = uiHigh;

    return  ((UINT64)((ulCounter << 32) | uiLow));
}
/*********************************************************************************************************
** 函数名称: armGlobalTimerCounterSet
** 功能描述: 设置全局定时器计数器值
** 输　入  : ulCounter         计数器值
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armGlobalTimerCounterSet (UINT64  ulCounter)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(ulCounter, (addr_t)&pTimer->uiCounterLow);
    write32(ulCounter >> 32, (addr_t)&pTimer->uiCounterHigh);
}
/*********************************************************************************************************
** 函数名称: armGlobalTimerComparatorSet
** 功能描述: 设置全局定时器比较器
** 输　入  : ulComparator      比较器值
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  armGlobalTimerComparatorSet (UINT64  ulComparator)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) & (~BTI_MASK_CTLR_COMPENABLE),
            (addr_t)&pTimer->uiControl);

    write32((UINT32)ulComparator, (addr_t)&pTimer->uiComparatorLow);
    write32(ulComparator >> 32, (addr_t)&pTimer->uiComparatorHigh);

    write32(read32((addr_t)&pTimer->uiControl) | BTI_MASK_CTLR_COMPENABLE,
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** 函数名称: armGlobalTimerComparatorGet
** 功能描述: 获得全局定时器比较器
** 输　入  : NONE
** 输　出  : 比较器值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT64  armGlobalTimerComparatorGet (VOID)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();
    REGISTER UINT32  uiLow, uiHigh, uiOldHigh;
    REGISTER UINT64  ulCounter;

    uiOldHigh = read32((addr_t)&pTimer->uiComparatorHigh);

    while (1) {
        uiLow  = read32((addr_t)&pTimer->uiComparatorLow);
        uiHigh = read32((addr_t)&pTimer->uiComparatorHigh);
        if (uiOldHigh == uiHigh) {
            break;
        } else {
            uiOldHigh = uiHigh;
        }
    }

    ulCounter = uiHigh;

    return  ((UINT64)((ulCounter << 32) | uiLow));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
