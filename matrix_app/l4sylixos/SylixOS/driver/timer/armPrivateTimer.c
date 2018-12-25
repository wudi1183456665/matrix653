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
** ��   ��   ��: armPrivateTimer.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2013 �� 12 �� 13 ��
**
** ��        ��: ˽�ж�ʱ��.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "timer/armPrivateTimer.h"
#include "arch/arm/common/cp15/armCp15.h"
/*********************************************************************************************************
  ˽�ж�ʱ���Ĵ���ƫ��
*********************************************************************************************************/
#define PRIVATE_TIMER_REGS_BASE_OFFSET      (0x600)     /*  ˽�ж�ʱ���Ĵ���ƫ��                        */
/*********************************************************************************************************
  ˽�ж�ʱ���Ĵ���
*********************************************************************************************************/
typedef struct {
    volatile UINT32     uiLoad;                         /*  Private Timer Load Register.                */
    volatile UINT32     uiCounter;                      /*  Private Timer Counter Register.             */
    volatile UINT32     uiControl;                      /*  Private Timer Control Register.             */
    volatile UINT32     uiIntStatus;                    /*  Private Timer Interrupt Status Register.    */
} PRIVATE_TIMER_REGS;
/*********************************************************************************************************
  PRIVATE_TIMER_REGS.uiControl �Ĵ���λ����
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
** ��������: armPrivateTimerGet
** ��������: ���˽�ж�ʱ��
** �䡡��  : NONE
** �䡡��  : ˽�ж�ʱ���Ĵ�����ַ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LW_INLINE PRIVATE_TIMER_REGS  *armPrivateTimerGet (VOID)
{
    REGISTER addr_t  ulBase = armPrivatePeriphBaseGet() + PRIVATE_TIMER_REGS_BASE_OFFSET;

    return  ((PRIVATE_TIMER_REGS *)ulBase);
}
/*********************************************************************************************************
** ��������: armPrivateTimerInit
** ��������: ��ʼ��˽�ж�ʱ��
** �䡡��  : bAutoReload           ���ں��Ƿ��Զ���װ
**           uiCounter             ������ֵ
**           uiPrescaler           Ԥ��Ƶ��ֵ
**           bIrqEnable            �Ƿ�ʹ�� IRQ �ж�
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: armPrivateTimerIntClear
** ��������: ���˽�ж�ʱ���ж�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armPrivateTimerIntClear (VOID)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer = armPrivateTimerGet();

    write32(1, (addr_t)&pTimer->uiIntStatus);
}
/*********************************************************************************************************
** ��������: armPrivateTimerStart
** ��������: ����˽�ж�ʱ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armPrivateTimerStart (VOID)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer = armPrivateTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) | BTI_MASK_CTLR_TIMERENABLE,
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** ��������: armPrivateTimerStop
** ��������: ֹͣ˽�ж�ʱ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armPrivateTimerStop (VOID)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer = armPrivateTimerGet();

    write32(read32((addr_t)&pTimer->uiControl) & (~BTI_MASK_CTLR_TIMERENABLE),
            (addr_t)&pTimer->uiControl);
}
/*********************************************************************************************************
** ��������: armPrivateTimerCounterGet
** ��������: ���˽�ж�ʱ��������ֵ
** �䡡��  : NONE
** �䡡��  : ������ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
UINT32  armPrivateTimerCounterGet (VOID)
{
    REGISTER PRIVATE_TIMER_REGS  *pTimer = armPrivateTimerGet();

    return  (read32((addr_t)&pTimer->uiCounter));
}
/*********************************************************************************************************
** ��������: armPrivateTimerCounterSet
** ��������: ����˽�ж�ʱ��������ֵ
** �䡡��  : uiCounter         ������ֵ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
