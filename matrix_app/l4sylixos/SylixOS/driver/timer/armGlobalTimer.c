/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: armGlobalTimer.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2013 �� 12 �� 13 ��
**
** ��        ��: ȫ�ֶ�ʱ��.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "timer/armGlobalTimer.h"
#include "arch/arm/common/cp15/armCp15.h"
/*********************************************************************************************************
  ȫ�ֶ�ʱ���Ĵ���ƫ��
*********************************************************************************************************/
#define GLOBAL_TIMER_REGS_BASE_OFFSET   (0x0200)        /*  ȫ�ֶ�ʱ���Ĵ���ƫ��                        */
/*********************************************************************************************************
  ȫ�ֶ�ʱ���Ĵ���
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
  GLOBAL_TIMER_REGS.uiControl �Ĵ���λ����
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
** ��������: armGlobalTimerGet
** ��������: ���ȫ�ֶ�ʱ��
** �䡡��  : NONE
** �䡡��  : ȫ�ֶ�ʱ���Ĵ�����ַ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LW_INLINE GLOBAL_TIMER_REGS  *armGlobalTimerGet (VOID)
{
    REGISTER addr_t  ulBase = armPrivatePeriphBaseGet() + GLOBAL_TIMER_REGS_BASE_OFFSET;

    return  ((GLOBAL_TIMER_REGS *)ulBase);
}
/*********************************************************************************************************
** ��������: armGlobalTimerInit
** ��������: ��ʼ��ȫ�ֶ�ʱ��
** �䡡��  : bAutoIncrement        ���ں� Comparator �Ƿ�����
**           uiIncrementValue      Comparator ����
**           uiPrescaler           Ԥ��Ƶ��ֵ
**           bIrqEnable            �Ƿ�ʹ�� IRQ �ж�
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: armGlobalTimerIntClear
** ��������: ���ȫ�ֶ�ʱ���ж�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGlobalTimerIntClear (VOID)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(1, (addr_t)&pTimer->uiIntStatus);
}
/*********************************************************************************************************
** ��������: armGlobalTimerStart
** ��������: ����ȫ�ֶ�ʱ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGlobalTimerStart (VOID)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) | BTI_MASK_CTLR_TIMERENABLE,
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** ��������: armGlobalTimerStop
** ��������: ֹͣȫ�ֶ�ʱ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGlobalTimerStop (VOID)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) & (~BTI_MASK_CTLR_TIMERENABLE),
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** ��������: armGlobalTimerCounterGet
** ��������: ���ȫ�ֶ�ʱ��������ֵ
** �䡡��  : NONE
** �䡡��  : ������ֵ
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: armGlobalTimerCounterSet
** ��������: ����ȫ�ֶ�ʱ��������ֵ
** �䡡��  : ulCounter         ������ֵ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGlobalTimerCounterSet (UINT64  ulCounter)
{
    REGISTER GLOBAL_TIMER_REGS  *pTimer = armGlobalTimerGet();

    write32(ulCounter, (addr_t)&pTimer->uiCounterLow);
    write32(ulCounter >> 32, (addr_t)&pTimer->uiCounterHigh);
}
/*********************************************************************************************************
** ��������: armGlobalTimerComparatorSet
** ��������: ����ȫ�ֶ�ʱ���Ƚ���
** �䡡��  : ulComparator      �Ƚ���ֵ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: armGlobalTimerComparatorGet
** ��������: ���ȫ�ֶ�ʱ���Ƚ���
** �䡡��  : NONE
** �䡡��  : �Ƚ���ֵ
** ȫ�ֱ���:
** ����ģ��:
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
