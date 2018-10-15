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
** 文   件   名: ecspi_regs.h
**
** 创   建   人: Hu.Hanzhang (胡瀚璋)
**
** 文件创建日期: 2016 年 06 月 12 日
**
** 描        述: IMX6 处理器 ECSPI 寄存器
*********************************************************************************************************/
#ifndef ECSPI_REGS_H_
#define ECSPI_REGS_H_

#define ECSPI_RXDATA        			(pSpiChannel->SPICHAN_pVirtAddrBase + 0x0)
#define ECSPI_TXDATA                    (pSpiChannel->SPICHAN_pVirtAddrBase + 0x4)
#define ECSPI_CONREG                    (pSpiChannel->SPICHAN_pVirtAddrBase + 0x8)
#define ECSPI_CONFIGREG                 (pSpiChannel->SPICHAN_pVirtAddrBase + 0xc)
#define ECSPI_INTREG                    (pSpiChannel->SPICHAN_pVirtAddrBase + 0x10)
#define ECSPI_DMAREG                    (pSpiChannel->SPICHAN_pVirtAddrBase + 0x14)
#define ECSPI_STATREG                   (pSpiChannel->SPICHAN_pVirtAddrBase + 0x18)
#define ECSPI_PERIODREG                 (pSpiChannel->SPICHAN_pVirtAddrBase + 0x1c)
#define ECSPI_TESTREG                   (pSpiChannel->SPICHAN_pVirtAddrBase + 0x20)
#define ECSPI_MSGDATA                   (pSpiChannel->SPICHAN_pVirtAddrBase + 0x40)

#define ECSPI_CONREG_EN                 (0x1 << 0)
#define ECSPI_CONREG_HT                 (0x1 << 1)
#define ECSPI_CONREG_XCH                (0x1 << 2)
#define ECSPI_CONREG_SMC                (0x1 << 3)
#define ECSPI_CONREG_CHMODE             (0xf << 4)
#define ECSPI_CONREG_POSTDIVIDER        (0xf << 8)
#define ECSPI_CONREG_PREDIVIDER         (0xf << 12)
#define ECSPI_CONREG_DRCTL              (0x3 << 16)
#define ECSPI_CONREG_CHSELECT           (0x3 << 18)
#define ECSPI_CONREG_BURSTLENGTH        (0xfff << 20)

#define ECSPI_CONFIGREG_SCLK_PHA        (0xf << 0)
#define ECSPI_CONFIGREG_SCLK_POL        (0xf << 4)
#define ECSPI_CONFIGREG_SS_CTL          (0xf << 8)
#define ECSPI_CONFIGREG_SS_POL          (0xf << 12)
#define ECSPI_CONFIGREG_DATA_CTL        (0xf << 16)
#define ECSPI_CONFIGREG_SCLK_CTL        (0xf << 20)
#define ECSPI_CONFIGREG_HT_LENGTH       (0xf << 24)

#define BP_ECSPI_STATREG_RO             (6)
#define BM_ECSPI_STATREG_RO             (0x00000040)
#define BP_ECSPI_STATREG_TC             (7)
#define BM_ECSPI_STATREG_TC             (0x00000080)

#endif                                                                  /*  ECSPI_REGS_H_               */
/*********************************************************************************************************
  END
*********************************************************************************************************/
