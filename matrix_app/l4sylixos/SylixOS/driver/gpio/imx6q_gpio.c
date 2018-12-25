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
** ��   ��   ��: imx6q_gpio.c
**
** ��   ��   ��: zhang.xu (����)
**
** �ļ���������: 2015 �� 12 �� 13 ��
**
** ��        ��: Imx6q �� GPIO ����
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include "gpio/imx6q_gpio.h"

#include "sdk.h"
#include "irq/irq_numbers.h"
#include "pinmux/iomux_config.h"
/*********************************************************************************************************
  ����
*********************************************************************************************************/
#define GPIO_CONTROLER_NR           (7)                                 /*  imx6q �� 7 �� gpio ������   */
#define GPIO_DR                     (0x00)                              /*  GPIO ��ƽ���üĴ���         */
#define GPIO_GDIR                   (0x04)                              /*  GPIO �������ѡ��Ĵ���     */
#define GPIO_PSR                    (0x08)                              /*  GPIO �ܽ�״̬�Ĵ���         */
#define GPIO_ICR1                   (0x0C)                              /*  GPIO �ж�ģʽ���üĴ��� 1   */
#define GPIO_ICR2                   (0x10)                              /*  GPIO �ж�ģʽ���üĴ��� 2   */
#define GPIO_IMR                    (0x14)                              /*  GPIO �ж�ʹ��/���μĴ���    */
#define GPIO_ISR                    (0x18)                              /*  GPIO �ж�״̬�Ĵ���         */
#define GPIO_EDGE_SEL               (0x1C)                              /*  GPIO ˫���ش����ж�ʹ��     */

/*********************************************************************************************************
  GPIO ���������Ͷ���
*********************************************************************************************************/
typedef struct {
    addr_t              GPIOC_ulPhyAddrBase;                            /*  �����ַ����ַ              */
    addr_t              GPIOC_stPhyAddrSize;                            /*  �����ַ�ռ��С            */
    ULONG               CPIOC_ulIrqNum1;                                /*  0~15 ���ŵ��жϺ�           */
    ULONG               CPIOC_ulIrqNum2;                                /*  16~32 ���ŵ��жϺ�          */
    UINT                GPIOC_uiPinNumber;
    addr_t              GPIOC_ulVirtAddrBase;                           /*  �����ַ����ַ              */
} IMX6Q_GPIO_CONTROLER, *PIMX6Q_GPIO_CONTROLER;
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
static IMX6Q_GPIO_CONTROLER  _G_imx6qGpioControlers[] = {
        {
        		GPIO1_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO1_INT15_0,
				IMX_INT_GPIO1_INT31_16,
                32,
                GPIO1_BASE_ADDR,                                         /*  Ĭ���������ַ             */
        }, {
        		GPIO2_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO2_INT15_0,
				IMX_INT_GPIO2_INT31_16,
                32,
                GPIO2_BASE_ADDR,
        }, {
        		GPIO3_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO3_INT15_0,
				IMX_INT_GPIO3_INT31_16,
                32,
                GPIO3_BASE_ADDR,
        }, {
        		GPIO4_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO4_INT15_0,
				IMX_INT_GPIO4_INT31_16,
                32,
                GPIO4_BASE_ADDR,
        }, {
        		GPIO5_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO5_INT15_0,
				IMX_INT_GPIO5_INT31_16,
                32,
                GPIO5_BASE_ADDR,
        }, {
        		GPIO6_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO6_INT15_0,
				IMX_INT_GPIO6_INT31_16,
                32,
                GPIO6_BASE_ADDR,
        }, {
        		GPIO7_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO7_INT15_0,
				IMX_INT_GPIO7_INT31_16,
                14,
                GPIO7_BASE_ADDR,
        },
};
static LW_GPIO_CHIP  _G_imx6qGpioChip ;
/*********************************************************************************************************
** ��������: imx6qGpioRequest
** ��������: ʵ�� GPIO �ܽŵ� PINMUX ����
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qGpioRequest (PLW_GPIO_CHIP  pGpioChip, UINT uiOffset)
{
    INT    iRet;

    iRet = boardGpioCheck(uiOffset);
    if (iRet == PX_ERROR) {
        return  (PX_ERROR);
    }

    if (uiOffset >= _G_imx6qGpioChip.GC_uiNGpios) {
        return (PX_ERROR);
    }

    iRet = GpioPinmuxSet(uiOffset / 32, uiOffset % 32, 0x10 | ALT5);    /*  ǿ������Ϊ����              */

    return (iRet);
}
/*********************************************************************************************************
** ��������: imx6qGpioFree
** ��������: �ͷ�һ�����ڱ�ʹ�õ� GPIO, �����ǰ���ж�ģʽ��, �����ж����빦��.
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  imx6qGpioFree (PLW_GPIO_CHIP  pGpioChip, UINT uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;

    uiRegVal = readl(atBase + GPIO_IMR) &(~(1 << (uiOffset % 32)));
    writel(uiRegVal, atBase + GPIO_IMR);                                /*  ��ֹ�� GPIO �ж�            */

    uiRegVal = readl(atBase + GPIO_GDIR) &(~(1 << (uiOffset % 32)));
    writel(uiRegVal, atBase + GPIO_GDIR);                               /*  ���ø� GPIO Ϊ����          */
}
/*********************************************************************************************************
** ��������: imx6qGpioGetDirection
** ��������: ���ָ�� GPIO ����
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
** ��  ��  : 0: ���� 1:���
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qGpioGetDirection (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;

    uiRegVal = readl(atBase + GPIO_GDIR) & (1 << (uiOffset % 32));
    if(uiRegVal) {
    	return  (1);
    } else {
    	return  (0);
    }
}
/*********************************************************************************************************
** ��������: imx6qGpioDirectionInput
** ��������: ����ָ�� GPIO Ϊ����ģʽ
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
** ��  ��  : 0: ��ȷ -1:����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qGpioDirectionInput (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;
    uiRegVal = readl(atBase + GPIO_GDIR) &(~(1 << (uiOffset % 32)));
    writel(uiRegVal, atBase + GPIO_GDIR);                               /*  ���ø� GPIO Ϊ����          */

    return  (0);
}
/*********************************************************************************************************
** ��������: imx6qGpioGet
** ��������: ���ָ�� GPIO ��ƽ
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
** ��  ��  : 0: �͵�ƽ 1:�ߵ�ƽ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qGpioGet (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;
    uiRegVal = readl(atBase + GPIO_PSR) & (1 << (uiOffset % 32));

    if(uiRegVal) {
    	return  (1);
    } else {
    	return  (0);
    }
}
/*********************************************************************************************************
** ��������: imx6qGpioDirectionOutput
** ��������: ����ָ�� GPIO Ϊ���ģʽ
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
**           iValue      �����ƽ
** ��  ��  : 0: ��ȷ -1:����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qGpioDirectionOutput (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset, INT  iValue)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;

    uiRegVal = readl(atBase + GPIO_GDIR) | (1 << (uiOffset % 32));
    writel(uiRegVal, atBase + GPIO_GDIR);                               /*  ���ø� GPIO Ϊ���          */

    if(iValue) {                                                        /*  ���ø� GPIO Ĭ�ϵ�ƽ        */
        uiRegVal = readl(atBase + GPIO_DR) | (1 << (uiOffset % 32));
        writel(uiRegVal, atBase + GPIO_DR);                             /*  ���ø� GPIO Ĭ�ϵ�ƽΪ 1    */

    } else {
        uiRegVal = readl(atBase + GPIO_DR) &(~(1 << (uiOffset % 32)));
        writel(uiRegVal, atBase + GPIO_DR);                             /*  ���ø� GPIO Ĭ�ϵ�ƽΪ 0    */

    }

    return  (0);
}
/*********************************************************************************************************
** ��������: imx6qGpioSet
** ��������: ����ָ�� GPIO ��ƽ
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
**           iValue      �����ƽ
** ��  ��  : 0: ��ȷ -1:����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  imx6qGpioSet (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset, INT  iValue)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;

    if(iValue) {                                                        /*  ���ø� GPIO ��ƽ            */
        uiRegVal = readl(atBase + GPIO_DR) | (1 << (uiOffset % 32));
        writel(uiRegVal, atBase + GPIO_DR);                             /*  ���ø� GPIO ��ƽΪ 1        */

    } else {
        uiRegVal = readl(atBase + GPIO_DR) &(~(1 << (uiOffset % 32)));
        writel(uiRegVal, atBase + GPIO_DR);                             /*  ���ø� GPIO ��ƽΪ 0        */

    }
}
/*********************************************************************************************************
** ��������: imx6qGpioSetDebounce
** ��������: ����ָ�� GPIO ��ȥ������
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
**           uiDebounce  ȥ������
** ��  ��  : 0: ��ȷ -1:����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qGpioSetDebounce (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset, UINT  uiDebounce)
{
	/*
	 * imx6q �� gpio �������к���û������
	 */
    return  (0);
}
/*********************************************************************************************************
** ��������: imx6qGpioSetupIrq
** ��������: ����ָ�� GPIO Ϊ�ⲿ�ж�����ܽ�
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
**           bIsLevel    �Ƿ�Ϊ��ƽ����
**           uiType      ���Ϊ��ƽ����, 1 ��ʾ�ߵ�ƽ����, 0 ��ʾ�͵�ƽ����
**                       ���Ϊ���ش���, 1 ��ʾ�����ش���, 0 ��ʾ�½��ش���, 2 ˫���ش���
** ��  ��  : IRQ ������ -1:����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ULONG  imx6qGpioSetupIrq (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset, BOOL  bIsLevel, UINT  uiType)
{

    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase ;
    UINT32                 uiRegVal;
    UINT32                 uiPinOff;
    UINT32                 uiIrqNum;

    uiPinOff       = uiOffset % 32;
    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase         = pGpioControler->GPIOC_ulVirtAddrBase;

    if(uiPinOff > 15) {                                                 /*  ������Ҫ���õļĴ���λ      */
    	atBase = atBase + GPIO_ICR2;
    	uiIrqNum  = pGpioControler->CPIOC_ulIrqNum2;
        uiPinOff  = (uiPinOff - 15) << 1;

    } else {
    	atBase = atBase + GPIO_ICR1;
    	uiIrqNum  = pGpioControler->CPIOC_ulIrqNum1;
        uiPinOff  = uiPinOff << 1;
    }

    uiRegVal = readl(pGpioControler->GPIOC_ulVirtAddrBase + GPIO_EDGE_SEL);/*  ���˫�����жϴ�������   */
    writel(uiRegVal &(~(1 << (uiOffset % 32))), pGpioControler->GPIOC_ulVirtAddrBase + GPIO_EDGE_SEL);

    if (bIsLevel) {                                                     /*  ����ǵ�ƽ�����ж�          */
    	if(uiType) {                                                    /*  �ߵ�ƽ����                  */
            uiRegVal = readl(atBase) & (~(0x3 << uiPinOff));
            uiRegVal = uiRegVal | 0x1 << uiPinOff;
            writel(uiRegVal, atBase);

    	} else {                                                        /*  �͵�ƽ����                  */
            uiRegVal = readl(atBase) & (~(0x3 << uiPinOff));
            uiRegVal = uiRegVal | 0x0 << uiPinOff;
            writel(uiRegVal, atBase);

    	}
    } else {
        if (uiType == 1) {                                              /*  �����ش���                  */
            uiRegVal = readl(atBase) & (~(0x3 << uiPinOff));
            uiRegVal = uiRegVal | 0x2 << uiPinOff;
            writel(uiRegVal, atBase);

        } else if (uiType == 0) {                                       /*  �½��ش���                  */
            uiRegVal = readl(atBase) & (~(0x3 << uiPinOff));
            uiRegVal = uiRegVal | 0x3 << uiPinOff;
            writel(uiRegVal, atBase);

        } else {                                                        /*  ˫���ش���                  */
            uiRegVal = readl(pGpioControler->GPIOC_ulVirtAddrBase + GPIO_EDGE_SEL) | (1<<(uiOffset%32));
            writel(uiRegVal, pGpioControler->GPIOC_ulVirtAddrBase + GPIO_EDGE_SEL);
        }
    }

    uiRegVal = readl(pGpioControler->GPIOC_ulVirtAddrBase + GPIO_IMR);  /*  ʹ�ܸ� GPIO �ж�            */
    writel(uiRegVal | (1 << (uiOffset % 32)), pGpioControler->GPIOC_ulVirtAddrBase + GPIO_IMR);
    /*
     * �����ô���
     */
    uiRegVal = readl(pGpioControler->GPIOC_ulVirtAddrBase + GPIO_IMR);

    return  (uiIrqNum);
}
/*********************************************************************************************************
** ��������: imx6qGpioClearIrq
** ��������: ���ָ�� GPIO �жϱ�־
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  imx6qGpioClearIrq (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;
    uiRegVal = readl(atBase + GPIO_ISR) | (1 << (uiOffset % 32));
    writel(uiRegVal, atBase + GPIO_ISR);                                /*  ����� GPIO �ж�״̬        */
}
/*********************************************************************************************************
** ��������: imx6qGpioSvrIrq
** ��������: �ж� GPIO �жϱ�־
** ��  ��  : pGpioChip   GPIO оƬ
**           uiOffset    GPIO ��� BASE ��ƫ����
** ��  ��  : �жϷ���ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static irqreturn_t  imx6qGpioSvrIrq (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

	pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
	atBase   = pGpioControler->GPIOC_ulVirtAddrBase;
    uiRegVal = readl(atBase + GPIO_ISR) & (1 << (uiOffset % 32));
    if (uiRegVal) {
        return  (LW_IRQ_HANDLED);
    } else {
        return  (LW_IRQ_NONE);
    }
}
/*********************************************************************************************************
  GPIO ��������
*********************************************************************************************************/
static LW_GPIO_CHIP  _G_imx6qGpioChip = {
        .GC_pcLabel              = "IMX6Q  GPIO",
        .GC_ulVerMagic           = LW_GPIO_VER_MAGIC,
        .GC_pfuncRequest         = imx6qGpioRequest,
        .GC_pfuncFree            = imx6qGpioFree,

        .GC_pfuncGetDirection    = imx6qGpioGetDirection,
        .GC_pfuncDirectionInput  = imx6qGpioDirectionInput,
        .GC_pfuncGet             = imx6qGpioGet,
        .GC_pfuncDirectionOutput = imx6qGpioDirectionOutput,
        .GC_pfuncSetDebounce     = imx6qGpioSetDebounce,
        .GC_pfuncSetPull         = LW_NULL,
        .GC_pfuncSet             = imx6qGpioSet,
        .GC_pfuncSetupIrq        = imx6qGpioSetupIrq,
        .GC_pfuncClearIrq        = imx6qGpioClearIrq,
        .GC_pfuncSvrIrq          = imx6qGpioSvrIrq,
};
/*********************************************************************************************************
** ��������: am335xGpioDrv
** ��������: ��װ AM335X GPIO ����
** ��  ��  : NONE
** ��  ��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qGpioDrv (VOID)
{
    PIMX6Q_GPIO_CONTROLER    pGpioControler;
    INT                      i;

    _G_imx6qGpioChip.GC_uiBase   = 0;
    _G_imx6qGpioChip.GC_uiNGpios = 0;

    /*
     * GPIO ������ʱ�Ӻ͵�Դʹ�ܣ�����ַӳ�䣬
     */
    for (i = 0; i < GPIO_CONTROLER_NR; i++) {
        pGpioControler = &_G_imx6qGpioControlers[i];
        _G_imx6qGpioChip.GC_uiNGpios += pGpioControler->GPIOC_uiPinNumber;
    }

    return  (API_GpioChipAdd(&_G_imx6qGpioChip));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
