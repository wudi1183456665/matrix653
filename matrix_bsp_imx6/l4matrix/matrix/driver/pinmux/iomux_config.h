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
** 文   件   名: iomux_config.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 01 月 21 日
**
** 描        述: imx6q 处理器 PINMUX 处理头文件
*********************************************************************************************************/
#ifndef _IOMUX_CONFIG_H_
#define _IOMUX_CONFIG_H_

#include "../sdk.h"

/*********************************************************************************************************
 * 管脚分配需要的寄存器宏定义
 ********************************************************************************************************/
#ifndef REGS_IOMUXC_BASE
#define HW_IOMUXC_INSTANCE_COUNT (1)
#define REGS_IOMUXC_BASE (0x020e0000)
#endif

//-------------------------------------------------------------------------------------------
// HW_IOMUXC_GPR3 - GPR
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_IOMUXC_GPR3 - GPR (RW)
 *
 * Reset value: 0x01e00000
 */
typedef union _hw_iomuxc_gpr3
{
    reg32_t U;
    struct _hw_iomuxc_gpr3_bitfields
    {
        unsigned RESERVED0           : 2; //!< [1:0] Reserved
        unsigned HDMI_MUX_CTL        : 2; //!< [3:2] HDMI MUX control
        unsigned MIPI_MUX_CTL        : 2; //!< [5:4] MIPI MUX control
        unsigned LVDS0_MUX_CTL       : 2; //!< [7:6] LVDS0 MUX control
        unsigned LVDS1_MUX_CTL       : 2; //!< [9:8] LVDS1 MUX control
        unsigned IPU_DIAG            : 1; //!< [10] IPU diagnostic debug bus mux
        unsigned TZASC1_BOOT_LOCK    : 1; //!< [11] TZASC-1 secure boot lock
        unsigned TZASC2_BOOT_LOCK    : 1; //!< [12] TZASC-2 secure boot lock
        unsigned CORE0_DBG_ACK_EN    : 1; //!< [13] Mask control of Core 1 debug acknowledge to global debug acknowledge
        unsigned CORE1_DBG_ACK_EN    : 1; //!< [14] Mask control of Core 1 debug acknowledge to global debug acknowledge.
        unsigned CORE2_DBG_ACK_EN    : 1; //!< [15] Mask control of Core 2 debug acknowledge to global debug acknowledge
        unsigned CORE3_DBG_ACK_EN    : 1; //!< [16] Mask control of Core 3 debug acknowledge to global debug acknowledge
        unsigned OCRAM_STATUS        : 4; //!< [20:17] This field shows the OCRAM pipeline settings status, controlled by OCRAM_CTL[24:21] bits respectively.
        unsigned OCRAM_CTL           : 4; //!< [24:21] OCRAM_CTL[24] write address pipeline control bit.
        unsigned USDHCX_RD_CACHE_CTL : 1; //!< [25] Control uSDHCx [1-4] blocks cacheable attribute of AXI read transactions
        unsigned USDHCX_WR_CACHE_CTL : 1; //!< [26] Control uSDHCx [1-4] blocks cacheable attribute of AXI write transactions
        unsigned BCH_RD_CACHE_CTL    : 1; //!< [27] Control BCH block cacheable attribute of AXI read transactions Set of the cache bits, enable packet optimization through the bus system to DDR controller.
        unsigned BCH_WR_CACHE_CTL    : 1; //!< [28] Control BCH block cacheable attribute of AXI write transactions
        unsigned GPU_DBG             : 2; //!< [30:29] GPU debug busses to IOMUX
        unsigned RESERVED1           : 1; //!< [31] Reserved
    } B;
} hw_iomuxc_gpr3_t;
#endif

/*!
 * @name Constants and macros for entire IOMUXC_GPR3 register
 */
//@{
#define HW_IOMUXC_GPR3_ADDR      (REGS_IOMUXC_BASE + 0xc)

#ifndef __LANGUAGE_ASM__
#define HW_IOMUXC_GPR3           (*(volatile hw_iomuxc_gpr3_t *) HW_IOMUXC_GPR3_ADDR)
#define HW_IOMUXC_GPR3_RD()      (HW_IOMUXC_GPR3.U)
#define HW_IOMUXC_GPR3_WR(v)     (HW_IOMUXC_GPR3.U = (v))
#define HW_IOMUXC_GPR3_SET(v)    (HW_IOMUXC_GPR3_WR(HW_IOMUXC_GPR3_RD() |  (v)))
#define HW_IOMUXC_GPR3_CLR(v)    (HW_IOMUXC_GPR3_WR(HW_IOMUXC_GPR3_RD() & ~(v)))
#define HW_IOMUXC_GPR3_TOG(v)    (HW_IOMUXC_GPR3_WR(HW_IOMUXC_GPR3_RD() ^  (v)))
#endif

/*  一下为正式使用，上面的这个结构体后续在修改代码中删除  */

#define HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_MOD_ADDR              (REGS_IOMUXC_BASE + 0x67c)
#define HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TCK_ADDR              (REGS_IOMUXC_BASE + 0x688)
#define HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDI_ADDR              (REGS_IOMUXC_BASE + 0x684)
#define HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDO_ADDR              (REGS_IOMUXC_BASE + 0x68c)
#define HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TMS_ADDR              (REGS_IOMUXC_BASE + 0x678)
#define HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TRSTB_ADDR            (REGS_IOMUXC_BASE + 0x680)

#define HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR        (REGS_IOMUXC_BASE + 0x790)
#define HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR            (REGS_IOMUXC_BASE + 0x7ac)


#define HW_IOMUXC_FLEXCAN1_RX_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x7e4)
#define HW_IOMUXC_FLEXCAN2_RX_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x7e8)
#define HW_IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x7f4)
#define HW_IOMUXC_ECSPI1_MISO_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x7f8)
#define HW_IOMUXC_ECSPI1_MOSI_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x7fc)
#define HW_IOMUXC_ECSPI1_SS0_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x800)
#define HW_IOMUXC_ECSPI1_SS1_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x804)
#define HW_IOMUXC_ECSPI1_SS2_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x808)
#define HW_IOMUXC_ECSPI1_SS3_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x80c)

#define HW_IOMUXC_ECSPI2_CSPI_CLK_IN_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x810)
#define HW_IOMUXC_ECSPI2_MISO_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x814)
#define HW_IOMUXC_ECSPI2_MOSI_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x818)
#define HW_IOMUXC_ECSPI2_SS0_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x81c)
#define HW_IOMUXC_ECSPI2_SS1_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x820)


#define HW_IOMUXC_ECSPI4_SS0_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x824)
#define HW_IOMUXC_ECSPI5_CSPI_CLK_IN_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x828)
#define HW_IOMUXC_ECSPI5_MISO_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x82c)
#define HW_IOMUXC_ECSPI5_MOSI_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x830)
#define HW_IOMUXC_ECSPI5_SS0_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x834)
#define HW_IOMUXC_ECSPI5_SS1_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x838)

#define HW_IOMUXC_ENET_REF_CLK_SELECT_INPUT_ADDR            (REGS_IOMUXC_BASE + 0x83c)
#define HW_IOMUXC_ENET_MAC0_MDIO_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x840)
#define HW_IOMUXC_ENET_MAC0_RX_CLK_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x844)
#define HW_IOMUXC_ENET_MAC0_RX_DATA0_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x848)
#define HW_IOMUXC_ENET_MAC0_RX_DATA1_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x84c)
#define HW_IOMUXC_ENET_MAC0_RX_DATA2_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x850)
#define HW_IOMUXC_ENET_MAC0_RX_DATA3_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x854)
#define HW_IOMUXC_ENET_MAC0_RX_EN_SELECT_INPUT_ADDR         (REGS_IOMUXC_BASE + 0x858)
#define HW_IOMUXC_ESAI_RX_FS_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x85c)
#define HW_IOMUXC_ESAI_TX_FS_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x860)
#define HW_IOMUXC_ESAI_RX_HF_CLK_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x864)
#define HW_IOMUXC_ESAI_TX_HF_CLK_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x868)
#define HW_IOMUXC_ESAI_RX_CLK_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x86c)
#define HW_IOMUXC_ESAI_TX_CLK_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x870)
#define HW_IOMUXC_ESAI_SDO0_SELECT_INPUT_ADDR               (REGS_IOMUXC_BASE + 0x874)
#define HW_IOMUXC_ESAI_SDO1_SELECT_INPUT_ADDR               (REGS_IOMUXC_BASE + 0x878)

#define HW_IOMUXC_ESAI_SDO2_SDI3_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x87c)
#define HW_IOMUXC_ESAI_SDO3_SDI2_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x880)
#define HW_IOMUXC_ESAI_SDO5_SDI0_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x888)
#define HW_IOMUXC_HDMI_ICECIN_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x88c)
#define HW_IOMUXC_HDMI_II2C_CLKIN_SELECT_INPUT_ADDR         (REGS_IOMUXC_BASE + 0x890)
#define HW_IOMUXC_HDMI_II2C_DATAIN_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x894)
#define HW_IOMUXC_I2C1_SCL_IN_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x898)
#define HW_IOMUXC_I2C1_SDA_IN_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x89c)
#define HW_IOMUXC_I2C2_SCL_IN_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x8a0)
#define HW_IOMUXC_I2C2_SDA_IN_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x8a4)
#define HW_IOMUXC_I2C3_SCL_IN_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x8a8)
#define HW_IOMUXC_I2C3_SDA_IN_SELECT_INPUT_ADDR             (REGS_IOMUXC_BASE + 0x8ac)
#define HW_IOMUXC_IPU2_SENS1_DATA10_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8b0)
#define HW_IOMUXC_IPU2_SENS1_DATA11_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8b4)
#define HW_IOMUXC_IPU2_SENS1_DATA12_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8b8)
#define HW_IOMUXC_IPU2_SENS1_DATA13_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8bc)
#define HW_IOMUXC_IPU2_SENS1_DATA14_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8c0)
#define HW_IOMUXC_IPU2_SENS1_DATA15_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8c4)
#define HW_IOMUXC_IPU2_SENS1_DATA16_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8c8)
#define HW_IOMUXC_IPU2_SENS1_DATA17_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8cc)
#define HW_IOMUXC_IPU2_SENS1_DATA18_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8d0)
#define HW_IOMUXC_IPU2_SENS1_DATA19_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x8d4)
#define HW_IOMUXC_IPU2_SENS1_DATA_EN_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x8d8)
#define HW_IOMUXC_IPU2_SENS1_HSYNC_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x8dc)
#define HW_IOMUXC_IPU2_SENS1_PIX_CLK_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x8e0)
#define HW_IOMUXC_IPU2_SENS1_VSYNC_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x8e4)
#define HW_IOMUXC_KEY_COL5_SELECT_INPUT_ADDR                (REGS_IOMUXC_BASE + 0x8e8)
#define HW_IOMUXC_KEY_COL6_SELECT_INPUT_ADDR                (REGS_IOMUXC_BASE + 0x8ec)
#define HW_IOMUXC_KEY_COL7_SELECT_INPUT_ADDR                (REGS_IOMUXC_BASE + 0x8f0)
#define HW_IOMUXC_KEY_ROW5_SELECT_INPUT_ADDR                (REGS_IOMUXC_BASE + 0x8f4)
#define HW_IOMUXC_KEY_ROW6_SELECT_INPUT_ADDR                (REGS_IOMUXC_BASE + 0x8f8)
#define HW_IOMUXC_KEY_ROW7_SELECT_INPUT_ADDR                (REGS_IOMUXC_BASE + 0x8fc)
#define HW_IOMUXC_MLB_MLB_CLK_IN_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x900)
#define HW_IOMUXC_MLB_MLB_DATA_IN_SELECT_INPUT_ADDR         (REGS_IOMUXC_BASE + 0x904)
#define HW_IOMUXC_MLB_MLB_SIG_IN_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x908)
#define HW_IOMUXC_SDMA_EVENTS14_SELECT_INPUT_ADDR           (REGS_IOMUXC_BASE + 0x90c)
#define HW_IOMUXC_SDMA_EVENTS15_SELECT_INPUT_ADDR           (REGS_IOMUXC_BASE + 0x910)
#define HW_IOMUXC_SPDIF_SPDIF_IN1_SELECT_INPUT_ADDR         (REGS_IOMUXC_BASE + 0x914)
#define HW_IOMUXC_SPDIF_TX_CLK2_SELECT_INPUT_ADDR           (REGS_IOMUXC_BASE + 0x918)
#define HW_IOMUXC_UART1_UART_RTS_B_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x91c)
#define HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x920)
#define HW_IOMUXC_UART2_UART_RTS_B_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x924)
#define HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x928)
#define HW_IOMUXC_UART3_UART_RTS_B_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x92c)
#define HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x930)
#define HW_IOMUXC_UART4_UART_RTS_B_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x934)
#define HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x938)
#define HW_IOMUXC_UART5_UART_RTS_B_SELECT_INPUT_ADDR        (REGS_IOMUXC_BASE + 0x93c)
#define HW_IOMUXC_UART5_UART_RX_DATA_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x940)
#define HW_IOMUXC_USB_OTG_OC_SELECT_INPUT_ADDR              (REGS_IOMUXC_BASE + 0x944)
#define HW_IOMUXC_USB_H1_OC_SELECT_INPUT_ADDR               (REGS_IOMUXC_BASE + 0x948)
#define HW_IOMUXC_USDHC1_WP_ON_SELECT_INPUT_ADDR            (REGS_IOMUXC_BASE + 0x94c)
#define HW_IOMUXC_ASRC_ASRCK_CLOCK_6_SELECT_INPUT_ADDR      (REGS_IOMUXC_BASE + 0x7b0)
#define HW_IOMUXC_AUD4_INPUT_DA_AMX_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x7b4)
#define HW_IOMUXC_AUD4_INPUT_DB_AMX_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x7b8)
#define HW_IOMUXC_AUD4_INPUT_RXCLK_AMX_SELECT_INPUT_ADDR    (REGS_IOMUXC_BASE + 0x7bc)
#define HW_IOMUXC_AUD4_INPUT_RXFS_AMX_SELECT_INPUT_ADDR     (REGS_IOMUXC_BASE + 0x7c0)
#define HW_IOMUXC_AUD4_INPUT_TXCLK_AMX_SELECT_INPUT_ADDR    (REGS_IOMUXC_BASE + 0x7c4)
#define HW_IOMUXC_AUD4_INPUT_TXFS_AMX_SELECT_INPUT_ADDR     (REGS_IOMUXC_BASE + 0x7c8)
#define HW_IOMUXC_AUD5_INPUT_DA_AMX_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x7cc)
#define HW_IOMUXC_AUD5_INPUT_DB_AMX_SELECT_INPUT_ADDR       (REGS_IOMUXC_BASE + 0x7d0)
#define HW_IOMUXC_AUD5_INPUT_RXCLK_AMX_SELECT_INPUT_ADDR    (REGS_IOMUXC_BASE + 0x7d4)
#define HW_IOMUXC_AUD5_INPUT_RXFS_AMX_SELECT_INPUT_ADDR     (REGS_IOMUXC_BASE + 0x7d8)
#define HW_IOMUXC_AUD5_INPUT_TXCLK_AMX_SELECT_INPUT_ADDR    (REGS_IOMUXC_BASE + 0x7dc)
#define HW_IOMUXC_AUD5_INPUT_TXFS_AMX_SELECT_INPUT_ADDR     (REGS_IOMUXC_BASE + 0x7e0)
#define HW_IOMUXC_CCM_PMIC_READY_SELECT_INPUT_ADDR          (REGS_IOMUXC_BASE + 0x7f0)

/*----------------------------------------- 电气特性设置寄存器定义 -------------------------------------*/
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO00_ADDR                (REGS_IOMUXC_BASE + 0x5f0)    // Pin 0
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO01_ADDR                (REGS_IOMUXC_BASE + 0x5f4)    // Pin 1
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO02_ADDR                (REGS_IOMUXC_BASE + 0x604)    // Pin 2
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO03_ADDR                (REGS_IOMUXC_BASE + 0x5fc)    // Pin 3
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO04_ADDR                (REGS_IOMUXC_BASE + 0x608)    // Pin 4
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO05_ADDR                (REGS_IOMUXC_BASE + 0x60c)    // Pin 5
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO06_ADDR                (REGS_IOMUXC_BASE + 0x600)    // Pin 6
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO07_ADDR                (REGS_IOMUXC_BASE + 0x610)    // Pin 7
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO08_ADDR                (REGS_IOMUXC_BASE + 0x614)    // Pin 8
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO09_ADDR                (REGS_IOMUXC_BASE + 0x5f8)    // Pin 9
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD2_CLK_ADDR               (REGS_IOMUXC_BASE + 0x73c)    // Pin 10
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD2_CMD_ADDR               (REGS_IOMUXC_BASE + 0x740)    // Pin 11
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA3_ADDR             (REGS_IOMUXC_BASE + 0x744)    // Pin 12
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA2_ADDR             (REGS_IOMUXC_BASE + 0x364)    // Pin 13
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA1_ADDR             (REGS_IOMUXC_BASE + 0x360)    // Pin 14
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA0_ADDR             (REGS_IOMUXC_BASE + 0x368)    // Pin 15
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA0_ADDR             (REGS_IOMUXC_BASE + 0x728)    // Pin 16
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA1_ADDR             (REGS_IOMUXC_BASE + 0x724)    // Pin 17
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD1_CMD_ADDR               (REGS_IOMUXC_BASE + 0x730)    // Pin 18
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA2_ADDR             (REGS_IOMUXC_BASE + 0x734)    // Pin 19
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD1_CLK_ADDR               (REGS_IOMUXC_BASE + 0x738)    // Pin 20
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA3_ADDR             (REGS_IOMUXC_BASE + 0x72c)    // Pin 21
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_MDIO_ADDR             (REGS_IOMUXC_BASE + 0x4e4)    // Pin 22
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_REF_CLK_ADDR          (REGS_IOMUXC_BASE + 0x4e8)    // Pin 23
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_RX_ER_ADDR            (REGS_IOMUXC_BASE + 0x4ec)    // Pin 24
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_CRS_DV_ADDR           (REGS_IOMUXC_BASE + 0x4f0)    // Pin 25
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_RX_DATA1_ADDR         (REGS_IOMUXC_BASE + 0x4f4)    // Pin 26
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_RX_DATA0_ADDR         (REGS_IOMUXC_BASE + 0x4f8)    // Pin 27
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_TX_EN_ADDR            (REGS_IOMUXC_BASE + 0x4fc)    // Pin 28
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_TX_DATA1_ADDR         (REGS_IOMUXC_BASE + 0x500)    // Pin 29
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_TX_DATA0_ADDR         (REGS_IOMUXC_BASE + 0x504)    // Pin 30
#define HW_IOMUXC_SW_PAD_CTL_PAD_ENET_MDC_ADDR              (REGS_IOMUXC_BASE + 0x508)    // Pin 31
// Bank 2
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA00_ADDR           (REGS_IOMUXC_BASE + 0x6e4)    // Pin 0
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA01_ADDR           (REGS_IOMUXC_BASE + 0x6e8)    // Pin 1
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA02_ADDR           (REGS_IOMUXC_BASE + 0x6ec)    // Pin 2
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA03_ADDR           (REGS_IOMUXC_BASE + 0x6f0)    // Pin 3
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04_ADDR           (REGS_IOMUXC_BASE + 0x6f4)    // Pin 4
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA05_ADDR           (REGS_IOMUXC_BASE + 0x6f8)    // Pin 5
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA06_ADDR           (REGS_IOMUXC_BASE + 0x6fc)    // Pin 6
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA07_ADDR           (REGS_IOMUXC_BASE + 0x700)    // Pin 7
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA0_ADDR             (REGS_IOMUXC_BASE + 0x704)    // Pin 8
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA1_ADDR             (REGS_IOMUXC_BASE + 0x708)    // Pin 9
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA2_ADDR             (REGS_IOMUXC_BASE + 0x70c)    // Pin 10
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA3_ADDR             (REGS_IOMUXC_BASE + 0x710)    // Pin 11
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA4_ADDR             (REGS_IOMUXC_BASE + 0x714)    // Pin 12
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA5_ADDR             (REGS_IOMUXC_BASE + 0x718)    // Pin 13
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA6_ADDR             (REGS_IOMUXC_BASE + 0x71c)    // Pin 14
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA7_ADDR             (REGS_IOMUXC_BASE + 0x720)    // Pin 15
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR22_ADDR            (REGS_IOMUXC_BASE + 0x3f0)    // Pin 16
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR21_ADDR            (REGS_IOMUXC_BASE + 0x3f4)    // Pin 17
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR20_ADDR            (REGS_IOMUXC_BASE + 0x3f8)    // Pin 18
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR19_ADDR            (REGS_IOMUXC_BASE + 0x3fc)    // Pin 19
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR18_ADDR            (REGS_IOMUXC_BASE + 0x400)    // Pin 20
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR17_ADDR            (REGS_IOMUXC_BASE + 0x404)    // Pin 21
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR16_ADDR            (REGS_IOMUXC_BASE + 0x408)    // Pin 22
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_CS0_ADDR               (REGS_IOMUXC_BASE + 0x40c)    // Pin 23
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_CS1_ADDR               (REGS_IOMUXC_BASE + 0x410)    // Pin 24
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_OE_ADDR                (REGS_IOMUXC_BASE + 0x414)    // Pin 25
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_RW_ADDR                (REGS_IOMUXC_BASE + 0x418)    // Pin 26
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_LBA_ADDR               (REGS_IOMUXC_BASE + 0x41c)    // Pin 27
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_EB0_ADDR               (REGS_IOMUXC_BASE + 0x420)    // Pin 28
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_EB1_ADDR               (REGS_IOMUXC_BASE + 0x424)    // Pin 29
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_EB2_ADDR               (REGS_IOMUXC_BASE + 0x3a0)    // Pin 30
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_EB3_ADDR               (REGS_IOMUXC_BASE + 0x3c4)    // Pin 31
// Bank 3
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD00_ADDR              (REGS_IOMUXC_BASE + 0x428)    // Pin 0
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD01_ADDR              (REGS_IOMUXC_BASE + 0x42c)    // Pin 1
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD02_ADDR              (REGS_IOMUXC_BASE + 0x430)    // Pin 2
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD03_ADDR              (REGS_IOMUXC_BASE + 0x434)    // Pin 3
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD04_ADDR              (REGS_IOMUXC_BASE + 0x438)    // Pin 4
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD05_ADDR              (REGS_IOMUXC_BASE + 0x43c)    // Pin 5
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD06_ADDR              (REGS_IOMUXC_BASE + 0x440)    // Pin 6
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD07_ADDR              (REGS_IOMUXC_BASE + 0x444)    // Pin 7
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD08_ADDR              (REGS_IOMUXC_BASE + 0x448)    // Pin 8
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD09_ADDR              (REGS_IOMUXC_BASE + 0x44c)    // Pin 9
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD10_ADDR              (REGS_IOMUXC_BASE + 0x450)    // Pin 10
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD11_ADDR              (REGS_IOMUXC_BASE + 0x454)    // Pin 11
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD12_ADDR              (REGS_IOMUXC_BASE + 0x458)    // Pin 12
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD13_ADDR              (REGS_IOMUXC_BASE + 0x45c)    // Pin 13
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD14_ADDR              (REGS_IOMUXC_BASE + 0x460)    // Pin 14
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_AD15_ADDR              (REGS_IOMUXC_BASE + 0x464)    // Pin 15
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA16_ADDR            (REGS_IOMUXC_BASE + 0x3a4)    // Pin 16
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA17_ADDR            (REGS_IOMUXC_BASE + 0x3a8)    // Pin 17
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA18_ADDR            (REGS_IOMUXC_BASE + 0x3ac)    // Pin 18
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA19_ADDR            (REGS_IOMUXC_BASE + 0x3b0)    // Pin 19
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA20_ADDR            (REGS_IOMUXC_BASE + 0x3b4)    // Pin 20
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA21_ADDR            (REGS_IOMUXC_BASE + 0x3b8)    // Pin 21
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA22_ADDR            (REGS_IOMUXC_BASE + 0x3bc)    // Pin 22
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA23_ADDR            (REGS_IOMUXC_BASE + 0x3c0)    // Pin 23
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA24_ADDR            (REGS_IOMUXC_BASE + 0x3c8)    // Pin 24
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA25_ADDR            (REGS_IOMUXC_BASE + 0x3cc)    // Pin 25
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_ADDR            (REGS_IOMUXC_BASE + 0x3d0)    // Pin 26
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_ADDR            (REGS_IOMUXC_BASE + 0x3d4)    // Pin 27
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA28_ADDR            (REGS_IOMUXC_BASE + 0x3d8)    // Pin 28
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA29_ADDR            (REGS_IOMUXC_BASE + 0x3dc)    // Pin 29
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_ADDR            (REGS_IOMUXC_BASE + 0x3e0)    // Pin 30
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA31_ADDR            (REGS_IOMUXC_BASE + 0x3e4)    // Pin 31
// Bank 4
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO19_ADDR                (REGS_IOMUXC_BASE + 0x624)    // Pin 5
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_ADDR              (REGS_IOMUXC_BASE + 0x5c8)    // Pin 6
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_ADDR              (REGS_IOMUXC_BASE + 0x5cc)    // Pin 7
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_ADDR              (REGS_IOMUXC_BASE + 0x5d0)    // Pin 8
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_ADDR              (REGS_IOMUXC_BASE + 0x5d4)    // Pin 9
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_ADDR              (REGS_IOMUXC_BASE + 0x5d8)    // Pin 10
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_ADDR              (REGS_IOMUXC_BASE + 0x5dc)    // Pin 11
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_ADDR              (REGS_IOMUXC_BASE + 0x5e0)    // Pin 12
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_ADDR              (REGS_IOMUXC_BASE + 0x5e4)    // Pin 13
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_ADDR              (REGS_IOMUXC_BASE + 0x5e8)    // Pin 14
#define HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_ADDR              (REGS_IOMUXC_BASE + 0x5ec)    // Pin 15
#define HW_IOMUXC_SW_PAD_CTL_PAD_DI0_DISP_CLK_ADDR          (REGS_IOMUXC_BASE + 0x470)    // Pin 16
#define HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN15_ADDR             (REGS_IOMUXC_BASE + 0x474)    // Pin 17
#define HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN02_ADDR             (REGS_IOMUXC_BASE + 0x478)    // Pin 18
#define HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN03_ADDR             (REGS_IOMUXC_BASE + 0x47c)    // Pin 19
#define HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN04_ADDR             (REGS_IOMUXC_BASE + 0x480)    // Pin 20
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA00_ADDR          (REGS_IOMUXC_BASE + 0x484)    // Pin 21
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA01_ADDR          (REGS_IOMUXC_BASE + 0x488)    // Pin 22
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA02_ADDR          (REGS_IOMUXC_BASE + 0x48c)    // Pin 23
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA03_ADDR          (REGS_IOMUXC_BASE + 0x490)    // Pin 24
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA04_ADDR          (REGS_IOMUXC_BASE + 0x494)    // Pin 25
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA05_ADDR          (REGS_IOMUXC_BASE + 0x498)    // Pin 26
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA06_ADDR          (REGS_IOMUXC_BASE + 0x49c)    // Pin 27
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA07_ADDR          (REGS_IOMUXC_BASE + 0x4a0)    // Pin 28
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA08_ADDR          (REGS_IOMUXC_BASE + 0x4a4)    // Pin 29
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA09_ADDR          (REGS_IOMUXC_BASE + 0x4a8)    // Pin 30
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA10_ADDR          (REGS_IOMUXC_BASE + 0x4ac)    // Pin 31
// Bank 5
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_WAIT_ADDR              (REGS_IOMUXC_BASE + 0x468)    // Pin 0
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR25_ADDR            (REGS_IOMUXC_BASE + 0x39c)    // Pin 2
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR24_ADDR            (REGS_IOMUXC_BASE + 0x3e8)    // Pin 4
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA11_ADDR          (REGS_IOMUXC_BASE + 0x4b0)    // Pin 5
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA12_ADDR          (REGS_IOMUXC_BASE + 0x4b4)    // Pin 6
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA13_ADDR          (REGS_IOMUXC_BASE + 0x4b8)    // Pin 7
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA14_ADDR          (REGS_IOMUXC_BASE + 0x4bc)    // Pin 8
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA15_ADDR          (REGS_IOMUXC_BASE + 0x4c0)    // Pin 9
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA16_ADDR          (REGS_IOMUXC_BASE + 0x4c4)    // Pin 10
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA17_ADDR          (REGS_IOMUXC_BASE + 0x4c8)    // Pin 11
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA18_ADDR          (REGS_IOMUXC_BASE + 0x4cc)    // Pin 12
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA19_ADDR          (REGS_IOMUXC_BASE + 0x4d0)    // Pin 13
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA20_ADDR          (REGS_IOMUXC_BASE + 0x4d4)    // Pin 14
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA21_ADDR          (REGS_IOMUXC_BASE + 0x4d8)    // Pin 15
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA22_ADDR          (REGS_IOMUXC_BASE + 0x4dc)    // Pin 16
#define HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA23_ADDR          (REGS_IOMUXC_BASE + 0x4e0)    // Pin 17
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_PIXCLK_ADDR           (REGS_IOMUXC_BASE + 0x628)    // Pin 18
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_HSYNC_ADDR            (REGS_IOMUXC_BASE + 0x62c)    // Pin 19
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA_EN_ADDR          (REGS_IOMUXC_BASE + 0x630)    // Pin 20
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_VSYNC_ADDR            (REGS_IOMUXC_BASE + 0x634)    // Pin 21
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA04_ADDR           (REGS_IOMUXC_BASE + 0x638)    // Pin 22
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA05_ADDR           (REGS_IOMUXC_BASE + 0x63c)    // Pin 23
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA06_ADDR           (REGS_IOMUXC_BASE + 0x640)    // Pin 24
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA07_ADDR           (REGS_IOMUXC_BASE + 0x644)    // Pin 25
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA08_ADDR           (REGS_IOMUXC_BASE + 0x648)    // Pin 26
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA09_ADDR           (REGS_IOMUXC_BASE + 0x64c)    // Pin 27
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_ADDR           (REGS_IOMUXC_BASE + 0x650)    // Pin 28
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_ADDR           (REGS_IOMUXC_BASE + 0x654)    // Pin 29
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_ADDR           (REGS_IOMUXC_BASE + 0x658)    // Pin 30
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_ADDR           (REGS_IOMUXC_BASE + 0x65c)    // Pin 31
// Bank 6
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA14_ADDR           (REGS_IOMUXC_BASE + 0x660)    // Pin 0
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA15_ADDR           (REGS_IOMUXC_BASE + 0x664)    // Pin 1
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA16_ADDR           (REGS_IOMUXC_BASE + 0x668)    // Pin 2
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA17_ADDR           (REGS_IOMUXC_BASE + 0x66c)    // Pin 3
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA18_ADDR           (REGS_IOMUXC_BASE + 0x670)    // Pin 4
#define HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA19_ADDR           (REGS_IOMUXC_BASE + 0x674)    // Pin 5
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR23_ADDR            (REGS_IOMUXC_BASE + 0x3ec)    // Pin 6
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_CLE_ADDR              (REGS_IOMUXC_BASE + 0x6bc)    // Pin 7
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_ALE_ADDR              (REGS_IOMUXC_BASE + 0x6c0)    // Pin 8
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_WP_B_ADDR             (REGS_IOMUXC_BASE + 0x6c4)    // Pin 9
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_READY_ADDR            (REGS_IOMUXC_BASE + 0x6c8)    // Pin 10
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_CS0_B_ADDR            (REGS_IOMUXC_BASE + 0x6cc)    // Pin 11
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_CS1_B_ADDR            (REGS_IOMUXC_BASE + 0x6d0)    // Pin 14
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_CS2_B_ADDR            (REGS_IOMUXC_BASE + 0x6d4)    // Pin 15
#define HW_IOMUXC_SW_PAD_CTL_PAD_NAND_CS3_B_ADDR            (REGS_IOMUXC_BASE + 0x6d8)    // Pin 16
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_ADDR             (REGS_IOMUXC_BASE + 0x690)    // Pin 17
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_ADDR             (REGS_IOMUXC_BASE + 0x694)    // Pin 18
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TXC_ADDR             (REGS_IOMUXC_BASE + 0x36c)    // Pin 19
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD0_ADDR             (REGS_IOMUXC_BASE + 0x370)    // Pin 20
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD1_ADDR             (REGS_IOMUXC_BASE + 0x374)    // Pin 21
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD2_ADDR             (REGS_IOMUXC_BASE + 0x378)    // Pin 22
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD3_ADDR             (REGS_IOMUXC_BASE + 0x37c)    // Pin 23
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RX_CTL_ADDR          (REGS_IOMUXC_BASE + 0x380)    // Pin 24
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD0_ADDR             (REGS_IOMUXC_BASE + 0x384)    // Pin 25
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TX_CTL_ADDR          (REGS_IOMUXC_BASE + 0x388)    // Pin 26
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD1_ADDR             (REGS_IOMUXC_BASE + 0x38c)    // Pin 27
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD2_ADDR             (REGS_IOMUXC_BASE + 0x390)    // Pin 28
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD3_ADDR             (REGS_IOMUXC_BASE + 0x394)    // Pin 29
#define HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RXC_ADDR             (REGS_IOMUXC_BASE + 0x398)    // Pin 30
#define HW_IOMUXC_SW_PAD_CTL_PAD_EIM_BCLK_ADDR              (REGS_IOMUXC_BASE + 0x46c)    // Pin 31
// Bank 7
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA5_ADDR             (REGS_IOMUXC_BASE + 0x698)    // Pin 0
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA4_ADDR             (REGS_IOMUXC_BASE + 0x69c)    // Pin 1
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_ADDR               (REGS_IOMUXC_BASE + 0x6a0)    // Pin 2
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_ADDR               (REGS_IOMUXC_BASE + 0x6a4)    // Pin 3
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_ADDR             (REGS_IOMUXC_BASE + 0x6a8)    // Pin 4
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_ADDR             (REGS_IOMUXC_BASE + 0x6ac)    // Pin 5
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_ADDR             (REGS_IOMUXC_BASE + 0x6b0)    // Pin 6
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_ADDR             (REGS_IOMUXC_BASE + 0x6b4)    // Pin 7
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD3_RESET_ADDR             (REGS_IOMUXC_BASE + 0x6b8)    // Pin 8
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_ADDR               (REGS_IOMUXC_BASE + 0x6dc)    // Pin 9
#define HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_ADDR               (REGS_IOMUXC_BASE + 0x6e0)    // Pin 10
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO16_ADDR                (REGS_IOMUXC_BASE + 0x618)    // Pin 11
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO17_ADDR                (REGS_IOMUXC_BASE + 0x61c)    // Pin 12
#define HW_IOMUXC_SW_PAD_CTL_PAD_GPIO18_ADDR                (REGS_IOMUXC_BASE + 0x620)    // Pin 13


/*------------------------------------------- PINMUX 部分代码 ---------------------------------------*/
// Bank 1
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO00_ADDR                (REGS_IOMUXC_BASE + 0x220)    // Pin 0
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO01_ADDR                (REGS_IOMUXC_BASE + 0x224)    // Pin 1
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO02_ADDR                (REGS_IOMUXC_BASE + 0x234)    // Pin 2
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO03_ADDR                (REGS_IOMUXC_BASE + 0x22c)    // Pin 3
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO04_ADDR                (REGS_IOMUXC_BASE + 0x238)    // Pin 4
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO05_ADDR                (REGS_IOMUXC_BASE + 0x23c)    // Pin 5
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO06_ADDR                (REGS_IOMUXC_BASE + 0x230)    // Pin 6
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO07_ADDR                (REGS_IOMUXC_BASE + 0x240)    // Pin 7
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO08_ADDR                (REGS_IOMUXC_BASE + 0x244)    // Pin 8
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO09_ADDR                (REGS_IOMUXC_BASE + 0x228)    // Pin 9
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD2_CLK_ADDR               (REGS_IOMUXC_BASE + 0x354)    // Pin 10
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD2_CMD_ADDR               (REGS_IOMUXC_BASE + 0x358)    // Pin 11
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA3_ADDR             (REGS_IOMUXC_BASE + 0x35c)    // Pin 12
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA2_ADDR             (REGS_IOMUXC_BASE + 0x50)     // Pin 13
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA1_ADDR             (REGS_IOMUXC_BASE + 0x4c)     // Pin 14
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA0_ADDR             (REGS_IOMUXC_BASE + 0x54)     // Pin 15
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA0_ADDR             (REGS_IOMUXC_BASE + 0x340)    // Pin 16
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1_ADDR             (REGS_IOMUXC_BASE + 0x33c)    // Pin 17
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD1_CMD_ADDR               (REGS_IOMUXC_BASE + 0x348)    // Pin 18
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2_ADDR             (REGS_IOMUXC_BASE + 0x34c)    // Pin 19
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD1_CLK_ADDR               (REGS_IOMUXC_BASE + 0x350)    // Pin 20
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3_ADDR             (REGS_IOMUXC_BASE + 0x344)    // Pin 21
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_MDIO_ADDR             (REGS_IOMUXC_BASE + 0x1d0)    // Pin 22
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_REF_CLK_ADDR          (REGS_IOMUXC_BASE + 0x1d4)    // Pin 23
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_RX_ER_ADDR            (REGS_IOMUXC_BASE + 0x1d8)    // Pin 24
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_CRS_DV_ADDR           (REGS_IOMUXC_BASE + 0x1dc)    // Pin 25
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_RX_DATA1_ADDR         (REGS_IOMUXC_BASE + 0x1e0)    // Pin 26
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_RX_DATA0_ADDR         (REGS_IOMUXC_BASE + 0x1e4)    // Pin 27
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_TX_EN_ADDR            (REGS_IOMUXC_BASE + 0x1e8)    // Pin 28
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_TX_DATA1_ADDR         (REGS_IOMUXC_BASE + 0x1ec)    // Pin 29
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_TX_DATA0_ADDR         (REGS_IOMUXC_BASE + 0x1f0)    // Pin 30
#define HW_IOMUXC_SW_MUX_CTL_PAD_ENET_MDC_ADDR              (REGS_IOMUXC_BASE + 0x1f4)    // Pin 31
// Bank 2
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA00_ADDR           (REGS_IOMUXC_BASE + 0x2fc)    // Pin 0
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA01_ADDR           (REGS_IOMUXC_BASE + 0x300)    // Pin 1
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA02_ADDR           (REGS_IOMUXC_BASE + 0x304)    // Pin 2
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA03_ADDR           (REGS_IOMUXC_BASE + 0x308)    // Pin 3
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA04_ADDR           (REGS_IOMUXC_BASE + 0x30c)    // Pin 4
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA05_ADDR           (REGS_IOMUXC_BASE + 0x310)    // Pin 5
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA06_ADDR           (REGS_IOMUXC_BASE + 0x314)    // Pin 6
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA07_ADDR           (REGS_IOMUXC_BASE + 0x318)    // Pin 7
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA0_ADDR             (REGS_IOMUXC_BASE + 0x31c)    // Pin 8
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA1_ADDR             (REGS_IOMUXC_BASE + 0x320)    // Pin 9
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA2_ADDR             (REGS_IOMUXC_BASE + 0x324)    // Pin 10
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA3_ADDR             (REGS_IOMUXC_BASE + 0x328)    // Pin 11
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA4_ADDR             (REGS_IOMUXC_BASE + 0x32c)    // Pin 12
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA5_ADDR             (REGS_IOMUXC_BASE + 0x330)    // Pin 13
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA6_ADDR             (REGS_IOMUXC_BASE + 0x334)    // Pin 14
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA7_ADDR             (REGS_IOMUXC_BASE + 0x338)    // Pin 15
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR22_ADDR            (REGS_IOMUXC_BASE + 0xdc)     // Pin 16
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR21_ADDR            (REGS_IOMUXC_BASE + 0xe0)     // Pin 17
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR20_ADDR            (REGS_IOMUXC_BASE + 0xe4)     // Pin 18
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR19_ADDR            (REGS_IOMUXC_BASE + 0xe8)     // Pin 19
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR18_ADDR            (REGS_IOMUXC_BASE + 0xec)     // Pin 20
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR17_ADDR            (REGS_IOMUXC_BASE + 0xf0)     // Pin 21
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR16_ADDR            (REGS_IOMUXC_BASE + 0xf4)     // Pin 22
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_CS0_ADDR               (REGS_IOMUXC_BASE + 0xf8)     // Pin 23
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_CS1_ADDR               (REGS_IOMUXC_BASE + 0xfc)     // Pin 24
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_OE_ADDR                (REGS_IOMUXC_BASE + 0x100)    // Pin 25
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_RW_ADDR                (REGS_IOMUXC_BASE + 0x104)    // Pin 26
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_LBA_ADDR               (REGS_IOMUXC_BASE + 0x108)    // Pin 27
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB0_ADDR               (REGS_IOMUXC_BASE + 0x10c)    // Pin 28
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB1_ADDR               (REGS_IOMUXC_BASE + 0x110)    // Pin 29
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_ADDR               (REGS_IOMUXC_BASE + 0x8c)     // Pin 30
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB3_ADDR               (REGS_IOMUXC_BASE + 0xb0)     // Pin 31
// Bank 3
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD00_ADDR              (REGS_IOMUXC_BASE + 0x114)    // Pin 0
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD01_ADDR              (REGS_IOMUXC_BASE + 0x118)    // Pin 1
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD02_ADDR              (REGS_IOMUXC_BASE + 0x11c)    // Pin 2
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD03_ADDR              (REGS_IOMUXC_BASE + 0x120)    // Pin 3
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD04_ADDR              (REGS_IOMUXC_BASE + 0x124)    // Pin 4
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD05_ADDR              (REGS_IOMUXC_BASE + 0x128)    // Pin 5
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD06_ADDR              (REGS_IOMUXC_BASE + 0x12c)    // Pin 6
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD07_ADDR              (REGS_IOMUXC_BASE + 0x130)    // Pin 7
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD08_ADDR              (REGS_IOMUXC_BASE + 0x134)    // Pin 8
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD09_ADDR              (REGS_IOMUXC_BASE + 0x138)    // Pin 9
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD10_ADDR              (REGS_IOMUXC_BASE + 0x13c)    // Pin 10
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD11_ADDR              (REGS_IOMUXC_BASE + 0x140)    // Pin 11
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD12_ADDR              (REGS_IOMUXC_BASE + 0x144)    // Pin 12
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD13_ADDR              (REGS_IOMUXC_BASE + 0x148)    // Pin 13
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD14_ADDR              (REGS_IOMUXC_BASE + 0x14c)    // Pin 14
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD15_ADDR              (REGS_IOMUXC_BASE + 0x150)    // Pin 15
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16_ADDR            (REGS_IOMUXC_BASE + 0x90)     // Pin 16
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17_ADDR            (REGS_IOMUXC_BASE + 0x94)     // Pin 17
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18_ADDR            (REGS_IOMUXC_BASE + 0x98)     // Pin 18
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA19_ADDR            (REGS_IOMUXC_BASE + 0x9c)     // Pin 19
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA20_ADDR            (REGS_IOMUXC_BASE + 0xa0)     // Pin 20
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA21_ADDR            (REGS_IOMUXC_BASE + 0xa4)     // Pin 21
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA22_ADDR            (REGS_IOMUXC_BASE + 0xa8)     // Pin 22
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA23_ADDR            (REGS_IOMUXC_BASE + 0xac)     // Pin 23
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA24_ADDR            (REGS_IOMUXC_BASE + 0xb4)     // Pin 24
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25_ADDR            (REGS_IOMUXC_BASE + 0xb8)     // Pin 25
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26_ADDR            (REGS_IOMUXC_BASE + 0xbc)     // Pin 26
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27_ADDR            (REGS_IOMUXC_BASE + 0xc0)     // Pin 27
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA28_ADDR            (REGS_IOMUXC_BASE + 0xc4)     // Pin 28
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA29_ADDR            (REGS_IOMUXC_BASE + 0xc8)     // Pin 29
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30_ADDR            (REGS_IOMUXC_BASE + 0xcc)     // Pin 30
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA31_ADDR            (REGS_IOMUXC_BASE + 0xd0)     // Pin 31
// Bank 4
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO19_ADDR                (REGS_IOMUXC_BASE + 0x254)    // Pin 5
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_ADDR              (REGS_IOMUXC_BASE + 0x1f8)    // Pin 6
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_ADDR              (REGS_IOMUXC_BASE + 0x1fc)    // Pin 7
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_ADDR              (REGS_IOMUXC_BASE + 0x200)    // Pin 8
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_ADDR              (REGS_IOMUXC_BASE + 0x204)    // Pin 9
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL2_ADDR              (REGS_IOMUXC_BASE + 0x208)    // Pin 10
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_ADDR              (REGS_IOMUXC_BASE + 0x20c)    // Pin 11
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_ADDR              (REGS_IOMUXC_BASE + 0x210)    // Pin 12
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_ADDR              (REGS_IOMUXC_BASE + 0x214)    // Pin 13
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_ADDR              (REGS_IOMUXC_BASE + 0x218)    // Pin 14
#define HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW4_ADDR              (REGS_IOMUXC_BASE + 0x21c)    // Pin 15
#define HW_IOMUXC_SW_MUX_CTL_PAD_DI0_DISP_CLK_ADDR          (REGS_IOMUXC_BASE + 0x15c)    // Pin 16
#define HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN15_ADDR             (REGS_IOMUXC_BASE + 0x160)    // Pin 17
#define HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN02_ADDR             (REGS_IOMUXC_BASE + 0x164)    // Pin 18
#define HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN03_ADDR             (REGS_IOMUXC_BASE + 0x168)    // Pin 19
#define HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN04_ADDR             (REGS_IOMUXC_BASE + 0x16c)    // Pin 20
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA00_ADDR          (REGS_IOMUXC_BASE + 0x170)    // Pin 21
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA01_ADDR          (REGS_IOMUXC_BASE + 0x174)    // Pin 22
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA02_ADDR          (REGS_IOMUXC_BASE + 0x178)    // Pin 23
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA03_ADDR          (REGS_IOMUXC_BASE + 0x17c)    // Pin 24
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA04_ADDR          (REGS_IOMUXC_BASE + 0x180)    // Pin 25
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA05_ADDR          (REGS_IOMUXC_BASE + 0x184)    // Pin 26
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA06_ADDR          (REGS_IOMUXC_BASE + 0x188)    // Pin 27
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA07_ADDR          (REGS_IOMUXC_BASE + 0x18c)    // Pin 28
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA08_ADDR          (REGS_IOMUXC_BASE + 0x190)    // Pin 29
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA09_ADDR          (REGS_IOMUXC_BASE + 0x194)    // Pin 30
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA10_ADDR          (REGS_IOMUXC_BASE + 0x198)    // Pin 31
// Bank 5
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_WAIT_ADDR              (REGS_IOMUXC_BASE + 0x154)    // Pin 0
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR25_ADDR            (REGS_IOMUXC_BASE + 0x88)     // Pin 2
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR24_ADDR            (REGS_IOMUXC_BASE + 0xd4)     // Pin 4
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA11_ADDR          (REGS_IOMUXC_BASE + 0x19c)    // Pin 5
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA12_ADDR          (REGS_IOMUXC_BASE + 0x1a0)    // Pin 6
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA13_ADDR          (REGS_IOMUXC_BASE + 0x1a4)    // Pin 7
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA14_ADDR          (REGS_IOMUXC_BASE + 0x1a8)    // Pin 8
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA15_ADDR          (REGS_IOMUXC_BASE + 0x1ac)    // Pin 9
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA16_ADDR          (REGS_IOMUXC_BASE + 0x1b0)    // Pin 10
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA17_ADDR          (REGS_IOMUXC_BASE + 0x1b4)    // Pin 11
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA18_ADDR          (REGS_IOMUXC_BASE + 0x1b8)    // Pin 12
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA19_ADDR          (REGS_IOMUXC_BASE + 0x1bc)    // Pin 13
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA20_ADDR          (REGS_IOMUXC_BASE + 0x1c0)    // Pin 14
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_ADDR          (REGS_IOMUXC_BASE + 0x1c4)    // Pin 15
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA22_ADDR          (REGS_IOMUXC_BASE + 0x1c8)    // Pin 16
#define HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA23_ADDR          (REGS_IOMUXC_BASE + 0x1cc)    // Pin 17
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_PIXCLK_ADDR           (REGS_IOMUXC_BASE + 0x258)    // Pin 18
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_HSYNC_ADDR            (REGS_IOMUXC_BASE + 0x25c)    // Pin 19
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA_EN_ADDR          (REGS_IOMUXC_BASE + 0x260)    // Pin 20
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_VSYNC_ADDR            (REGS_IOMUXC_BASE + 0x264)    // Pin 21
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA04_ADDR           (REGS_IOMUXC_BASE + 0x268)    // Pin 22
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA05_ADDR           (REGS_IOMUXC_BASE + 0x26c)    // Pin 23
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA06_ADDR           (REGS_IOMUXC_BASE + 0x270)    // Pin 24
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA07_ADDR           (REGS_IOMUXC_BASE + 0x274)    // Pin 25
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08_ADDR           (REGS_IOMUXC_BASE + 0x278)    // Pin 26
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09_ADDR           (REGS_IOMUXC_BASE + 0x27c)    // Pin 27
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10_ADDR           (REGS_IOMUXC_BASE + 0x280)    // Pin 28
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11_ADDR           (REGS_IOMUXC_BASE + 0x284)    // Pin 29
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12_ADDR           (REGS_IOMUXC_BASE + 0x288)    // Pin 30
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13_ADDR           (REGS_IOMUXC_BASE + 0x28c)    // Pin 31
// Bank 6
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA14_ADDR           (REGS_IOMUXC_BASE + 0x290)    // Pin 0
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA15_ADDR           (REGS_IOMUXC_BASE + 0x294)    // Pin 1
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA16_ADDR           (REGS_IOMUXC_BASE + 0x298)    // Pin 2
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA17_ADDR           (REGS_IOMUXC_BASE + 0x29c)    // Pin 3
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA18_ADDR           (REGS_IOMUXC_BASE + 0x2a0)    // Pin 4
#define HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA19_ADDR           (REGS_IOMUXC_BASE + 0x2a4)    // Pin 5
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR23_ADDR            (REGS_IOMUXC_BASE + 0xd8)     // Pin 6
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CLE_ADDR              (REGS_IOMUXC_BASE + 0x2d4)    // Pin 7
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_ALE_ADDR              (REGS_IOMUXC_BASE + 0x2d8)    // Pin 8
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_WP_B_ADDR             (REGS_IOMUXC_BASE + 0x2dc)    // Pin 9
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_READY_ADDR            (REGS_IOMUXC_BASE + 0x2e0)    // Pin 10
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CS0_B_ADDR            (REGS_IOMUXC_BASE + 0x2e4)    // Pin 11
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CS1_B_ADDR            (REGS_IOMUXC_BASE + 0x2e8)    // Pin 14
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CS2_B_ADDR            (REGS_IOMUXC_BASE + 0x2ec)    // Pin 15
#define HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CS3_B_ADDR            (REGS_IOMUXC_BASE + 0x2f0)    // Pin 16
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7_ADDR             (REGS_IOMUXC_BASE + 0x2a8)    // Pin 17
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6_ADDR             (REGS_IOMUXC_BASE + 0x2ac)    // Pin 18
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TXC_ADDR             (REGS_IOMUXC_BASE + 0x58)     // Pin 19
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD0_ADDR             (REGS_IOMUXC_BASE + 0x5c)     // Pin 20
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD1_ADDR             (REGS_IOMUXC_BASE + 0x60)     // Pin 21
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD2_ADDR             (REGS_IOMUXC_BASE + 0x64)     // Pin 22
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD3_ADDR             (REGS_IOMUXC_BASE + 0x68)     // Pin 23
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RX_CTL_ADDR          (REGS_IOMUXC_BASE + 0x6c)     // Pin 24
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD0_ADDR             (REGS_IOMUXC_BASE + 0x70)     // Pin 25
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TX_CTL_ADDR          (REGS_IOMUXC_BASE + 0x74)     // Pin 26
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD1_ADDR             (REGS_IOMUXC_BASE + 0x78)     // Pin 27
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD2_ADDR             (REGS_IOMUXC_BASE + 0x7c)     // Pin 28
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD3_ADDR             (REGS_IOMUXC_BASE + 0x80)     // Pin 29
#define HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RXC_ADDR             (REGS_IOMUXC_BASE + 0x84)     // Pin 30
#define HW_IOMUXC_SW_MUX_CTL_PAD_EIM_BCLK_ADDR              (REGS_IOMUXC_BASE + 0x158)    // Pin 31
// Bank 7
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA5_ADDR             (REGS_IOMUXC_BASE + 0x2b0)    // Pin 0
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA4_ADDR             (REGS_IOMUXC_BASE + 0x2b4)    // Pin 1
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CMD_ADDR               (REGS_IOMUXC_BASE + 0x2b8)    // Pin 2
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CLK_ADDR               (REGS_IOMUXC_BASE + 0x2bc)    // Pin 3
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA0_ADDR             (REGS_IOMUXC_BASE + 0x2c0)    // Pin 4
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA1_ADDR             (REGS_IOMUXC_BASE + 0x2c4)    // Pin 5
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA2_ADDR             (REGS_IOMUXC_BASE + 0x2c8)    // Pin 6
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA3_ADDR             (REGS_IOMUXC_BASE + 0x2cc)    // Pin 7
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD3_RESET_ADDR             (REGS_IOMUXC_BASE + 0x2d0)    // Pin 8
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CMD_ADDR               (REGS_IOMUXC_BASE + 0x2f4)    // Pin 9
#define HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CLK_ADDR               (REGS_IOMUXC_BASE + 0x2f8)    // Pin 10
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO16_ADDR                (REGS_IOMUXC_BASE + 0x248)    // Pin 11
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO17_ADDR                (REGS_IOMUXC_BASE + 0x24c)    // Pin 12
#define HW_IOMUXC_SW_MUX_CTL_PAD_GPIO18_ADDR                (REGS_IOMUXC_BASE + 0x250)    // Pin 13
/*********************************************************************************************************
  GPIO 管脚功能分配寄存器
*********************************************************************************************************/
#define GPIO_PIN_COUNT (32)
extern const UINT32 GpioPinmuxReg[HW_GPIO_INSTANCE_COUNT][GPIO_PIN_COUNT];
UINT32 GpioPinmuxSet(UINT32  uiPinGroup, UINT32 uiPinNum, UINT32 uiCfg);
/*********************************************************************************************************
  管脚功能选择宏
*********************************************************************************************************/
#define ALT0 0x0
#define ALT1 0x1
#define ALT2 0x2
#define ALT3 0x3
#define ALT4 0x4
#define ALT5 0x5
#define ALT6 0x6
#define ALT7 0x7
/*********************************************************************************************************
  板级相关的引脚复用配置
*********************************************************************************************************/
//#include "../../bsp/board.h"

#endif                                                                  /* _IOMUX_CONFIG_H_             */
/*********************************************************************************************************
  END
*********************************************************************************************************/
