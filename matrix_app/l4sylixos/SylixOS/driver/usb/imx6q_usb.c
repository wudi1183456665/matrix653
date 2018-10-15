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
** 文   件   名: imx6q_usb.c
**
** 创   建   人: zhang.xu (张旭)
**
** 文件创建日期: 2015 年 12 月 13 日
**
** 描        述: I.MX6Q 处理器的 USB 控制器硬件设置
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
  USB  相关宏定义 USB cores for mx6dq/mx6dl & USB phy types
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
** 函数名称: imx6_usb_channel_used
** 功能描述: 向 USB 说明使用的 USB 端口
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块: 返回1 表示可用，0表示不可用
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
** 函数名称: imx6qUsbSymbolInit
** 功能描述: 初始化目标系统符号表环境，目前仅仅是 USB 使用,
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块: 该函数需要在 bspInit.c 文件的 halSymbolInit 函数中调用
*********************************************************************************************************/
VOID  imx6qUsbSymbolInit (VOID)
{
    API_SymbolAdd("imx6_usb_channel_used", (caddr_t)imx6_usb_channel_used, LW_SYMBOL_FLAG_XEN);
}
/*********************************************************************************************************
** 函数名称: imx6qUSBtest
** 功能描述: USB接口初始化函数，USB驱动部分使用独立的内核模块，这些初始化在内核模块加载之前使用
**           USB控制器时钟、电源、PHY使能初始化,本函数内容是将 SDK 中的 usbh_init 函数展开
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
