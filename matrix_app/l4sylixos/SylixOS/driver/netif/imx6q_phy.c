/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: imx6q_phy.c
**
** ��   ��   ��: Zhang.Xu (����)
**
** �ļ���������: 2016 �� 02 �� 23 ��
**
** ��        ��: imx6q ������ phy оƬ��ʼ�����롣��phy ��ʼ�������У�ϵͳ������ 0��31 ��ַ
**               ��Χ��phyоƬ��������ȡ��id�����е�phy����ƥ�䣬������ͬ�򱨴�
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "netif/imx6q_phy.h"
/*********************************************************************************************************
  PHY �����궨��
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
  ��������
*********************************************************************************************************/
VOID  AR8031PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);
VOID  KSZ9021PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);
VOID  RTL8211EPhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);
VOID  KSZ9031PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);
/*********************************************************************************************************
** ��������:  imx6qPhyInit
** ��������:  ��ʼ�� imx6q ��������PHYоƬ���ú������Զ����� PHY оƬ��ַ������IDƥ��������Ӧ��ʼ����
** ��    ��:  pPhyRead   mii ����д�뺯��
**            pPhyWrite  mii ���߶�ȡ����
** ��    ��:
** ��    ��: ERROR CODE
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
	 * ���� ��KSZ90921 phy �������⴦��
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
** ��������:  imx6qGetPhyLinkStatus
** ��������:  ��ʼ�� imx6q ��������PHYоƬ���ú������Զ����� PHY оƬ��ַ������IDƥ��������Ӧ��ʼ����
** ��    ��:  usPhyAddr  phy оƬ�ĵ�ַ
**            pPhyRead   mii ����д�뺯��
**            pPhyWrite  mii ���߶�ȡ����
** ��    ��:
** ��    ��:  ��������״̬���԰���PHY����ʹ��0x01�Ĵ������ɣ�100��10�Ƿ�ȫ˫�����ɣ�ǧ��PHY�µİ���
**            ״̬Ҳ������0x05�Ĵ����л�ȡ����ǧ����״̬����Ҫ��0xA�����еļĴ����л�ȡ��һ��ʹ��0x05�Ĵ�
**            ����0xA�����Ĵ���ʵ�ֶ�PHYоƬ������״̬��ȡ��
*********************************************************************************************************/
UINT32 imx6qGetPhyLinkStatus (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
    UINT32  uiStatus = 0;
    UINT16  ucLink, ucFeVal, ucGigVal;
    /*
     * ͨ�� 0x1 �Ĵ�����ȡ phy оƬ����������״̬
     */
    pPhyRead(usPhyAddr, PHY_STATUS_REG, &ucLink);
    if (ucLink & PHY_STATUS_LINK_ST) {
        uiStatus |= ENET_STATUS_LINK_ON;

         /*
          * ��ȡǧ����������״̬
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
       	      * ����������״̬����ǧ����������£�����������״̬
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
** ��������:  AR8031PhyInit
** ��������:  ��ʼ�� imx6q ��������PHYоƬ���ú������Զ����� PHY оƬ��ַ������IDƥ��������Ӧ��ʼ����
** ��    ��:  usPhyAddr  phy оƬ�ĵ�ַ
**            pPhyRead   mii ����д�뺯��
**            pPhyWrite  mii ���߶�ȡ����
** ��    ��:
** ��    ��: ERROR CODE
*********************************************************************************************************/
VOID AR8031PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
	UINT16  usValue   = 0;
    INT     iRetryCnt = 1000;

    /*
     * ��AR8031phy оƬ���и�λ
     */
    pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);                   /*  Reset PHY                  */
	usValue |= PHY_CTRL_RESET;
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);
    /*
     * �ȴ�оƬ��λ���
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
     * �ο� board-mx6q_sabresd.c �ļ��� mx6q_sabresd_fec_phy_init��������platform sdk��
     * Ҳ���������ã�
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
** ��������:  KSZ9021PhyInit
** ��������:  ��ʼ�� imx6q ��������PHYоƬ���ú������Զ����� PHY оƬ��ַ������IDƥ��������Ӧ��ʼ����
** ��    ��:  usPhyAddr  phy оƬ�ĵ�ַ
**            pPhyRead   mii ����д�뺯��
**            pPhyWrite  mii ���߶�ȡ����
** ��    ��:
** ��    ��: ERROR CODE
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
     *  �ȴ�оƬ��λ���
     */
    do {
    	pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);
    } while (usValue & PHY_CTRL_RESET);

    pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);  	                    /* restart auto-negotiation     */
	usValue |= 0x1200;
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);
}
/*********************************************************************************************************
** ��������:  RTL8211EPhyInit
** ��������:  ��ʼ�� imx6q ��������PHYоƬ���ú������Զ����� PHY оƬ��ַ������IDƥ��������Ӧ��ʼ����
** ��    ��:  usPhyAddr  phy оƬ�ĵ�ַ
**            pPhyRead   mii ����д�뺯��
**            pPhyWrite  mii ���߶�ȡ����
** ��    ��:
** ��    ��: ERROR CODE
*********************************************************************************************************/
VOID RTL8211EPhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
	UINT16  usValue   = 0;
    INT     iRetryCnt = 1000;

    pPhyWrite(usPhyAddr, 0x4, 0x01E1);
    pPhyWrite(usPhyAddr, 0x9, 0x0000);                                  /*  ���������ֲ���������        */

    pPhyRead(usPhyAddr, PHY_CTRL_REG, &usValue);                        /*  Reset PHY                   */
	usValue |= PHY_CTRL_RESET;
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);

    /*
     *  �ȴ�оƬ��λ���
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
	usValue |= 0x1300;                                                  /*  ����Ӧ������ȫ˫��          */
	pPhyWrite(usPhyAddr, PHY_CTRL_REG, usValue);
}
/*********************************************************************************************************
** ��������:  KSZ9031PhyInit
** ��������:  KSZ9031 PHY оƬ��ʼ������������ Linux ���룬�� PHY ����Ҫ�������ã�
** ��    ��:  usPhyAddr  phy оƬ�ĵ�ַ
**            pPhyRead   mii ����д�뺯��
**            pPhyWrite  mii ���߶�ȡ����
** ��    ��:
** ��    ��: ERROR CODE
*********************************************************************************************************/
VOID  KSZ9031PhyInit (UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite)
{
	UINT16  usValue  = 0;

	pPhyRead(usPhyAddr, GMII_BMCR, &usValue);                           /* Reset PHY                    */
	usValue |= GMII_RESET;
	pPhyWrite(usPhyAddr, GMII_BMCR, usValue);

    /*
     *  �ȴ�оƬ��λ���
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
