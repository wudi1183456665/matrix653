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
** 文   件   名: imx6q_netif.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 01 月 21 日
**
** 描        述: imx6q 处理器的以太网驱动
**
** 2016.11.09  适配netdev网卡框架
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "sdk.h"

#include "netdev.h"
#include "netif/imx6q_phy.h"
/*********************************************************************************************************
  ENET-MAC Programmable Registers (MCIMX28RM.pdf-->26.4) (部分)
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
 *  以下内容从为 imx6 独有寄存器  接收 fifo 相关寄存器
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
 *  以下内容从为 imx6 独有寄存器位定义
 */
#define FEC_ENET_RSEM_V		    (0x84)
#define FEC_ENET_RSFL_V		    (16  )
#define FEC_ENET_RAEM_V		    (0x8 )
#define FEC_ENET_RAFL_V		    (0x8 )
#define FEC_ENET_OPD_V		    (0xFFF0)
/*********************************************************************************************************
  中断标志寄存器 EIR 标志位定义
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
#define ENET_DEFAULT_INTE       (ENET_EIR_TXF | ENET_EIR_RXF)           /*  默认中断状态                */

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
#define ENET_BD_RX_CR           ((UINT16)0x0004)                        /*　Rx CRC or Rx Frame error　  */
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
 * 额外添加的宏定义，这些宏定义多用于 PHY 寄存器，后续使用 MII 驱动可以将此部分删除
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
  PHY 寄存器读写操作所需要的宏定义
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
  驱动基本配置宏定义，根据Linux驱动进行了更改，就手册看与 imx283 相同应该也可以
*********************************************************************************************************/
#define ENET_NAME                "PHY"
#define ENET_RING_SZ             (16)
#define ENET_TX_RING_SZ          (128)                                  /*  2 的倍数                    */
#define ENET_RX_RING_SZ          (2 * 192)                              /*  2 的倍数                    */
#define ENET_RING_MASK           (15)
#define ENET_BUF_SZ              (LW_CFG_VMM_PAGE_SIZE >> 1)            /*  半页                        */
#define ENET_BUFD_LENGTH         (1520)
#define ENET_PKT_SZ              (1518)
#define FEC_ENET_PHYID           (0x00221610)                           /*  PHY_KSZ9021RN_ID            */
#define ENET_PHYADDR             (6)                                    /*  phy addr 是6                */
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
  ENET 控制器 FIFO/DMA 工作描述符结构体
*********************************************************************************************************/
typedef struct bufferDesc {
    UINT16              BUFD_usDataLen;                                 /*  缓冲描述符中数据长度        */
    UINT16              BUFD_usStatus;                                  /*  缓冲描述符状态              */
    ULONG               BUFD_uiBufAddr;                                 /*  缓冲区地址                  */
} BUFD;
/*********************************************************************************************************
  ENET 私有结构
*********************************************************************************************************/
typedef struct enet {
    addr_t              ENET_atIobase;                                  /*  ENET 基地址　　　　　　　　 */
    UINT32              ENET_uiPhyspeed;                                /*  PHY 速度                    */
    UINT16              ENET_ucPhyAddr;
    UINT16              ENET_ucRsv;
    INT                 ENET_iFull;                                     /*  发送队列满                  */
    BUFD               *ENET_pbufdRxbdBase;                             /*  接收缓冲描述符指针          */
    BUFD               *ENET_pbufdTxbdBase;                             /*  发送缓冲描述符指针          */
    BUFD               *ENET_pbufdCurRxbd;                              /*  当前接收描述符              */
    BUFD               *ENET_pbufdCurTxbd;                              /*  当前发送描述符              */
    BUFD               *ENET_pbufdTxDirty;                              /*  已经发送完成的描述符        */
    PUCHAR              ENET_pcTxBuf;                                   /*  发送缓冲区地址              */
    PUCHAR              ENET_pcRxBuf;                                   /*  接收缓冲区地址              */
    UINT32              ENET_uiMcastHi;                                 /*  组播设置高32位              */
    UINT32              ENET_uiMcastLo;                                 /*  组播设置低32位              */

    LW_SPINLOCK_DEFINE  (ENET_slLock);
    LW_OBJECT_HANDLE    ENET_hSem;

} ENET;
/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/
static  ENET              _G_enetInfo  = {
        .ENET_atIobase  = ENET_BASE_ADDR,                               /*  ENET-MAC 的地址             */
        .ENET_iFull     = 0,
        .ENET_uiMcastHi = 0,
        .ENET_uiMcastLo = 0,
};
UINT       uiNetLinkStatus = 0;                                           /*  记录网络 phy 芯片的连接状态 */
/*********************************************************************************************************
** 函数名称:  imxPhyRead,
** 功能描述:  PHY 寄存器值读取函数
** 输    入:  phyAddr  phy芯片地址
**            regAddr  phy芯片寄存器地址
**           *value   读取的寄存器数值
** 输    出: NONE
** 返    回: ERROR CODE
*********************************************************************************************************/
INT imxPhyRead(UCHAR phyAddr, UCHAR regAddr, UINT16 *value)
{
    INT  rValue;
    INT  iRetryCnt = 10000;

    /*
     *  一个cpu有多个mac时，mdio总线往往只有一个，因此这里使用固定地址操作
     */
    rValue = readl(ENET_BASE_ADDR + HW_ENET_MAC_EIR);
    if (rValue & ENET_EIR_MII ) {
    	writel(rValue, ENET_BASE_ADDR + HW_ENET_MAC_EIR);
    }

    writel(FEC_MII_READ(phyAddr, regAddr), ENET_BASE_ADDR + HW_ENET_MAC_MMFR);

    while (--iRetryCnt > 0) {
        rValue = readl(ENET_BASE_ADDR + HW_ENET_MAC_EIR);               /* 如果有中断标志，             */
        if (rValue & ENET_EIR_MII ) {
        	writel(rValue, ENET_BASE_ADDR + HW_ENET_MAC_EIR);           /* 清除中断标志                 */
            rValue = readl(ENET_BASE_ADDR + HW_ENET_MAC_MMFR);          /* 读取 PHY 寄存器数值          */
            *value = FEC_MII_GET_DATA(rValue);
            return  (ERROR_NONE);
        }
        udelay(2);                                                      /* 延时2us                      */
    }
    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称:  imxPhyWrite
** 功能描述:  PHY 寄存器值写取函数
** 输    入:  phyAddr  phy芯片地址
**            regAddr  phy芯片寄存器地址
**            value    phy芯片寄存器写入数据
** 输    出:
** 返    回: ERROR CODE
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
** 函数名称: ksz9021rnPhyInit
** 功能描述: 初始化 PHY 芯片 KSZ9021RN
** 输    入:
** 输    出:
** 返    回: ERROR CODE
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
** 函数名称: enetThread
** 功能描述: 以太网link状态监测线程
** 输　入  : pvArg  :  线程参数
** 输　出  : NULL
** 全局变量:
** 调用模块:
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
               * 读取千兆网的连接状态
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
            	      * 在网络链接状态不是千兆网的情况下，检测百兆网的状态
            	      */
                     writel(ENET_ECR_ETHER_EN | ENET_ETHER_LITTLE_ENDIAN,
                        		atBase + HW_ENET_MAC_ECR);
                     uiStatus &= ~ENET_STATUS_1000M;
                     pNetDev->speed = 100000000;                        /*  百兆网状态                       */
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
              * 使用重新初始化的方式并不合适，可以清除发送和接受缓冲区来避免网络错误
              */
             printk("ENET %0d:link down\n", usPhyAddr);
           }
       }
    	API_TimeSSleep(1);
    }
    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: enetSetHwaddr
** 功能描述: 设置硬件地址
** 输　入  : pNetDev  :  网络结构
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
     *  设置以太网 MAC 地址
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
** 函数名称: enetBuffReset
** 功能描述: 描述符复位
** 输　入  : pNetDev  :  网络结构
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
     *  设置接收描述符边界标志
     */
    pbufd--;
    pbufd->BUFD_usStatus |= ENET_BD_RX_WRAP;

    pbufd = pEnet->ENET_pbufdTxbdBase;
    for (i = 0; i < ENET_TX_RING_SZ; ++i) {
        pbufd->BUFD_usStatus = 0;
        pbufd++;
    }
    /*
     *  设置发送描述符边界标志
     */
    pbufd--;
    pbufd->BUFD_usStatus |= ENET_BD_TX_WRAP;
}
/*********************************************************************************************************
** 函数名称: enetBuffDescInit
** 功能描述: 初始化 ENET 缓冲描述符
** 输　入  : pEnet :   ENET 结构指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  enetBuffDescInit (struct netdev  *pNetDev)
{
	BUFD   *pbufd;
    ENET   *pEnet;
    INT     i;

    pEnet = pNetDev->priv;

    /*
     * 分配物理地址连续的内存供DMA使用，
     */
    pbufd = (BUFD *)API_VmmDmaAlloc(LW_CFG_VMM_PAGE_SIZE);
    if (pbufd == LW_NULL) {
        return;
    }

    pEnet->ENET_pbufdRxbdBase = pbufd;
    pEnet->ENET_pbufdTxbdBase = pEnet->ENET_pbufdRxbdBase + ENET_RX_RING_SZ;

    /*
     * 给收发 FIFO 描述符和收发缓冲区分别地址空间
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
** 函数名称: enetCoreStart
** 功能描述: ENET 控制器初始化函数
** 输　入  : pNetDev  :  网络结构
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
     *  清除中断事件位,根据Linux代码修改为ffc00000，
     */
    writel(0x00000000, atBase + HW_ENET_MAC_EIMR);
    writel(0xffc00000, atBase + HW_ENET_MAC_EIR);

    /*
     *  清除所有的单播和多播
     */
    writel(0, atBase + HW_ENET_MAC_IAUR);
    writel(0, atBase + HW_ENET_MAC_IALR);
    writel(pEnet->ENET_uiMcastHi, atBase + HW_ENET_MAC_GAUR);
    writel(pEnet->ENET_uiMcastLo, atBase + HW_ENET_MAC_GALR);

    /*
     *  设置最大缓冲区ENET_BUFD_LENGTH,在imx6q的sdk裸奔代码中设置值为2048 - 16， ---sylixos
     *  在手册中介绍，此寄存器的低四位始终为0，2032 = 0x7F0。根据Linux该为1520 = 5F0
     */
    writel(ENET_BUFD_LENGTH, atBase + HW_ENET_MAC_EMRBR);

    /*
     *  设置发送接收描述符基地址寄存器，需要64bits对其，即最低为地址始终为0
     */
    writel((addr_t)pEnet->ENET_pbufdRxbdBase, atBase + HW_ENET_MAC_ERDSR);
    writel((addr_t)pEnet->ENET_pbufdTxbdBase, atBase + HW_ENET_MAC_ETDSR);

    enetBuffReset(pNetDev);
    pEnet->ENET_pbufdCurRxbd = pEnet->ENET_pbufdRxbdBase;
    pEnet->ENET_pbufdCurTxbd = pEnet->ENET_pbufdTxbdBase;
    pEnet->ENET_pbufdTxDirty = pEnet->ENET_pbufdCurTxbd;

    /*
     * 芯片是否是全双工状态应该可以通过 PHY 芯片寄存器读取，这里默认设置为全双工
     */
    writel(FEC_RCR_MAX_FL(ENET_PKT_SZ) | 0x04, atBase + HW_ENET_MAC_RCR);
    writel(FEC_TCR_FDEN, atBase + HW_ENET_MAC_TCR);

    writel(1518, atBase + 0x1b0);

    /*
     *  使能 RGMII
     */
    writel(readl(atBase + HW_ENET_MAC_RCR) | FEC_RCR_MII_MODE, atBase + HW_ENET_MAC_RCR);

    /*
     *  参考Linux的驱动 fec.c文件的fec_restart函，在配置以太网RGMII工作模式后，增加若干内容配置
     */
	writel(FEC_ENET_RSEM_V, atBase + FEC_R_FIFO_RSEM);
	writel(FEC_ENET_RSFL_V, atBase + FEC_R_FIFO_RSFL);
	writel(FEC_ENET_RAEM_V, atBase + FEC_R_FIFO_RAEM);
	writel(FEC_ENET_RAFL_V, atBase + FEC_R_FIFO_RAFL);
	writel(FEC_ENET_OPD_V, atBase + HW_ENET_MAC_OPD);

    uiRate = imx6qMainClkGet(IPG_CLK);
	pEnet->ENET_uiPhyspeed = ((uiRate + (5000000 - 1)) / 5000000) << 1;
    writel((readl(atBase + HW_ENET_MAC_MSCR) & (~0x7E)) |               /*  设置MII速度，               */
    		pEnet->ENET_uiPhyspeed, atBase + HW_ENET_MAC_MSCR);
    /*
     * 使能forward mode  HW_ENET_MAC_TFW_SFCR = 0x00000100 参考uboot进行
     */
    writel(readl(atBase + HW_ENET_MAC_TFW_SFCR) | 0x00000100, atBase + HW_ENET_MAC_TFW_SFCR);

    /*
     *  现在使能 ENET ，并设置小端工作模式，随后使能 ENET MAC 接收
     */
    writel(ENET_ECR_ETHER_EN | ENET_ETHER_SPEED_1000M | ENET_ETHER_LITTLE_ENDIAN,
    		atBase + HW_ENET_MAC_ECR);

    writel(ENET_RDAR_RX_ACTIVE, atBase + HW_ENET_MAC_RDAR);             /*  使能接收工作                */

    writel(ENET_DEFAULT_INTE, atBase + HW_ENET_MAC_EIMR);               /*  使能中断事件                */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: enetCoreRecv
** 功能描述: 网络接收函数
** 输　入  : pNetDev  :  网络结构
** 输　出  : 接收的长度
** 全局变量:
** 调用模块:
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
        ucFrame = (UINT8 *)pBufd->BUFD_uiBufAddr;                       /*  获取接收的帧                */
        usLen -= 4;                                                     /*  除去 FCS                    */

        pBuf = netdev_pbuf_alloc(usLen);
        if (!pBuf) {
            netdev_linkinfo_memerr_inc(pNetDev);
            netdev_linkinfo_drop_inc(pNetDev);
            netdev_statinfo_discards_inc(pNetDev, LINK_INPUT);

        } else {
            pbuf_take(pBuf, ucFrame, (UINT16)usLen);
            KN_SMP_WMB();

            if (input(pNetDev, pBuf)) {                                 /*  提交数据到协议栈            */
                netdev_pbuf_free(pBuf);
                netdev_statinfo_discards_inc(pNetDev, LINK_INPUT);

            } else {
                netdev_linkinfo_recv_inc(pNetDev);
                netdev_statinfo_total_add(pNetDev, LINK_INPUT, usLen);  /*   统计发送数据长度           */

                if (((UINT8 *)pBuf->payload)[0] & 1) {
                    netdev_statinfo_mcasts_inc(pNetDev, LINK_INPUT);    /*   统计发送广播数据包数       */
                } else {
                    netdev_statinfo_ucasts_inc(pNetDev, LINK_INPUT);    /*   统计发送单播数据包数       */
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

        usStatus = pBufd->BUFD_usStatus;                                /*  下一轮的描述符状态          */
        KN_SMP_WMB();
        writel(ENET_RDAR_RX_ACTIVE, atBase + HW_ENET_MAC_RDAR);         /*  使能 enet 控制器接收        */
    }
    pEnet->ENET_pbufdCurRxbd = pBufd;
    writel(ENET_DEFAULT_INTE, atBase + HW_ENET_MAC_EIMR);
}
/*********************************************************************************************************
** 函数名称: enetCoreTx
** 功能描述: 网络发送函数
** 输　入  : pNetDev  :  网络结构
**           pbuf    :  lwip 核心 buff
** 输　出  : 错误号
** 全局变量:
** 调用模块:
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
	 * 如果网络是断开状态，返回错误，flags 在用户程序中为只读，其设置在
	 * link_down link_up 函数中设置
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

    writel(ENET_TDAR_TX_ACTIVE, atBase + HW_ENET_MAC_TDAR);             /*  使能 enet 控制器发送,原有位置     */

    LW_SPIN_LOCK_QUICK(&pEnet->ENET_slLock, &iregFlag);
    /*
     *  如果这是最后一个发送描述符，返回最开始
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
        netdev_statinfo_mcasts_inc(pNetDev, LINK_OUTPUT);   /*   统计发送广播数据包数       */
    } else {
        netdev_statinfo_ucasts_inc(pNetDev, LINK_OUTPUT);   /*   统计发送单播数据包数       */
    }

    API_SemaphoreMPost(pEnet->ENET_hSem);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: enetCoreSendComplete
** 功能描述: ENET 发送完成函数
** 输　入  : pNetDev  :  网络结构
** 输　出  : 错误号
** 全局变量:
** 调用模块:
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
** 函数名称: enetCoreStop
** 功能描述: 释放数据缓冲区
** 输　入  : pNetDev  :  网络结构
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
     *  复位
     */
    writel(readl(atBase + HW_ENET_MAC_ECR) | ENET_ECR_RESET, atBase + HW_ENET_MAC_ECR);
    API_TimeMSleep(1);

    /*
     *  设置 RMII，设置为千兆网模式
     */
    writel(readl(atBase + HW_ENET_MAC_RCR) |
    		FEC_RCR_MII_MODE, atBase + HW_ENET_MAC_RCR);
    /*
     *  设置MII速度
     */
    writel((readl(atBase + HW_ENET_MAC_MSCR) & (~0x7E)) |
            pEnet->ENET_uiPhyspeed, atBase + HW_ENET_MAC_MSCR);
    /*
     *  清除MII中断
     */
    writel(ENET_EIR_MII, atBase + HW_ENET_MAC_EIR);
    /*
     * 使能forward mode  HW_ENET_MAC_TFW_SFCR = 0x00000100 参考uboot进行
     */
    writel(readl(atBase + HW_ENET_MAC_TFW_SFCR) | 0x00000100, atBase + HW_ENET_MAC_TFW_SFCR);
    /*
     *  现在使能 ENET
     */
    writel(ENET_ECR_ETHER_EN | ENET_ETHER_SPEED_1000M | ENET_ETHER_LITTLE_ENDIAN,
    		atBase + HW_ENET_MAC_ECR);
    /*
     *  使能我们关心的中断事件 对照初始化函数进行修改  ----sylixos
     */
    writel( ENET_DEFAULT_INTE, atBase + HW_ENET_MAC_EIMR);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: enetPollHandler
** 功能描述: 以太网中断响应函数
** 输　入  : pvArg   :  中断参数
**           uiVector:  中断向量号
** 输　出  : 中断返回值
** 全局变量:
** 调用模块:
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

    if (ie & ENET_EIR_TXF) {                                            /*  已经完成了发送              */
        enetCoreSendComplete(pnetdev);
    }

    if (ie & ENET_EIR_RXF) {                                            /*  已经完成了接收              */
        netdev_notify(pnetdev, LINK_INPUT, 0);
        writel(ENET_EIR_TXF, atBase + HW_ENET_MAC_EIMR);                /*  关闭接收中断                */
    }

    if (ie & ENET_EIR_MII) {
       /*
        *  MII 读写事件，由于本驱动没有使用 MII 的中断，因此此部分代码无实际用途，调试时使用
        */
    }
}
/*********************************************************************************************************
** 函数名称: enetIsr
** 功能描述: 以太网中断响应函数
** 输　入  : pvArg   :  中断参数
**           uiVector:  中断向量号
** 输　出  : 中断返回值
** 全局变量:
** 调用模块:
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

    if (ie & ENET_EIR_TXF) {                                            /*  已经完成了发送              */
        enetCoreSendComplete(pNetDev);
        return (LW_IRQ_HANDLED);
    }

    if (ie & ENET_EIR_RXF) {                                            /*  已经完成了接收              */
        netdev_notify(pNetDev, LINK_INPUT, 1);
        writel(ENET_EIR_TXF, atBase + HW_ENET_MAC_EIMR);                /*  关闭接收中断                */
    }

    if (ie & ENET_EIR_MII) {
       /*
        *  MII 读写事件，由于本驱动没有使用 MII 的中断，因此此部分代码无实际用途，调试时使用
        */
    }

    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
** 函数名称: enetIoctl
** 功能描述: 网络设备 IO 控制
** 输　入  : pnetdev   网络设备对象
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  enetIoctl (struct netdev *pnetdev, INT iFlag, PVOID parg)
{
    errno = ENOSYS;

    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: __enetIoctl
** 功能描述: 网络设备 IO 控制
** 输　入  : pnetdev   网络设备对象
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  enetSetPromiscMode (struct netdev  *pnetdev)
{
    ENET     *pEnet;
    addr_t    atBase;
    UINT32    uiRcr;

    pEnet   = pnetdev->priv;
    atBase  = pEnet->ENET_atIobase;

    uiRcr  = read32(atBase + HW_ENET_MAC_RCR);
    uiRcr |= FEC_RCR_PROM;                                              /*  设置promisc位               */
    write32(uiRcr, atBase + HW_ENET_MAC_RCR);
}
/*********************************************************************************************************
** 函数名称: enetSetMulticastStart
** 功能描述: 开始设置组播参数
** 输　入  : pnetdev   网络设备对象
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  enetSetMulticastStart (struct netdev  *pnetdev)
{
    ENET  *pEnet = pnetdev->priv;

    pEnet->ENET_uiMcastHi = 0;
    pEnet->ENET_uiMcastLo = 0;
}
/*********************************************************************************************************
** 函数名称: enetSetMulticastOne
** 功能描述: 设置一个组播参数
** 输　入  : pnetdev   网络设备对象
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
** 函数名称: enetSetMulticastFinish
** 功能描述: 设置组播参数完成
** 输　入  : pnetdev   网络设备对象
**           iFlags    网络设备属性标志
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  enetSetMulticastFinish (struct netdev  *pnetdev, INT  iFlags)
{
    ENET     *pEnet;
    addr_t    atBase;
    UINT      uiRcr;

    pEnet   = pnetdev->priv;
    atBase  = pEnet->ENET_atIobase;

    uiRcr  = read32(atBase + HW_ENET_MAC_RCR);
    uiRcr &= ~FEC_RCR_PROM;                                             /*  清除promisc位               */
    write32(uiRcr, atBase + HW_ENET_MAC_RCR);

    writel(pEnet->ENET_uiMcastHi, atBase + HW_ENET_MAC_GAUR);           /*  更新组播过滤寄存器          */
    writel(pEnet->ENET_uiMcastLo, atBase + HW_ENET_MAC_GALR);
}
/*********************************************************************************************************
** 函数名称: enetRxMode
** 功能描述: 设置网卡接收模式
** 输　入  : pnetdev   网络设备对象
**           flags     接收模式标志
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  enetRxMode (struct netdev  *pnetdev, INT  iFlags)
{
    ENET               *pEnet;
    addr_t              atBase;
    struct netdev_mac  *ha;

    pEnet   = pnetdev->priv;
    atBase  = pEnet->ENET_atIobase;

    if (iFlags & IFF_PROMISC) {                                         /*  混杂模式                    */
        enetSetPromiscMode(pnetdev);

    } else if (iFlags & IFF_ALLMULTI) {                                 /*  接收所有组播帧              */
        enetSetMulticastStart(pnetdev);
        pEnet->ENET_uiMcastHi = 0xffffffffU;
        pEnet->ENET_uiMcastLo = 0xffffffffU;
        enetSetMulticastFinish(pnetdev, iFlags);

    } else if (netdev_macfilter_count(pnetdev) > 0) {                   /*  接收指定组播帧              */
        enetSetMulticastStart(pnetdev);
        if (netdev_macfilter_count(pnetdev) > FEC_MAX_MULTICAST_ADDRS) {
            pEnet->ENET_uiMcastHi = 0xffffffffU;                        /*  接收所有组播帧              */
            pEnet->ENET_uiMcastLo = 0xffffffffU;
        } else {
            NETDEV_MACFILTER_FOREACH(pnetdev, ha) {                     /*  接收指定组播帧              */
                enetSetMulticastOne(pnetdev, ha->hwaddr);
            }
        }
        enetSetMulticastFinish(pnetdev, iFlags);

    } else {                                                            /*  禁止接收组播帧              */
        writel(pEnet->ENET_uiMcastHi, atBase + HW_ENET_MAC_GAUR);
        writel(pEnet->ENET_uiMcastLo, atBase + HW_ENET_MAC_GALR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: enetThreadCreate
** 功能描述: 增加 ENET 工作队列
** 输　入  : pNetDev  :  网络结构
** 输　出  : 错误号
** 全局变量:
** 调用模块:
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
** 函数名称: enetCoreInit
** 功能描述: Lwip 网络初始化函数
** 输　入  : pNetDev  :  网络结构
** 输　出  : 错误号
** 全局变量:
** 调用模块:
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
    enetCoreStart(pNetDev);                                             /*  全双工模式                  */
    ksz9021rnPhyInit(pNetDev);                                          /*  必须放在mac设置后           */

#if 1
    /*
     * 绑定中断函数
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
** 函数名称: enetPoolRecv
** 功能描述: i.MX6 网卡轮询接收数据流程
** 输　入  : pnetdev      - 网络设备控制结构
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  enetPoolRecv (struct netdev  *pnetdev)
{
    enetPollHandler(pnetdev);
}
/*********************************************************************************************************
** 函数名称: enetIntctl
** 功能描述: i.MX6 网卡中断控制接口
** 输　入  : netdev      - 网络设备控制结构
**           en          - 0:关闭中断; 1:开启中断;
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
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
** 函数名称: enetRegister
** 功能描述: 注册网络接口
** 输　入  : pNetDev  :  网络结构
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
** 函数名称: enetInit
** 功能描述: ENET 初始化
** 输　入  : NONE
** 输　出  : 错误码
** 全局变量:
** 调用模块:
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
