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
** ��   ��   ��: imx6q_netif.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2015 �� 01 �� 21 ��
**
** ��        ��: imx6q ����������̫������
**
** 2016.11.09  ����netdev�������
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "sdk.h"

#include "netdev.h"
#include "netif/imx6q_phy.h"
/*********************************************************************************************************
  ENET-MAC Programmable Registers (MCIMX28RM.pdf-->26.4) (����)
*********************************************************************************************************/
#define HW_ENET_MAC_EIR         (0x004)                                 /*  Interrupt Event Register    */
#define HW_ENET_MAC_EIMR        (0x008)                                 /*  Interrupt Mask Register     */
#define HW_ENET_MAC_RDAR        (0x010)                                 /*  Receive Descriptor Active   */
                                                                        /*  Register                    */
#define HW_ENET_MAC_TDAR        (0x014)                                 /*  Transmit Descriptor Active  */
                                                                        /*  Register                    */
#define HW_ENET_MAC_ECR         (0x024)                                 /*  Control Register            */
#define HW_ENET_MAC_MMFR        (0x040)                                 /*  MII Management Frame        */
                                                                        /*  Register                    */
#define HW_ENET_MAC_MSCR        (0x044)                                 /*  MII Speed Control Register  */
#define HW_ENET_MAC_MIBC        (0x064)                                 /*  MIB Control/Status Register */
#define HW_ENET_MAC_RCR         (0x084)                                 /*  Receive Control Register    */
#define HW_ENET_MAC_TCR         (0x0C4)                                 /*  Transmit Control reg        */
#define HW_ENET_MAC_PALR        (0x0E4)                                 /*  Low 32bits MAC address      */
#define HW_ENET_MAC_PAUR        (0x0E8)                                 /*  High 16bits MAC address     */
#define HW_ENET_MAC_OPD         (0x0EC)                                 /*  Opcode + Pause duration     */
#define HW_ENET_MAC_IAUR        (0x118)                                 /*  High 32bits hash table      */
#define HW_ENET_MAC_IALR        (0x11C)                                 /*  Low 32bits hash table       */
#define HW_ENET_MAC_GAUR        (0x120)                                 /*  High 32bits hash table      */
#define HW_ENET_MAC_GALR        (0x124)                                 /*  Low 32bits hash table       */
#define HW_ENET_MAC_TFW_SFCR    (0x144)                                 /*  FIFO transmit water mark    */
#define HW_ENET_MAC_ERDSR       (0x180)                                 /*  Receive descriptor ring     */
#define HW_ENET_MAC_ETDSR       (0x184)                                 /*  Transmit descriptor ring    */
#define HW_ENET_MAC_EMRBR       (0x188)                                 /*  Maximum receive buff size   */
/*
 *  �������ݴ�Ϊ imx6 ���мĴ���  ���� fifo ��ؼĴ���
 */
#define FEC_R_FIFO_RSFL		    (0x190)                                 /*  section full threshold      */
#define FEC_R_FIFO_RSEM		    (0x194)                                 /*  section empty threshold     */
#define FEC_R_FIFO_RAEM		    (0x198)                                 /*  almost empty threshold      */
#define FEC_R_FIFO_RAFL		    (0x19c)                                 /*  almost full threshold       */

#define FEC_R_FIFO_TSEM		    (0x1A0)                                 /*  section empty threshold     */
#define FEC_R_FIFO_TAEM		    (0x1A4)                                 /*  almost empty threshold      */
#define FEC_R_FIFO_TAFL		    (0x1A8)                                 /*  almost full threshold       */
#define FEC_R_FIFO_TIPG		    (0x1AC)                                 /*  section empty threshold     */
/*
 *  �������ݴ�Ϊ imx6 ���мĴ���λ����
 */
#define FEC_ENET_RSEM_V		    (0x84)
#define FEC_ENET_RSFL_V		    (16  )
#define FEC_ENET_RAEM_V		    (0x8 )
#define FEC_ENET_RAFL_V		    (0x8 )
#define FEC_ENET_OPD_V		    (0xFFF0)
/*********************************************************************************************************
  �жϱ�־�Ĵ��� EIR ��־λ����
*********************************************************************************************************/
#define ENET_EIR_HBERR          ((UINT)0x80000000)                      /*  Heartbeat error             */
#define ENET_EIR_BABR           ((UINT)0x40000000)                      /*  Babbling receiver           */
#define ENET_EIR_BABT           ((UINT)0x20000000)                      /*  Babbling transmitter        */
#define ENET_EIR_GRA            ((UINT)0x10000000)                      /*  Graceful stop complete      */
#define ENET_EIR_TXF            ((UINT)0x08000000)                      /*  Full frame transmitted      */
#define ENET_EIR_TXB            ((UINT)0x04000000)                      /*  A buffer was transmitted    */
#define ENET_EIR_RXF            ((UINT)0x02000000)                      /*  Full frame received         */
#define ENET_EIR_RXB            ((UINT)0x01000000)                      /*  A buffer was received       */
#define ENET_EIR_MII            ((UINT)0x00800000)                      /*  MII interrupt               */
#define ENET_EIR_EBERR          ((UINT)0x00400000)                      /*  SDMA bus error              */
#define ENET_EIR_TS_AVAIL       ((UINT)0x00010000)
#define ENET_EIR_TS_TIMER       ((UINT)0x00008000)
#define ENET_EIR_UN             ((UINT)0x00080000)                      /*  Transmit FIFO Underrun      */
#define ENET_DEFAULT_INTE       (ENET_EIR_TXF | ENET_EIR_RXF)           /*  Ĭ���ж�״̬                */

#define ENET_EVENT_LINK         (0x00000001)
/*********************************************************************************************************
  ENET MAC Control Register  (MCIMX28RM.pdf--> 26.4.5)
*********************************************************************************************************/
#define ENET_ECR_RESET          (0x00000001)                            /*  ENET-MAC reset              */
#define ENET_ECR_ETHER_EN       (0x00000002)                            /*  MAC is enabled              */
#define ENET_ECR_MAGIC_ENA      (0x00000004)                            /*  Enable Magic Packet Detec   */
#define ENET_ECR_SLEEP          (0x00000008)                            /*  controller in sleep mode    */
#define ENET_ECR_ENA_1588       (0x00000010)                            /*  IEEE1588 Enable             */
#define ENET_ECR_DBG_EN         (0X00000040)                            /*  Enable the debug input pin  */
/*********************************************************************************************************
  ENET MAC Physical Address Upper Register (MCIMX28RM.pdf--> 26.4.12)
*********************************************************************************************************/
#define ENET_PAUR_PAUSE_F_T     (0x8808)                                /*  Type field in PAUSE frames  */
/*********************************************************************************************************
  ENET MAC Receive Descriptor Active Register (MCIMX28RM.pdf--> 26.4.3)
*********************************************************************************************************/
#define ENET_RDAR_RX_ACTIVE     (0x01000000)
/*********************************************************************************************************
  ENET MAC Transmit Descriptor Active Register (MCIMX28RM.pdf--> 26.4.4)
*********************************************************************************************************/
#define ENET_TDAR_TX_ACTIVE     (0x01000000)
/*********************************************************************************************************
  Buffer descriptor control/status  (Receive Buffer Descriptor)
*********************************************************************************************************/
#define ENET_BD_RX_EMPTY        ((UINT16)0x8000)                        /*  Recieve is empty            */
#define ENET_BD_RX_RO1          ((UINT16)0x4000)                        /*  Receive software ownership  */
#define ENET_BD_RX_WRAP         ((UINT16)0x2000)                        /*  Wrap. Written by user       */
#define ENET_BD_RX_INTR         ((UINT16)0x1000)                        /*  Receive software ownership  */
#define ENET_BD_RX_RO2           ENET_BD_RX_INTR                        /*  Receive software ownership  */
#define ENET_BD_RX_LAST         ((UINT16)0x0800)                        /*  Last in frame               */
#define ENET_BD_RX_MISS         ((UINT16)0x0100)                        /*  Miss. Written by the uDMA   */
#define ENET_BD_RX_BC           ((UINT16)0x0080)                        /*  Set if the DA is broadcast  */
#define ENET_BD_RX_MC           ((UINT16)0x0040)                        /*  Set if the DA is multicast  */
                                                                        /*  and not BC                  */
#define ENET_BD_RX_LG           ((UINT16)0x0020)                        /*  Rx frame length violation   */
#define ENET_BD_RX_NO           ((UINT16)0x0010)                        /*  Receive non-octet aligned   */
                                                                        /*  frame                       */
#define ENET_BD_RX_CR           ((UINT16)0x0004)                        /*��Rx CRC or Rx Frame error��  */
#define ENET_BD_RX_OV           ((UINT16)0x0002)                        /*  Overrun                     */
#define ENET_BD_RX_TR           ((UINT16)0x0001)                        /*  Will be set if the receive  */
                                                                        /*  frame is truncated          */
#define ENET_BD_RX_STATS        ((UINT16)0x013f)
/*********************************************************************************************************
  Buffer descriptor control/status  (Receive Buffer Descriptor)
*********************************************************************************************************/
#define ENET_BD_TX_READY        ((UINT16)0x8000)                        /*  Transmit is ready           */
#define ENET_BD_TX_TO1          ((UINT16)0x4000)                        /*  Transmit software ownership */
#define ENET_BD_TX_WRAP         ((UINT16)0x2000)                        /*  Wrap. Written by user       */
#define ENET_BD_TX_TO2          ((UINT16)0x1000)                        /*  Transmit software ownership */
#define ENET_BD_TX_LAST         ((UINT16)0x0800)                        /*  Last in frame               */
#define ENET_BD_TX_TC           ((UINT16)0x0400)                        /*  Tx CRC                      */
#define ENET_BD_TX_ABC          ((UINT16)0x0200)                        /*  Append bad CRC              */
#define ENET_BD_TX_STATS        ((UINT16)0x03ff)
/*********************************************************************************************************
  Bit definitions and macros for FEC_RCR
*********************************************************************************************************/
#define FEC_RCR_MAX_FL(x)       (((x)&0x7FF)<<16)
#define FEC_RCR_LOOP            (0x00000001)                            /*  Internal Loopback           */
#define FEC_RCR_DRT             (0x00000002)                            /*  Disable Receive On Transmit */
#define FEC_RCR_MII_MODE        (0x00000040)                            /*  MII or RMII mode. MUST be 1 */
#define FEC_RCR_PROM            (0x00000080)                            /*  Promiscuous Mode            */
#define FEC_RCR_BC_REJ          (0x00000100)                            /*  Broadcast Frame Reject      */
#define FEC_RCR_FCE             (0x00000200)                            /*  Flow Control Enable         */
#define FEC_RCR_RGMII_EN        (0x00000400)                            /*  RGMII Mode Enable           */
#define FEC_RCR_RMII_MODE       (0x00001000)                            /*  RMII Mode Enable            */
#define FEC_RCR_RMII_10T        (0x00002000)                            /*  Enables 10-Mbit/s mode      */
#define FEC_RCR_GRS             (0x80000000)                            /*  Graceful Receive Stopped    */
/*********************************************************************************************************
  Bit definitions and macros for FEC_TCR
*********************************************************************************************************/
#define FEC_TCR_FDEN            (0x00000004)
/*
 * ������ӵĺ궨�壬��Щ�궨������� PHY �Ĵ���������ʹ�� MII �������Խ��˲���ɾ��
 */
#define ENET_ETHER_SPEED_1000M   (1<<5)
#define ENET_ETHER_LITTLE_ENDIAN (1<<8)
#define PHY_CTRL_REG             (0x00
#define PHY_CTRL_RESET           (0x8000)
#define PHY_CTRL_AUTO_NEG        (0x1000)
#define PHY_CTRL_FULL_DPLX       (0x0100)
#define PHY_STATUS_REG           (0x01)
#define PHY_STATUS_LINK_ST       (0x0004)
#define ENET_STATUS_LINK_ON      (0x80000000)
#define ENET_STATUS_FULL_DPLX    (0x40000000)
#define ENET_STATUS_AUTO_NEG     (0x20000000)
#define ENET_STATUS_10M          (0x8000000)
#define ENET_STATUS_100M         (0x10000000)
#define ENET_STATUS_1000M        (0x20000000)
#define PHY_ID1_SHIFT            (16)
#define PHY_ID1_MASK             (0xFFFF)
#define PHY_ID2_SHIFT            (0)
#define PHY_ID2_MASK             (0xFFFF)
#define PHY_IDENTIFY_1           (0x02)
#define PHY_IDENTIFY_2           (0x03)
/*********************************************************************************************************
  PHY �Ĵ�����д��������Ҫ�ĺ궨��
*********************************************************************************************************/
#define FEC_MII_ST               (0x40000000)
#define FEC_MII_OP_OFF           (28)
#define FEC_MII_OP_MASK          (0x03)
#define FEC_MII_OP_RD            (0x02)
#define FEC_MII_OP_WR            (0x01)
#define FEC_MII_PA_OFF           (23)
#define FEC_MII_PA_MASK          (0xFF)
#define FEC_MII_RA_OFF           (18)
#define FEC_MII_RA_MASK          (0xFF)
#define FEC_MII_TA               (0x00020000)
#define FEC_MII_DATA_OFF         (0)
#define FEC_MII_DATA_MASK        (0x0000FFFF)

#define FEC_MII_FRAME   (FEC_MII_ST | FEC_MII_TA)
#define FEC_MII_OP(x)   (((x) & FEC_MII_OP_MASK) << FEC_MII_OP_OFF)
#define FEC_MII_PA(pa)  (((pa) & FEC_MII_PA_MASK) << FEC_MII_PA_OFF)
#define FEC_MII_RA(ra)  (((ra) & FEC_MII_RA_MASK) << FEC_MII_RA_OFF)
#define FEC_MII_SET_DATA(v) (((v) & FEC_MII_DATA_MASK) << FEC_MII_DATA_OFF)
#define FEC_MII_GET_DATA(v) (((v) >> FEC_MII_DATA_OFF) & FEC_MII_DATA_MASK)
#define FEC_MII_READ(pa, ra) ((FEC_MII_FRAME | FEC_MII_OP(FEC_MII_OP_RD)) |\
                    FEC_MII_PA(pa) | FEC_MII_RA(ra))
#define FEC_MII_WRITE(pa, ra, v) (FEC_MII_FRAME | FEC_MII_OP(FEC_MII_OP_WR)|\
            FEC_MII_PA(pa) | FEC_MII_RA(ra) | FEC_MII_SET_DATA(v))
/*********************************************************************************************************
  �����������ú궨�壬����Linux���������˸��ģ����ֲῴ�� imx283 ��ͬӦ��Ҳ����
*********************************************************************************************************/
#define ENET_NAME                "PHY"
#define ENET_RING_SZ             (16)
#define ENET_TX_RING_SZ          (128)                                  /*  2 �ı���                    */
#define ENET_RX_RING_SZ          (2 * 192)                              /*  2 �ı���                    */
#define ENET_RING_MASK           (15)
#define ENET_BUF_SZ              (LW_CFG_VMM_PAGE_SIZE >> 1)            /*  ��ҳ                        */
#define ENET_BUFD_LENGTH         (1520)
#define ENET_PKT_SZ              (1518)
#define FEC_ENET_PHYID           (0x00221610)                           /*  PHY_KSZ9021RN_ID            */
#define ENET_PHYADDR             (6)                                    /*  phy addr ��6                */
#define ENET_SYNCQ_SZ            (512)
#define IFNAME0                  'e'
#define IFNAME1                  'n'
#if LWIP_NETIF_HOSTNAME
#define HOSTNAME                 "lwIP"
#endif

#define PHY_STATUS_REG           (0x01)
#define PHY_STATUS_LINK_ST       (0x0004)
#define ENET_STATUS_LINK_ON      (0x80000000)
#define ENET_STATUS_FULL_DPLX    (0x40000000)
#define ENET_STATUS_AUTO_NEG     (0x20000000)
#define ENET_STATUS_10M          (0x8000000)
#define ENET_STATUS_100M         (0x10000000)
#define ENET_STATUS_1000M        (0x20000000)

#define PHY_LINK_1000BT_FD       (0x0800u)
#define PHY_LINK_1000BT_HD       (0x0400u)
#define PHY_LINK_100BT_FD        (0x0100u)
#define PHY_LINK_100BT_HD        (0x0080u)
#define PHY_LINK_10BT_FD         (0x0040u)
#define PHY_LINK_10BT_HD         (0x0020u)
#define PHY_LINK_PARTNER_ABLTY   (0x05)
#define PHY_1000BT_CONTROL       (0x09)
#define PHY_1000BT_STATUS        (0x0A)
/*********************************************************************************************************
  CRC polynomium used by the FEC for the multicast group filtering
*********************************************************************************************************/
#define FEC_CRC_POLY             0x04C11DB7
#define FEC_MAX_MULTICAST_ADDRS  64
/*********************************************************************************************************
  IRQ Number
*********************************************************************************************************/
#define ENET_IRQ_NUM             150
/*********************************************************************************************************
  ENET ������ FIFO/DMA �����������ṹ��
*********************************************************************************************************/
typedef struct bufferDesc {
    UINT16              BUFD_usDataLen;                                 /*  ���������������ݳ���        */
    UINT16              BUFD_usStatus;                                  /*  ����������״̬              */
    ULONG               BUFD_uiBufAddr;                                 /*  ��������ַ                  */
} BUFD;
/*********************************************************************************************************
  ENET ˽�нṹ
*********************************************************************************************************/
typedef struct enet {
    addr_t              ENET_atIobase;                                  /*  ENET ����ַ���������������� */
    UINT32              ENET_uiPhyspeed;                                /*  PHY �ٶ�                    */
    UINT16              ENET_ucPhyAddr;
    UINT16              ENET_ucRsv;
    INT                 ENET_iFull;                                     /*  ���Ͷ�����                  */
    BUFD               *ENET_pbufdRxbdBase;                             /*  ���ջ���������ָ��          */
    BUFD               *ENET_pbufdTxbdBase;                             /*  ���ͻ���������ָ��          */
    BUFD               *ENET_pbufdCurRxbd;                              /*  ��ǰ����������              */
    BUFD               *ENET_pbufdCurTxbd;                              /*  ��ǰ����������              */
    BUFD               *ENET_pbufdTxDirty;                              /*  �Ѿ�������ɵ�������        */
    PUCHAR              ENET_pcTxBuf;                                   /*  ���ͻ�������ַ              */
    PUCHAR              ENET_pcRxBuf;                                   /*  ���ջ�������ַ              */
    UINT32              ENET_uiMcastHi;                                 /*  �鲥���ø�32λ              */
    UINT32              ENET_uiMcastLo;                                 /*  �鲥���õ�32λ              */

    LW_SPINLOCK_DEFINE  (ENET_slLock);
    LW_OBJECT_HANDLE    ENET_hSem;

} ENET;
/*********************************************************************************************************
  ȫ�ֱ�������
*********************************************************************************************************/
static  ENET              _G_enetInfo  = {
        .ENET_atIobase  = ENET_BASE_ADDR,                               /*  ENET-MAC �ĵ�ַ             */
        .ENET_iFull     = 0,
        .ENET_uiMcastHi = 0,
        .ENET_uiMcastLo = 0,
};
UINT       uiNetLinkStatus = 0;                                           /*  ��¼���� phy оƬ������״̬ */
/*********************************************************************************************************
** ��������:  imxPhyRead,
** ��������:  PHY �Ĵ���ֵ��ȡ����
** ��    ��:  phyAddr  phyоƬ��ַ
**            regAddr  phyоƬ�Ĵ�����ַ
**           *value   ��ȡ�ļĴ�����ֵ
** ��    ��: NONE
** ��    ��: ERROR CODE
*********************************************************************************************************/
INT imxPhyRead(UCHAR phyAddr, UCHAR regAddr, UINT16 *value)
{
    INT  rValue;
    INT  iRetryCnt = 10000;

    /*
     *  һ��cpu�ж��macʱ��mdio��������ֻ��һ�����������ʹ�ù̶���ַ����
     */
    rValue = readl(ENET_BASE_ADDR + HW_ENET_MAC_EIR);
    if (rValue & ENET_EIR_MII ) {
    	writel(rValue, ENET_BASE_ADDR + HW_ENET_MAC_EIR);
    }

    writel(FEC_MII_READ(phyAddr, regAddr), ENET_BASE_ADDR + HW_ENET_MAC_MMFR);

    while (--iRetryCnt > 0) {
        rValue = readl(ENET_BASE_ADDR + HW_ENET_MAC_EIR);               /* ������жϱ�־��             */
        if (rValue & ENET_EIR_MII ) {
        	writel(rValue, ENET_BASE_ADDR + HW_ENET_MAC_EIR);           /* ����жϱ�־                 */
            rValue = readl(ENET_BASE_ADDR + HW_ENET_MAC_MMFR);          /* ��ȡ PHY �Ĵ�����ֵ          */
            *value = FEC_MII_GET_DATA(rValue);
            return  (ERROR_NONE);
        }
        udelay(2);                                                      /* ��ʱ2us                      */
    }
    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������:  imxPhyWrite
** ��������:  PHY �Ĵ���ֵдȡ����
** ��    ��:  phyAddr  phyоƬ��ַ
**            regAddr  phyоƬ�Ĵ�����ַ
**            value    phyоƬ�Ĵ���д������
** ��    ��:
** ��    ��: ERROR CODE
*********************************************************************************************************/
INT imxPhyWrite(UCHAR phyAddr, UCHAR regAddr, UINT16 value)
{
    INT  rValue;
    INT  iRetryCnt  = 10000;

    rValue = readl(ENET_BASE_ADDR + HW_ENET_MAC_EIR);
    if (rValue & ENET_EIR_MII ) {
    	writel(rValue, ENET_BASE_ADDR + HW_ENET_MAC_EIR);
    }

    writel(FEC_MII_WRITE(phyAddr, regAddr, value), ENET_BASE_ADDR + HW_ENET_MAC_MMFR);

    while (--iRetryCnt > 0) {
        rValue = readl(ENET_BASE_ADDR + HW_ENET_MAC_EIR);
        if (rValue & ENET_EIR_MII ) {
        	writel(rValue, ENET_BASE_ADDR + HW_ENET_MAC_EIR);
            return  (ERROR_NONE);
        }
        udelay(2);
    }
    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: ksz9021rnPhyInit
** ��������: ��ʼ�� PHY оƬ KSZ9021RN
** ��    ��:
** ��    ��:
** ��    ��: ERROR CODE
*********************************************************************************************************/
VOID ksz9021rnPhyInit(struct netdev  *pNetDev)
{
    ENET    *pEnet;
    UINT16   uiPhyAddr;

    pEnet = pNetDev->priv;

    imx6qPhyInit((FUNCPTR)imxPhyRead, (FUNCPTR)imxPhyWrite, &uiPhyAddr);
    pEnet->ENET_ucPhyAddr = uiPhyAddr;
}
/*********************************************************************************************************
** ��������: enetThread
** ��������: ��̫��link״̬����߳�
** �䡡��  : pvArg  :  �̲߳���
** �䡡��  : NULL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static PVOID  enetThread (PVOID  pvArg)
{
    struct netdev  *pNetDev = (struct netdev *)pvArg;
    ENET           *pEnet;
    addr_t          atBase;
    UINT16          usValue, usPhyAddr;
    UINT16          ucFeVal, ucGigVal;

    UINT32          uiStatus = 0;

    if (!pNetDev) {
        return  (LW_NULL);
    }

    pEnet = pNetDev->priv;
    if (!pEnet) {
        return  (LW_NULL);
    }
    atBase    = pEnet->ENET_atIobase;
    usPhyAddr = pEnet->ENET_ucPhyAddr;

    while(1) {
       imxPhyRead(usPhyAddr, PHY_STATUS_REG, &usValue);
       if (usValue & PHY_STATUS_LINK_ST)  {
          if(uiNetLinkStatus == ENET_STATUS_LINK_ON){

          } else {
       	      uiNetLinkStatus = ENET_STATUS_LINK_ON;
       	      uiStatus |= ENET_STATUS_LINK_ON;

              /*
               * ��ȡǧ����������״̬
               */
              imxPhyRead(usPhyAddr, PHY_1000BT_STATUS, &ucGigVal);
              if (ucGigVal & PHY_LINK_1000BT_FD) {
              	  uiStatus |= ENET_STATUS_FULL_DPLX;
            	  uiStatus |= ENET_STATUS_1000M;
            	  writel(ENET_ECR_ETHER_EN | ENET_ETHER_SPEED_1000M | ENET_ETHER_LITTLE_ENDIAN,
            	     		atBase + HW_ENET_MAC_ECR);
            	  pNetDev->speed = 1000000000;

              } else if(ucGigVal & PHY_LINK_1000BT_HD){
                  uiStatus &= ~ENET_STATUS_FULL_DPLX;
            	  uiStatus |= ENET_STATUS_1000M;
            	  writel(ENET_ECR_ETHER_EN | ENET_ETHER_SPEED_1000M | ENET_ETHER_LITTLE_ENDIAN,
            	     		atBase + HW_ENET_MAC_ECR);
            	  pNetDev->speed = 1000000000;

              } else {
            	     /*
            	      * ����������״̬����ǧ����������£�����������״̬
            	      */
                     writel(ENET_ECR_ETHER_EN | ENET_ETHER_LITTLE_ENDIAN,
                        		atBase + HW_ENET_MAC_ECR);
                     uiStatus &= ~ENET_STATUS_1000M;
                     pNetDev->speed = 100000000;                        /*  ������״̬                       */
                     imxPhyRead(usPhyAddr, PHY_LINK_PARTNER_ABLTY, &ucFeVal);
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
              netdev_set_linkup(pNetDev, 1, pNetDev->speed);

              printk("ENET %0d: %s %s %s\n", usPhyAddr,
                      (uiStatus & ENET_STATUS_FULL_DPLX) ? "FULL_DUPLEX" : "HALF_DUPLEX",
                      (uiStatus & ENET_STATUS_LINK_ON) ? "connected" : "disconnected",
                      (uiStatus & ENET_STATUS_1000M) ? "1000M bps" : (uiStatus & ENET_STATUS_100M) ?
                      "100M bps" : "10M bps");
           }
       } else {
           if(uiNetLinkStatus == ~ENET_STATUS_LINK_ON){

           } else {
    	     uiNetLinkStatus = ~ENET_STATUS_LINK_ON;

    	     uiStatus &= ~ENET_STATUS_LINK_ON;

    	     netdev_set_linkup(pNetDev, 0, 0);

             /*
              * ʹ�����³�ʼ���ķ�ʽ�������ʣ�����������ͺͽ��ܻ������������������
              */
             printk("ENET %0d:link down\n", usPhyAddr);
           }
       }
    	API_TimeSSleep(1);
    }
    return  (LW_NULL);
}
/*********************************************************************************************************
** ��������: enetSetHwaddr
** ��������: ����Ӳ����ַ
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetSetHwaddr (struct netdev  *pNetDev)
{
    ENET     *pEnet;
    UINT8    *ucHwaddr;
    addr_t    atBase;

    pEnet    = pNetDev->priv;
    atBase   = pEnet->ENET_atIobase;
    ucHwaddr = pNetDev->hwaddr;

    /*
     *  ������̫�� MAC ��ַ
     */
    writel((ucHwaddr[0] << 24) +                                          /*  Bytes 0 (bits 31:24)      */
           (ucHwaddr[1] << 16) +                                          /*  Bytes 1 (bits 23:16)      */
           (ucHwaddr[2] << 8)  +                                          /*  Bytes 2 (bits 15:8)       */
           (ucHwaddr[3]),                                                 /*  Bytes 3 (bits 7:0)        */
           atBase + HW_ENET_MAC_PALR);
    writel((ucHwaddr[4] << 24) +                                          /*  Bytes 4 (bits 31:24)      */
           (ucHwaddr[5] << 16) +                                          /*  Bytes 5 (bits 23:16)      */
           ENET_PAUR_PAUSE_F_T,
           atBase + HW_ENET_MAC_PAUR);
}
/*********************************************************************************************************
** ��������: enetBuffReset
** ��������: ��������λ
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetBuffReset (struct netdev  *pNetDev)
{
    ENET             *pEnet;
    volatile BUFD    *pbufd;
    volatile INT      i;

    pEnet  = pNetDev->priv;
    pbufd  = pEnet->ENET_pbufdRxbdBase;

    for (i = 0; i < ENET_RX_RING_SZ; ++i) {
        pbufd->BUFD_usStatus = ENET_BD_RX_EMPTY;
        pbufd++;
    }

    /*
     *  ���ý����������߽��־
     */
    pbufd--;
    pbufd->BUFD_usStatus |= ENET_BD_RX_WRAP;

    pbufd = pEnet->ENET_pbufdTxbdBase;
    for (i = 0; i < ENET_TX_RING_SZ; ++i) {
        pbufd->BUFD_usStatus = 0;
        pbufd++;
    }
    /*
     *  ���÷����������߽��־
     */
    pbufd--;
    pbufd->BUFD_usStatus |= ENET_BD_TX_WRAP;
}
/*********************************************************************************************************
** ��������: enetBuffDescInit
** ��������: ��ʼ�� ENET ����������
** �䡡��  : pEnet :   ENET �ṹָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetBuffDescInit (struct netdev  *pNetDev)
{
	BUFD   *pbufd;
    ENET   *pEnet;
    INT     i;

    pEnet = pNetDev->priv;

    /*
     * ���������ַ�������ڴ湩DMAʹ�ã�
     */
    pbufd = (BUFD *)API_VmmDmaAlloc(LW_CFG_VMM_PAGE_SIZE);
    if (pbufd == LW_NULL) {
        return;
    }

    pEnet->ENET_pbufdRxbdBase = pbufd;
    pEnet->ENET_pbufdTxbdBase = pEnet->ENET_pbufdRxbdBase + ENET_RX_RING_SZ;

    /*
     * ���շ� FIFO ���������շ��������ֱ��ַ�ռ�
     */
    pbufd = pEnet->ENET_pbufdRxbdBase;
    for (i = 0; i < ENET_RX_RING_SZ; i += 2) {
        pEnet->ENET_pcRxBuf   = (PUCHAR)API_VmmDmaAlloc(LW_CFG_VMM_PAGE_SIZE);
        pbufd->BUFD_uiBufAddr = (UINT32)pEnet->ENET_pcRxBuf;
        pbufd++;

        pbufd->BUFD_uiBufAddr = (UINT32)(pEnet->ENET_pcRxBuf + ENET_BUF_SZ);
        pbufd++;
    }

    pbufd = pEnet->ENET_pbufdTxbdBase;
    for (i = 0; i < ENET_TX_RING_SZ; i += 2) {
        pEnet->ENET_pcTxBuf   = (PUCHAR)API_VmmDmaAlloc(LW_CFG_VMM_PAGE_SIZE);
        pbufd->BUFD_uiBufAddr = (UINT32)pEnet->ENET_pcTxBuf;
        pbufd++;

        pbufd->BUFD_uiBufAddr = (UINT32)(pEnet->ENET_pcTxBuf + ENET_BUF_SZ);
        pbufd++;
    }

    enetBuffReset(pNetDev);
}
/*********************************************************************************************************
** ��������: enetCoreStart
** ��������: ENET ��������ʼ������
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  enetCoreStart (struct netdev  *pNetDev)
{

    ENET     *pEnet;
    addr_t    atBase;
    UINT32    uiRate;

    pEnet   = pNetDev->priv;
    atBase  = pEnet->ENET_atIobase;

    writel(readl(atBase + HW_ENET_MAC_ECR) | ENET_ECR_RESET, atBase + HW_ENET_MAC_ECR);
    API_TimeMSleep(1);

    enetSetHwaddr(pNetDev);

    /*
     *  ����ж��¼�λ,����Linux�����޸�Ϊffc00000��
     */
    writel(0x00000000, atBase + HW_ENET_MAC_EIMR);
    writel(0xffc00000, atBase + HW_ENET_MAC_EIR);

    /*
     *  ������еĵ����Ͷಥ
     */
    writel(0, atBase + HW_ENET_MAC_IAUR);
    writel(0, atBase + HW_ENET_MAC_IALR);
    writel(pEnet->ENET_uiMcastHi, atBase + HW_ENET_MAC_GAUR);
    writel(pEnet->ENET_uiMcastLo, atBase + HW_ENET_MAC_GALR);

    /*
     *  ������󻺳���ENET_BUFD_LENGTH,��imx6q��sdk�㱼����������ֵΪ2048 - 16�� ---sylixos
     *  ���ֲ��н��ܣ��˼Ĵ����ĵ���λʼ��Ϊ0��2032 = 0x7F0������Linux��Ϊ1520 = 5F0
     */
    writel(ENET_BUFD_LENGTH, atBase + HW_ENET_MAC_EMRBR);

    /*
     *  ���÷��ͽ�������������ַ�Ĵ�������Ҫ64bits���䣬�����Ϊ��ַʼ��Ϊ0
     */
    writel((addr_t)pEnet->ENET_pbufdRxbdBase, atBase + HW_ENET_MAC_ERDSR);
    writel((addr_t)pEnet->ENET_pbufdTxbdBase, atBase + HW_ENET_MAC_ETDSR);

    enetBuffReset(pNetDev);
    pEnet->ENET_pbufdCurRxbd = pEnet->ENET_pbufdRxbdBase;
    pEnet->ENET_pbufdCurTxbd = pEnet->ENET_pbufdTxbdBase;
    pEnet->ENET_pbufdTxDirty = pEnet->ENET_pbufdCurTxbd;

    /*
     * оƬ�Ƿ���ȫ˫��״̬Ӧ�ÿ���ͨ�� PHY оƬ�Ĵ�����ȡ������Ĭ������Ϊȫ˫��
     */
    writel(FEC_RCR_MAX_FL(ENET_PKT_SZ) | 0x04, atBase + HW_ENET_MAC_RCR);
    writel(FEC_TCR_FDEN, atBase + HW_ENET_MAC_TCR);

    writel(1518, atBase + 0x1b0);

    /*
     *  ʹ�� RGMII
     */
    writel(readl(atBase + HW_ENET_MAC_RCR) | FEC_RCR_MII_MODE, atBase + HW_ENET_MAC_RCR);

    /*
     *  �ο�Linux������ fec.c�ļ���fec_restart������������̫��RGMII����ģʽ������������������
     */
	writel(FEC_ENET_RSEM_V, atBase + FEC_R_FIFO_RSEM);
	writel(FEC_ENET_RSFL_V, atBase + FEC_R_FIFO_RSFL);
	writel(FEC_ENET_RAEM_V, atBase + FEC_R_FIFO_RAEM);
	writel(FEC_ENET_RAFL_V, atBase + FEC_R_FIFO_RAFL);
	writel(FEC_ENET_OPD_V, atBase + HW_ENET_MAC_OPD);

    uiRate = imx6qMainClkGet(IPG_CLK);
	pEnet->ENET_uiPhyspeed = ((uiRate + (5000000 - 1)) / 5000000) << 1;
    writel((readl(atBase + HW_ENET_MAC_MSCR) & (~0x7E)) |               /*  ����MII�ٶȣ�               */
    		pEnet->ENET_uiPhyspeed, atBase + HW_ENET_MAC_MSCR);
    /*
     * ʹ��forward mode  HW_ENET_MAC_TFW_SFCR = 0x00000100 �ο�uboot����
     */
    writel(readl(atBase + HW_ENET_MAC_TFW_SFCR) | 0x00000100, atBase + HW_ENET_MAC_TFW_SFCR);

    /*
     *  ����ʹ�� ENET ��������С�˹���ģʽ�����ʹ�� ENET MAC ����
     */
    writel(ENET_ECR_ETHER_EN | ENET_ETHER_SPEED_1000M | ENET_ETHER_LITTLE_ENDIAN,
    		atBase + HW_ENET_MAC_ECR);

    writel(ENET_RDAR_RX_ACTIVE, atBase + HW_ENET_MAC_RDAR);             /*  ʹ�ܽ��չ���                */

    writel(ENET_DEFAULT_INTE, atBase + HW_ENET_MAC_EIMR);               /*  ʹ���ж��¼�                */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: enetCoreRecv
** ��������: ������պ���
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : ���յĳ���
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetCoreRecv (struct netdev  *pNetDev, INT (*input)(struct netdev *, struct pbuf *))
{
    ENET         *pEnet;
    struct pbuf  *pBuf;
    UINT8        *ucFrame;
    BUFD         *pBufd;
    addr_t        atBase;
    UINT16        usStatus;
    UINT16        usLen = 0;

    pEnet  = pNetDev->priv;
    atBase = pEnet->ENET_atIobase;
    pBufd  = pEnet->ENET_pbufdCurRxbd;

    usStatus = pBufd->BUFD_usStatus;
    KN_SMP_WMB();
    while (!(usStatus & ENET_BD_RX_EMPTY)) {

        if (usStatus & ENET_BD_RX_LG) {
            netdev_linkinfo_lenerr_inc(pNetDev);
        }

        if (usStatus & ENET_BD_RX_CR) {
            netdev_linkinfo_chkerr_inc(pNetDev);
        }

        if (usStatus & ENET_BD_RX_OV) {
            netdev_linkinfo_memerr_inc(pNetDev);
        }

        if (usStatus & ENET_BD_RX_TR) {
            netdev_linkinfo_err_inc(pNetDev);
            netdev_linkinfo_memerr_inc(pNetDev);
            goto rx_done;
        }

        usLen = pBufd->BUFD_usDataLen;
        ucFrame = (UINT8 *)pBufd->BUFD_uiBufAddr;                       /*  ��ȡ���յ�֡                */
        usLen -= 4;                                                     /*  ��ȥ FCS                    */

        pBuf = netdev_pbuf_alloc(usLen);
        if (!pBuf) {
            netdev_linkinfo_memerr_inc(pNetDev);
            netdev_linkinfo_drop_inc(pNetDev);
            netdev_statinfo_discards_inc(pNetDev, LINK_INPUT);

        } else {
            pbuf_take(pBuf, ucFrame, (UINT16)usLen);
            KN_SMP_WMB();

            if (input(pNetDev, pBuf)) {                                 /*  �ύ���ݵ�Э��ջ            */
                netdev_pbuf_free(pBuf);
                netdev_statinfo_discards_inc(pNetDev, LINK_INPUT);

            } else {
                netdev_linkinfo_recv_inc(pNetDev);
                netdev_statinfo_total_add(pNetDev, LINK_INPUT, usLen);  /*   ͳ�Ʒ������ݳ���           */

                if (((UINT8 *)pBuf->payload)[0] & 1) {
                    netdev_statinfo_mcasts_inc(pNetDev, LINK_INPUT);    /*   ͳ�Ʒ��͹㲥���ݰ���       */
                } else {
                    netdev_statinfo_ucasts_inc(pNetDev, LINK_INPUT);    /*   ͳ�Ʒ��͵������ݰ���       */
                }
            }
        }
rx_done:
        usStatus &= ~ENET_BD_RX_STATS;
        usStatus |= ENET_BD_RX_EMPTY;
        pBufd->BUFD_usStatus = usStatus;

        if (usStatus & ENET_BD_RX_WRAP) {
            pBufd = pEnet->ENET_pbufdRxbdBase;
        } else {
            pBufd++;
        }

        usStatus = pBufd->BUFD_usStatus;                                /*  ��һ�ֵ�������״̬          */
        KN_SMP_WMB();
        writel(ENET_RDAR_RX_ACTIVE, atBase + HW_ENET_MAC_RDAR);         /*  ʹ�� enet ����������        */
    }
    pEnet->ENET_pbufdCurRxbd = pBufd;
    writel(ENET_DEFAULT_INTE, atBase + HW_ENET_MAC_EIMR);
}
/*********************************************************************************************************
** ��������: enetCoreTx
** ��������: ���緢�ͺ���
** �䡡��  : pNetDev  :  ����ṹ
**           pbuf    :  lwip ���� buff
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT enetCoreTx (struct netdev *pNetDev, struct pbuf *pbuf)
{
    ENET     *pEnet;
    addr_t    atBase;
    UINT16    usStatus;
    UINT16    usLen;
    BUFD     *pbufd;
    INT       iLinkUp;
    INTREG    iregFlag;

    pEnet  = pNetDev->priv;
    atBase = pEnet->ENET_atIobase;

	/*
	 * ��������ǶϿ�״̬�����ش���flags ���û�������Ϊֻ������������
	 * link_down link_up ����������
	 */
    netdev_get_linkup(pNetDev, &iLinkUp);
    if(!iLinkUp) {
        return  (PX_ERROR);
    }

    if (API_SemaphoreMPend(pEnet->ENET_hSem, LW_OPTION_WAIT_INFINITE)) {
        return  (PX_ERROR);
    }

	LW_SPIN_LOCK_QUICK(&pEnet->ENET_slLock, &iregFlag);
    pbufd = pEnet->ENET_pbufdCurTxbd;
    usStatus = pbufd->BUFD_usStatus;
    LW_SPIN_UNLOCK_IRQ(&pEnet->ENET_slLock, iregFlag);

    KN_SMP_WMB();
    if (usStatus & ENET_BD_TX_READY) {
        printk("Send queue full!!\n");
        return  (PX_ERROR);
    }

    usStatus &= ~ENET_BD_TX_STATS;
    usStatus |= ENET_BD_TX_TC | ENET_BD_TX_LAST | ENET_BD_TX_READY | ENET_BD_TX_TO2;
    usLen     = pbuf->tot_len;

    pbuf_copy_partial(pbuf, (PVOID)pbufd->BUFD_uiBufAddr, usLen, 0);
    pbufd->BUFD_usDataLen = usLen;
    pbufd->BUFD_usStatus  = usStatus;
    KN_SMP_WMB();

    writel(ENET_TDAR_TX_ACTIVE, atBase + HW_ENET_MAC_TDAR);             /*  ʹ�� enet ����������,ԭ��λ��     */

    LW_SPIN_LOCK_QUICK(&pEnet->ENET_slLock, &iregFlag);
    /*
     *  ����������һ�������������������ʼ
     */
    if (usStatus & ENET_BD_TX_WRAP) {
        pbufd = pEnet->ENET_pbufdTxbdBase;
    } else {
        pbufd++;
    }

    if (pbufd == pEnet->ENET_pbufdTxDirty) {
        pEnet->ENET_iFull = 1;
    }

    pEnet->ENET_pbufdCurTxbd = pbufd;
    LW_SPIN_UNLOCK_IRQ(&pEnet->ENET_slLock, iregFlag);

    netdev_statinfo_total_add(pNetDev, LINK_OUTPUT, usLen);

    if (((UINT8 *)pbuf->payload)[0] & 1) {
        netdev_statinfo_mcasts_inc(pNetDev, LINK_OUTPUT);   /*   ͳ�Ʒ��͹㲥���ݰ���       */
    } else {
        netdev_statinfo_ucasts_inc(pNetDev, LINK_OUTPUT);   /*   ͳ�Ʒ��͵������ݰ���       */
    }

    API_SemaphoreMPost(pEnet->ENET_hSem);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: enetCoreSendComplete
** ��������: ENET ������ɺ���
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  enetCoreSendComplete (struct netdev  *pNetDev)
{
    ENET     *pEnet;
    UINT16    usStatus;
    BUFD     *pBufd;
    INTREG    iregFlag;

    pEnet = pNetDev->priv;

	LW_SPIN_LOCK_QUICK(&pEnet->ENET_slLock, &iregFlag);
    pBufd = pEnet->ENET_pbufdTxDirty;

    while (((usStatus = pBufd->BUFD_usStatus) & ENET_BD_TX_READY) == 0) {
        if (pBufd == pEnet->ENET_pbufdCurTxbd && pEnet->ENET_iFull == 0) {
            break;
        }

        if (usStatus & ENET_BD_TX_WRAP) {
            pBufd = pEnet->ENET_pbufdTxbdBase;
        } else {
            pBufd++;
        }

        if (pEnet->ENET_iFull) {
            pEnet->ENET_iFull = 0;
        }
    }
    pEnet->ENET_pbufdTxDirty = pBufd;
	LW_SPIN_UNLOCK_QUICK(&pEnet->ENET_slLock, iregFlag);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: enetCoreStop
** ��������: �ͷ����ݻ�����
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  enetCoreStop (struct netdev  *pNetDev)
{
    ENET       *pEnet;
    addr_t      atBase;
    INT         iLinkUp;

    pEnet  = pNetDev->priv;
    atBase = pEnet->ENET_atIobase;

    netdev_get_linkup(pNetDev, &iLinkUp);
    if(iLinkUp) {
        writel(1, atBase + HW_ENET_MAC_TCR);
        udelay(10);
        if (!(readl(atBase + HW_ENET_MAC_EIR) & ENET_EIR_GRA)) {
            printk("Graceful failed.\n");
        }
    }
    /*
     *  ��λ
     */
    writel(readl(atBase + HW_ENET_MAC_ECR) | ENET_ECR_RESET, atBase + HW_ENET_MAC_ECR);
    API_TimeMSleep(1);

    /*
     *  ���� RMII������Ϊǧ����ģʽ
     */
    writel(readl(atBase + HW_ENET_MAC_RCR) |
    		FEC_RCR_MII_MODE, atBase + HW_ENET_MAC_RCR);
    /*
     *  ����MII�ٶ�
     */
    writel((readl(atBase + HW_ENET_MAC_MSCR) & (~0x7E)) |
            pEnet->ENET_uiPhyspeed, atBase + HW_ENET_MAC_MSCR);
    /*
     *  ���MII�ж�
     */
    writel(ENET_EIR_MII, atBase + HW_ENET_MAC_EIR);
    /*
     * ʹ��forward mode  HW_ENET_MAC_TFW_SFCR = 0x00000100 �ο�uboot����
     */
    writel(readl(atBase + HW_ENET_MAC_TFW_SFCR) | 0x00000100, atBase + HW_ENET_MAC_TFW_SFCR);
    /*
     *  ����ʹ�� ENET
     */
    writel(ENET_ECR_ETHER_EN | ENET_ETHER_SPEED_1000M | ENET_ETHER_LITTLE_ENDIAN,
    		atBase + HW_ENET_MAC_ECR);
    /*
     *  ʹ�����ǹ��ĵ��ж��¼� ���ճ�ʼ�����������޸�  ----sylixos
     */
    writel( ENET_DEFAULT_INTE, atBase + HW_ENET_MAC_EIMR);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: enetPollHandler
** ��������: ��̫���ж���Ӧ����
** �䡡��  : pvArg   :  �жϲ���
**           uiVector:  �ж�������
** �䡡��  : �жϷ���ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID enetPollHandler (struct netdev  *pnetdev)
{
    ENET           *pEnet;
    INT             ie = 0;
    addr_t          atBase;
    INTREG          iregFlag;

    pEnet = pnetdev->priv;
    atBase = pEnet->ENET_atIobase;

    LW_SPIN_LOCK_QUICK(&pEnet->ENET_slLock, &iregFlag);
    ie = readl(atBase + HW_ENET_MAC_EIR);
    writel(ie, atBase + HW_ENET_MAC_EIR);
    LW_SPIN_UNLOCK_QUICK(&pEnet->ENET_slLock, iregFlag);

    KN_SMP_WMB();

    if (ie & ENET_EIR_TXF) {                                            /*  �Ѿ�����˷���              */
        enetCoreSendComplete(pnetdev);
    }

    if (ie & ENET_EIR_RXF) {                                            /*  �Ѿ�����˽���              */
        netdev_notify(pnetdev, LINK_INPUT, 0);
        writel(ENET_EIR_TXF, atBase + HW_ENET_MAC_EIMR);                /*  �رս����ж�                */
    }

    if (ie & ENET_EIR_MII) {
       /*
        *  MII ��д�¼������ڱ�����û��ʹ�� MII ���жϣ���˴˲��ִ�����ʵ����;������ʱʹ��
        */
    }
}
/*********************************************************************************************************
** ��������: enetIsr
** ��������: ��̫���ж���Ӧ����
** �䡡��  : pvArg   :  �жϲ���
**           uiVector:  �ж�������
** �䡡��  : �жϷ���ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static irqreturn_t enetIsr (PVOID  pvArg, UINT32  uiVector)
{
    struct netdev  *pNetDev = (struct netdev *)pvArg;
    ENET           *pEnet;
    INT             ie = 0;
    addr_t          atBase;

    pEnet = pNetDev->priv;
    atBase = pEnet->ENET_atIobase;

    ie = readl(atBase + HW_ENET_MAC_EIR);
    writel(ie, atBase + HW_ENET_MAC_EIR);
    KN_SMP_WMB();

    if (ie & ENET_EIR_TXF) {                                            /*  �Ѿ�����˷���              */
        enetCoreSendComplete(pNetDev);
        return (LW_IRQ_HANDLED);
    }

    if (ie & ENET_EIR_RXF) {                                            /*  �Ѿ�����˽���              */
        netdev_notify(pNetDev, LINK_INPUT, 1);
        writel(ENET_EIR_TXF, atBase + HW_ENET_MAC_EIMR);                /*  �رս����ж�                */
    }

    if (ie & ENET_EIR_MII) {
       /*
        *  MII ��д�¼������ڱ�����û��ʹ�� MII ���жϣ���˴˲��ִ�����ʵ����;������ʱʹ��
        */
    }

    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
** ��������: enetIoctl
** ��������: �����豸 IO ����
** �䡡��  : pnetdev   �����豸����
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  enetIoctl (struct netdev *pnetdev, INT iFlag, PVOID parg)
{
    errno = ENOSYS;

    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: __enetIoctl
** ��������: �����豸 IO ����
** �䡡��  : pnetdev   �����豸����
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetSetPromiscMode (struct netdev  *pnetdev)
{
    ENET     *pEnet;
    addr_t    atBase;
    UINT32    uiRcr;

    pEnet   = pnetdev->priv;
    atBase  = pEnet->ENET_atIobase;

    uiRcr  = read32(atBase + HW_ENET_MAC_RCR);
    uiRcr |= FEC_RCR_PROM;                                              /*  ����promiscλ               */
    write32(uiRcr, atBase + HW_ENET_MAC_RCR);
}
/*********************************************************************************************************
** ��������: enetSetMulticastStart
** ��������: ��ʼ�����鲥����
** �䡡��  : pnetdev   �����豸����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetSetMulticastStart (struct netdev  *pnetdev)
{
    ENET  *pEnet = pnetdev->priv;

    pEnet->ENET_uiMcastHi = 0;
    pEnet->ENET_uiMcastLo = 0;
}
/*********************************************************************************************************
** ��������: enetSetMulticastOne
** ��������: ����һ���鲥����
** �䡡��  : pnetdev   �����豸����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetSetMulticastOne (struct netdev  *pnetdev, const UINT8  *pcMac)
{
    ENET     *pEnet;
    UINT8     ucByte, ucMsb;
    UINT32    uiCrc, uiVal, uiHashIdx;
    UINT32    i, j;

    pEnet = pnetdev->priv;
    uiCrc = 0xffffffff;
    for (i = 0; i < 6; i++) {
        ucByte = pcMac[i];

        for (j = 0; j < 8; j++) {
            ucMsb = uiCrc >> 31;
            uiCrc <<= 1;

            if (ucMsb ^ (ucByte & 0x1)) {
                uiCrc ^= FEC_CRC_POLY;
            }
            ucByte >>= 1;
        }
    }

    uiVal     = (uiCrc & 0x3f) >> 1;
    uiHashIdx = ((uiVal & 0x01) << 4) |
                ((uiVal & 0x02) << 2) |
                ((uiVal & 0x04))      |
                ((uiVal & 0x08) >> 2) |
                ((uiVal & 0x10) >> 4);
    uiVal = 1 << uiHashIdx;

    if (uiCrc & 1) {
        pEnet->ENET_uiMcastHi |= uiVal;
    } else {
        pEnet->ENET_uiMcastLo |= uiVal;
    }
}
/*********************************************************************************************************
** ��������: enetSetMulticastFinish
** ��������: �����鲥�������
** �䡡��  : pnetdev   �����豸����
**           iFlags    �����豸���Ա�־
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetSetMulticastFinish (struct netdev  *pnetdev, INT  iFlags)
{
    ENET     *pEnet;
    addr_t    atBase;
    UINT      uiRcr;

    pEnet   = pnetdev->priv;
    atBase  = pEnet->ENET_atIobase;

    uiRcr  = read32(atBase + HW_ENET_MAC_RCR);
    uiRcr &= ~FEC_RCR_PROM;                                             /*  ���promiscλ               */
    write32(uiRcr, atBase + HW_ENET_MAC_RCR);

    writel(pEnet->ENET_uiMcastHi, atBase + HW_ENET_MAC_GAUR);           /*  �����鲥���˼Ĵ���          */
    writel(pEnet->ENET_uiMcastLo, atBase + HW_ENET_MAC_GALR);
}
/*********************************************************************************************************
** ��������: enetRxMode
** ��������: ������������ģʽ
** �䡡��  : pnetdev   �����豸����
**           flags     ����ģʽ��־
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  enetRxMode (struct netdev  *pnetdev, INT  iFlags)
{
    ENET               *pEnet;
    addr_t              atBase;
    struct netdev_mac  *ha;

    pEnet   = pnetdev->priv;
    atBase  = pEnet->ENET_atIobase;

    if (iFlags & IFF_PROMISC) {                                         /*  ����ģʽ                    */
        enetSetPromiscMode(pnetdev);

    } else if (iFlags & IFF_ALLMULTI) {                                 /*  ���������鲥֡              */
        enetSetMulticastStart(pnetdev);
        pEnet->ENET_uiMcastHi = 0xffffffffU;
        pEnet->ENET_uiMcastLo = 0xffffffffU;
        enetSetMulticastFinish(pnetdev, iFlags);

    } else if (netdev_macfilter_count(pnetdev) > 0) {                   /*  ����ָ���鲥֡              */
        enetSetMulticastStart(pnetdev);
        if (netdev_macfilter_count(pnetdev) > FEC_MAX_MULTICAST_ADDRS) {
            pEnet->ENET_uiMcastHi = 0xffffffffU;                        /*  ���������鲥֡              */
            pEnet->ENET_uiMcastLo = 0xffffffffU;
        } else {
            NETDEV_MACFILTER_FOREACH(pnetdev, ha) {                     /*  ����ָ���鲥֡              */
                enetSetMulticastOne(pnetdev, ha->hwaddr);
            }
        }
        enetSetMulticastFinish(pnetdev, iFlags);

    } else {                                                            /*  ��ֹ�����鲥֡              */
        writel(pEnet->ENET_uiMcastHi, atBase + HW_ENET_MAC_GAUR);
        writel(pEnet->ENET_uiMcastLo, atBase + HW_ENET_MAC_GALR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: enetThreadCreate
** ��������: ���� ENET ��������
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  enetThreadCreate (struct netdev  *pNetDev)
{
    LW_HANDLE            handle;
    LW_CLASS_THREADATTR  threadattr;

    if (!pNetDev) {
        return  (PX_ERROR);
    }

    threadattr = API_ThreadAttrGetDefault();
    threadattr.THREADATTR_pvArg      = (PVOID)pNetDev;
    threadattr.THREADATTR_ucPriority = LW_PRIO_T_NETJOB + 10;
    threadattr.THREADATTR_ulOption  |= LW_OPTION_OBJECT_GLOBAL;

    handle = API_ThreadCreate("t_enetlink",
                              (PTHREAD_START_ROUTINE)enetThread,
                              (PLW_CLASS_THREADATTR)&threadattr,
                              LW_NULL);
    if (handle == LW_OBJECT_HANDLE_INVALID) {
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: enetCoreInit
** ��������: Lwip �����ʼ������
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PVOID   _enetIsr (PVOID  pvArg)
{
    for (;;) {
        API_TimeSleep(1);
        enetIsr(pvArg, 0);
    }
}
static INT  enetCoreInit (struct netdev  *pNetDev)
{
    if (!pNetDev) {
        return  (PX_ERROR);
    }

    enetBuffDescInit(pNetDev);
    enetCoreStart(pNetDev);                                             /*  ȫ˫��ģʽ                  */
    ksz9021rnPhyInit(pNetDev);                                          /*  �������mac���ú�           */

#if 1
    /*
     * ���жϺ���
     */
    API_InterVectorConnect(150,
                           (PINT_SVR_ROUTINE)enetIsr,
                           (PVOID)pNetDev,
                           "enet_isr");
    API_InterVectorEnable(ENET_IRQ_NUM);
#else
    {
        LW_CLASS_THREADATTR  threadattr;
        threadattr = API_ThreadAttrGetDefault();
        threadattr.THREADATTR_pvArg            = (PVOID)pNetDev;
        threadattr.THREADATTR_ucPriority       = 111;
        threadattr.THREADATTR_stStackByteSize  = 200 * 1024;
        threadattr.THREADATTR_ulOption        |= LW_OPTION_OBJECT_GLOBAL;
        API_ThreadCreate("t_enet_isr", _enetIsr, &threadattr, LW_NULL);
    }
#endif

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: enetPoolRecv
** ��������: i.MX6 ������ѯ������������
** �䡡��  : pnetdev      - �����豸���ƽṹ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetPoolRecv (struct netdev  *pnetdev)
{
    enetPollHandler(pnetdev);
}
/*********************************************************************************************************
** ��������: enetIntctl
** ��������: i.MX6 �����жϿ��ƽӿ�
** �䡡��  : netdev      - �����豸���ƽṹ
**           en          - 0:�ر��ж�; 1:�����ж�;
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  enetIntctl (struct netdev  *netdev, int  en)
{
    if (en) {
        API_InterVectorEnable(ENET_IRQ_NUM);
    } else {
        API_InterVectorDisable(ENET_IRQ_NUM);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: enetRegister
** ��������: ע������ӿ�
** �䡡��  : pNetDev  :  ����ṹ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  enetRegister (struct netdev  *pNetDev)
{
    char                  ip[20]        = {0};
    char                  netmask[20]   = {0};
    char                  gw[20]        = {0};
    static UINT           uiIndex       = 0;

    static struct netdev_funcs  imx6_net_drv = {
        .init      = enetCoreInit,
        .transmit  = enetCoreTx,
        .receive   = enetCoreRecv,
        .rxmode    = enetRxMode,
        .up        = enetCoreStart,
        .down      = enetCoreStop,
        .ioctl     = enetIoctl,
        .pollrecv  = enetPoolRecv,
        .intctl    = enetIntctl,
    };

    pNetDev->magic_no = NETDEV_MAGIC;
    sprintf(pNetDev->dev_name, "imx6q_%d", uiIndex);
    lib_strcpy(pNetDev->if_name,  "en");

    pNetDev->if_hostname = "SylixOS_IMX6";
    pNetDev->init_flags  = NETDEV_INIT_LOAD_PARAM
                         | NETDEV_INIT_LOAD_DNS
                         | NETDEV_INIT_IPV6_AUTOCFG
                         | NETDEV_INIT_AS_DEFAULT;

    pNetDev->chksum_flags = NETDEV_CHKSUM_ENABLE_ALL;
    pNetDev->net_type     = NETDEV_TYPE_ETHERNET;
    pNetDev->speed        = 0;
    pNetDev->mtu          = 1500;
    pNetDev->drv          = &imx6_net_drv;

    pNetDev->hwaddr[0]    = 0x00;
    pNetDev->hwaddr[1]    = 0x04;
    pNetDev->hwaddr[2]    = 0x9F;
    pNetDev->hwaddr[3]    = 0x02;
    pNetDev->hwaddr[4]    = 0x00;
    pNetDev->hwaddr[5]    = 0x28;
    pNetDev->hwaddr_len   = 6;

    lib_strcpy(ip,      "10.4.0.161");
    lib_strcpy(gw,      "10.4.0.1");
    lib_strcpy(netmask, "255.0.0.0");

   if (netdev_add(pNetDev, ip, netmask,gw,
                  IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST) == 0) {
       enetThreadCreate(pNetDev);
   }
}
/*********************************************************************************************************
** ��������: enetInit
** ��������: ENET ��ʼ��
** �䡡��  : NONE
** �䡡��  : ������
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  enetInit (VOID)
{
    static struct netdev    netdev;

    netdev.priv = &_G_enetInfo;

    LW_SPIN_INIT(&_G_enetInfo.ENET_slLock);
    _G_enetInfo.ENET_hSem = API_SemaphoreMCreate("net_mutex",
                                                 LW_PRIO_DEF_CEILING,
                                                (LW_OPTION_WAIT_PRIORITY    |
                                                 LW_OPTION_DELETE_SAFE      |
                                                 LW_OPTION_INHERIT_PRIORITY |
                                                 LW_OPTION_OBJECT_GLOBAL),
                                                 LW_NULL);
    enetRegister(&netdev);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
