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
** ��   ��   ��: imx6q_usb.c
**
** ��   ��   ��: zhang.xu (����)
**
** �ļ���������: 2015 �� 12 �� 13 ��
**
** ��        ��: I.MX6Q �������� USB ������Ӳ������
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include <linux/compat.h>
#include "usb/imx6q_usb.h"

#include "sdk.h"
#include "regsccm.h"
#include "regsccmanalog.h"
/*********************************************************************************************************
  USB  ��غ궨�� USB cores for mx6dq/mx6dl & USB phy types
*********************************************************************************************************/
#define OTG                (0)
#define Host1              (1)
#define Host2              (2)
#define Host3              (3)
#define OTG1               (4)                                          /* MX6SL first OTG controller   */
#define OTG2               (5)
#define HOST               (6)

#define Utmi               (0)	                                        /* USB Transceiver Macrocell    */
#define Ulpi 	           (1)                                          /* UTMI+ low pin interface      */
#define Serial 	           (2)                                          /* Serial interface             */
#define Hsic	           (3)                                          /* High-Speed Inter-Chip        */

#define HW_USBPHY1         (1)                                          /* Instance number for USBPHY1  */
#define HW_USBPHY2         (2)                                          /* Instance number for USBPHY2  */
#define REGS_USBPHY1_BASE  (0x020c9000)                                 /* BaseAddr for HW_USBPHY1      */
#define REGS_USBPHY2_BASE  (0x020ca000)                                 /* BaseAddr for HW_USBPHY2      */
/*********************************************************************************************************
** ��������: imx6_usb_channel_used
** ��������: �� USB ˵��ʹ�õ� USB �˿�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: ����1 ��ʾ���ã�0��ʾ������
*********************************************************************************************************/
INT imx6_usb_channel_used (INT channel)
{
    switch(channel) {
    case 0:
         return (0);
    case 1:
         return (1);
    case 2:
         return (0);
    case 3:
         return (0);
    default:
         return (0);
    }
}
/*********************************************************************************************************
** ��������: imx6qUsbSymbolInit
** ��������: ��ʼ��Ŀ��ϵͳ���ű�����Ŀǰ������ USB ʹ��,
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: �ú�����Ҫ�� bspInit.c �ļ��� halSymbolInit �����е���
*********************************************************************************************************/
VOID  imx6qUsbSymbolInit (VOID)
{
    API_SymbolAdd("imx6_usb_channel_used", (caddr_t)imx6_usb_channel_used, LW_SYMBOL_FLAG_XEN);
}
/*********************************************************************************************************
** ��������: imx6qUSBtest
** ��������: USB�ӿڳ�ʼ��������USB��������ʹ�ö������ں�ģ�飬��Щ��ʼ�����ں�ģ�����֮ǰʹ��
**           USB������ʱ�ӡ���Դ��PHYʹ�ܳ�ʼ��,�����������ǽ� SDK �е� usbh_init ����չ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
UINT32  imx6qUsbPhyInit (VOID)
{
	UINT32 uiRegVal;

    /*!
     * Enable the USB clock for the controller
     */
    uiRegVal = readl(HW_CCM_CCGR6_ADDR) | 0x3;
	writel(uiRegVal, HW_CCM_CCGR6_ADDR);

    /*!
     * Enable the PLL: OTG, Host2 and Host3 controllers use USB_PLL0; Host1 controller uses USB_PLL1.
     */
    switch (Host1) {
    case OTG:
    case Host2:
    case Host3:
    case OTG1:
	    writel(0x00001000, HW_CCM_ANALOG_PLL_USB1_SET_ADDR);            /* Turn PLL power on            */
	    writel(0x00000040, HW_CCM_ANALOG_PLL_USB1_SET_ADDR);            /* Power 9-phase PLL for USBPHY0*/
        while(!(readl(HW_CCM_ANALOG_PLL_USB1_ADDR) & 0x80000000));      /* Wait for PLL to lock         */
	    writel(0x00010000, HW_CCM_ANALOG_PLL_USB1_CLR_ADDR);            /* Clear bypass                 */
	    writel(0x00002000, HW_CCM_ANALOG_PLL_USB1_SET_ADDR);            /* Enable PLL clock for the PHY */
        break;

    case Host1:
    case OTG2:
	    writel(0x00001000, HW_CCM_ANALOG_PLL_USB2_SET_ADDR);            /* Turn PLL power on            */
	    writel(0x00000040, HW_CCM_ANALOG_PLL_USB2_SET_ADDR);            /* Power 9-phase PLL for USBPHY0*/
        while(!(readl(HW_CCM_ANALOG_PLL_USB2_ADDR) & 0x80000000));      /* Wait for PLL to lock         */
	    writel(0x00010000, HW_CCM_ANALOG_PLL_USB2_CLR_ADDR);            /* Clear bypass                 */
	    writel(0x00002000, HW_CCM_ANALOG_PLL_USB2_SET_ADDR);            /* Enable PLL clock for the PHY */
        break;

    default:
        return (PX_ERROR);
    }

    /*
     *  Enable and initialize transceiver interface for controller
     */
	writel(0x80000000, REGS_USBPHY2_BASE + 0x38);                       /* clear SFTRST                 */
	writel(0x40000000, REGS_USBPHY2_BASE + 0x38);                       /* clear CLKGATE                */
	writel(0x00000000, REGS_USBPHY2_BASE + 0x00);                       /* clear all power down bits    */
	uiRegVal = (0x00004000) | (0x00008000) | (0x00000002);
	writel(uiRegVal, REGS_USBPHY2_BASE + 0x34);                         /* clear SFTRST                 */

    uiRegVal = readl(0x020c81b0) | 1 << 20;                             /* disable the charger detector */
 	writel(uiRegVal, 0x020c81b0);

 	/*
     * Check if all power down bits are clear HW_USBPHY_PWD
     */
    if (readl(REGS_USBPHY2_BASE + 0x00) != 0) {
        return (PX_ERROR);
    } else {
        return (ERROR_NONE);
    }

    uiRegVal = readl(0x02184000 + 0x384) & (~(0xc0000000 | 0x02000000));/*  HW_USBC_PORTSC1             */
    writel(uiRegVal, (0x02184000 + 0x384));

    uiRegVal =  readl(0x02184000 + 0x340) | 0x00000002;                 /* Rst ctler after switch PHY   */
    writel(uiRegVal, (0x02184000 + 0x340));

    while (readl(0x02184000 + 0x340) & 0x00000002);                     /* wait for reset to complete   */

    writel(0x3, (0x02184000 + 0x3a8));                                  /* set controller to host mode  */

    uiRegVal =  ((3 << 8) & 0x00000300) | ((0 << 16) & 0x00ff0000);     /* Set Asynchronous & irq       */
    writel(uiRegVal, (0x02184000 + 0x340));

    uiRegVal =  readl(0x02184000 + 0x380) | 0x00000001;                 /* start the controller         */
    writel(uiRegVal, (0x02184000 + 0x380));

    uiRegVal =  readl(0x02184000 + 0x384) | 0x00001000;                 /* Enable port power            */
    writel(uiRegVal, (0x02184000 + 0x384));

    return (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
