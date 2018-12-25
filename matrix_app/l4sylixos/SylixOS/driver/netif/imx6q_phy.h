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
** ��   ��   ��: imx6q_phy.h
**
** ��   ��   ��: Zhang.Xu (����)
**
** �ļ���������: 2016 �� 02 �� 23 ��
**
** ��        ��: imx6q ������ phy оƬ��ʼ�����롣��phy ��ʼ�������У�ϵͳ������ 0��31 ��ַ
**               ��Χ��phyоƬ��������ȡ��id�����е�phy����ƥ�䣬������ͬ�򱨴�
*********************************************************************************************************/
#ifndef IMX6Q_PHY_H_
#define IMX6Q_PHY_H_

/*********************************************************************************************************
   ǧ����̫�� PHY оƬ�� IEEE ��׼�Ĵ���
*********************************************************************************************************/
#define GMII_BMCR        0                                              /* Basic Mode Control Register  */
#define GMII_BMSR        1                                              /* Basic Mode Status Register   */
#define GMII_PHYID1      2                                              /* PHY Idendifier Register 1    */
#define GMII_PHYID2      3                                              /* PHY Idendifier Register 2    */
#define GMII_ANAR        4                                              /* Auto_Neg Advertisement       */
#define GMII_ANLPAR      5                                              /* Auto_neg Link Partner Ability*/
#define GMII_ANER        6                                              /* Auto-neg Expansion Register  */
#define GMII_ANNPR       7                                              /* Auto-neg Next Page Register  */
#define GMII_ANLPNPAR    8                                              /* Auto_neg Link NextPageAbility*/
#define GMII_1000BTCR    9                                              /* 1000Base-T Control           */
#define GMII_1000BTSR   10                                              /* 1000Base-T Status            */
#define GMII_ERCR       11                                              /* Control Register             */
#define GMII_ERDWR      12                                              /* Data Write Register          */
#define GMII_ERDRR      13                                              /* Data Read Register           */
#define GMII_REV        14                                              /*                              */
#define GMII_EMSR       15                                              /* Extend MII Status Register   */
/*********************************************************************************************************
   ǧ����̫�� PHY оƬ�� �����Զ���Ĵ������ⲿ�ּĴ������ڸ��� PHY оƬ��ʼ��������ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
   PHY �Ĵ���λ����
*********************************************************************************************************/
#define GMII_RESET                         (1 << 15)                    /*  GMII_BMCR                   */
#define GMII_LOOPBACK                      (1 << 14)
#define GMII_SPEED_SELECT_LSB              (1 << 13)
#define GMII_AUTONEG                       (1 << 12)
#define GMII_POWER_DOWN                    (1 << 11)
#define GMII_ISOLATE                       (1 << 10)
#define GMII_RESTART_AUTONEG               (1 << 9)
#define GMII_DUPLEX_MODE                   (1 << 8)
#define GMII_SPEED_SELECT_MSB              (1 << 6)

#define GMII_100BASE_T4                    (1 << 15)                    /*  GMII_BMSR                   */
#define GMII_100BASE_TX_FD                 (1 << 14)
#define GMII_100BASE_T4_HD                 (1 << 13)
#define GMII_10BASE_T_FD                   (1 << 12)
#define GMII_10BASE_T_HD                   (1 << 11)
#define GMII_EXTEND_STATUS                 (1 << 8)
#define GMII_MF_PREAMB_SUPPR               (1 << 6)
#define GMII_AUTONEG_COMP                  (1 << 5)
#define GMII_REMOTE_FAULT                  (1 << 4)
#define GMII_AUTONEG_ABILITY               (1 << 3)
#define GMII_LINK_STATUS                   (1 << 2)
#define GMII_JABBER_DETECT                 (1 << 1)
#define GMII_EXTEND_CAPAB                  (1 << 0)

#define GMII_NP                            (1 << 15)                    /*  GMII_ANAR GMII_ANLPAR       */
#define GMII_RF                            (1 << 13)
#define GMII_PAUSE_MASK                    (3 << 11)
#define GMII_T4                            (1 << 9)
#define GMII_100TX_FDX                     (1 << 8)
#define GMII_100TX_HDX                     (1 << 7)
#define GMII_10_FDX                        (1 << 6)
#define GMII_10_HDX                        (1 << 5)
#define GMII_AN_IEEE_802_3                 (1 << 0)

#define GMII_PDF                           (1 << 4)                     /*  GMII_ANER                   */
#define GMII_LP_NP_ABLE                    (1 << 3)
#define GMII_NP_ABLE                       (1 << 2)
#define GMII_PAGE_RX                       (1 << 1)
#define GMII_LP_AN_ABLE                    (1 << 0)

#define GMII_1000BaseT_HALF_DUPLEX         (1 << 8)                     /*  GMII_1000BTCR               */
#define GMII_1000BaseT_FULL_DUPLEX         (1 << 9)
#define GMII_MARSTER_SLAVE_ENABLE          (1 << 12)
#define GMII_MARSTER_SLAVE_CONFIG          (1 << 11)
#define GMII_PORT_TYPE                     (1 << 10)

#define GMII_LINKP_1000BaseT_HALF_DUPLEX   (1 << 10)                    /*  GMII_1000BTSR               */
#define GMII_LINKP_1000BaseT_FULL_DUPLEX   (1 << 11)
/*********************************************************************************************************
  PHY оƬ Link ״̬ʹ�ú궨��
*********************************************************************************************************/
#define PHY_STATUS_LINK_ST                  (0x0004)
#define ENET_STATUS_LINK_ON                 (0x80000000)
#define ENET_STATUS_FULL_DPLX               (0x40000000)
#define ENET_STATUS_AUTO_NEG                (0x20000000)
#define ENET_STATUS_10M                     (0x8000000)
#define ENET_STATUS_100M                    (0x10000000)
#define ENET_STATUS_1000M                   (0x20000000)

#define PHY_LINK_1000BT_FD                  (0x0800u)
#define PHY_LINK_1000BT_HD                  (0x0400u)
#define PHY_LINK_100BT_FD                   (0x0100u)
#define PHY_LINK_100BT_HD                   (0x0080u)
#define PHY_LINK_10BT_FD                    (0x0040u)
#define PHY_LINK_10BT_HD                    (0x0020u)
/*********************************************************************************************************
  PHY ��ʼ������
*********************************************************************************************************/
VOID   imx6qPhyInit(FUNCPTR pPhyRead, FUNCPTR pPhyWrite, UINT16 *uipPhyAddr);
UINT32 imx6qGetPhyLinkStatus(UINT16 usPhyAddr, FUNCPTR pPhyRead, FUNCPTR pPhyWrite);

#endif                                                                  /*  IMX6Q_PHY_H_                */
/*********************************************************************************************************
  END
*********************************************************************************************************/
