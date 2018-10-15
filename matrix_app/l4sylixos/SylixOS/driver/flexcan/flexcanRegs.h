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
** 文   件   名: flexcanRegs.h
**
** 创   建   人: Hu.Hanzhang (胡瀚璋)
**
** 文件创建日期: 2016 年 06 月 07 日
**
** 描        述: imx6q flexcan 控制器寄存器
*********************************************************************************************************/
#ifndef FLEXCANREGS_H_
#define FLEXCANREGS_H_

#define CAN_SFF_MASK 0x000007FFU                                        /* standard frame format (SFF)  */
#define CAN_EFF_MASK 0x1FFFFFFFU                                        /* extended frame format (EFF)  */

#define FLEXCAN_MCR                 0x000
#define FLEXCAN_CTRL1               0x004
#define FLEXCAN_TIMER               0x008
#define FLEXCAN_RESERVED1           0x00C
#define FLEXCAN_RXMGMASK            0x010
#define FLEXCAN_RX14MASK            0x014
#define FLEXCAN_RX15MASK            0x018
#define FLEXCAN_ECR                 0x01C
#define FLEXCAN_ESR1                0x020
#define FLEXCAN_IMASK2              0x024
#define FLEXCAN_IMASK1              0x028
#define FLEXCAN_IFLAG2              0x02C
#define FLEXCAN_IFLAG1              0x030
#define FLEXCAN_CTRL2               0x034
#define FLEXCAN_ESR2                0x038
#define FLEXCAN_RESERVED2           0x03C
#define FLEXCAN_CRCR                0x044
#define FLEXCAN_RXFGMASK            0x048
#define FLEXCAN_RXFIR               0x04C
#define FLEXCAN_RESERVED3           0x050
#define FLEXCAN_MB_CTRL(n)          (0x080 + (0x10 * n) + 0x0)
#define FLEXCAN_MB_ID(n)            (0x080 + (0x10 * n) + 0x4)
#define FLEXCAN_MB_DATA0(n)         (0x080 + (0x10 * n) + 0x8)
#define FLEXCAN_MB_DATA1(n)         (0x080 + (0x10 * n) + 0xC)
#define FLEXCAN_RXIMR(n)            (0x880 + (0x4 * n))
#define FLEXCAN_GFWR                0x9E0

#define BIT(x)                      ((UINT32)0x1 << (x))


#define FLEXCAN_MCR_MDIS            BIT(31)
#define FLEXCAN_MCR_FRZ             BIT(30)
#define FLEXCAN_MCR_RFEN            BIT(29)
#define FLEXCAN_MCR_HALT            BIT(28)
#define FLEXCAN_MCR_NOT_RDY         BIT(27)
#define FLEXCAN_MCR_WAK_MSK         BIT(26)
#define FLEXCAN_MCR_SOFT_RST        BIT(25)
#define FLEXCAN_MCR_FRZ_ACK         BIT(24)
#define FLEXCAN_MCR_SUPV            BIT(23)
#define FLEXCAN_MCR_SLF_WAK         BIT(22)
#define FLEXCAN_MCR_WRN_EN          BIT(21)
#define FLEXCAN_MCR_LPM_ACK         BIT(20)
#define FLEXCAN_MCR_WAK_SRC         BIT(19)
#define FLEXCAN_MCR_SRX_DIS         BIT(17)
#define FLEXCAN_MCR_IRMQ            BIT(16)
#define FLEXCAN_MCR_LPRIO_EN        BIT(13)
#define FLEXCAN_MCR_AEN             BIT(12)

#define FLEXCAN_MCR_MAXMB_MSK       0x3f
#define FLEXCAN_MCR_MAXMB(x)        ((x) & 0x3f)
#define FLEXCAN_MCR_IDAM_A          (0 << 8)
#define FLEXCAN_MCR_IDAM_B          (1 << 8)
#define FLEXCAN_MCR_IDAM_C          (2 << 8)
#define FLEXCAN_MCR_IDAM_D          (3 << 8)


#define FLEXCAN_CTRL1_PRESDIV(x)    (((x) & 0xff) << 24)
#define FLEXCAN_CTRL1_RJW(x)        (((x) & 0x03) << 22)
#define FLEXCAN_CTRL1_PSEG1(x)      (((x) & 0x07) << 19)
#define FLEXCAN_CTRL1_PSEG2(x)      (((x) & 0x07) << 16)

#define FLEXCAN_CTRL1_BOFF_MSK      BIT(15)
#define FLEXCAN_CTRL1_ERR_MSK       BIT(14)
#define FLEXCAN_CTRL1_LPB           BIT(12)
#define FLEXCAN_CTRL1_TWRN_MSK      BIT(11)
#define FLEXCAN_CTRL1_RWRN_MSK      BIT(10)
#define FLEXCAN_CTRL1_SMP           BIT(7)
#define FLEXCAN_CTRL1_BOFF_REC      BIT(6)
#define FLEXCAN_CTRL1_TSYN          BIT(5)
#define FLEXCAN_CTRL1_LBUF          BIT(4)
#define FLEXCAN_CTRL1_LOM           BIT(3)

#define FLEXCAN_CTRL1_PROP_SEG(x)   ((x) & 0x07)
#define FLEXCAN_CTRL1_ERR_STATE     (FLEXCAN_CTRL1_TWRN_MSK | \
                                    FLEXCAN_CTRL1_RWRN_MSK  | \
                                    FLEXCAN_CTRL1_BOFF_MSK)
#define FLEXCAN_CTRL1_ERR_ALL       (FLEXCAN_CTRL1_ERR_MSK  | \
                                    FLEXCAN_CTRL1_ERR_STATE)

#define FLEXCAN_ESR1_TWRN_INT       BIT(17)
#define FLEXCAN_ESR1_RWRN_INT       BIT(16)
#define FLEXCAN_ESR1_BIT1_ERR       BIT(15)
#define FLEXCAN_ESR1_BIT0_ERR       BIT(14)
#define FLEXCAN_ESR1_ACK_ERR        BIT(13)
#define FLEXCAN_ESR1_CRC_ERR        BIT(12)
#define FLEXCAN_ESR1_FRM_ERR        BIT(11)
#define FLEXCAN_ESR1_STF_ERR        BIT(10)
#define FLEXCAN_ESR1_TX_WRN         BIT(9)
#define FLEXCAN_ESR1_RX_WRN         BIT(8)
#define FLEXCAN_ESR1_IDLE           BIT(7)
#define FLEXCAN_ESR1_TX             BIT(6)
#define FLEXCAN_ESR1_RX             BIT(3)
#define FLEXCAN_ESR1_BOFF_INT       BIT(2)
#define FLEXCAN_ESR1_ERR_INT        BIT(1)
#define FLEXCAN_ESR1_WAK_INT        BIT(0)

#define FLEXCAN_ESR1_FLT_CONF_SHIFT     (4)
#define FLEXCAN_ESR1_FLT_CONF_MASK      (0x3 << FLEXCAN_ESR1_FLT_CONF_SHIFT)
#define FLEXCAN_ESR1_FLT_CONF_ACTIVE    (0x0 << FLEXCAN_ESR1_FLT_CONF_SHIFT)
#define FLEXCAN_ESR1_FLT_CONF_PASSIVE   (0x1 << FLEXCAN_ESR1_FLT_CONF_SHIFT)

#define FLEXCAN_ESR1_ERR_BUS        (FLEXCAN_ESR1_BIT1_ERR  | \
                                    FLEXCAN_ESR1_BIT0_ERR   | \
                                    FLEXCAN_ESR1_ACK_ERR    | \
                                    FLEXCAN_ESR1_CRC_ERR    | \
                                    FLEXCAN_ESR1_FRM_ERR    | \
                                    FLEXCAN_ESR1_STF_ERR)
#define FLEXCAN_ESR1_ERR_STATE      (FLEXCAN_ESR1_TWRN_INT  | \
                                    FLEXCAN_ESR1_RWRN_INT   | \
                                    FLEXCAN_ESR1_BOFF_INT)
#define FLEXCAN_ESR1_ERR_ALL        (FLEXCAN_ESR1_ERR_BUS   | \
                                    FLEXCAN_ESR1_ERR_STATE  | \
                                    FLEXCAN_ESR1_ERR_INT)


#define FLEXCAN_IFLAG_BUF(x)            BIT(x)
#define FLEXCAN_IFLAG_RX_FIFO_OVERFLOW  BIT(7)
#define FLEXCAN_IFLAG_RX_FIFO_WARN      BIT(6)
#define FLEXCAN_IFLAG_RX_FIFO_AVAILABLE BIT(5)

#define FLEXCAN_IFLAG_DEFAULT           (FLEXCAN_IFLAG_RX_FIFO_OVERFLOW | \
                                        FLEXCAN_IFLAG_RX_FIFO_AVAILABLE)

#define FLEXCAN_MB_NUM                  64
#define FLEXCAN_TX_BUF_ID               8

#define FLEXCAN_MB_CNT_SRR              BIT(22)
#define FLEXCAN_MB_CNT_IDE              BIT(21)
#define FLEXCAN_MB_CNT_RTR              BIT(20)

#define FLEXCAN_MB_CNT_CODE_SHIFT       (24)
#define FLEXCAN_MB_CNT_CODE(x)          (((x) & 0xf) << 24)
#define FLEXCAN_MB_CNT_LENGTH(x)        (((x) & 0xf) << 16)
#define FLEXCAN_MB_CNT_TIMESTAMP(x)     ((x) & 0xffff)
#define FLEXCAN_MB_CODE_MASK            (0xf0ffffff)

#endif                                                                  /* FLEXCANREGS_H_               */
/*********************************************************************************************************
  END
*********************************************************************************************************/
