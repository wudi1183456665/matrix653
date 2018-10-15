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
** 文   件   名: board.h
**
** 创   建   人: zhang.xu (张旭)
**
** 文件创建日期: 2016年2月23日
**
** 描        述: 板级相关支持函数
*********************************************************************************************************/
#ifndef BOARD_H_
#define BOARD_H_

/*********************************************************************************************************
  目标电路板的 bsp 适配函数
*********************************************************************************************************/
VOID boardTimeInit(VOID);
VOID boardBusInit(VOID);
VOID boardDrvInit(VOID);
VOID boardDevInit(VOID);
VOID boardNetifAttch(VOID);
INT  boardGpioCheck(UINT32  uiOffset);
INT  boardSdQurikInfo(INT iChannel);
/*********************************************************************************************************
  目标电路板的 管脚分配函数
*********************************************************************************************************/
VOID arm_iomux_config(VOID);
VOID asrc_iomux_config(VOID);
VOID audmux_iomux_config(VOID);
VOID ccm_iomux_config(VOID);
VOID dcic1_iomux_config(VOID);
VOID dcic2_iomux_config(VOID);
VOID sabrelite_spi1_iomux_cfg(VOID);
VOID ecspi1_iomux_config(VOID);
VOID ecspi2_iomux_config(VOID);
VOID ecspi3_iomux_config(VOID);
VOID ecspi4_iomux_config(VOID);
VOID ecspi5_iomux_config(VOID);
VOID eim_iomux_config(VOID);
VOID enet_iomux_config(VOID);
VOID enet_iomux_reconfig(VOID);
VOID imx_enet_iomux(VOID);
VOID epit1_iomux_config(VOID);
VOID epit2_iomux_config(VOID);
VOID esai_iomux_config(VOID);
VOID flexcan1_iomux_config(VOID);
VOID flexcan2_iomux_config(VOID);
VOID gpmi_iomux_config(VOID);
VOID gpt_iomux_config(VOID);
VOID hdmi_iomux_config(VOID);
VOID i2c1_iomux_config(VOID);
VOID i2c2_iomux_config(VOID);
VOID i2c3_iomux_config(VOID);
VOID ipu1_iomux_config(VOID);
VOID ipu2_iomux_config(VOID);
VOID kpp_iomux_config(VOID);
VOID ldb_iomux_config(VOID);
VOID mipi_csi_iomux_config(VOID);
VOID mipi_dsi_iomux_config(VOID);
VOID mipi_hsi_iomux_config(VOID);
VOID mlb_iomux_config(VOID);
VOID mmdc_iomux_config(VOID);
VOID pcie_iomux_config(VOID);
VOID pmu_iomux_config(VOID);
VOID pwm1_iomux_config(VOID);
VOID pwm2_iomux_config(VOID);
VOID pwm3_iomux_config(VOID);
VOID pwm4_iomux_config(VOID);
VOID sata_phy_iomux_config(VOID);
VOID sdma_iomux_config(VOID);
VOID sjc_iomux_config(VOID);
VOID snvs_iomux_config(VOID);
VOID spdif_iomux_config(VOID);
VOID src_iomux_config(VOID);
VOID uart1_iomux_config(VOID);
VOID uart2_iomux_config(VOID);
VOID uart3_iomux_config(VOID);
VOID uart4_iomux_config(VOID);
VOID uart5_iomux_config(VOID);
VOID usb_iomux_config(VOID);
VOID usdhc1_iomux_config(VOID);
VOID usdhc2_iomux_config(VOID);
VOID usdhc3_iomux_config(VOID);
VOID usdhc4_iomux_config(VOID);
VOID wdog1_iomux_config(VOID);
VOID wdog2_iomux_config(VOID);
VOID xtalosc_iomux_config(VOID);
VOID hdmi_pgm_iomux(VOID);
VOID ipu1_lcdif_iomux_config(VOID);
VOID e9_i2c1_iomux_cfg(VOID);

#endif                                                                  /* BOARD_H_                     */
/*********************************************************************************************************
  END
*********************************************************************************************************/
