/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: imx6q_phy.c
**
** 创   建   人: Zhang.Xu (张旭)
**
** 文件创建日期: 2016 年 02 月 23 日
**
** 描        述: imx6q 处理器 phy 芯片初始化代码。在phy 初始化函数中，系统会搜索 0～31 地址
**               范围的phy芯片，并将读取的id与已有的phy驱动匹配，若不相同则报错。
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "netif/imx6q_phy.h"
/*********************************************************************************************************
  PHY 操作宏定义
*********************************************************************************************************/
#define PHY_MAX_ADDR            (32)
#define PHY_IDENTIFY_1          (0x02)
#define PHY_IDENTIFY_2          (0x03)
#define PHY_ID1_SHIFT           (16)
#define PHY_ID1_MASK            (0xFFFF)
#define PHY_ID2_SHIFT           (0)
#define PHY_ID2_MASK            (0xFFFF)

#define PHY_AR8031_ID           (0x004DD074)
#define PHY_AR8031_MASK         (0xFFFFFFFF)
#define PHY_AR8035_ID           (0x004DD072)
#define PHY_AR8035_MASK         (0xFFFFFFFF)

#define PHY_KSZ9021RN_ID        (0x00221610)
#define PHY_KSZ9031RN_ID        (0x00221620)
#define PHY_RTL8211E_ID         (0x001CC915)

#define PHY_CTRL_REG            (0x00)
#define PHY_CTRL_RESET          (0x8000)
#define PHY_AUTONEG_RESTART     (0x0200u)

#define PHY_LINK_PARTNER_ABLTY  (0x05)
#define PHY_1000BT_CONTROL      (0x09)
#define PHY_1000BT_STATUS       (0x0A)
#define PHY_STATUS_REG          (0x01)
#define PHY_STATUS_LINK_ST      (0x0004)
#define ENET_STATUS_LINK_ON     (0x80000000)
#define ENET_STATUS_FULL_DPLX   (0x40000000)
#define ENET_STATUS_AUTO_NEG    (0x20000000)
#define ENET_STATUS_10M         (0x8000000)
#define ENET_STATUS_100M        (0x10000000)
#define ENET_STATUS_1000M       (0x20000000)

#define PHY_LINK_1000BT_FD      (0x0800u)
#define PHY_LINK_1000BT_HD      (0x0400u)
#define PHY_LINK_100BT_FD       (0x0100u)
#define PHY_LINK_100BT_HD       (0x0080u)
#define PHY_LINK_10BT_FD        (0x0040u)
#define PHY_LINK_10BT_HD        (0x0020u)
#define PHY_AUTONEG_ENABLE      (0x1000u)
#define PHY_SPEED_MASK          (0xDFBF)
#define PHY_SPEED_10MBPS        (0x0000u)
#define PHY_SPEED_100MBPS       (0x2000u)
#define PHY_SPEED_1000MBPS      (0x0040)
#define PHY_ADV_VAL_MASK        (0x01e0)
#define PHY_GIG_ADV_VAL_MASK    (0x0300)
#define PHY_1000BT_FD           (0x0200u)
#define PHY_AUTONEG_RESTART     (0x0200u)
#define PHY_100BTX              (0x0080u)
#define PHY_100BTX_FD           (0x0100u)
#define PHY_10BT                (0x0020u)
#define PHY_10BT_FD             (0x0040u)
#define BMCR_PDOWN              (0x0800)

/*********************************************************************************************************
  函数声明
*********************************************************************************************************/
VOID  AR8031PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);
VOID  KSZ9021PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);
VOID  RTL8211EPhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);
VOID  KSZ9031PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);
/*********************************************************************************************************
** 函数名称:  imx6qPhyInit
** 功能描述:  初始化 imx6q 处理器的PHY芯片，该函数会自动查找 PHY 芯片地址并进行ID匹配后进行相应初始化。
** 输    入:  pPhyRead   mii 总线写入函数
**            pPhyWrite  mii 总线读取函数
** 输    出:
** 返    回: ERROR CODE
*********************************************************************************************************/
VOID  imx6qPhyInit (FUNCPTR pPhyRead, FUNCPTR pPhyWrite, UINT16 *uipPhyAddr)
{
	UINT32  uiID;
	UINT16  uiAddr, usValue   = 0;

	for(uiAddr = 0; uiAddr < PHY_MAX_ADDR; uiAddr++) {

	    pPhyRead(uiAddr, PHY_IDENTIFY_1, &usValue);
	    uiID = (usValue & PHY_ID1_MASK) << PHY_ID1_SHIFT;
	    pPhyRead(uiAddr, PHY_IDENTIFY_2, &usValue);
	    uiID |= (usValue & PHY_ID2_MASK) << PHY_ID2_SHIFT;
	    uiID &= 0xFFFFFFFF;
	    if((uiID != 0x0) & (uiID != 0xFFFFFFFF)) {
	    	break;
	    }
	}
	/*
	 * 由于 对KSZ90921 phy 进行特殊处理
	 */
	if((uiID & 0xFFFFFFF0) == PHY_KSZ9021RN_ID) {
		uiID = PHY_KSZ9021RN_ID;
	}

	if((uiID & 0xFFFFFFF0) == PHY_KSZ9031RN_ID) {
		uiID = PHY_KSZ9031RN_ID;
	}
	printk("PHY%d:%08x\n", uiAddr, uiID);
	*uipPhyAddr = uiAddr;

    switch (uiID) {
    case PHY_RTL8211E_ID:
    	RTL8211EPhyInit(uiAddr, pPhyRead, pPhyWrite);
        break;
    case PHY_AR8031_ID:
    	AR8031PhyInit(uiAddr, pPhyRead, pPhyWrite);
        break;
    case PHY_AR8035_ID:
        AR8031PhyInit(uiAddr, pPhyRead, pPhyWrite);
        break;
    case PHY_KSZ9021RN_ID:
    	KSZ9021PhyInit(uiAddr, pPhyRead, pPhyWrite);
        break;
    case PHY_KSZ9031RN_ID:
    	KSZ9031PhyInit(uiAddr, pPhyRead, pPhyWrite);
    	break;
    default:
        printk("[Warning] ENET not connect right PHY: ID=%lx\n", uiID);

    }
}
/*********************************************************************************************************
** 函数名称:  imx6qGetPhyLinkStatus
** 功能描述:  初始化 imx6q 处理器的PHY芯片，该函数会自动查找 PHY 芯片地址并进行ID匹配后进行相应初始化。
** 输    入:  usPhyAddr  phy 芯片的地址
**            pPhyRead   mii 总线写入函数
**            pPhyWrite  mii 总线读取函数
** 输    出:
** 返    回:  网络连接状态，对百兆PHY来讲使用0x01寄存器即可，100、10是否全双工均可，千兆PHY下的百兆
**            状态也可以在0x05寄存器中获取，而千兆网状态则需要在0xA或特有的寄存器中获取，一般使用0x05寄存
**            器和0xA两个寄存器实现对PHY芯片的链接状态读取。
*********************************************************************************************************/
UINT32 imx6qGetPhyLinkStatus (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
    UINT32  uiStatus = 0;
    UINT16  ucLink, ucFeVal, ucGigVal;
    /*
     * 通过 0x1 寄存器获取 phy 芯片的网络连接状态
     */
    pPhyRead(usPhyAddr, PHY_STATUS_REG, &ucLink);
    if (ucLink & PHY_STATUS_LINK_ST) {
        uiStatus |= ENET_STATUS_LINK_ON;

         /*
          * 读取千兆网的连接状态
          */
         pPhyRead(usPhyAddr, PHY_1000BT_STATUS, &ucGigVal);
         if (ucGigVal & PHY_LINK_1000BT_FD) {
         	 uiStatus |= ENET_STATUS_FULL_DPLX;
       	     uiStatus |= ENET_STATUS_1000M;
         } else if(ucGigVal & PHY_LINK_1000BT_HD){
             uiStatus &= ~ENET_STATUS_FULL_DPLX;
       	     uiStatus |= ENET_STATUS_1000M;
         } else {

       	     /*
       	      * 在网络链接状态不是千兆网的情况下，检测百兆网的状态
       	      */
       	     pPhyRead(usPhyAddr, PHY_LINK_PARTNER_ABLTY, &ucFeVal);
             if(ucFeVal & PHY_LINK_100BT_FD) {
             	  uiStatus |= ENET_STATUS_FULL_DPLX;
           	  uiStatus |= ENET_STATUS_100M;
             } else if(ucFeVal & PHY_LINK_100BT_HD) {
         	      uiStatus &= ~ENET_STATUS_FULL_DPLX;
           	  uiStatus |= ENET_STATUS_100M;
             } else if(ucFeVal & PHY_LINK_10BT_FD) {
             	  uiStatus |= ENET_STATUS_FULL_DPLX;
           	  uiStatus |= ENET_STATUS_10M;
             } else if(ucFeVal & PHY_LINK_10BT_FD) {
         	      uiStatus &= ~ENET_STATUS_FULL_DPLX;
           	  uiStatus |= ENET_STATUS_10M;
             }
         }
    } else {
        uiStatus &= ~ENET_STATUS_LINK_ON;
    }
    return (uiStatus);
}
/*********************************************************************************************************
** 函数名称:  AR8031PhyInit
** 功能描述:  初始化 imx6q 处理器的PHY芯片，该函数会自动查找 PHY 芯片地址并进行ID匹配后进行相应初始化。
** 输    入:  usPhyAddr  phy 芯片的地址
**            pPhyRead   mii 总线写入函数
**            pPhyWrite  mii 总线读取函数
** 输    出:
** 返    回: ERROR CODE
*********************************************************************************************************/
VOID AR8031PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
	UINT16  usValue   = 0;
    INT     iRetryCnt = 1000;

    /*
     * 对AR8031phy 芯片进行复位
     */
    pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);                   /*  Reset PHY                  */
	usValue |= PHY_CTRL_RESET;
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);
    /*
     * 等待芯片复位完成
     */
	iRetryCnt = 1000;
	while (--iRetryCnt > 0) {
		pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);
	    if (usValue & PHY_CTRL_RESET) {
		    udelay(2);
	    } else {
	    	break;
	    }
	}
    /*
     * 参考 board-mx6q_sabresd.c 文件的 mx6q_sabresd_fec_phy_init函数，在platform sdk中
     * 也是这样设置，
     */
    pPhyWrite(usPhyAddr, 0xd, 0x3);
    pPhyWrite(usPhyAddr, 0xe, 0x805d);
    pPhyWrite(usPhyAddr, 0xd, 0x4003);
    pPhyRead(usPhyAddr, 0xe, &usValue);
    usValue &= ~(0x1 << 8);
    pPhyWrite(usPhyAddr, 0x0e, usValue);
	/*
	 * To enable AR8031 ouput a 125MHz clk from CLK_25M
	 */
    pPhyWrite(usPhyAddr, 0xd, 0x7);
    pPhyWrite(usPhyAddr, 0xe, 0x8016);
    pPhyWrite(usPhyAddr, 0xd, 0x4007);
    pPhyRead(usPhyAddr, 0xe, &usValue);
    usValue &= 0xffe3;
    usValue |= 0x18;
    pPhyWrite(usPhyAddr, 0x0e, usValue);
	/* Introduce tx clock delay */
    pPhyWrite(usPhyAddr, 0x1d, 0x5);
    pPhyRead(usPhyAddr, 0x1e, &usValue);
    usValue |= 0x0100;
    pPhyWrite(usPhyAddr, 0x1e, usValue);
	/*check phy power*/
    pPhyRead(usPhyAddr, 0x0, &usValue);
    if(usValue & BMCR_PDOWN) {
    	usValue = usValue & ~BMCR_PDOWN;
    	pPhyWrite(usPhyAddr, 0x0, usValue);
    }

    /*
     * Enable Auto Negotiation
     */
    pPhyRead(usPhyAddr, 0x0, &usValue);
    usValue |= PHY_AUTONEG_ENABLE;
    usValue &= PHY_SPEED_MASK;
    usValue |= PHY_SPEED_1000MBPS;
	pPhyWrite(usPhyAddr, 0x0, usValue);

	/*
	 * Write Auto Negotiation capabilities
	 */
    pPhyRead(usPhyAddr, 0x4, &usValue);
    usValue &= ~PHY_ADV_VAL_MASK;
    usValue |= PHY_100BTX | PHY_100BTX_FD | PHY_10BT | PHY_10BT_FD;
    pPhyWrite(usPhyAddr, 0x4, usValue);

    /*
     * Write Auto Negotiation Gigabyte capabilities
     */
    pPhyRead(usPhyAddr, 0x9, &usValue);
    usValue &= ~PHY_GIG_ADV_VAL_MASK;
    usValue |= PHY_1000BT_FD;
    pPhyWrite(usPhyAddr, 0x9, usValue);

    /*
     * Start Auto Negotiation
     */
    pPhyRead(usPhyAddr, 0x0, &usValue);
    usValue |= PHY_AUTONEG_RESTART;
    pPhyWrite(usPhyAddr, 0x0, usValue);

}

/*********************************************************************************************************
** 函数名称:  KSZ9021PhyInit
** 功能描述:  初始化 imx6q 处理器的PHY芯片，该函数会自动查找 PHY 芯片地址并进行ID匹配后进行相应初始化。
** 输    入:  usPhyAddr  phy 芯片的地址
**            pPhyRead   mii 总线写入函数
**            pPhyWrite  mii 总线读取函数
** 输    出:
** 返    回: ERROR CODE
*********************************************************************************************************/
VOID KSZ9021PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
	UINT16  usValue  = 0;

    pPhyWrite(usPhyAddr, 0x9, 0x1c00);

    /*
     * min rx data delay
     */
    pPhyWrite(usPhyAddr, 0x0b, 0x8105);
    pPhyWrite(usPhyAddr, 0x0c, 0x0000);

    /*
     * max rx/tx clock delay, min rx/tx control delay
     */
    pPhyWrite(usPhyAddr, 0x0b, 0x8104);
    pPhyWrite(usPhyAddr, 0x0c, 0xf0f0);
    pPhyWrite(usPhyAddr, 0x0b, 0x104);


    pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);                        /* Reset PHY                    */
	usValue |= PHY_CTRL_RESET;
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);

    /*
     *  等待芯片复位完成
     */
    do {
    	pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);
    } while (usValue & PHY_CTRL_RESET);

    pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);  	                    /* restart auto-negotiation     */
	usValue |= 0x1200;
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);
}
/*********************************************************************************************************
** 函数名称:  RTL8211EPhyInit
** 功能描述:  初始化 imx6q 处理器的PHY芯片，该函数会自动查找 PHY 芯片地址并进行ID匹配后进行相应初始化。
** 输    入:  usPhyAddr  phy 芯片的地址
**            pPhyRead   mii 总线写入函数
**            pPhyWrite  mii 总线读取函数
** 输    出:
** 返    回: ERROR CODE
*********************************************************************************************************/
VOID RTL8211EPhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
	UINT16  usValue   = 0;
    INT     iRetryCnt = 1000;

    pPhyWrite(usPhyAddr, 0x4, 0x01E1);
    pPhyWrite(usPhyAddr, 0x9, 0x0000);                                  /*  根据数据手册重新配置        */

    pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);                        /*  Reset PHY                   */
	usValue |= PHY_CTRL_RESET;
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);

    /*
     *  等待芯片复位完成
     */
	iRetryCnt = 1000;
	while (--iRetryCnt > 0) {
		pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);
	    if (usValue & PHY_CTRL_RESET) {
		    udelay(2);
	    } else {
	    	break;
	    }
	}
    pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);  	                    /*  restart auto-negotiation    */
	usValue |= 0x1300;                                                  /*  自适应交换和全双工          */
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);
}
/*********************************************************************************************************
** 函数名称:  KSZ9031PhyInit
** 功能描述:  KSZ9031 PHY 芯片初始化函数，根据 Linux 代码，此 PHY 不需要参数设置，
** 输    入:  usPhyAddr  phy 芯片的地址
**            pPhyRead   mii 总线写入函数
**            pPhyWrite  mii 总线读取函数
** 输    出:
** 返    回: ERROR CODE
*********************************************************************************************************/
VOID  KSZ9031PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
	UINT16  usValue  = 0;

	pPhyRead(usPhyAddr, GMII_BMCR, &usValue);                           /* Reset PHY                    */
	usValue |= GMII_RESET;
	pPhyWrite(usPhyAddr, GMII_BMCR, usValue);

    /*
     *  等待芯片复位完成
     */
    do {
    	pPhyRead(usPhyAddr, GMII_BMCR, &usValue);
    	udelay(100);
    } while (usValue & GMII_RESET);

    /*
     * restart auto-negotiation
     */
    pPhyRead(usPhyAddr, GMII_BMCR, &usValue);
    usValue |= (GMII_AUTONEG | GMII_RESTART_AUTONEG);
    usValue &= ~(GMII_ISOLATE);
    pPhyWrite(usPhyAddr, GMII_BMCR, usValue);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
