#ifndef __SDK_H__
#define __SDK_H__

#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include <linux/compat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "io.h"
#include "irq/irq_numbers.h"
#include "pinmux/iomux_config.h"
#include "ccm_pll/ccm_pll.h"
#include "cpu_utility/cpu_utility.h"
#include "regsccm.h"
#include "regsccmanalog.h"
#include "regsgpc.h"
#include "regsssi.h"
#include "regsepit.h"
#include "regsgpt.h"
#include "regsi2c.h"
#include "regsspdif.h"
#include "regsspba.h"
#include "regssdmaarm.h"
#include "regsecspi.h"
#include "regsgpmi.h"
#include "regsesai.h"
#include "regseim.h"
#include "regsgpu2d.h"
#include "regsgpu3d.h"

#define bool	_Bool
#define true	1
#define false	0

typedef enum {
    CPU_0,
    CPU_1,
    CPU_2,
    CPU_3,
} cpuid_e;

enum display_type {
    DISP_DEV_NULL = 0,
    DISP_DEV_TFTLCD,
    DISP_DEV_LVDS,
    DISP_DEV_VGA,
    DISP_DEV_HDMI,
    DISP_DEV_TV,
    DISP_DEV_MIPI,
};
#define HW_UART_INSTANCE_COUNT (5)                                      /* Number of the UART module.   */
#define HW_UART1 (1)                                                    /* Instance number for UART1    */
#define HW_UART2 (2)                                                    /* Instance number for UART2    */
#define HW_UART3 (3)                                                    /* Instance number for UART3    */
#define HW_UART4 (4)                                                    /* Instance number for UART4    */
#define HW_UART5 (5)                                                    /* Instance number for UART5    */
#define REGS_UART1_BASE (0x02020000)                                    /* Base address UART 1.         */
#define REGS_UART2_BASE (0x021e8000)                                    /* Base address UART 2.         */
#define REGS_UART3_BASE (0x021ec000)                                    /* Base address UART 3.         */
#define REGS_UART4_BASE (0x021f0000)                                    /* Base address UART 4.         */
#define REGS_UART5_BASE (0x021f4000)                                    /* Base address UART 5.         */
/*********************************************************************************************************
 * 以下内容来源于soc_memory_map.h 文件
 ********************************************************************************************************/
#define BAAD_STATUS		0xbaadbaad
#define GOOD_STATUS		0x900d900d


#define OCRAM_BASE_ADDR			0x00900000
#define OCRAM_END_ADDR			0x009FFFFF
#define IRAM_BASE_ADDR          OCRAM_BASE_ADDR
#define PCIE_BASE_ADDR			0x01000000
#define PCIE_END_ADDR			0x01FFFFFF

#define ROMCP_ARB_BASE_ADDR     0x00000000
#define ROMCP_ARB_END_ADDR      0x000FFFFF
#define BOOT_ROM_BASE_ADDR      ROMCP_ARB_BASE_ADDR
#define CAAM_SEC_RAM_START_ADDR	0x00100000
#define CAAM_SEC_RAM_END_ADDR	0x00103FFF
#define GPMI_BASE_ADDR          0x00112000
#define APBH_DMA_BASE_ADDR		0x00110000
#define APBH_DMA_END_ADDR		0x00117FFF
#define HDMI_BASE_ADDR			0x00120000
#define HDMI_END_ADDR			0x00128FFF
#define GPU_3D_BASE_ADDR		0x00130000
#define GPU_3D_END_ADDR			0x00133FFF
#define GPU_2D_BASE_ADDR		0x00134000
#define GPU_2D_END_ADDR			0x00137FFF
#define DTCP_BASE_ADDR			0x00138000
#define DTCP_END_ADDR			0x0013BFFF
#define GPU_MEM_BASE_ADDR       GPU_3D_BASE_ADDR

#define GPV0_BASE_ADDR          0x00B00000
#define GPV1_BASE_ADDR          0x00C00000
#define GPV2_BASE_ADDR          0x00200000
#define GPV3_BASE_ADDR          0x00300000
#define GPV4_BASE_ADDR          0x00800000

#define AIPS1_ARB_BASE_ADDR     0x02000000
#define AIPS1_ARB_END_ADDR      0x020FFFFF
#define AIPS2_ARB_BASE_ADDR     0x02100000
#define AIPS2_ARB_END_ADDR      0x021FFFFF
#define SATA_BASE_ADDR          0x02200000
#define SATA_END_ADDR           0x02203FFF
#define OPENVG_BASE_ADDR        0x02204000
#define OPENVG_END_ADDR         0x02207FFF
#define HSI_BASE_ADDR           0x02208000
#define HSI_END_ADDR            0x0220BFFF
#define IPU1_BASE_ADDR          0x02400000
#define IPU1_END_ADDR           0x027FFFFF
#define IPU2_BASE_ADDR          0x02800000
#define IPU2_END_ADDR           0x02BFFFFF
#define WEIM_CS_BASE_ADDR       0x08000000
#define WEIM_CS_END_ADDR        0x0FFFFFFF

// CoreSight (ARM Debug)
#define DEBUG_ROM_BASE_ADDR     0x02140000
#define ETB_BASE_ADDR           0x02141000
#define EXT_CTI_BASE_ADDR       0x02142000
#define TPIU_BASE_ADDR          0x02143000
#define FUNNEL_BASE_ADDR        0x02144000
#define CORTEX_ROM_TABLE        0x0214F000
#define CORTEX_DEBUG_UNIT       0x02150000
#define CORE0_DEBUG_UNIT		0x02150000
#define PMU0_BASE_ADDR			0x02151000
#define CORE1_DEBUG_UNIT		0x02152000
#define PMU1_BASE_ADDR			0x02153000
#define CORE2_DEBUG_UNIT		0x02154000
#define PMU2_BASE_ADDR			0x02155000
#define CORE3_DEBUG_UNIT		0x02156000
#define PMU3_BASE_ADDR			0x02157000
#define CTI0_BASE_ADDR			0x02158000
#define CTI1_BASE_ADDR			0x02159000
#define CTI2_BASE_ADDR			0x0215A000
#define CTI3_BASE_ADDR			0x0215B000
#define PTM0_BASE_ADDR			0x0215C000
#define PTM_BASE_ADDR			0x0215C000
#define PTM1_BASE_ADDR			0x0215D000
#define PTM2_BASE_ADDR			0x0215E000
#define PTM3_BASE_ADDR			0x0215F000

#define AIPS_TZ1_BASE_ADDR		AIPS1_ARB_BASE_ADDR
#define AIPS_TZ2_BASE_ADDR		AIPS2_ARB_BASE_ADDR

#define SPDIF_BASE_ADDR			(AIPS_TZ1_BASE_ADDR+0x04000)
#define ECSPI1_BASE_ADDR		(AIPS_TZ1_BASE_ADDR+0x08000)
#define ECSPI2_BASE_ADDR		(AIPS_TZ1_BASE_ADDR+0x0C000)
#define ECSPI3_BASE_ADDR		(AIPS_TZ1_BASE_ADDR+0x10000)
#define ECSPI4_BASE_ADDR		(AIPS_TZ1_BASE_ADDR+0x14000)
#define ECSPI5_BASE_ADDR		(AIPS_TZ1_BASE_ADDR+0x18000)
#define UART1_BASE_ADDR			(AIPS_TZ1_BASE_ADDR+0x20000)
#define ESAI1_BASE_ADDR			(AIPS_TZ1_BASE_ADDR+0x24000)
#define SSI1_BASE_ADDR			(AIPS_TZ1_BASE_ADDR+0x28000)
#define SSI2_BASE_ADDR			(AIPS_TZ1_BASE_ADDR+0x2C000)
#define SSI3_BASE_ADDR			(AIPS_TZ1_BASE_ADDR+0x30000)
#define ASRC_BASE_ADDR          (AIPS_TZ1_BASE_ADDR+0x34000)
#define SPBA_BASE_ADDR			(AIPS_TZ1_BASE_ADDR+0x3C000)
#define VPU_BASE_ADDR			(AIPS_TZ1_BASE_ADDR+0x40000)

#define AIPS1_ON_BASE_ADDR		(AIPS_TZ1_BASE_ADDR+0x7C000)
#define AIPS1_OFF_BASE_ADDR		(AIPS_TZ1_BASE_ADDR+0x80000)

#define PWM1_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x0000)
#define PWM2_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x4000)
#define PWM3_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x8000)
#define PWM4_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0xC000)
#define CAN1_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x10000)
#define CAN2_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x14000)
#define GPT_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x18000)
#define GPIO1_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x1C000)
#define GPIO2_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x20000)
#define GPIO3_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x24000)
#define GPIO4_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x28000)
#define GPIO5_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x2C000)
#define GPIO6_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x30000)
#define GPIO7_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x34000)
#define KPP_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x38000)
#define WDOG1_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x3C000)
#define WDOG2_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x40000)
#define CCM_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x44000)
#define IP2APB_USBPHY1_BASE_ADDR    (AIPS1_OFF_BASE_ADDR+0x49000)
#define IP2APB_USBPHY2_BASE_ADDR    (AIPS1_OFF_BASE_ADDR+0x4A000)
#define SNVS_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x4C000)
#define EPIT1_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x50000)
#define EPIT2_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x54000)
#define SRC_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x58000)
#define GPC_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x5C000)
#define IOMUXC_BASE_ADDR		(AIPS1_OFF_BASE_ADDR+0x60000)
#define DCIC1_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x64000)
#define DCIC2_BASE_ADDR			(AIPS1_OFF_BASE_ADDR+0x68000)
#define SDMA_BASE_ADDR          (AIPS1_OFF_BASE_ADDR+0x6C000)
#define SDMA_IPS_HOST_BASE_ADDR SDMA_BASE_ADDR

#define AIPS2_ON_BASE_ADDR      (AIPS_TZ2_BASE_ADDR+0x7C000)
#define AIPS2_OFF_BASE_ADDR     (AIPS_TZ2_BASE_ADDR+0x80000)

#define AIPS1_BASE_ADDR			AIPS1_ON_BASE_ADDR
#define AIPS2_BASE_ADDR			AIPS2_ON_BASE_ADDR

#define CAAM_BASE_ADDR			AIPS_TZ2_BASE_ADDR
#define ARM_IPS_BASE_ADDR		(AIPS_TZ2_BASE_ADDR+0x40000)

#define USBOH3_PL301_BASE_ADDR	(AIPS2_OFF_BASE_ADDR+0x0000)
#define USBOH3_USB_BASE_ADDR	(AIPS2_OFF_BASE_ADDR+0x4000)
#define ENET_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0x8000)
#define MLB_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0xC000)
#define USDHC1_BASE_ADDR		(AIPS2_OFF_BASE_ADDR+0x10000)
#define USDHC2_BASE_ADDR		(AIPS2_OFF_BASE_ADDR+0x14000)
#define USDHC3_BASE_ADDR		(AIPS2_OFF_BASE_ADDR+0x18000)
#define USDHC4_BASE_ADDR		(AIPS2_OFF_BASE_ADDR+0x1C000)
#define I2C1_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0x20000)
#define I2C2_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0x24000)
#define I2C3_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0x28000)
#define ROMCP_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0x2C000)
#define MMDC_P0_BASE_ADDR		(AIPS2_OFF_BASE_ADDR+0x30000)
#define MMDC_P1_BASE_ADDR		(AIPS2_OFF_BASE_ADDR+0x34000)
#define WEIM_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0x38000)
#define OCOTP_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0x3C000)
#define CSU_BASE_ADDR			(AIPS2_OFF_BASE_ADDR+0x40000)
#define PERFMON1_BASE_ADDR      (AIPS2_OFF_BASE_ADDR+0x44000)
#define PERFMON2_BASE_ADDR      (AIPS2_OFF_BASE_ADDR+0x48000)
#define PERFMON3_BASE_ADDR      (AIPS2_OFF_BASE_ADDR+0x4C000)
#define IP2APB_TZASC1_BASE_ADDR (AIPS2_OFF_BASE_ADDR+0x50000)
#define IP2APB_TZASC2_BASE_ADDR (AIPS2_OFF_BASE_ADDR+0x54000)
#define AUDMUX_BASE_ADDR		(AIPS2_OFF_BASE_ADDR+0x58000)
#define MIPI_CSI2_BASE_ADDR		(AIPS2_OFF_BASE_ADDR+0x5C000)
#define MIPI_DSI_BASE_ADDR	    (AIPS2_OFF_BASE_ADDR+0x60000)
#define VDOA_BASE_ADDR		    (AIPS2_OFF_BASE_ADDR+0x64000)
#define UART2_BASE_ADDR	    	(AIPS2_OFF_BASE_ADDR+0x68000)
#define UART3_BASE_ADDR	    	(AIPS2_OFF_BASE_ADDR+0x6C000)
#define UART4_BASE_ADDR	    	(AIPS2_OFF_BASE_ADDR+0x70000)
#define UART5_BASE_ADDR	    	(AIPS2_OFF_BASE_ADDR+0x74000)

// Cortex-A9 MPCore private memory region
#define ARM_PERIPHBASE			    	0x00A00000
#define SCU_BASE_ADDR			    	ARM_PERIPHBASE
#define IC_INTERFACES_BASE_ADDR		    (ARM_PERIPHBASE+0x0100)
#define GLOBAL_TIMER_BASE_ADDR		    (ARM_PERIPHBASE+0x0200)
#define PRIVATE_TIMERS_WD_BASE_ADDR	    (ARM_PERIPHBASE+0x0600)
#define IC_DISTRIBUTOR_BASE_ADDR		(ARM_PERIPHBASE+0x1000)

//Add from mx53 for ds90ur124.c
#define GPR_BASE_ADDR      (IOMUXC_BASE_ADDR + 0x0)
#define OBSRV_BASE_ADDR    (GPR_BASE_ADDR + 0x38)
#define SW_MUX_BASE_ADDR   (OBSRV_BASE_ADDR + 0x14)


/*********************************************************************************************************
 * 以下内容来源于 register.h 文件
 ********************************************************************************************************/
#include "regs.h"

//#################################################
//#
//# Description:
//#   This file lists instances offset memory location
//#
//#################################################

//#########################################
//# ARM PLATFORM INTERNALS - SCU REGISTERS
//#########################################
#define SCU_CONTROL		    SCU_BASE_ADDR   //  SCU Control Register
#define SCU_CONFIG		    SCU_BASE_ADDR+0x04  //  SCU Control Register
#define SCU_CPU_PWR_STAT	SCU_BASE_ADDR+0x08  //  SCU Control Register
#define SCU_INVAL_SECURE	SCU_BASE_ADDR+0x0C  //  SCU Control Register
#define SCU_FILTERSTART	    SCU_BASE_ADDR+0x40  //  Filtering Start Address Register
#define SCU_FILTEREND	    SCU_BASE_ADDR+0x44  //  Filtering End Address Register

//#########################################
//# BOOT ROM
//#########################################
#define BOOTROM1_ADDR_BOT_OFFSET	0x00000000  //  boot rom section 1 bottom address
//#########################################
//# WDOG
//#########################################
#define WDOG_WCR_OFFSET	0x00    //  16bit watchdog control reg
#define WDOG_WSR_OFFSET	0x02    //  16bit watchdog service reg
#define WDOG_WRSR_OFFSET	0x04    //  16bit watchdog reset status reg
#define WDOG_WICR_OFFSET	0x06    // Watchdog Interrupt Control Register
//#########################################
//# GPT
//#########################################
#define GPT_GPTCR_OFFSET	0x00    //  32bit timer 1 control reg
#define GPT_GPTPR_OFFSET	0x04    //  32bit timer 1 prescaler reg
#define GPT_GPTSR_OFFSET	0x08    //  32bit timer 1 compare reg
#define GPT_GPTIR_OFFSET	0x0C    //  32bit timer 1 capture reg
#define GPT_GPTOCR1_OFFSET	0x10    //  32bit timer 1 counter reg
#define GPT_GPTOCR2_OFFSET	0x14    //  32bit timer 1 status reg
#define GPT_GPTOCR3_OFFSET	0x18    // 32 bit read & write
#define GPT_GPTICR1_OFFSET	0x1C    // 32 bit read
#define GPT_GPTICR2_OFFSET	0x20    // 32 bit read
#define GPT_GPTCNT_OFFSET	0x24    // 32 bit read
//#########################################
//# EPIT
//#########################################
#define EPITCR_OFFSET	0x00    //  32bit timer 3 control reg
#define EPITSR_OFFSET	0x04    //  32bit timer 3 prescaler reg
#define EPITLR_OFFSET	0x08    //  32bit timer 3 compare reg
#define EPITCMPR_OFFSET	0x0C    //  32bit timer 3 capture reg
#define EPITCNR_OFFSET	0x10    //  32bit timer 3 counter reg
//#########################################
//# PWM
//#########################################
#define PWM_PWMCR_OFFSET	0x00    //  32bit pwm control reg
#define PWM_PWMSR_OFFSET	0x04    //  32bit pwm sample reg
#define PWM_PWMIR_OFFSET	0x08    //  32bit pwm period reg
#define PWM_PWMSAR_OFFSET	0x0C    //  32bit pwm counter reg
#define PWM_PWMPR_OFFSET	0x10    //  32bit pwm test reg
#define PWM_PWMCNR_OFFSET	0x14
//#########################################
//# KPP
//#########################################
#define KPP_KPCR_OFFSET	0x00    //  16bit kpp keypad control reg
#define KPP_KPSR_OFFSET	0x02    //  16bit kpp keypad status reg
#define KPP_KDDR_OFFSET	0x04    //  16bit kpp keypad data directon reg
#define KPP_KPDR_OFFSET	0x06    //  16bit kpp keypad data reg
//#########################################
//# I2C
//#########################################
#define I2C_IADR_OFFSET	0x00    //  16bit i2c address reg
#define I2C_IFDR_OFFSET	0x04    //  16bit i2c frequency divider reg
#define I2C_I2CR_OFFSET	0x08    //  16bit i2c control reg
#define I2C_I2SR_OFFSET	0x0C    //  16bit i2c status reg
#define I2C_I2DR_OFFSET	0x10    //  16bit i2c data i/o reg
//#########################################
//# GPIO
//#########################################
#define GPIO_DR_OFFSET  0x00    //  32bit gpio pta data direction reg
#define GPIO_GDIR_OFFSET    0x04    //  32bit gpio pta output config 1 reg
#define GPIO_PSR_OFFSET	0x08    //  32bit gpio pta output config 2 reg
#define GPIO_ICR1_OFFSET	0x0C    //  32bit gpio pta input config A1 reg
#define GPIO_ICR2_OFFSET	0x10    //  32bit gpio pta input config A2 reg
#define GPIO_IMR_OFFSET	0x14    //  32bit gpio pta input config B1 reg
#define GPIO_ISR_OFFSET	0x18    // GPIO Interrupt Status Register
#define GPIO_EDGE_SEL_OFFSET	0x1C    // GPIO Edge Detect Register

//#########################################
//# ESAI
//#########################################
#define ESAI_ETDR_OFFSET           0x00
#define ESAI_ERDR_OFFSET           0x04
#define ESAI_ECR_OFFSET            0x08
#define ESAI_ESR_OFFSET            0x0c
#define ESAI_TFCR_OFFSET           0x10
#define ESAI_TFSR_OFFSET           0x14
#define ESAI_RFCR_OFFSET           0x18
#define ESAI_RFSR_OFFSET           0x1c
#define ESAI_TX0_OFFSET            0x80
#define ESAI_TX1_OFFSET            0x84
#define ESAI_TX2_OFFSET            0x88
#define ESAI_TX3_OFFSET            0x8c
#define ESAI_TX4_OFFSET            0x90
#define ESAI_TX5_OFFSET            0x94
#define ESAI_TSR_OFFSET            0x98
#define ESAI_RX0_OFFSET            0xA0
#define ESAI_RX1_OFFSET            0xA4
#define ESAI_RX2_OFFSET            0xA8
#define ESAI_RX3_OFFSET            0xAC
#define ESAI_SAISR_OFFSET          0xCC
#define ESAI_SAICR_OFFSET          0xD0
#define ESAI_TCR_OFFSET            0xD4
#define ESAI_TCCR_OFFSET           0xD8
#define ESAI_RCR_OFFSET            0xDC
#define ESAI_RCCR_OFFSET           0xE0
#define ESAI_TSMA_OFFSET           0xE4
#define ESAI_TSMB_OFFSET           0xE8
#define ESAI_RSMA_OFFSET           0xEC
#define ESAI_RSMB_OFFSET           0xF0
#define ESAI_PDRC_OFFSET           0xF4
#define ESAI_PRRC_OFFSET           0xF8
#define ESAI_PCRC_OFFSET           0xFC

//#########################################
//# ECSPI
//#########################################
#define ECSPI_RXDATA_OFFSET	0x00    //  32bit CSPI receive data reg
#define ECSPI_TXDATA_OFFSET	0x04    //  32bit CSPI transmit data reg
#define ECSPI_CONREG_OFFSET	0x08    //  32bit CSPI control reg
#define ECSPI_CONFIG_OFFSET	0x0C
#define ECSPI_INTREG_OFFSET	0x10    //  32bit CSPI interrupt stat/ctr reg
#define ECSPI_DMAREG_OFFSET	0x14    //  32bit CSPI test reg
#define ECSPI_STATREG_OFFSET	0x18    //  32bit CSPI sample period ctrl reg
#define ECSPI_PERIODREG_OFFSET	0x1C    //  32bit CSPI dma ctrl reg
#define ECSPI_TESTREG_OFFSET	0x20    //  32bit CSPI soft reset reg
#define ECSPI_MSG0REG_OFFSET	0x24
#define ECSPI_MSG1REG_OFFSET	0x28
#define ECSPI_MSG2REG_OFFSET	0x2c
#define ECSPI_MSG3REG_OFFSET	0x30
#define ECSPI_MSG4REG_OFFSET	0x34
#define ECSPI_MSG5REG_OFFSET	0x38
#define ECSPI_MSG6REG_OFFSET	0x3c
#define ECSPI_MSG7REG_OFFSET	0x40
#define ECSPI_MSG8REG_OFFSET	0x44
#define ECSPI_MSG9REG_OFFSET	0x48
#define ECSPI_MSG10REG_OFFSET	0x4c
#define ECSPI_MSG11REG_OFFSET	0x50
#define ECSPI_MSG12REG_OFFSET	0x54
#define ECSPI_MSG13REG_OFFSET	0x58
#define ECSPI_MSG14REG_OFFSET	0x5c
#define ECSPI_MSG15REG_OFFSET	0x60
//#########################################
//# PERFMON 1/2/3
//#########################################
#define HW_PERFMON_CTRL_OFFSET 			 0x00
#define HW_PERFMON_CTRL_SET_OFFSET 		 0x04
#define HW_PERFMON_CTRL_CLR_OFFSET 		 0x08
#define HW_PERFMON_CTRL_TOG_OFFSET 		 0x0C
#define HW_PERFMON_MASTER_ON_OFFSET 	 0x10
#define HW_PERFMON_TRAP_ADDR_LOW_OFFSET  0x20
#define HW_PERFMON_TRAP_ADDR_HIGH_OFFSET 0x30
#define HW_PERFMON_LAT_THRESHOLD_OFFSET  0x40
#define HW_PERFMON_ACTIVE_CYCLE_OFFSET 	 0x50
#define HW_PERFMON_TRANSFER_COUNT_OFFSET 0x60
#define HW_PERFMON_TOTAL_LATENCY_OFFSET  0x70
#define HW_PERFMON_DATA_COUNT_OFFSET 	 0x80
#define HW_PERFMON_MAX_LATENCY_OFFSET 	 0x90
#define HW_PERFMON_DEBUG_OFFSET	 	     0xA0
#define HW_PERFMON_VERSION_OFFSET 		 0xB0

//#########################################
//# uSDHC
//#########################################
#define USDHC_DSADDR_OFFSET	0x00    //  32bit SDHC control reg
#define USDHC_BLKATTR_OFFSET	0x04    //  32bit SDHC status reg
#define USDHC_CMDARG_OFFSET	0x08    //  32bit SDHC clock rate reg
#define USDHC_XFERTYP_OFFSET	0x0C    //  32bit SDHC cmd/data control reg
#define USDHC_CMDRSP0_OFFSET	0x10    //  32bit SDHC response time out reg
#define USDHC_CMDRSP1_OFFSET	0x14    //  32bit SDHC read time out reg
#define USDHC_CMDRSP2_OFFSET	0x18    //  32bit SDHC block length reg
#define USDHC_CMDRSP3_OFFSET	0x1C    //  32bit SDHC number of blocks reg
#define USDHC_DATPORT_OFFSET	0x20    //  32bit SDHC revision number reg
#define USDHC_PRSSTATE_OFFSET	0x24    //  32bit SDHC interrupt mask reg
#define USDHC_PROCTL_OFFSET	0x28    //  32bit SDHC command code reg
#define USDHC_SYSCTRL_OFFSET	0x2C    //  32bit SDHC argument (high+low) reg
#define USDHC_IRQSTAT_OFFSET	0x30    //  32bit SDHC response fifo reg
#define USDHC_IRQSTATEN_OFFSET	0x34    //  32bit SDHC buffer access reg
#define USDHC_IRQSIGEN_OFFSET	0x38    //  32bit SDHC remaining NUM reg
#define USDHC_AUTOC12ERR_OFFSET	0x3C    //  32bit SDHC remaining block bytes  reg
#define USDHC_HOSTCAPBLT_OFFSET	0x40
#define USDHC_WML_OFFSET	0x44
#define USDHC_FEVTR_OFFSET	0x50
#define USDHC_HOSTVER_OFFSET	0xfc

//#########################################
//# SSI
//#########################################
#define SSI_STX0_OFFSET	0x00    //  32bit SSI tx reg 0
#define SSI_STX1_OFFSET	0x04    //  32bit SSI tx reg 1
#define SSI_SRX0_OFFSET	0x08    //  32bit SSI rx reg 0
#define SSI_SRX1_OFFSET	0x0C    //  32bit SSI rx reg 1
#define SSI_SCR_OFFSET	0x10    //  32bit SSI control reg
#define SSI_SISR_OFFSET	0x14    //  32bit SSI intr status reg
#define SSI_SIER_OFFSET	0x18    //  32bit SSI intr enable reg
#define SSI_STCR_OFFSET	0x1C    //  32bit SSI tx config reg
#define SSI_SRCR_OFFSET	0x20    //  32bit SSI rx config reg
#define SSI_STCCR_OFFSET	0x24    //  32bit SSI tx clock control reg
#define SSI_SRCCR_OFFSET	0x28    //  32bit SSI rx clock control reg
#define SSI_SFCSR_OFFSET	0x2C    //  32bit SSI fifo control/status reg
#define SSI_STR_OFFSET	0x30    //  32bit SSI test reg
#define SSI_SOR_OFFSET	0x34    //  32bit SSI option reg
#define SSI_SACNT_OFFSET	0x38    //  32bit SSI ac97 control reg
#define SSI_SACADD_OFFSET	0x3C    //  32bit SSI ac97 cmd addr reg
#define SSI_SACDAT_OFFSET	0x40    //  32bit SSI ac97 cmd data reg
#define SSI_SATAG_OFFSET	0x44    //  32bit SSI ac97 tag reg
#define SSI_STMSK_OFFSET	0x48    //  32bit SSI tx time slot mask reg
#define SSI_SRMSK_OFFSET	0x4C    //  32bit SSI rx time slot mask reg
#define SSI_SACCST_OFFSET	0x50
#define SSI_SACCEN_OFFSET	0x54
#define SSI_SACCDIS_OFFSET	0x58

//#########################################
//# AUDMUX
//#########################################
#define AUDMUX_PTCR1_OFFSET	0x00    // Port Timing Control Register 1
#define AUDMUX_PDCR1_OFFSET	0x04    // Port Data Control Register 1
#define AUDMUX_PTCR2_OFFSET	0x08    // Port Timing Control Register 2
#define AUDMUX_PDCR2_OFFSET	0x0C    // Port Data Control Register 2
#define AUDMUX_PTCR3_OFFSET	0x10    // Port Timing Control Register 3
#define AUDMUX_PDCR3_OFFSET	0x14    // Port Data Control Register 3
#define AUDMUX_PTCR4_OFFSET	0x18    // Port Timing Control Register 4
#define AUDMUX_PDCR4_OFFSET	0x1C    // Port Data Control Register 4
#define AUDMUX_PTCR5_OFFSET	0x20    // Port Timing Control Register 5
#define AUDMUX_PDCR5_OFFSET	0x24    // Port Data Control Register 5
#define AUDMUX_PTCR6_OFFSET	0x28    // Port Timing Control Register 6
#define AUDMUX_PDCR6_OFFSET	0x2C    // Port Data Control Register 6
#define AUDMUX_PTCR7_OFFSET	0x30    // Port Timing Control Register 7
#define AUDMUX_PDCR7_OFFSET	0x34    // Port Data Control Register 7
#define AUDMUX_CNMCR_OFFSET	0x38    // CE Bus Network Mode Control Register
//#########################################
//# SPBA
//#########################################
#define SPBA_PER0_START_ADDR_OFFSET	0x00    //  32bit gpio pta data direction reg
#define SPBA_PER1_START_ADDR_OFFSET	0x4000  //  32bit gpio pta output config 1 reg
#define SPBA_PER2_START_ADDR_OFFSET	0x8000  //  32bit gpio pta output config 2 reg
#define SPBA_PER3_START_ADDR_OFFSET	0xC000
#define SPBA_PER4_START_ADDR_OFFSET	0x10000
#define SPBA_PER5_START_ADDR_OFFSET	0x14000
#define SPBA_PER6_START_ADDR_OFFSET	0x18000
#define SPBA_PER7_START_ADDR_OFFSET	0x1C000
#define SPBA_PER8_START_ADDR_OFFSET	0x20000
#define SPBA_PER9_START_ADDR_OFFSET	0x24000
#define SPBA_PER10_START_ADDR_OFFSET	0x28000
#define SPBA_PER11_START_ADDR_OFFSET	0x2C000
#define SPBA_PER12_START_ADDR_OFFSET	0x30000
#define SPBA_PER13_START_ADDR_OFFSET	0x34000
#define SPBA_PER14_START_ADDR_OFFSET	0x38000
#define SPBA_REG_ADDR_OFFSET	0x3C000
#define SPBA_PRR0_OFFSET	0x00
#define SPBA_PRR1_OFFSET	0x04
#define SPBA_PRR2_OFFSET	0x08
#define SPBA_PRR3_OFFSET	0x0C
#define SPBA_PRR4_OFFSET	0x10
#define SPBA_PRR5_OFFSET	0x14
#define SPBA_PRR6_OFFSET	0x18
#define SPBA_PRR7_OFFSET	0x1C
#define SPBA_PRR8_OFFSET	0x20
#define SPBA_PRR9_OFFSET	0x24
#define SPBA_PRR10_OFFSET	0x28
#define SPBA_PRR11_OFFSET	0x2C
#define SPBA_PRR12_OFFSET	0x30
#define SPBA_PRR13_OFFSET	0x34
#define SPBA_PRR14_OFFSET	0x38

//#########################################
//# DCIC
//#########################################
#define DCICC_REG_OFFSET      0x0   //DCICC
#define DCICIC_REG_OFFSET     0x4   //DCICIC
#define DCICS_REG_OFFSET      0x8   //DCICS
#define DCI_RES_C_REG_OFFSET  0xc   //RESERVED
#define DCICRC0_REG_OFFSET    0x10  //DCICRCm
#define DCICRS0_REG_OFFSET    0x14  //DCICRSm
#define DCICRRS0_REG_OFFSET   0x18  //DCICRRSm
#define DCICRCS0_REG_OFFSET   0x1c  //DCICRCSm
#define DCICRC1_REG_OFFSET    0x20  //DCICRCm
#define DCICRS1_REG_OFFSET    0x24  //DCICRSm
#define DCICRRS1_REG_OFFSET   0x28  //DCICRRSm
#define DCICRCS1_REG_OFFSET   0x2c  //DCICRCSm
#define DCICRC2_REG_OFFSET    0x30  //DCICRCm
#define DCICRS2_REG_OFFSET    0x34  //DCICRSm
#define DCICRRS2_REG_OFFSET   0x38  //DCICRRSm
#define DCICRCS2_REG_OFFSET   0x3c  //DCICRCSm
#define DCICRC3_REG_OFFSET    0x40  //DCICRCm
#define DCICRS3_REG_OFFSET    0x44  //DCICRSm
#define DCICRRS3_REG_OFFSET   0x48  //DCICRRSm
#define DCICRCS3_REG_OFFSET   0x4c  //DCICRCSm
#define DCICRC4_REG_OFFSET    0x50  //DCICRCm
#define DCICRS4_REG_OFFSET    0x54  //DCICRSm
#define DCICRRS4_REG_OFFSET   0x58  //DCICRRSm
#define DCICRCS4_REG_OFFSET   0x5c  //DCICRCSm
#define DCICRC5_REG_OFFSET    0x60  //DCICRCm
#define DCICRS5_REG_OFFSET    0x64  //DCICRSm
#define DCICRRS5_REG_OFFSET   0x68  //DCICRRSm
#define DCICRCS5_REG_OFFSET   0x6c  //DCICRCSm
#define DCICRC6_REG_OFFSET    0x70  //DCICRCm
#define DCICRS6_REG_OFFSET    0x74  //DCICRSm
#define DCICRRS6_REG_OFFSET   0x78  //DCICRRSm
#define DCICRCS6_REG_OFFSET   0x7c  //DCICRCSm
#define DCICRC7_REG_OFFSET    0x80  //DCICRCm
#define DCICRS7_REG_OFFSET    0x84  //DCICRSm
#define DCICRRS7_REG_OFFSET   0x88  //DCICRRSm
#define DCICRCS7_REG_OFFSET   0x8c  //DCICRCSm
#define DCICRC8_REG_OFFSET    0x90  //DCICRCm
#define DCICRS8_REG_OFFSET    0x94  //DCICRSm
#define DCICRRS8_REG_OFFSET   0x98  //DCICRRSm
#define DCICRCS8_REG_OFFSET   0x9c  //DCICRCSm
#define DCICRC9_REG_OFFSET    0xa0  //DCICRCm
#define DCICRS9_REG_OFFSET    0xa4  //DCICRSm
#define DCICRRS9_REG_OFFSET   0xa8  //DCICRRSm
#define DCICRCS9_REG_OFFSET   0xac  //DCICRCSm
#define DCICRC10_REG_OFFSET   0xb0  //DCICRCm
#define DCICRS10_REG_OFFSET   0xb4  //DCICRSm
#define DCICRRS10_REG_OFFSET  0xb8  //DCICRRSm
#define DCICRCS10_REG_OFFSET  0xbc  //DCICRCSm
#define DCICRC11_REG_OFFSET   0xc0  //DCICRCm
#define DCICRS11_REG_OFFSET   0xc4  //DCICRSm
#define DCICRRS11_REG_OFFSET  0xc8  //DCICRRSm
#define DCICRCS11_REG_OFFSET  0xcc  //DCICRCSm
#define DCICRC12_REG_OFFSET   0xd0  //DCICRCm
#define DCICRS12_REG_OFFSET   0xd4  //DCICRSm
#define DCICRRS12_REG_OFFSET  0xd8  //DCICRRSm
#define DCICRCS12_REG_OFFSET  0xdc  //DCICRCSm
#define DCICRC13_REG_OFFSET   0xe0  //DCICRCm
#define DCICRS13_REG_OFFSET   0xe4  //DCICRSm
#define DCICRRS13_REG_OFFSET  0xe8  //DCICRRSm
#define DCICRCS13_REG_OFFSET  0xec  //DCICRCSm
#define DCICRC14_REG_OFFSET   0xf0  //DCICRCm
#define DCICRS14_REG_OFFSET   0xf4  //DCICRSm
#define DCICRRS14_REG_OFFSET  0xf8  //DCICRRSm
#define DCICRCS14_REG_OFFSET  0xfc  //DCICRCSm
#define DCICRC15_REG_OFFSET   0x100 //DCICRCm
#define DCICRS15_REG_OFFSET   0x104 //DCICRSm
#define DCICRRS15_REG_OFFSET  0x108 //DCICRRSm
#define DCICRCS15_REG_OFFSET  0x10c //DCICRCSm
//#########################################
//# GPU
//#########################################
#define GPU3D_BASE_ADDRESS_OFFSET GPU_BASE_ADDR
#define GMEM_BASE_ADDRESS_OFFSET  GPU_MEM_BASE_ADDR

#define GPU3D_RBBM_RTL_RELEASE_OFFSET                    0x0
#define GPU3D_RBBM_PATCH_RELEASE_OFFSET                  0x4
#define GPU3D_RBBM_AUXILIARY_CONFIG_OFFSET               0x8
#define GPU3D_GPU3D_GPIO_REG_OFFSET                      0x1FFFE
#define GPU3D_BIOS_0_SCRATCH_OFFSET                      0x10
#define GPU3D_BIOS_1_SCRATCH_OFFSET                      0x14
#define GPU3D_BIOS_2_SCRATCH_OFFSET                      0x18
#define GPU3D_BIOS_3_SCRATCH_OFFSET                      0x1C
#define GPU3D_BIOS_4_SCRATCH_OFFSET                      0x20
#define GPU3D_BIOS_5_SCRATCH_OFFSET                      0x24
#define GPU3D_BIOS_6_SCRATCH_OFFSET                      0x28
#define GPU3D_BIOS_7_SCRATCH_OFFSET                      0x2C
#define GPU3D_RBBM_CNTL_OFFSET                           0xEC
#define GPU3D_RBBM_SOFT_RESET_OFFSET                     0xF0
#define GPU3D_RBBM_SKEW_CNTL_OFFSET                      0xF4
#define GPU3D_MH_MMU_CONFIG_OFFSET                       0x100
#define GPU3D_MH_MMU_VA_RANGE_OFFSET                     0x104
#define GPU3D_MH_MMU_PT_BASE_OFFSET                      0x108
#define GPU3D_MH_MMU_PAGE_FAULT_OFFSET                   0x10C
#define GPU3D_MH_MMU_TRAN_ERROR_OFFSET                   0x110
#define GPU3D_MH_MMU_INVALIDATE_OFFSET                   0x114
#define GPU3D_MH_MMU_MPU_BASE_OFFSET                     0x118
#define GPU3D_MH_MMU_MPU_END_OFFSET                      0x11C
#define GPU3D_CP_PFP_UCODE_ADDR_OFFSET                   0x300
#define GPU3D_CP_PFP_UCODE_DATA_OFFSET                   0x304
#define GPU3D_CP_RB_BASE_OFFSET                          0x700
#define GPU3D_CP_RB_CNTL_OFFSET                          0x704
#define GPU3D_CP_RB_RPTR_ADDR_OFFSET                     0x70C
#define GPU3D_CP_RB_RPTR_OFFSET                          0x710
#define GPU3D_CP_RB_WPTR_OFFSET                          0x714
#define GPU3D_CP_RB_WPTR_DELAY_OFFSET                    0x718
#define GPU3D_CP_RB_RPTR_WR_OFFSET                       0x71C
#define GPU3D_CP_RB_WPTR_BASE_OFFSET                     0x720
#define GPU3D_CP_QUEUE_THRESHOLDS_OFFSET                 0x754
#define GPU3D_CP_MEQ_THRESHOLDS_OFFSET                   0x758
#define GPU3D_CP_CSQ_AVAIL_OFFSET                        0x75C
#define GPU3D_CP_STQ_AVAIL_OFFSET                        0x760
#define GPU3D_CP_MEQ_AVAIL_OFFSET                        0x764
#define GPU3D_CP_CMD_INDEX_OFFSET                        0x768
#define GPU3D_CP_CMD_DATA_OFFSET                         0x76C
#define GPU3D_SCRATCH_UMSK_OFFSET                        0x770
#define GPU3D_SCRATCH_ADDR_OFFSET                        0x774
#define GPU3D_CP_ME_RDADDR_OFFSET                        0x7A8
#define GPU3D_CP_STATE_DEBUG_INDEX_OFFSET                0x7B0
#define GPU3D_CP_STATE_DEBUG_DATA_OFFSET                 0x7B4
#define GPU3D_CP_NV_FLAGS_0_OFFSET                       0x7B8
#define GPU3D_CP_NV_FLAGS_1_OFFSET                       0x7BC
#define GPU3D_CP_NV_FLAGS_2_OFFSET                       0x7C0
#define GPU3D_CP_NV_FLAGS_3_OFFSET                       0x7C4
#define GPU3D_CP_INT_CNTL_OFFSET                         0x7C8
#define GPU3D_CP_INT_STATUS_OFFSET                       0x7CC
#define GPU3D_CP_INT_ACK_OFFSET                          0x7D0
#define GPU3D_CP_ME_CNTL_OFFSET                          0x7D8
#define GPU3D_CP_ME_STATUS_OFFSET                        0x7DC
#define GPU3D_CP_ME_RAM_WADDR_OFFSET                     0x7E0
#define GPU3D_CP_ME_RAM_RADDR_OFFSET                     0x7E4
#define GPU3D_CP_ME_RAM_DATA_OFFSET                      0x7E8
#define GPU3D_CP_DEBUG_OFFSET                            0x7F0
#define GPU3D_CP_CSQ_RB_STAT_OFFSET                      0x7F4
#define GPU3D_CP_CSQ_IB1_STAT_OFFSET                     0x7F8
#define GPU3D_CP_CSQ_IB2_STAT_OFFSET                     0x7FC
#define GPU3D_RBBM_DSPLY_OFFSET                          0xE44
#define GPU3D_RBBM_RENDER_LATEST_OFFSET                  0xE48
#define GPU3D_NQWAIT_UNTIL_OFFSET                        0xE50
#define GPU3D_RBBM_PERFCOUNTER1_SELECT_OFFSET            0xE54
#define GPU3D_RBBM_PERFCOUNTER1_LO_OFFSET                0xE5C
#define GPU3D_RBBM_PERFCOUNTER1_HI_OFFSET                0xE60
#define GPU3D_RBBM_DEBUG_OFFSET                          0xE6C
#define GPU3D_RBBM_PM_OVERRIDE1_OFFSET                   0xE70
#define GPU3D_RBBM_PM_OVERRIDE2_OFFSET                   0xE74
#define GPU3D_GC_SYS_IDLE_OFFSET                         0xE78
#define GPU3D_RBBM_DEBUG_OUT_OFFSET                      0xE80
#define GPU3D_RBBM_DEBUG_CNTL_OFFSET                     0xE84
#define GPU3D_RBBM_WAIT_IDLE_CLOCKS_OFFSET               0xEC8
#define GPU3D_RBBM_READ_ERROR_OFFSET                     0xECC
#define GPU3D_RBBM_INT_CNTL_OFFSET                       0xED0
#define GPU3D_RBBM_INT_STATUS_OFFSET                     0xED4
#define GPU3D_RBBM_INT_ACK_OFFSET                        0xED8
#define GPU3D_MASTER_INT_SIGNAL_OFFSET                   0xEDC
#define GPU3D_RBBM_PERIPHID0_OFFSET                      0xFE0
#define GPU3D_RBBM_PERIPHID1_OFFSET                      0xFE4
#define GPU3D_RBBM_PERIPHID2_OFFSET                      0xFE8
#define GPU3D_RBBM_PERIPHID3_OFFSET                      0xFEC
#define GPU3D_CP_NON_PREFETCH_CNTRS_OFFSET               0x1100
#define GPU3D_CP_STQ_ST_STAT_OFFSET                      0x110C
#define GPU3D_CP_PERFMON_CNTL_OFFSET                     0x1110
#define GPU3D_CP_PERFCOUNTER_SELECT_OFFSET               0x1114
#define GPU3D_CP_PERFCOUNTER_LO_OFFSET                   0x1118
#define GPU3D_CP_PERFCOUNTER_HI_OFFSET                   0x111C
#define GPU3D_CP_PROG_COUNTER_OFFSET                     0x112C
#define GPU3D_CP_ST_BASE_OFFSET                          0x1134
#define GPU3D_CP_ST_BUFSZ_OFFSET                         0x1138
#define GPU3D_CP_MEQ_STAT_OFFSET                         0x113C
#define GPU3D_CP_MIU_TAG_STAT_OFFSET                     0x1148
#define GPU3D_CP_BIN_MASK_LO_OFFSET                      0x1150
#define GPU3D_CP_BIN_MASK_HI_OFFSET                      0x1154
#define GPU3D_CP_BIN_SELECT_LO_OFFSET                    0x1158
#define GPU3D_CP_BIN_SELECT_HI_OFFSET                    0x115C
#define GPU3D_CP_IB1_BASE_OFFSET                         0x1160
#define GPU3D_CP_IB1_BUFSZ_OFFSET                        0x1164
#define GPU3D_CP_IB2_BASE_OFFSET                         0x1168
#define GPU3D_CP_IB2_BUFSZ_OFFSET                        0x116C
#define GPU3D_CP_STAT_OFFSET                             0x11FC
#define GPU3D_SCRATCH_REG0_OFFSET                        0x15E0
#define GPU3D_SCRATCH_REG1_OFFSET                        0x15E4
#define GPU3D_SCRATCH_REG2_OFFSET                        0x15E8
#define GPU3D_SCRATCH_REG3_OFFSET                        0x15EC
#define GPU3D_SCRATCH_REG4_OFFSET                        0x15F0
#define GPU3D_SCRATCH_REG5_OFFSET                        0x15F4
#define GPU3D_SCRATCH_REG6_OFFSET                        0x15F8
#define GPU3D_SCRATCH_REG7_OFFSET                        0x15FC
#define GPU3D_BIOS_8_SCRATCH_OFFSET                      0x1600
#define GPU3D_BIOS_9_SCRATCH_OFFSET                      0x1604
#define GPU3D_BIOS_10_SCRATCH_OFFSET                     0x1608
#define GPU3D_BIOS_11_SCRATCH_OFFSET                     0x160C
#define GPU3D_BIOS_12_SCRATCH_OFFSET                     0x1610
#define GPU3D_BIOS_13_SCRATCH_OFFSET                     0x1614
#define GPU3D_BIOS_14_SCRATCH_OFFSET                     0x1618
#define GPU3D_BIOS_15_SCRATCH_OFFSET                     0x161C
#define GPU3D_WAIT_UNTIL_OFFSET                          0x1720
#define GPU3D_RBBM_ISYNC_CNTL_OFFSET                     0x1724
#define GPU3D_RBBM_STATUS_OFFSET                         0x1740
#define GPU3D_CP_ME_VS_EVENT_SRC_OFFSET                  0x1800
#define GPU3D_CP_ME_VS_EVENT_ADDR_OFFSET                 0x1804
#define GPU3D_CP_ME_VS_EVENT_DATA_OFFSET                 0x1808
#define GPU3D_CP_ME_VS_EVENT_ADDR_SWM_OFFSET             0x180C
#define GPU3D_CP_ME_VS_EVENT_DATA_SWM_OFFSET             0x1810
#define GPU3D_CP_ME_PS_EVENT_SRC_OFFSET                  0x1814
#define GPU3D_CP_ME_PS_EVENT_ADDR_OFFSET                 0x1818
#define GPU3D_CP_ME_PS_EVENT_DATA_OFFSET                 0x181C
#define GPU3D_CP_ME_PS_EVENT_ADDR_SWM_OFFSET             0x1820
#define GPU3D_CP_ME_PS_EVENT_DATA_SWM_OFFSET             0x1824
#define GPU3D_CP_ME_CF_EVENT_SRC_OFFSET                  0x1828
#define GPU3D_CP_ME_CF_EVENT_ADDR_OFFSET                 0x182C
#define GPU3D_CP_ME_CF_EVENT_DATA_OFFSET                 0x1830
#define GPU3D_CP_ME_NRT_ADDR_OFFSET                      0x1834
#define GPU3D_CP_ME_NRT_DATA_OFFSET                      0x1838
#define GPU3D_CP_ME_VS_FETCH_DONE_SRC_OFFSET             0x1848
#define GPU3D_CP_ME_VS_FETCH_DONE_ADDR_OFFSET            0x184C
#define GPU3D_CP_ME_VS_FETCH_DONE_DATA_OFFSET            0x1850
#define GPU3D_COHER_SIZE_PM4_OFFSET                      0x28A4
#define GPU3D_COHER_BASE_PM4_OFFSET                      0x28A8
#define GPU3D_COHER_STATUS_PM4_OFFSET                    0x28AC
#define GPU3D_COHER_SIZE_HOST_OFFSET                     0x28BC
#define GPU3D_COHER_BASE_HOST_OFFSET                     0x28C0
#define GPU3D_COHER_STATUS_HOST_OFFSET                   0x28C4
#define GPU3D_MH_ARBITER_CONFIG_OFFSET                   0x2900
#define GPU3D_MH_CLNT_AXI_ID_REUSE_OFFSET                0x2904
#define GPU3D_MH_INTERRUPT_MASK_OFFSET                   0x2908
#define GPU3D_MH_INTERRUPT_STATUS_OFFSET                 0x290C
#define GPU3D_MH_INTERRUPT_CLEAR_OFFSET                  0x2910
#define GPU3D_MH_AXI_ERROR_OFFSET                        0x2914
#define GPU3D_MH_PERFCOUNTER0_SELECT_OFFSET              0x2918
#define GPU3D_MH_PERFCOUNTER0_CONFIG_OFFSET              0x291C
#define GPU3D_MH_PERFCOUNTER0_LOW_OFFSET                 0x2920
#define GPU3D_MH_PERFCOUNTER0_HI_OFFSET                  0x2924
#define GPU3D_MH_PERFCOUNTER1_SELECT_OFFSET              0x2928
#define GPU3D_MH_PERFCOUNTER1_CONFIG_OFFSET              0x292C
#define GPU3D_MH_PERFCOUNTER1_LOW_OFFSET                 0x2930
#define GPU3D_MH_PERFCOUNTER1_HI_OFFSET                  0x2934
#define GPU3D_MH_DEBUG_CTRL_OFFSET                       0x2938
#define GPU3D_MH_DEBUG_DATA_OFFSET                       0x293C
#define GPU3D_MH_AXI_HALT_CONTROL_OFFSET                 0x2940
#define GPU3D_VGT_VTX_VECT_EJECT_REG_OFFSET              0x30B0
#define GPU3D_VGT_LAST_COPY_STATE_OFFSET                 0x30C0
#define GPU3D_VGT_DEBUG_CNTL_OFFSET                      0x30E0
#define GPU3D_VGT_DEBUG_DATA_OFFSET                      0x30E4
#define GPU3D_VGT_CRC_SQ_DATA_OFFSET                     0x30E8
#define GPU3D_VGT_CRC_SQ_CTRL_OFFSET                     0x30EC
#define GPU3D_VGT_CNTL_STATUS_OFFSET                     0x30F0
#define GPU3D_PA_SC_LINE_STIPPLE_STATE_OFFSET            0x3100
#define GPU3D_PA_SC_VIZ_QUERY_STATUS_OFFSET              0x3110
#define GPU3D_VGT_PERFCOUNTER0_SELECT_OFFSET             0x3120
#define GPU3D_VGT_PERFCOUNTER1_SELECT_OFFSET             0x3124
#define GPU3D_VGT_PERFCOUNTER2_SELECT_OFFSET             0x3128
#define GPU3D_VGT_PERFCOUNTER3_SELECT_OFFSET             0x312C
#define GPU3D_VGT_PERFCOUNTER0_LOW_OFFSET                0x3130
#define GPU3D_VGT_PERFCOUNTER0_HI_OFFSET                 0x3134
#define GPU3D_VGT_PERFCOUNTER1_LOW_OFFSET                0x3138
#define GPU3D_VGT_PERFCOUNTER1_HI_OFFSET                 0x313C
#define GPU3D_VGT_PERFCOUNTER2_LOW_OFFSET                0x3140
#define GPU3D_VGT_PERFCOUNTER2_HI_OFFSET                 0x3144
#define GPU3D_VGT_PERFCOUNTER3_LOW_OFFSET                0x3148
#define GPU3D_VGT_PERFCOUNTER3_HI_OFFSET                 0x314C
#define GPU3D_PA_SU_DEBUG_CNTL_OFFSET                    0x3200
#define GPU3D_PA_SU_DEBUG_DATA_OFFSET                    0x3204
#define GPU3D_PA_SC_DEBUG_CNTL_OFFSET                    0x3208
#define GPU3D_PA_SC_DEBUG_DATA_OFFSET                    0x320C
#define GPU3D_PA_CL_CNTL_STATUS_OFFSET                   0x3210
#define GPU3D_PA_CL_ENHANCE_OFFSET                       0x3214
#define GPU3D_PA_SU_FACE_DATA_OFFSET                     0x3218
#define GPU3D_PA_SU_PERFCOUNTER0_SELECT_OFFSET           0x3220
#define GPU3D_PA_SU_PERFCOUNTER1_SELECT_OFFSET           0x3224
#define GPU3D_PA_SU_PERFCOUNTER2_SELECT_OFFSET           0x3228
#define GPU3D_PA_SU_PERFCOUNTER3_SELECT_OFFSET           0x322C
#define GPU3D_PA_SU_PERFCOUNTER0_LOW_OFFSET              0x3230
#define GPU3D_PA_SU_PERFCOUNTER0_HI_OFFSET               0x3234
#define GPU3D_PA_SU_PERFCOUNTER1_LOW_OFFSET              0x3238
#define GPU3D_PA_SU_PERFCOUNTER1_HI_OFFSET               0x323C
#define GPU3D_PA_SU_PERFCOUNTER2_LOW_OFFSET              0x3240
#define GPU3D_PA_SU_PERFCOUNTER2_HI_OFFSET               0x3244
#define GPU3D_PA_SU_PERFCOUNTER3_LOW_OFFSET              0x3248
#define GPU3D_PA_SU_PERFCOUNTER3_HI_OFFSET               0x324C
#define GPU3D_PA_SU_CNTL_STATUS_OFFSET                   0x3250
#define GPU3D_PA_SC_PERFCOUNTER0_SELECT_OFFSET           0x3260
#define GPU3D_PA_SC_PERFCOUNTER0_LOW_OFFSET              0x3264
#define GPU3D_PA_SC_PERFCOUNTER0_HI_OFFSET               0x3268
#define GPU3D_PA_SC_CNTL_STATUS_OFFSET                   0x3290
#define GPU3D_PA_SC_ENHANCE_OFFSET                       0x3294
#define GPU3D_SQ_GPR_MANAGEMENT_OFFSET                   0x3400
#define GPU3D_SQ_FLOW_CONTROL_OFFSET                     0x3404
#define GPU3D_SQ_INST_STORE_MANAGMENT_OFFSET             0x3408
#define GPU3D_SQ_RESOURCE_MANAGMENT_OFFSET               0x340C
#define GPU3D_SQ_ACTIVITY_METER_CNTL_OFFSET              0x3418
#define GPU3D_SQ_ACTIVITY_METER_STATUS_OFFSET            0x341C
#define GPU3D_SQ_INPUT_ARB_PRIORITY_OFFSET               0x3420
#define GPU3D_SQ_THREAD_ARB_PRIORITY_OFFSET              0x3424
#define GPU3D_SQ_VS_WATCHDOG_TIMER_OFFSET                0x3428
#define GPU3D_SQ_PS_WATCHDOG_TIMER_OFFSET                0x342C
#define GPU3D_SQ_INT_CNTL_OFFSET                         0x34D0
#define GPU3D_SQ_INT_STATUS_OFFSET                       0x34D4
#define GPU3D_SQ_INT_ACK_OFFSET                          0x34D8
#define GPU3D_SQ_DEBUG_INPUT_FSM_OFFSET                  0x36B8
#define GPU3D_SQ_DEBUG_CONST_MGR_FSM_OFFSET              0x36BC
#define GPU3D_SQ_DEBUG_TP_FSM_OFFSET                     0x36C0
#define GPU3D_SQ_DEBUG_FSM_ALU_0_OFFSET                  0x36C4
#define GPU3D_SQ_DEBUG_FSM_ALU_1_OFFSET                  0x36C8
#define GPU3D_SQ_DEBUG_EXP_ALLOC_OFFSET                  0x36CC
#define GPU3D_SQ_DEBUG_PTR_BUFF_OFFSET                   0x36D0
#define GPU3D_SQ_DEBUG_GPR_VTX_OFFSET                    0x36D4
#define GPU3D_SQ_DEBUG_GPR_PIX_OFFSET                    0x36D8
#define GPU3D_SQ_DEBUG_TB_STATUS_SEL_OFFSET              0x36DC
#define GPU3D_SQ_DEBUG_VTX_TB_0_OFFSET                   0x36E0
#define GPU3D_SQ_DEBUG_VTX_TB_1_OFFSET                   0x36E4
#define GPU3D_SQ_DEBUG_VTX_TB_STATUS_REG_OFFSET          0x36E8
#define GPU3D_SQ_DEBUG_VTX_TB_STATE_MEM_OFFSET           0x36EC
#define GPU3D_SQ_DEBUG_PIX_TB_0_OFFSET                   0x36F0
#define GPU3D_SQ_DEBUG_PIX_TB_STATUS_REG_0_OFFSET        0x36F4
#define GPU3D_SQ_DEBUG_PIX_TB_STATUS_REG_1_OFFSET        0x36F8
#define GPU3D_SQ_DEBUG_PIX_TB_STATUS_REG_2_OFFSET        0x36FC
#define GPU3D_SQ_DEBUG_PIX_TB_STATUS_REG_3_OFFSET        0x3700
#define GPU3D_SQ_DEBUG_PIX_TB_STATE_MEM_OFFSET           0x3704
#define GPU3D_SQ_PERFCOUNTER0_SELECT_OFFSET              0x3720
#define GPU3D_SQ_PERFCOUNTER1_SELECT_OFFSET              0x3724
#define GPU3D_SQ_PERFCOUNTER2_SELECT_OFFSET              0x3728
#define GPU3D_SQ_PERFCOUNTER3_SELECT_OFFSET              0x372C
#define GPU3D_SQ_PERFCOUNTER0_LOW_OFFSET                 0x3730
#define GPU3D_SQ_PERFCOUNTER0_HI_OFFSET                  0x3734
#define GPU3D_SQ_PERFCOUNTER1_LOW_OFFSET                 0x3738
#define GPU3D_SQ_PERFCOUNTER1_HI_OFFSET                  0x373C
#define GPU3D_SQ_PERFCOUNTER2_LOW_OFFSET                 0x3740
#define GPU3D_SQ_PERFCOUNTER2_HI_OFFSET                  0x3744
#define GPU3D_SQ_PERFCOUNTER3_LOW_OFFSET                 0x3748
#define GPU3D_SQ_PERFCOUNTER3_HI_OFFSET                  0x374C
#define GPU3D_SX_PERFCOUNTER0_SELECT_OFFSET              0x3750
#define GPU3D_SX_PERFCOUNTER0_LOW_OFFSET                 0x3760
#define GPU3D_SX_PERFCOUNTER0_HI_OFFSET                  0x3764
#define GPU3D_TC_CNTL_STATUS_OFFSET                      0x3800
#define GPU3D_TCR_CHICKEN_OFFSET                         0x3808
#define GPU3D_TCF_CHICKEN_OFFSET                         0x380C
#define GPU3D_TCM_CHICKEN_OFFSET                         0x3810
#define GPU3D_TCR_PERFCOUNTER0_SELECT_OFFSET             0x3814
#define GPU3D_TCR_PERFCOUNTER0_HI_OFFSET                 0x3818
#define GPU3D_TCR_PERFCOUNTER0_LOW_OFFSET                0x381C
#define GPU3D_TCR_PERFCOUNTER1_SELECT_OFFSET             0x3820
#define GPU3D_TCR_PERFCOUNTER1_HI_OFFSET                 0x3824
#define GPU3D_TCR_PERFCOUNTER1_LOW_OFFSET                0x3828
#define GPU3D_TP_TC_CLKGATE_CNTL_OFFSET                  0x385C
#define GPU3D_TPC_CNTL_STATUS_OFFSET                     0x3860
#define GPU3D_TPC_DEBUG0_OFFSET                          0x3864
#define GPU3D_TPC_DEBUG1_OFFSET                          0x3868
#define GPU3D_TPC_CHICKEN_OFFSET                         0x386C
#define GPU3D_TP0_CNTL_STATUS_OFFSET                     0x3870
#define GPU3D_TP0_DEBUG_OFFSET                           0x3874
#define GPU3D_TP0_CHICKEN_OFFSET                         0x3878
#define GPU3D_TP0_PERFCOUNTER0_SELECT_OFFSET             0x387C
#define GPU3D_TP0_PERFCOUNTER0_HI_OFFSET                 0x3880
#define GPU3D_TP0_PERFCOUNTER0_LOW_OFFSET                0x3884
#define GPU3D_TP0_PERFCOUNTER1_SELECT_OFFSET             0x3888
#define GPU3D_TP0_PERFCOUNTER1_HI_OFFSET                 0x388C
#define GPU3D_TP0_PERFCOUNTER1_LOW_OFFSET                0x3890
#define GPU3D_TCM_PERFCOUNTER0_SELECT_OFFSET             0x3950
#define GPU3D_TCM_PERFCOUNTER0_HI_OFFSET                 0x3954
#define GPU3D_TCM_PERFCOUNTER0_LOW_OFFSET                0x3958
#define GPU3D_TCM_PERFCOUNTER1_SELECT_OFFSET             0x395C
#define GPU3D_TCM_PERFCOUNTER1_HI_OFFSET                 0x3960
#define GPU3D_TCM_PERFCOUNTER1_LOW_OFFSET                0x3964
#define GPU3D_TCF_PERFCOUNTER0_SELECT_OFFSET             0x3968
#define GPU3D_TCF_PERFCOUNTER0_HI_OFFSET                 0x396C
#define GPU3D_TCF_PERFCOUNTER0_LOW_OFFSET                0x3970
#define GPU3D_TCF_PERFCOUNTER1_SELECT_OFFSET             0x3974
#define GPU3D_TCF_PERFCOUNTER1_HI_OFFSET                 0x3978
#define GPU3D_TCF_PERFCOUNTER1_LOW_OFFSET                0x397C
#define GPU3D_TCF_PERFCOUNTER2_SELECT_OFFSET             0x3980
#define GPU3D_TCF_PERFCOUNTER2_HI_OFFSET                 0x3984
#define GPU3D_TCF_PERFCOUNTER2_LOW_OFFSET                0x3988
#define GPU3D_TCF_PERFCOUNTER3_SELECT_OFFSET             0x398C
#define GPU3D_TCF_PERFCOUNTER3_HI_OFFSET                 0x3990
#define GPU3D_TCF_PERFCOUNTER3_LOW_OFFSET                0x3994
#define GPU3D_TCF_PERFCOUNTER4_SELECT_OFFSET             0x3998
#define GPU3D_TCF_PERFCOUNTER4_HI_OFFSET                 0x399C
#define GPU3D_TCF_PERFCOUNTER4_LOW_OFFSET                0x39A0
#define GPU3D_TCF_PERFCOUNTER5_SELECT_OFFSET             0x39A4
#define GPU3D_TCF_PERFCOUNTER5_HI_OFFSET                 0x39A8
#define GPU3D_TCF_PERFCOUNTER5_LOW_OFFSET                0x39AC
#define GPU3D_TCF_PERFCOUNTER6_SELECT_OFFSET             0x39B0
#define GPU3D_TCF_PERFCOUNTER6_HI_OFFSET                 0x39B4
#define GPU3D_TCF_PERFCOUNTER6_LOW_OFFSET                0x39B8
#define GPU3D_TCF_PERFCOUNTER7_SELECT_OFFSET             0x39BC
#define GPU3D_TCF_PERFCOUNTER7_HI_OFFSET                 0x39C0
#define GPU3D_TCF_PERFCOUNTER7_LOW_OFFSET                0x39C4
#define GPU3D_TCF_PERFCOUNTER8_SELECT_OFFSET             0x39C8
#define GPU3D_TCF_PERFCOUNTER8_HI_OFFSET                 0x39CC
#define GPU3D_TCF_PERFCOUNTER8_LOW_OFFSET                0x39D0
#define GPU3D_TCF_PERFCOUNTER9_SELECT_OFFSET             0x39D4
#define GPU3D_TCF_PERFCOUNTER9_HI_OFFSET                 0x39D8
#define GPU3D_TCF_PERFCOUNTER9_LOW_OFFSET                0x39DC
#define GPU3D_TCF_PERFCOUNTER10_SELECT_OFFSET            0x39E0
#define GPU3D_TCF_PERFCOUNTER10_HI_OFFSET                0x39E4
#define GPU3D_TCF_PERFCOUNTER10_LOW_OFFSET               0x39E8
#define GPU3D_TCF_PERFCOUNTER11_SELECT_OFFSET            0x39EC
#define GPU3D_TCF_PERFCOUNTER11_HI_OFFSET                0x39F0
#define GPU3D_TCF_PERFCOUNTER11_LOW_OFFSET               0x39F4
#define GPU3D_TCF_DEBUG_OFFSET                           0x3B00
#define GPU3D_TCA_FIFO_DEBUG_OFFSET                      0x3B04
#define GPU3D_TCA_PROBE_DEBUG_OFFSET                     0x3B08
#define GPU3D_TCA_TPC_DEBUG_OFFSET                       0x3B0C
#define GPU3D_TCB_CORE_DEBUG_OFFSET                      0x3B10
#define GPU3D_TCB_TAG0_DEBUG_OFFSET                      0x3B14
#define GPU3D_TCB_TAG1_DEBUG_OFFSET                      0x3B18
#define GPU3D_TCB_TAG2_DEBUG_OFFSET                      0x3B1C
#define GPU3D_TCB_TAG3_DEBUG_OFFSET                      0x3B20
#define GPU3D_TCB_FETCH_GEN_SECTOR_WALKER0_DEBUG_OFFSET  0x3B24
#define GPU3D_TCB_FETCH_GEN_WALKER_DEBUG_OFFSET          0x3B2C
#define GPU3D_TCB_FETCH_GEN_PIPE0_DEBUG_OFFSET           0x3B30
#define GPU3D_TCD_INPUT0_DEBUG_OFFSET                    0x3B40
#define GPU3D_TCD_DEGAMMA_DEBUG_OFFSET                   0x3B50
#define GPU3D_TCD_DXTMUX_SCTARB_DEBUG_OFFSET             0x3B54
#define GPU3D_TCD_DXTC_ARB_DEBUG_OFFSET                  0x3B58
#define GPU3D_TCD_STALLS_DEBUG_OFFSET                    0x3B5C
#define GPU3D_TCO_STALLS_DEBUG_OFFSET                    0x3B80
#define GPU3D_TCO_QUAD0_DEBUG0_OFFSET                    0x3B84
#define GPU3D_TCO_QUAD0_DEBUG1_OFFSET                    0x3B88
#define GPU3D_RB_BC_CONTROL_OFFSET                       0x3C04
#define GPU3D_RB_EDRAM_INFO_OFFSET                       0x3C08
#define GPU3D_RB_PERFCOUNTER0_SELECT_OFFSET              0x3C10
#define GPU3D_RB_PERFCOUNTER0_LOW_OFFSET                 0x3C20
#define GPU3D_RB_PERFCOUNTER0_HI_OFFSET                  0x3C24
#define GPU3D_RB_CRC_RD_PORT_OFFSET                      0x3C30
#define GPU3D_RB_CRC_CONTROL_OFFSET                      0x3C34
#define GPU3D_RB_CRC_MASK_OFFSET                         0x3C38
#define GPU3D_RB_TOTAL_SAMPLES_OFFSET                    0x3C3C
#define GPU3D_RB_ZPASS_SAMPLES_OFFSET                    0x3C40
#define GPU3D_RB_ZFAIL_SAMPLES_OFFSET                    0x3C44
#define GPU3D_RB_SFAIL_SAMPLES_OFFSET                    0x3C48
#define GPU3D_RB_DEBUG_0_OFFSET                          0x3C98
#define GPU3D_RB_DEBUG_1_OFFSET                          0x3C9C
#define GPU3D_RB_DEBUG_2_OFFSET                          0x3CA0
#define GPU3D_RB_DEBUG_3_OFFSET                          0x3CA4
#define GPU3D_RB_DEBUG_4_OFFSET                          0x3CA8
#define GPU3D_RB_FLAG_CONTROL_OFFSET                     0x3CAC
#define GPU3D_RB_BC_SPARES_OFFSET                        0x3CB0
#define GPU3D_RB_SURFACE_INFO_OFFSET                     0x8000
#define GPU3D_RB_COLOR_INFO_OFFSET                       0x8004
#define GPU3D_RB_DEPTH_INFO_OFFSET                       0x8008
#define GPU3D_COHER_DEST_BASE_0_OFFSET                   0x8018
#define GPU3D_COHER_DEST_BASE_1_OFFSET                   0x801C
#define GPU3D_COHER_DEST_BASE_2_OFFSET                   0x8020
#define GPU3D_COHER_DEST_BASE_3_OFFSET                   0x8024
#define GPU3D_COHER_DEST_BASE_4_OFFSET                   0x8028
#define GPU3D_COHER_DEST_BASE_5_OFFSET                   0x802C
#define GPU3D_COHER_DEST_BASE_6_OFFSET                   0x8030
#define GPU3D_COHER_DEST_BASE_7_OFFSET                   0x8034
#define GPU3D_PA_SC_SCREEN_SCISSOR_TL_OFFSET             0x8038
#define GPU3D_PA_SC_SCREEN_SCISSOR_BR_OFFSET             0x803C
#define GPU3D_PA_SC_WINDOW_OFFSET_OFFSET                 0x8200
#define GPU3D_PA_SC_WINDOW_SCISSOR_TL_OFFSET             0x8204
#define GPU3D_PA_SC_WINDOW_SCISSOR_BR_OFFSET             0x8208
#define GPU3D_VGT_MAX_VTX_INDX_OFFSET                    0x8400
#define GPU3D_VGT_MIN_VTX_INDX_OFFSET                    0x8404
#define GPU3D_VGT_INDX_OFFSET_OFFSET                     0x8408
#define GPU3D_VGT_MULTI_PRIM_IB_RESET_INDX_OFFSET        0x840C
#define GPU3D_RB_COLOR_MASK_OFFSET                       0x8410
#define GPU3D_RB_BLEND_RED_OFFSET                        0x8414
#define GPU3D_RB_BLEND_GREEN_OFFSET                      0x8418
#define GPU3D_RB_BLEND_BLUE_OFFSET                       0x841C
#define GPU3D_RB_BLEND_ALPHA_OFFSET                      0x8420
#define GPU3D_RB_FOG_COLOR_OFFSET                        0x8424
#define GPU3D_RB_STENCILREFMASK_BF_OFFSET                0x8430
#define GPU3D_RB_STENCILREFMASK_OFFSET                   0x8434
#define GPU3D_RB_ALPHA_REF_OFFSET                        0x8438
#define GPU3D_PA_CL_VPORT_XSCALE_OFFSET                  0x843C
#define GPU3D_PA_CL_VPORT_XOFFSET_OFFSET                 0x8440
#define GPU3D_PA_CL_VPORT_YSCALE_OFFSET                  0x8444
#define GPU3D_PA_CL_VPORT_YOFFSET_OFFSET                 0x8448
#define GPU3D_PA_CL_VPORT_ZSCALE_OFFSET                  0x844C
#define GPU3D_PA_CL_VPORT_ZOFFSET_OFFSET                 0x8450
#define GPU3D_SQ_PROGRAM_CNTL_OFFSET                     0x8600
#define GPU3D_SQ_CONTEXT_MISC_OFFSET                     0x8604
#define GPU3D_SQ_INTERPOLATOR_CNTL_OFFSET                0x8608
#define GPU3D_SQ_WRAPPING_0_OFFSET                       0x860C
#define GPU3D_SQ_WRAPPING_1_OFFSET                       0x8610
#define GPU3D_GFX_COPY_STATE_OFFSET                      0x87D0
#define GPU3D_SQ_CF_RD_BASE_OFFSET                       0x87D4
#define GPU3D_SQ_PS_PROGRAM_OFFSET                       0x87D8
#define GPU3D_SQ_VS_PROGRAM_OFFSET                       0x87DC
#define GPU3D_VGT_EVENT_INITIATOR_OFFSET                 0x87E4
#define GPU3D_VGT_DMA_BASE_OFFSET                        0x87E8
#define GPU3D_VGT_DMA_SIZE_OFFSET                        0x87EC
#define GPU3D_VGT_DRAW_INITIATOR_OFFSET                  0x87F0
#define GPU3D_VGT_IMMED_DATA_OFFSET                      0x87F4
#define GPU3D_VGT_BIN_BASE_OFFSET                        0x87F8
#define GPU3D_VGT_BIN_SIZE_OFFSET                        0x87FC
#define GPU3D_RB_DEPTHCONTROL_OFFSET                     0x8800
#define GPU3D_RB_BLENDCONTROL_OFFSET                     0x8804
#define GPU3D_RB_COLORCONTROL_OFFSET                     0x8808
#define GPU3D_VGT_CURRENT_BIN_ID_MAX_OFFSET              0x880C
#define GPU3D_PA_CL_CLIP_CNTL_OFFSET                     0x8810
#define GPU3D_PA_SU_SC_MODE_CNTL_OFFSET                  0x8814
#define GPU3D_PA_CL_VTE_CNTL_OFFSET                      0x8818
#define GPU3D_VGT_CURRENT_BIN_ID_MIN_OFFSET              0x881C
#define GPU3D_RB_MODECONTROL_OFFSET                      0x8820
#define GPU3D_PA_SU_POINT_SIZE_OFFSET                    0x8A00
#define GPU3D_PA_SU_POINT_MINMAX_OFFSET                  0x8A04
#define GPU3D_PA_SU_LINE_CNTL_OFFSET                     0x8A08
#define GPU3D_PA_SC_LINE_STIPPLE_OFFSET                  0x8A0C
#define GPU3D_PA_SC_VIZ_QUERY_OFFSET                     0x8A4C
#define GPU3D_VGT_ENHANCE_OFFSET                         0x8A50
#define GPU3D_PA_SC_LINE_CNTL_OFFSET                     0x8C00
#define GPU3D_PA_SC_AA_CONFIG_OFFSET                     0x8C04
#define GPU3D_PA_SU_VTX_CNTL_OFFSET                      0x8C08
#define GPU3D_PA_CL_GB_VERT_CLIP_ADJ_OFFSET              0x8C0C
#define GPU3D_PA_CL_GB_VERT_DISC_ADJ_OFFSET              0x8C10
#define GPU3D_PA_CL_GB_HORZ_CLIP_ADJ_OFFSET              0x8C14
#define GPU3D_PA_CL_GB_HORZ_DISC_ADJ_OFFSET              0x8C18
#define GPU3D_SQ_VS_CONST_OFFSET                         0x8C1C
#define GPU3D_SQ_PS_CONST_OFFSET                         0x8C20
#define GPU3D_PA_SC_AA_MASK_OFFSET                       0x8C48
#define GPU3D_VGT_VERTEX_REUSE_BLOCK_CNTL_OFFSET         0x8C58
#define GPU3D_VGT_OUT_DEALLOC_CNTL_OFFSET                0x8C5C
#define GPU3D_RB_COPY_CONTROL_OFFSET                     0x8C60
#define GPU3D_RB_COPY_DEST_BASE_OFFSET                   0x8C64
#define GPU3D_RB_COPY_DEST_PITCH_OFFSET                  0x8C68
#define GPU3D_RB_COPY_DEST_INFO_OFFSET                   0x8C6C
#define GPU3D_RB_COPY_DEST_PIXEL_OFFSET_OFFSET           0x8C70
#define GPU3D_RB_DEPTH_CLEAR_OFFSET                      0x8C74
#define GPU3D_RB_SAMPLE_COUNT_CTL_OFFSET                 0x8C90
#define GPU3D_RB_SAMPLE_COUNT_ADDR_OFFSET                0x8C94
#define GPU3D_RB_COLOR_DEST_MASK_OFFSET                  0x8C98
#define GPU3D_PA_SU_POLY_OFFSET_FRONT_SCALE_OFFSET       0x8E00
#define GPU3D_PA_SU_POLY_OFFSET_FRONT_OFFSET_OFFSET      0x8E04
#define GPU3D_PA_SU_POLY_OFFSET_BACK_SCALE_OFFSET        0x8E08
#define GPU3D_PA_SU_POLY_OFFSET_BACK_OFFSET_OFFSET       0x8E0C
#define GPU3D_SQ_CONSTANT_0_OFFSET                       0x10000
#define GPU3D_SQ_CONSTANT_1_OFFSET                       0x10004
#define GPU3D_SQ_CONSTANT_2_OFFSET                       0x10008
#define GPU3D_SQ_CONSTANT_3_OFFSET                       0x1000C
#define GPU3D_SQ_FETCH_0_OFFSET                          0x12000
#define GPU3D_SQ_FETCH_1_OFFSET                          0x12004
#define GPU3D_SQ_FETCH_2_OFFSET                          0x12008
#define GPU3D_SQ_FETCH_3_OFFSET                          0x1200C
#define GPU3D_SQ_FETCH_4_OFFSET                          0x12010
#define GPU3D_SQ_FETCH_5_OFFSET                          0x12014
#define GPU3D_SQ_CF_BOOLEANS_OFFSET                      0x12400
#define GPU3D_SQ_CF_LOOP_OFFSET                          0x12420
#define GPU3D_SQ_INSTRUCTION_ALU_0_OFFSET                0x14000
#define GPU3D_SQ_INSTRUCTION_ALU_1_OFFSET                0x14004
#define GPU3D_SQ_INSTRUCTION_ALU_2_OFFSET                0x14008
#define GPU3D_SQ_INSTRUCTION_VFETCH_0_OFFSET             0x14100
#define GPU3D_SQ_INSTRUCTION_VFETCH_1_OFFSET             0x14104
#define GPU3D_SQ_INSTRUCTION_VFETCH_2_OFFSET             0x14108
#define GPU3D_SQ_INSTRUCTION_TFETCH_0_OFFSET             0x1410C
#define GPU3D_SQ_INSTRUCTION_TFETCH_1_OFFSET             0x14110
#define GPU3D_SQ_INSTRUCTION_TFETCH_2_OFFSET             0x14114
#define GPU3D_SQ_INSTRUCTION_CF_EXEC_0_OFFSET            0x14200
#define GPU3D_SQ_INSTRUCTION_CF_EXEC_1_OFFSET            0x14204
#define GPU3D_SQ_INSTRUCTION_CF_EXEC_2_OFFSET            0x14208
#define GPU3D_SQ_INSTRUCTION_CF_LOOP_0_OFFSET            0x1420C
#define GPU3D_SQ_INSTRUCTION_CF_LOOP_1_OFFSET            0x14210
#define GPU3D_SQ_INSTRUCTION_CF_LOOP_2_OFFSET            0x14214
#define GPU3D_SQ_INSTRUCTION_CF_JMP_CALL_0_OFFSET        0x14218
#define GPU3D_SQ_INSTRUCTION_CF_JMP_CALL_1_OFFSET        0x1421C
#define GPU3D_SQ_INSTRUCTION_CF_JMP_CALL_2_OFFSET        0x14220
#define GPU3D_SQ_INSTRUCTION_CF_ALLOC_0_OFFSET           0x14224
#define GPU3D_SQ_INSTRUCTION_CF_ALLOC_1_OFFSET           0x14228
#define GPU3D_SQ_INSTRUCTION_CF_ALLOC_2_OFFSET           0x1422C

//#########################################
//# GPU2D - OPENVG registers
//#########################################
#define GPU2D_BASE_ADDR  OPENVG_BASE_ADDR

#define GPU2D_WR_IRQENABLE_OFFSET 0x438
#define GPU2D_WR_IRQENABLE   (GPU2D_BASE_ADDR+GPU2D_WR_IRQENABLE_OFFSET)
#define GPU2D_RD_IRQENABLE_OFFSET 0x038
#define GPU2D_RD_IRQENABLE   (GPU2D_BASE_ADDR+GPU2D_RD_IRQENABLE_OFFSET)

//#########################################
//# L2CC Program control registers
//#########################################
#define L2CC_R0_CACHE_ID_OFFSET	0x0000
#define L2CC_R0_CACHE_TYPE_OFFSET	0x0004
#define L2CC_R1_CNTL_OFFSET	0x0100
#define L2CC_R1_AUX_CNTL_OFFSET	0x0104
#define L2CC_R2_EVENT_CNT_CNTL_OFFSET	0x0200
#define L2CC_R2_EVENT_CNT1_CONFIG_OFFSET	0x0204
#define L2CC_R2_EVENT_CNT0_CONFIG_OFFSET	0x0208
#define L2CC_R2_EVENT_CNT1_VALUE_OFFSET	0x020C
#define L2CC_R2_EVENT_CNT0_VALUE_OFFSET	0x0210
#define L2CC_R2_INT_MASK_OFFSET	0x0214
#define L2CC_R2_MASKED_INT_STATUS_OFFSET	0x0218
#define L2CC_R2_RAW_INT_STATUS_OFFSET	0x021C
#define L2CC_R2_INT_CLEAR_OFFSET	0x0220
#define L2CC_R7_CACHE_SYNC_OFFSET	0x0730
#define L2CC_R7_INVAL_BY_PA_OFFSET	0x0770
#define L2CC_R7_INVAL_BY_WAY_OFFSET	0x077C
#define L2CC_R7_CLEAN_BY_PA_OFFSET	0x07B0
#define L2CC_R7_CLEAN_BY_IDXWAY_OFFSET	0x07B8
#define L2CC_R7_CLEAN_BY_WAY_OFFSET	0x07BC
#define L2CC_R7_CLEANINVAL_LINE_BY_PA_OFFSET	0x07F0
#define L2CC_R7_CLEANINVAL_LINE_BY_IDXWAY_OFFSET	0x07FB
#define L2CC_R7_CLEANINVAL_BY_WAY_OFFSET	0x07FC
#define L2CC_R9_LOCKDOWN_BY_WAY_D_OFFSET	0x0900
#define L2CC_R9_LOCKDOWN_BY_WAY_I_OFFSET	0x0904
#define L2CC_R15_TEST_OPER_OFFSET	0x0F00
#define L2CC_R15_LINE_DATA_0_OFFSET	0x0F10
#define L2CC_R15_LINE_DATA_1_OFFSET	0x0F14
#define L2CC_R15_LINE_DATA_2_OFFSET	0x0F18
#define L2CC_R15_LINE_DATA_3_OFFSET	0x0F1C
#define L2CC_R15_LINE_DATA_4_OFFSET	0x0F20
#define L2CC_R15_LINE_DATA_5_OFFSET	0x0F24
#define L2CC_R15_LINE_DATA_6_OFFSET	0x0F28
#define L2CC_R15_LINE_DATA_7_OFFSET	0x0F2C
#define L2CC_R15_LINE_TAG_OFFSET                        	0x0F30
#define L2CC_R15_DBG_CNTL_OFFSET	0x0F40
//#########################################
//# EVTMON
//#########################################
#define EVTMON_EMMC_OFFSET          0x000
#define EVTMON_EMCS_OFFSET          0x004
#define EVTMON_EMCC0_OFFSET         0x008
#define EVTMON_EMCC1_OFFSET         0x00c
#define EVTMON_EMCC2_OFFSET         0x010
#define EVTMON_EMCC3_OFFSET         0x014
#define EVTMON_EMCC4_OFFSET         0x018
#define EVTMON_EMCC5_OFFSET         0x01c
#define EVTMON_EMC0_OFFSET          0x020
#define EVTMON_EMC1_OFFSET          0x024
#define EVTMON_EMC2_OFFSET          0x028
#define EVTMON_EMC3_OFFSET          0x02c
#define EVTMON_EMC4_OFFSET          0x030
#define EVTMON_EMC5_OFFSET          0x034
#define EVTMON_PNNC0_OFFSET         0x040
#define EVTMON_PNNC1_OFFSET         0x044
#define EVTMON_PNNC2_OFFSET         0x048
#define EVTMON_CCNT_OFFSET          0x04c
#define EVTMON_PMN0_OFFSET          0x050
#define EVTMON_PMN1_OFFSET          0x054
#define EVTMON_PMN2_OFFSET          0x058
#define EVTMON_PMN3_OFFSET          0x05c
#define EVTMON_PMN4_OFFSET          0x060
#define EVTMON_PMN5_OFFSET          0x064
//#########################################
//# Debug ROM
//#########################################

//#########################################
//# ETB
//#########################################

//#########################################
//# ETM
//#########################################

//#########################################
//# TPIU
//#########################################

//#########################################
//# CTI
//#########################################
#define CTICONTROL_OFFSET	0x000
#define CTIINTACK_OFFSET	0x010
#define CTIAPPSET_OFFSET	0x014
#define CTIAPPCLEAR_OFFSET	0x018
#define CTIAPPPULSE_OFFSET	0x01C
#define CTIINEN0_OFFSET	0x020
#define CTIINEN1_OFFSET	0x024
#define CTIINEN2_OFFSET	0x028
#define CTIINEN3_OFFSET	0x02C
#define CTIINEN4_OFFSET	0x030
#define CTIINEN5_OFFSET	0x034
#define CTIINEN6_OFFSET	0x038
#define CTIINEN7_OFFSET	0x03C
#define CTIOUTEN0_OFFSET	0x0A0
#define CTIOUTEN1_OFFSET	0x0A4
#define CTIOUTEN2_OFFSET	0x0A8
#define CTIOUTEN3_OFFSET	0x0AC
#define CTIOUTEN4_OFFSET	0x0B0
#define CTIOUTEN5_OFFSET	0x0B4
#define CTIOUTEN6_OFFSET	0x0B8
#define CTIOUTEN7_OFFSET	0x0BC
#define CTITRIGINSTATUS_OFFSET	0x130
#define CTITRIGOUTSTATUS_OFFSET	0x134
#define CTICHINSTATUS_OFFSET	0x138
#define CTICHOUTSTATUS_OFFSET	0x13C
#define CTIGATE_OFFSET	0x140
#define ASICCTL_OFFSET	0x144
#define CTS_OFFSET	0xFA0
#define CTC_OFFSET	0xFA4
#define ITCR_OFFSET	0xF00
#define ITCHINACK_OFFSET	0xEDC
#define ITTRIGINACK_OFFSET	0xEE0
#define ITCHOUT_OFFSET	0xEE4
#define ITTRIGOUT_OFFSET	0xEE8
#define ITCHOUTACK_OFFSET	0xEEC
#define ITTRIGOUTACK_OFFSET	0xEF0
#define ITCHIN_OFFSET	0xEF4
#define ITTRIGIN_OFFSET	0xEF8
#define CTILOCK_OFFSET	0xFB0
#define CTILOCKSTATUS_OFFSET	0xFB4
#define AUTHENSTATUS_OFFSET	0xFB8
#define DEVICEID_OFFSET	0xFC8
#define DEVICETYPEID_OFFSET	0xFCC
#define PERIPHID0_OFFSET	0xFE0
#define PERIPHID1_OFFSET	0xFE4
#define PERIPHID2_OFFSET	0xFE8
#define PERIPHID3_OFFSET	0xFEC
#define PERIPHID4_OFFSET	0xFD0
#define COMPONENTID0_OFFSET	0xFF0
#define COMPONENTID1_OFFSET	0xFF4
#define COMPONENTID2_OFFSET	0xFF8
#define COMPONENTID3_OFFSET	0xFFC

//#########################################
//# Debug Data Funnel
//#########################################

//#########################################
//# CSD0 SDRAM/DDR
//#########################################

//#########################################
//# CSD1 SDRAM/DDR
//#########################################

//#########################################
//# CS0 (Flash) 128MB
//#########################################

//#########################################
//# CS1 (Flash) 64MB
//#########################################

//#########################################
//# CS2 (sram)
//#########################################

//#########################################
//# CS3 (Spare)
//#########################################

//#########################################
//#  CS4 (Spare)
//#########################################

//#########################################
//# CS5 (spare)
//#########################################

//#########################################
//# Nand Flash
//#########################################

//#########################################
//# MMDC registers
//#########################################

//DDR LOGIC
#define MDCTL_OFFSET	0x00
#define MDPDC_OFFSET	0x04
#define MDOTC_OFFSET	0x08
#define MDCFG0_OFFSET	0x0C
#define MDCFG1_OFFSET	0x10
#define MDCFG2_OFFSET	0x14
#define MDMISC_OFFSET	0x18
#define MDSCR_OFFSET	0x1C
#define MDREF_OFFSET	0x20
#define MDWCC_OFFSET	0x24
#define MDRCC_OFFSET	0x28
#define MDRWD_OFFSET	0x2C
#define MDOR_OFFSET	    0x30
#define MDMRR_OFFSET	0x34
#define MDCFG3LP_OFFSET	0x38
#define MDMR4_OFFSET	0x3C
#define MDASP_OFFSET	0x40

//ADOPT
#define ADOPT_BASE_ADDR  0x400
#define MAARCR_OFFSET     ADOPT_BASE_ADDR + 0x00    // ARCR
#define MAPSR_OFFSET      ADOPT_BASE_ADDR + 0x04    // PSR (former MCR0 and PSM0)
#define MAEXIDR0_OFFSET   ADOPT_BASE_ADDR + 0x08    // Exclusive ID Monitor register0
#define MAEXIDR1_OFFSET   ADOPT_BASE_ADDR + 0x0c    // Exclusive ID Monitor register1
#define MADPCR0_OFFSET    ADOPT_BASE_ADDR + 0x10    // Debug and Profiling Control register0
#define MADPCR1_OFFSET    ADOPT_BASE_ADDR + 0x14    // Debug and Profiling Control register1
#define MADPSR0_OFFSET    ADOPT_BASE_ADDR + 0x18    // Debug and Profiling Control register0
#define MADPSR1_OFFSET    ADOPT_BASE_ADDR + 0x1c    // Debug and Profiling Control register1
#define MADPSR2_OFFSET    ADOPT_BASE_ADDR + 0x20    // Debug and Profiling Control register2
#define MADPSR3_OFFSET    ADOPT_BASE_ADDR + 0x24    // Debug and Profiling Control register3
#define MADPSR4_OFFSET    ADOPT_BASE_ADDR + 0x28    // Debug and Profiling Control register2
#define MADPSR5_OFFSET    ADOPT_BASE_ADDR + 0x2c    // Debug and Profiling Control register2
#define MASBS0_OFFSET     ADOPT_BASE_ADDR + 0x30    // Step by Step Address
#define MASBS1_OFFSET     ADOPT_BASE_ADDR + 0x34    // Step by Step Controls
#define MAGENP_OFFSET     ADOPT_BASE_ADDR + 0x40    // General purpose register

//DDR_PHY
#define PHY_BASE_ADDR  0x800
#define MPZQHWCTRL_OFFSET   PHY_BASE_ADDR + 0x00
#define MPZQSWCTRL_OFFSET   PHY_BASE_ADDR + 0x04
#define MPWLGCR_OFFSET      PHY_BASE_ADDR + 0x08
#define MPWLDECTRL0_OFFSET  PHY_BASE_ADDR + 0x0C
#define MPWLDECTRL1_OFFSET  PHY_BASE_ADDR + 0x10
#define MPWLDLST_OFFSET     PHY_BASE_ADDR + 0x14
#define MPODTCTRL_OFFSET    PHY_BASE_ADDR + 0x18
#define MPRDDQBY0DL_OFFSET  PHY_BASE_ADDR + 0x1C
#define MPRDDQBY1DL_OFFSET  PHY_BASE_ADDR + 0x20
#define MPRDDQBY2DL_OFFSET  PHY_BASE_ADDR + 0x24
#define MPRDDQBY3DL_OFFSET  PHY_BASE_ADDR + 0x28
#define MPWRDQBY0DL_OFFSET  PHY_BASE_ADDR + 0x2C
#define MPWRDQBY1DL_OFFSET  PHY_BASE_ADDR + 0x30
#define MPWRDQBY2DL_OFFSET  PHY_BASE_ADDR + 0x34
#define MPWRDQBY3DL_OFFSET  PHY_BASE_ADDR + 0x38
#define MPDGCTRL0_OFFSET    PHY_BASE_ADDR + 0x3C
#define MPDGCTRL1_OFFSET    PHY_BASE_ADDR + 0x40
#define MPDGDLST_OFFSET     PHY_BASE_ADDR + 0x44
#define MPRDDLCTL_OFFSET    PHY_BASE_ADDR + 0x48
#define MPRDDLST_OFFSET     PHY_BASE_ADDR + 0x4C
#define MPWRDLCTL_OFFSET    PHY_BASE_ADDR + 0x50
#define MPWRDLST_OFFSET     PHY_BASE_ADDR + 0x54
#define MPSDCTRL_OFFSET     PHY_BASE_ADDR + 0x58
#define MPZQLP2CTL_OFFSET   PHY_BASE_ADDR + 0x5C
#define MPRDDLHWCTL_OFFSET  PHY_BASE_ADDR + 0x60
#define MPWRDLHWCTL_OFFSET  PHY_BASE_ADDR + 0x64
#define MPRDDLHWST0_OFFSET  PHY_BASE_ADDR + 0x68
#define MPRDDLHWST1_OFFSET  PHY_BASE_ADDR + 0x6C
#define MPWRDLHWST0_OFFSET  PHY_BASE_ADDR + 0x70
#define MPWRDLHWST1_OFFSET  PHY_BASE_ADDR + 0x74
#define MPWLHWERR_OFFSET    PHY_BASE_ADDR + 0x78
#define MPDGHWST0_OFFSET    PHY_BASE_ADDR + 0x7C
#define MPDGHWST1_OFFSET    PHY_BASE_ADDR + 0x80
#define MPDGHWST2_OFFSET    PHY_BASE_ADDR + 0x84
#define MPDGHWST3_OFFSET    PHY_BASE_ADDR + 0x88
#define MPPDCMPR1_OFFSET    PHY_BASE_ADDR + 0x8C
#define MPPDCMPR2_OFFSET    PHY_BASE_ADDR + 0x90
#define MPSWDAR_OFFSET      PHY_BASE_ADDR + 0x94
#define MPSWDRDR0_OFFSET    PHY_BASE_ADDR + 0x98
#define MPSWDRDR1_OFFSET    PHY_BASE_ADDR + 0x9C
#define MPSWDRDR2_OFFSET    PHY_BASE_ADDR + 0xA0
#define MPSWDRDR3_OFFSET    PHY_BASE_ADDR + 0xA4
#define MPSWDRDR4_OFFSET    PHY_BASE_ADDR + 0xA8
#define MPSWDRDR5_OFFSET    PHY_BASE_ADDR + 0xAC
#define MPSWDRDR6_OFFSET    PHY_BASE_ADDR + 0xB0
#define MPSWDRDR7_OFFSET    PHY_BASE_ADDR + 0xB4
#define MPMUR0_OFFSET       PHY_BASE_ADDR + 0xB8
#define MPWRCADL_OFFSET     PHY_BASE_ADDR + 0xBC
#define MPDCCR_OFFSET       PHY_BASE_ADDR + 0xC0
#define MPBC_OFFSET  		PHY_BASE_ADDR + 0xC4

//#########################################
//# WEIM registers
//#########################################
#define WEIM_CS0_CTL_REG	0x000
#define WEIM_CS0_CTL_REG2	0x004
#define WEIM_CS0_RD_CTL_REG1	0x008
#define WEIM_CS0_RD_CTL_REG2	0x00C
#define WEIM_CS0_WR_CTL_REG	0x010
#define WEIM_CS0_WR_CTL_REG2	0x014
#define WEIM_CS1_CTL_REG	0x018
#define WEIM_CS1_CTL_REG2	0x01C
#define WEIM_CS1_RD_CTL_REG1	0x020
#define WEIM_CS1_RD_CTL_REG2	0x024
#define WEIM_CS1_WR_CTL_REG	0x028
#define WEIM_CS1_WR_CTL_REG2	0x02C
#define WEIM_CS2_CTL_REG	0x030
#define WEIM_CS2_CTL_REG2	0x034
#define WEIM_CS2_RD_CTL_REG1	0x038
#define WEIM_CS2_RD_CTL_REG2	0x03c
#define WEIM_CS2_WR_CTL_REG	0x040
#define WEIM_CS2_WR_CTL_REG2	0x044
#define WEIM_CS3_CTL_REG	0x048
#define WEIM_CS3_CTL_REG2	0x04C
#define WEIM_CS3_RD_CTL_REG1	0x050
#define WEIM_CS3_RD_CTL_REG2	0x054
#define WEIM_CS3_WR_CTL_REG	0x058
#define WEIM_CS3_WR_CTL_REG2	0x05c
#define WEIM_CS4_CTL_REG	0x060
#define WEIM_CS4_CTL_REG2	0x064
#define WEIM_CS4_RD_CTL_REG1	0x068
#define WEIM_CS4_RD_CTL_REG2	0x06C
#define WEIM_CS4_WR_CTL_REG	0x070
#define WEIM_CS4_WR_CTL_REG2	0x074
#define WEIM_CS5_CTL_REG	0x078
#define WEIM_CS5_CTL_REG2	0x07c
#define WEIM_CS5_RD_CTL_REG1	0x080
#define WEIM_CS5_RD_CTL_REG2	0x084
#define WEIM_CS5_WR_CTL_REG	0x088
#define WEIM_CS5_WR_CTL_REG2	0x08c
#define WEIM_CONFIG_REG	0x090
#define WEIM_IP_ACCESS_REG	0x094
#define WEIM_ERR_ADDR_REG	0x098

#define WEIM_CS0_CTL_REG_ADDR       WEIM_IPS_BASE_ADDR+WEIM_CS0_CTL_REG
#define WEIM_CS0_CTL_REG2_ADDR      WEIM_IPS_BASE_ADDR+WEIM_CS0_CTL_REG2
#define WEIM_CS0_RD_CTL_REG1_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS0_RD_CTL_REG1
#define WEIM_CS0_RD_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS0_RD_CTL_REG2
#define WEIM_CS0_WR_CTL_REG_ADDR    WEIM_IPS_BASE_ADDR+WEIM_CS0_WR_CTL_REG
#define WEIM_CS0_WR_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS0_WR_CTL_REG2
#define WEIM_CS1_CTL_REG_ADDR       WEIM_IPS_BASE_ADDR+WEIM_CS1_CTL_REG
#define WEIM_CS1_CTL_REG2_ADDR      WEIM_IPS_BASE_ADDR+WEIM_CS1_CTL_REG2
#define WEIM_CS1_RD_CTL_REG1_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS1_RD_CTL_REG1
#define WEIM_CS1_RD_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS1_RD_CTL_REG2
#define WEIM_CS1_WR_CTL_REG_ADDR    WEIM_IPS_BASE_ADDR+WEIM_CS1_WR_CTL_REG
#define WEIM_CS1_WR_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS1_WR_CTL_REG2
#define WEIM_CS2_CTL_REG_ADDR       WEIM_IPS_BASE_ADDR+WEIM_CS2_CTL_REG
#define WEIM_CS2_CTL_REG2_ADDR      WEIM_IPS_BASE_ADDR+WEIM_CS2_CTL_REG2
#define WEIM_CS2_RD_CTL_REG1_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS2_RD_CTL_REG1
#define WEIM_CS2_RD_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS2_RD_CTL_REG2
#define WEIM_CS2_WR_CTL_REG_ADDR    WEIM_IPS_BASE_ADDR+WEIM_CS2_WR_CTL_REG
#define WEIM_CS2_WR_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS2_WR_CTL_REG2
#define WEIM_CS3_CTL_REG_ADDR       WEIM_IPS_BASE_ADDR+WEIM_CS3_CTL_REG
#define WEIM_CS3_CTL_REG2_ADDR      WEIM_IPS_BASE_ADDR+WEIM_CS3_CTL_REG2
#define WEIM_CS3_RD_CTL_REG1_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS3_RD_CTL_REG1
#define WEIM_CS3_RD_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS3_RD_CTL_REG2
#define WEIM_CS3_WR_CTL_REG_ADDR    WEIM_IPS_BASE_ADDR+WEIM_CS3_WR_CTL_REG
#define WEIM_CS3_WR_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS3_WR_CTL_REG2
#define WEIM_CS4_CTL_REG_ADDR       WEIM_IPS_BASE_ADDR+WEIM_CS4_CTL_REG
#define WEIM_CS4_CTL_REG2_ADDR      WEIM_IPS_BASE_ADDR+WEIM_CS4_CTL_REG2
#define WEIM_CS4_RD_CTL_REG1_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS4_RD_CTL_REG1
#define WEIM_CS4_RD_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS4_RD_CTL_REG2
#define WEIM_CS4_WR_CTL_REG_ADDR    WEIM_IPS_BASE_ADDR+WEIM_CS4_WR_CTL_REG
#define WEIM_CS4_WR_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS4_WR_CTL_REG2
#define WEIM_CS5_CTL_REG_ADDR       WEIM_IPS_BASE_ADDR+WEIM_CS5_CTL_REG
#define WEIM_CS5_CTL_REG2_ADDR      WEIM_IPS_BASE_ADDR+WEIM_CS5_CTL_REG2
#define WEIM_CS5_RD_CTL_REG1_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS5_RD_CTL_REG1
#define WEIM_CS5_RD_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS5_RD_CTL_REG2
#define WEIM_CS5_WR_CTL_REG_ADDR    WEIM_IPS_BASE_ADDR+WEIM_CS5_WR_CTL_REG
#define WEIM_CS5_WR_CTL_REG2_ADDR   WEIM_IPS_BASE_ADDR+WEIM_CS5_WR_CTL_REG2
#define WEIM_CONFIG_REG_ADDR        WEIM_IPS_BASE_ADDR+WEIM_CONFIG_REG
#define WEIM_IP_ACCESS_REG_ADDR     WEIM_IPS_BASE_ADDR+WEIM_IP_ACCESS_REG
#define WEIM_ERR_ADDR_REG_ADDR      WEIM_IPS_BASE_ADDR+WEIM_ERR_ADDR_REG

//#########################################
//# AIPS 1 off platform global module enable no.1
//#########################################
#define AIPS_MPROT0_7_OFFSET	0x00
#define AIPS_MPROT8_15_OFFSET	0x04
#define AIPS_PACR0_7_OFFSET	0x20
#define AIPS_PACR8_15_OFFSET	0x24
#define AIPS_PACR16_23_OFFSET	0x28
#define AIPS_PACR24_31_OFFSET	0x2C
#define AIPS_OPACR0_7_OFFSET	0x40
#define AIPS_OPACR8_15_OFFSET	0x44
#define AIPS_OPACR16_23_OFFSET	0x48
#define AIPS_OPACR24_31_OFFSET	0x4C
#define AIPS_OPACR32_33_OFFSET	0x50

//#########################################
//#  CSU
//#########################################
#define CSU_CSL0_OFFSET	0x0
#define CSU_CSL1_OFFSET	0x4
#define CSU_CSL2_OFFSET	0x8
#define CSU_CSL3_OFFSET	0xC
#define CSU_CSL4_OFFSET	0x10
#define CSU_CSL5_OFFSET	0x14
#define CSU_CSL6_OFFSET	0x18
#define CSU_CSL7_OFFSET	0x1C
#define CSU_CSL8_OFFSET	0x20
#define CSU_CSL9_OFFSET	0x24
#define CSU_CSL10_OFFSET	0x28
#define CSU_CSL11_OFFSET	0x2C
#define CSU_CSL12_OFFSET	0x30
#define CSU_CSL13_OFFSET	0x34
#define CSU_CSL14_OFFSET	0x38
#define CSU_CSL15_OFFSET	0x3C
#define CSU_CSL16_OFFSET	0x40
#define CSU_CSL17_OFFSET	0x44
#define CSU_CSL18_OFFSET	0x48
#define CSU_CSL19_OFFSET	0x4C
#define CSU_CSL20_OFFSET	0x50
#define CSU_CSL21_OFFSET	0x54
#define CSU_CSL22_OFFSET	0x58
#define CSU_CSL23_OFFSET	0x5C
#define CSU_CSL24_OFFSET	0x60
#define CSU_CSL25_OFFSET	0x64
#define CSU_CSL26_OFFSET	0x68
#define CSU_CSL27_OFFSET	0x6C
#define CSU_CSL28_OFFSET	0x70
#define CSU_CSL29_OFFSET	0x74
#define CSU_CSL30_OFFSET	0x78
#define CSU_CSL31_OFFSET	0x7C
#define CSU_CSL32_OFFSET	0x80
#define CSU_CSL33_OFFSET	0x84
#define CSU_CSL34_OFFSET	0x88
#define CSU_CSL35_OFFSET	0x8C
#define CSU_CSL36_OFFSET	0x90
#define CSU_CSL37_OFFSET	0x94
#define CSU_CSL38_OFFSET	0x98
#define CSU_CSL39_OFFSET	0x9C
#define CSU_CSL40_OFFSET	0xA0
#define CSU_CSL41_OFFSET	0xA4
#define CSU_CSL42_OFFSET	0xA8
#define CSU_CSL43_OFFSET	0xAC
#define CSU_CSL44_OFFSET	0xB0
#define CSU_CSL45_OFFSET	0xB4
#define CSU_CSL46_OFFSET	0xB8
#define CSU_CSL47_OFFSET	0xBC
#define CSU_CSL48_OFFSET	0xC0
#define CSU_CSL49_OFFSET	0xC4
#define CSU_CSL50_OFFSET	0xC8
#define CSU_CSL51_OFFSET	0xCC
#define CSU_CSL52_OFFSET	0xD0
#define CSU_CSL53_OFFSET	0xD4
#define CSU_CSL54_OFFSET	0xD8
#define CSU_CSL55_OFFSET	0xDC
#define CSU_CSL56_OFFSET	0xE0
#define CSU_CSL57_OFFSET	0xE4
#define CSU_CSL58_OFFSET	0xE8
#define CSU_CSL59_OFFSET	0xEC
#define CSU_CSL60_OFFSET	0xF0
#define CSU_CSL61_OFFSET	0xF4
#define CSU_CSL62_OFFSET	0xF8
#define CSU_CSL63_OFFSET	0xFC
#define CSU_HP0_OFFSET	0x200
#define CSU_HP1_OFFSET	0x204
#define CSU_SA0_OFFSET	0x218
#define CSU_SA1_OFFSET	0x21C
#define CSU_AMASK0_OFFSET	0x230
#define CSU_AMASK1_OFFSET	0x234
#define CSU_AROUT0_OFFSET	0x244
#define CSU_AROUT1_OFFSET	0x248
#define CSU_AROUT2_OFFSET	0x24C
#define CSU_AROUT3_OFFSET	0x250
#define CSU_AROUT4_OFFSET	0x254
#define CSU_AROUT5_OFFSET	0x258
#define CSU_AROUT6_OFFSET	0x25C
#define CSU_AROUT7_OFFSET	0x260
#define CSU_AROUT8_OFFSET	0x264
#define CSU_AROUT9_OFFSET	0x268
#define CSU_AROUT10_OFFSET	0x26C
#define CSU_AROUT11_OFFSET	0x270
#define CSU_AROUT12_OFFSET	0x274
#define CSU_AROUT13_OFFSET	0x278
#define CSU_AROUT14_OFFSET	0x27C
#define CSU_AROUT15_OFFSET	0x280
#define CSU_AROUT16_OFFSET	0x284
#define CSU_AROUT17_OFFSET	0x288
#define CSU_AROUT18_OFFSET	0x28C
#define CSU_AROUT19_OFFSET	0x290
#define CSU_AROUT20_OFFSET	0x294
#define CSU_AROUT21_OFFSET	0x298
#define CSU_AROUT22_OFFSET	0x29C
#define CSU_AROUT23_OFFSET	0x2A0
#define CSU_AROUT24_OFFSET	0x2A4
#define CSU_AROUT25_OFFSET	0x2A8
#define CSU_AROUT26_OFFSET	0x2AC
#define CSU_AROUT27_OFFSET	0x2B0
#define CSU_AROUT28_OFFSET	0x2B4
#define CSU_AROUT29_OFFSET	0x2B8
#define CSU_AROUT30_OFFSET	0x2BC
#define CSU_AROUT31_OFFSET	0x2C0
#define CSU_ASOFT0_OFFSET	0x344
#define CSU_ACOUNTER0_OFFSET	0x348
#define CSU_ACONTROL0_OFFSET	0x34C
#define CSU_HPCONTROL0_OFFSET	0x358
#define CSU_HPCONTROL1_OFFSET	0x35C
#define CSU_ICR0_OFFSET	0x368
#define CSU_ICR1_OFFSET	0x36C
#define CSU_ISR0_OFFSET	0x378
#define CSU_ISR1_OFFSET	0x37C
//#########################################
//# SDMA(pors IPS_HOST)
//#########################################
#define SDMA_MC0PTR_OFFSET         	0x00
#define SDMA_INTR_OFFSET          	0x04
#define SDMA_STOP_STAT_OFFSET      	0x08
#define SDMA_HSTART_OFFSET         	0x0C
#define SDMA_EVTOVR_OFFSET         	0x10
#define SDMA_DSPOVR_OFFSET         	0x14
#define SDMA_HOSTOVR_OFFSET        	0x18
#define SDMA_EVTPEND_OFFSET        	0x1C
#define SDMA_DSPENBL_OFFSET       	0x20
#define SDMA_RESET_OFFSET          	0x24
#define SDMA_EVTERR_OFFSET         	0x28
#define SDMA_INTRMASK_OFFSET       	0x2C
#define SDMA_PSW_OFFSET            	0x30
#define SDMA_EVTERRDBG_OFFSET     	0x34
#define SDMA_CONFIG_OFFSET        	0x38
#define SDMA_LOCK_OFFSET           	0x3C
#define SDMA_ONCE_ENB_OFFSET       	0x40
#define SDMA_ONCE_DATA_OFFSET      	0x44
#define SDMA_ONCE_INSTR_OFFSET     	0x48
#define SDMA_ONCE_STAT_OFFSET      	0x4C
#define SDMA_ONCE_CMD_OFFSET       	0x50
#define SDMA_ILLINSTADDR_OFFSET    	0x58
#define SDMA_CHN0ADDR_OFFSET       	0x5C
#define SDMA_EVT_MIRROR_OFFSET     	0x60
#define SDMA_EVT_MIRROR2_OFFSET    	0x64
#define SDMA_XTRIG_CONF1_OFFSET    	0x70
#define SDMA_XTRIG_CONF2_OFFSET    	0x74
#define SDMA_CHNPRI00_OFFSET      	0x100
#define SDMA_CHNPRI01_OFFSET       	0x104
#define SDMA_CHNPRI02_OFFSET      	0x108
#define SDMA_CHNPRI03_OFFSET      	0x10C
#define SDMA_CHNPRI04_OFFSET       	0x110
#define SDMA_CHNPRI05_OFFSET       	0x114
#define SDMA_CHNPRI06_OFFSET      	0x118
#define SDMA_CHNPRI07_OFFSET       	0x11C
#define SDMA_CHNPRI08_OFFSET       	0x120
#define SDMA_CHNPRI09_OFFSET       	0x124
#define SDMA_CHNPRI10_OFFSET      	0x128
#define SDMA_CHNPRI11_OFFSET      	0x12C
#define SDMA_CHNPRI12_OFFSET       	0x130
#define SDMA_CHNPRI13_OFFSET       	0x134
#define SDMA_CHNPRI14_OFFSET       	0x138
#define SDMA_CHNPRI15_OFFSET       	0x13C
#define SDMA_CHNPRI16_OFFSET       	0x140
#define SDMA_CHNPRI17_OFFSET       	0x144
#define SDMA_CHNPRI18_OFFSET       	0x148
#define SDMA_CHNPRI19_OFFSET       	0x14C
#define SDMA_CHNPRI20_OFFSET       	0x150
#define SDMA_CHNPRI21_OFFSET       	0x154
#define SDMA_CHNPRI22_OFFSET       	0x158
#define SDMA_CHNPRI23_OFFSET       	0x15C
#define SDMA_CHNPRI24_OFFSET       	0x160
#define SDMA_CHNPRI25_OFFSET       	0x164
#define SDMA_CHNPRI26_OFFSET       	0x168
#define SDMA_CHNPRI27_OFFSET       	0x16C
#define SDMA_CHNPRI28_OFFSET       	0x170
#define SDMA_CHNPRI29_OFFSET       	0x174
#define SDMA_CHNPRI30_OFFSET       	0x178
#define SDMA_CHNPRI31_OFFSET       	0x17C
#define SDMA_CHNENBL00_OFFSET       	0x200
#define SDMA_CHNENBL01_OFFSET       	0x204
#define SDMA_CHNENBL02_OFFSET       	0x208
#define SDMA_CHNENBL03_OFFSET       	0x20C
#define SDMA_CHNENBL04_OFFSET       	0x210
#define SDMA_CHNENBL05_OFFSET       	0x214
#define SDMA_CHNENBL06_OFFSET       	0x218
#define SDMA_CHNENBL07_OFFSET       	0x21C
#define SDMA_CHNENBL08_OFFSET       	0x220
#define SDMA_CHNENBL09_OFFSET       	0x224
#define SDMA_CHNENBL10_OFFSET       	0x228
#define SDMA_CHNENBL11_OFFSET       	0x22C
#define SDMA_CHNENBL12_OFFSET       	0x230
#define SDMA_CHNENBL13_OFFSET       	0x234
#define SDMA_CHNENBL14_OFFSET       	0x238
#define SDMA_CHNENBL15_OFFSET       	0x23C
#define SDMA_CHNENBL16_OFFSET       	0x240
#define SDMA_CHNENBL17_OFFSET       	0x244
#define SDMA_CHNENBL18_OFFSET       	0x248
#define SDMA_CHNENBL19_OFFSET       	0x24C
#define SDMA_CHNENBL20_OFFSET       	0x250
#define SDMA_CHNENBL21_OFFSET       	0x254
#define SDMA_CHNENBL22_OFFSET       	0x258
#define SDMA_CHNENBL23_OFFSET       	0x25C
#define SDMA_CHNENBL24_OFFSET       	0x260
#define SDMA_CHNENBL25_OFFSET       	0x264
#define SDMA_CHNENBL26_OFFSET       	0x268
#define SDMA_CHNENBL27_OFFSET       	0x26C
#define SDMA_CHNENBL28_OFFSET       	0x270
#define SDMA_CHNENBL29_OFFSET       	0x274
#define SDMA_CHNENBL30_OFFSET       	0x278
#define SDMA_CHNENBL31_OFFSET       	0x27C
#define SDMA_CHNENBL32_OFFSET       	0x280
#define SDMA_CHNENBL33_OFFSET       	0x284
#define SDMA_CHNENBL34_OFFSET       	0x288
#define SDMA_CHNENBL35_OFFSET       	0x28C
#define SDMA_CHNENBL36_OFFSET       	0x290
#define SDMA_CHNENBL37_OFFSET       	0x294
#define SDMA_CHNENBL38_OFFSET       	0x298
#define SDMA_CHNENBL39_OFFSET       	0x29C
#define SDMA_CHNENBL40_OFFSET       	0x2A0
#define SDMA_CHNENBL41_OFFSET       	0x2A4
#define SDMA_CHNENBL42_OFFSET       	0x2A8
#define SDMA_CHNENBL43_OFFSET       	0x2AC
#define SDMA_CHNENBL44_OFFSET       	0x2B0
#define SDMA_CHNENBL45_OFFSET       	0x2B4
#define SDMA_CHNENBL46_OFFSET       	0x2B8
#define SDMA_CHNENBL47_OFFSET       	0x2BC

//#########################################
//# ROMCP(via IPSYNC)
//#########################################
#define ROMCP_CNTL_OFFSET	0x00F4
#define ROMCP_ENL_OFFSET	0x00FC
#define ROMCP_ENH_OFFSET	0x00F8
#define ROMCP_STAT_OFFSET	0x0208
#define ROMCP_ADDR0_OFFSET	0x0100
#define ROMCP_ADDR1_OFFSET	0x0104
#define ROMCP_ADDR2_OFFSET	0x0108
#define ROMCP_ADDR3_OFFSET	0x010C
#define ROMCP_ADDR4_OFFSET	0x0110
#define ROMCP_ADDR5_OFFSET	0x0114
#define ROMCP_ADDR6_OFFSET	0x0118
#define ROMCP_ADDR7_OFFSET	0x011C
#define ROMCP_ADDR8_OFFSET	0x0120
#define ROMCP_ADDR9_OFFSET	0x0124
#define ROMCP_ADDRA_OFFSET	0x0128
#define ROMCP_ADDRB_OFFSET	0x012C
#define ROMCP_ADDRC_OFFSET	0x0130
#define ROMCP_ADDRD_OFFSET	0x0134
#define ROMCP_ADDRE_OFFSET	0x0138
#define ROMCP_ADDRF_OFFSET	0x013C
#define ROMCP_DATA0_OFFSET	0x00F0
#define ROMCP_DATA1_OFFSET	0x00EC
#define ROMCP_DATA2_OFFSET	0x00E8
#define ROMCP_DATA3_OFFSET	0x00E4
#define ROMCP_DATA4_OFFSET	0x00E0
#define ROMCP_DATA5_OFFSET	0x00DC
#define ROMCP_DATA6_OFFSET	0x00D8
#define ROMCP_DATA7_OFFSET	0x00D4

//#########################################
//# USB
//#########################################
#define USBOH3_USB_IPS_BASE_ADDR   (USBOH3_USB_BASE_ADDR)
#define USB_OTG_BASE_ADDR          (USBOH3_USB_IPS_BASE_ADDR+0x000)
#define USB_H1_BASE_ADDR           (USBOH3_USB_IPS_BASE_ADDR+0x200)
#define USB_H2_BASE_ADDR           (USBOH3_USB_IPS_BASE_ADDR+0x400)
#define USB_H3_BASE_ADDR           (USBOH3_USB_IPS_BASE_ADDR+0x600)
#define USB_OTG_CTRL_REG           (USBOH3_USB_IPS_BASE_ADDR+0x800)
#define USB_UH1_CTRL_REG           (USBOH3_USB_IPS_BASE_ADDR+0x804)
#define USB_UH2_CTRL_REG           (USBOH3_USB_IPS_BASE_ADDR+0x808)
#define USB_UH3_CTRL_REG           (USBOH3_USB_IPS_BASE_ADDR+0x80C)
#define USB_UH2_HSIC_REG           (USBOH3_USB_IPS_BASE_ADDR+0x810)
#define USB_UH3_HSIC_REG           (USBOH3_USB_IPS_BASE_ADDR+0x814)
#define USB_OTG_UTMIPHY_0          (USBOH3_USB_IPS_BASE_ADDR+0x818)
#define USB_UH1_UTMIPHY_0          (USBOH3_USB_IPS_BASE_ADDR+0x81C)
#define USB_UH2_HSIC_DLL_CFG1      (USBOH3_USB_IPS_BASE_ADDR+0x820)
#define USB_UH2_HSIC_DLL_CFG2      (USBOH3_USB_IPS_BASE_ADDR+0x824)
#define USB_UH2_HSIC_DLL_CFG3      (USBOH3_USB_IPS_BASE_ADDR+0x828)
#define USB_UH2_HSIC_DLL_STS       (USBOH3_USB_IPS_BASE_ADDR+0x82C)
#define USB_UH3_HSIC_DLL_CFG1      (USBOH3_USB_IPS_BASE_ADDR+0x830)
#define USB_UH3_HSIC_DLL_CFG2      (USBOH3_USB_IPS_BASE_ADDR+0x834)
#define USB_UH3_HSIC_DLL_CFG3      (USBOH3_USB_IPS_BASE_ADDR+0x838)
#define USB_UH3_HSIC_DLL_STS       (USBOH3_USB_IPS_BASE_ADDR+0x83C)

#define USB_H1_ID                  (USB_H1_BASE_ADDR+0x000) // Identification Register
#define USB_H1_HWGENERAL           (USB_H1_BASE_ADDR+0x004) // General Hardware Parameters
#define USB_H1_HWHOST              (USB_H1_BASE_ADDR+0x008) // Host Hardware Parameters
#define USB_H1_HWTXBUF             (USB_H1_BASE_ADDR+0x010) // TX Buffer Hardware Parameters
#define USB_H1_HWRXBUF             (USB_H1_BASE_ADDR+0x014) // RX Buffer Hardware Parameters
#define USB_H1_GPTIMER0LD          (USB_H1_BASE_ADDR+0x080)
#define USB_H1_GPTIMER0CTRL        (USB_H1_BASE_ADDR+0x084)
#define USB_H1_GPTIMER1LD          (USB_H1_BASE_ADDR+0x088)
#define USB_H1_GPTIMER1CTRL        (USB_H1_BASE_ADDR+0x08C)
#define USB_H1_SBUSCFG             (USB_H1_BASE_ADDR+0x090)
#define USB_H1_CAPLENGTH           (USB_H1_BASE_ADDR+0x100) // Capability Register Length
#define USB_H1_HCIVERSION          (USB_H1_BASE_ADDR+0x102) // Host Interface Version Number
#define USB_H1_HCSPARAMS           (USB_H1_BASE_ADDR+0x104) // Host Ctrl. Structural Parameters
#define USB_H1_HCCPARAMS           (USB_H1_BASE_ADDR+0x108) // Host Ctrl. Capability Parameters
#define USB_H1_USBCMD              (USB_H1_BASE_ADDR+0x140) // USB Command
#define USB_H1_USBSTS              (USB_H1_BASE_ADDR+0x144) // USB Status
#define USB_H1_USBINTR             (USB_H1_BASE_ADDR+0x148) // USB Interrupt Enable
#define USB_H1_FRINDEX             (USB_H1_BASE_ADDR+0x14C) // USB Frame Index
#define USB_H1_PERIODICLISTBASE    (USB_H1_BASE_ADDR+0x154) // Frame List Base Address
#define USB_H1_ASYNCLISTADDR       (USB_H1_BASE_ADDR+0x158) // Next Asynchronous List Address
#define USB_H1_BURSTSIZE           (USB_H1_BASE_ADDR+0x160) // Programmable Burst Size
#define USB_H1_TXFILLTUNING        (USB_H1_BASE_ADDR+0x164) // Host Transmit Pre-Buffer Packet Tuning
#define USB_H1_ICUSB               (USB_H1_BASE_ADDR+0x16C) // Host IC USB
#define USB_H1_CONFIGFLAG          (USB_H1_BASE_ADDR+0x180) // Configured Flag Register
#define USB_H1_PORTSC1             (USB_H1_BASE_ADDR+0x184) // Port Status/Control
#define USB_H1_USBMODE             (USB_H1_BASE_ADDR+0x1A8) // USB Device Mode

#define USB_H2_ID                  (USB_H2_BASE_ADDR+0x000) // Identification Register
#define USB_H2_HWGENERAL           (USB_H2_BASE_ADDR+0x004) // General Hardware Parameters
#define USB_H2_HWHOST              (USB_H2_BASE_ADDR+0x008) // Host Hardware Parameters
#define USB_H2_HWTXBUF             (USB_H2_BASE_ADDR+0x010) // TX Buffer Hardware Parameters
#define USB_H2_HWRXBUF             (USB_H2_BASE_ADDR+0x014) // RX Buffer Hardware Parameters
#define USB_H2_GPTIMER0LD          (USB_H2_BASE_ADDR+0x080)
#define USB_H2_GPTIMER0CTRL        (USB_H2_BASE_ADDR+0x084)
#define USB_H2_GPTIMER1LD          (USB_H2_BASE_ADDR+0x088)
#define USB_H2_GPTIMER1CTRL        (USB_H2_BASE_ADDR+0x08C)
#define USB_H2_SBUSCFG             (USB_H2_BASE_ADDR+0x090)
#define USB_H2_CAPLENGTH           (USB_H2_BASE_ADDR+0x100) // Capability Register Length
#define USB_H2_HCIVERSION          (USB_H2_BASE_ADDR+0x102) // Host Interface Version Number
#define USB_H2_HCSPARAMS           (USB_H2_BASE_ADDR+0x104) // Host Ctrl. Structural Parameters
#define USB_H2_HCCPARAMS           (USB_H2_BASE_ADDR+0x108) // Host Ctrl. Capability Parameters
#define USB_H2_USBCMD              (USB_H2_BASE_ADDR+0x140) // USB Command
#define USB_H2_USBSTS              (USB_H2_BASE_ADDR+0x144) // USB Status
#define USB_H2_USBINTR             (USB_H2_BASE_ADDR+0x148) // USB Interrupt Enable
#define USB_H2_FRINDEX             (USB_H2_BASE_ADDR+0x14C) // USB Frame Index
#define USB_H2_PERIODICLISTBASE    (USB_H2_BASE_ADDR+0x154) // Frame List Base Address
#define USB_H2_ASYNCLISTADDR       (USB_H2_BASE_ADDR+0x158) // Next Asynchronous List Address
#define USB_H2_BURSTSIZE           (USB_H2_BASE_ADDR+0x160) // Programmable Burst Size
#define USB_H2_TXFILLTUNING        (USB_H2_BASE_ADDR+0x164) // Host Transmit Pre-Buffer Packet Tuning
#define USB_H2_ICUSB               (USB_H2_BASE_ADDR+0x16C) // Host IC USB
#define USB_H2_CONFIGFLAG          (USB_H2_BASE_ADDR+0x180) // Configured Flag Register
#define USB_H2_PORTSC1             (USB_H2_BASE_ADDR+0x184) // Port Status/Control
#define USB_H2_USBMODE             (USB_H2_BASE_ADDR+0x1A8) // USB Device Mode

#define USB_H3_ID                  (USB_H3_BASE_ADDR+0x000) // Identification Register
#define USB_H3_HWGENERAL           (USB_H3_BASE_ADDR+0x004) // General Hardware Parameters
#define USB_H3_HWHOST              (USB_H3_BASE_ADDR+0x008) // Host Hardware Parameters
#define USB_H3_HWTXBUF             (USB_H3_BASE_ADDR+0x010) // TX Buffer Hardware Parameters
#define USB_H3_HWRXBUF             (USB_H3_BASE_ADDR+0x014) // RX Buffer Hardware Parameters
#define USB_H3_GPTIMER0LD          (USB_H3_BASE_ADDR+0x080)
#define USB_H3_GPTIMER0CTRL        (USB_H3_BASE_ADDR+0x084)
#define USB_H3_GPTIMER1LD          (USB_H3_BASE_ADDR+0x088)
#define USB_H3_GPTIMER1CTRL        (USB_H3_BASE_ADDR+0x08C)
#define USB_H3_SBUSCFG             (USB_H3_BASE_ADDR+0x090)
#define USB_H3_CAPLENGTH           (USB_H3_BASE_ADDR+0x100) // Capability Register Length
#define USB_H3_HCIVERSION          (USB_H3_BASE_ADDR+0x102) // Host Interface Version Number
#define USB_H3_HCSPARAMS           (USB_H3_BASE_ADDR+0x104) // Host Ctrl. Structural Parameters
#define USB_H3_HCCPARAMS           (USB_H3_BASE_ADDR+0x108) // Host Ctrl. Capability Parameters
#define USB_H3_USBCMD              (USB_H3_BASE_ADDR+0x140) // USB Command
#define USB_H3_USBSTS              (USB_H3_BASE_ADDR+0x144) // USB Status
#define USB_H3_USBINTR             (USB_H3_BASE_ADDR+0x148) // USB Interrupt Enable
#define USB_H3_FRINDEX             (USB_H3_BASE_ADDR+0x14C) // USB Frame Index
#define USB_H3_PERIODICLISTBASE    (USB_H3_BASE_ADDR+0x154) // Frame List Base Address
#define USB_H3_ASYNCLISTADDR       (USB_H3_BASE_ADDR+0x158) // Next Asynchronous List Address
#define USB_H3_BURSTSIZE           (USB_H3_BASE_ADDR+0x160) // Programmable Burst Size
#define USB_H3_TXFILLTUNING        (USB_H3_BASE_ADDR+0x164) // Host Transmit Pre-Buffer Packet Tuning
#define USB_H3_ICUSB               (USB_H3_BASE_ADDR+0x16C) // Host IC USB
#define USB_H3_CONFIGFLAG          (USB_H3_BASE_ADDR+0x180) // Configured Flag Register
#define USB_H3_PORTSC1             (USB_H3_BASE_ADDR+0x184) // Port Status/Control
#define USB_H3_USBMODE             (USB_H3_BASE_ADDR+0x1A8) // USB Device Mode

#define USB_OTG_ID                 (USB_OTG_BASE_ADDR+0x000)    // Identification Register
#define USB_OTG_HWGENERAL          (USB_OTG_BASE_ADDR+0x004)    // General Hardware Parameters
#define USB_OTG_HWHOST             (USB_OTG_BASE_ADDR+0x008)    // Host Hardware Parameters
#define USB_OTG_HWDEVICE           (USB_OTG_BASE_ADDR+0x00C)    // Device Hardware Parameters
#define USB_OTG_HWTXBUF            (USB_OTG_BASE_ADDR+0x010)    // TX Buffer Hardware Parameters
#define USB_OTG_HWRXBUF            (USB_OTG_BASE_ADDR+0x014)    // RX Buffer Hardware Parameters
#define USB_OTG_GPTIMER0LD         (USB_OTG_BASE_ADDR+0x080)
#define USB_OTG_GPTIMER0CTRL       (USB_OTG_BASE_ADDR+0x084)
#define USB_OTG_GPTIMER1LD         (USB_OTG_BASE_ADDR+0x088)
#define USB_OTG_GPTIMER1CTRL       (USB_OTG_BASE_ADDR+0x08C)
#define USB_OTG_SBUSCFG            (USB_OTG_BASE_ADDR+0x090)
#define USB_OTG_EPSEL              (USB_OTG_BASE_ADDR+0x094)
#define USB_OTG_CAPLENGTH          (USB_OTG_BASE_ADDR+0x100)    // Capability Register Length
#define USB_OTG_HCIVERSION         (USB_OTG_BASE_ADDR+0x102)    // Host Interface Version Number
#define USB_OTG_HCSPARAMS          (USB_OTG_BASE_ADDR+0x104)    // Host Ctrl. Structural Parameters
#define USB_OTG_HCCPARAMS          (USB_OTG_BASE_ADDR+0x108)    // Host Ctrl. Capability Parameters
#define USB_OTG_DCIVERSION         (USB_OTG_BASE_ADDR+0x120)    // Dev. Interface Version Number
#define USB_OTG_DCCPARAMS          (USB_OTG_BASE_ADDR+0x124)    // Device Ctrl. Capability Parameters
#define USB_OTG_USBCMD             (USB_OTG_BASE_ADDR+0x140)    // USB Command
#define USB_OTG_USBSTS             (USB_OTG_BASE_ADDR+0x144)    // USB Status
#define USB_OTG_USBINTR            (USB_OTG_BASE_ADDR+0x148)    // USB Interrupt Enable
#define USB_OTG_FRINDEX            (USB_OTG_BASE_ADDR+0x14C)    // USB Frame Index
#define USB_OTG_PERIODICLISTBASE   (USB_OTG_BASE_ADDR+0x154)    // Frame List Base Address
#define USB_OTG_ASYNCLISTADDR      (USB_OTG_BASE_ADDR+0x158)    // Next Asynchronous List Address
#define USB_OTG_BURSTSIZE          (USB_OTG_BASE_ADDR+0x160)    // Programmable Burst Size
#define USB_OTG_TXFILLTUNING       (USB_OTG_BASE_ADDR+0x164)    // Host Transmit Pre-Buffer Packet Tuning
#define USB_OTG_ICUSB              (USB_OTG_BASE_ADDR+0x16C)    // OTG IC USB
#define USB_OTG_CONFIGFLAG         (USB_OTG_BASE_ADDR+0x180)    // Configured Flag Register
#define USB_OTG_PORTSC1            (USB_OTG_BASE_ADDR+0x184)    // Port Status/Control
#define USB_OTG_OTGSC              (USB_OTG_BASE_ADDR+0x1A4)    // On-The-Go (OTG) Status and Control
#define USB_OTG_USBMODE            (USB_OTG_BASE_ADDR+0x1A8)    // USB Device Mode
#define USB_OTG_ENPDTSETUPSTAT     (USB_OTG_BASE_ADDR+0x1AC)    // Endpoint Setup Status
#define USB_OTG_ENDPTPRIME         (USB_OTG_BASE_ADDR+0x1B0)    // Endpoint Initialization
#define USB_OTG_ENDPTFLUSH         (USB_OTG_BASE_ADDR+0x1B4)    // Endpoint De-Initialize
#define USB_OTG_ENDPTSTATUS        (USB_OTG_BASE_ADDR+0x1B8)    // Endpoint Status
#define USB_OTG_ENDPTCOMPLETE      (USB_OTG_BASE_ADDR+0x1BC)    // Endpoint Complete
#define USB_OTG_ENDPTCTRL0         (USB_OTG_BASE_ADDR+0x1C0)    // Endpoint Control 0
#define USB_OTG_ENDPTCTRL1         (USB_OTG_BASE_ADDR+0x1C4)    // Endpoint Control 1
#define USB_OTG_ENDPTCTRL2         (USB_OTG_BASE_ADDR+0x1C8)    // Endpoint Control 2
#define USB_OTG_ENDPTCTRL3         (USB_OTG_BASE_ADDR+0x1CC)    // Endpoint Control 3
#define USB_OTG_ENDPTCTRL4         (USB_OTG_BASE_ADDR+0x1D0)    // Endpoint Control 4
#define USB_OTG_ENDPTCTRL5         (USB_OTG_BASE_ADDR+0x1D4)    // Endpoint Control 5
#define USB_OTG_ENDPTCTRL6         (USB_OTG_BASE_ADDR+0x1D8)    // Endpoint Control 6
#define USB_OTG_ENDPTCTRL7         (USB_OTG_BASE_ADDR+0x1DC)    // Endpoint Control 7

//#########################################
//#SJC
//#########################################
#define SJC_GPUSR1_OFFSET	0x00
#define SJC_GPUSR2_OFFSET	0x01
#define SJC_GPUSR3_OFFSET	0x02
#define SJC_GPSSR_OFFSET	0x03
#define SJC_DCR_OFFSET	0x04
#define SJC_SSR_OFFSET	0x05
#define SJC_CPCR_OFFSET	0x06
#define SJC_GPCCR_OFFSET	0x07
#define SJC_PLLBR_OFFSET	0x08
#define SJC_GPUCR1_OFFSET	0x09
#define SJC_GPUCR2_OFFSET	0x0A
#define SJC_GPUCR3_OFFSET	0x0B
#define SJC_GPSCR_OFFSET	0x0C
#define SJC_TESTREG_OFFSET	0x0D
#define SJC_SASR_OFFSET	0x0E
#define SJC_BISTCR1_OFFSET	0x0F
#define SJC_BISTCR2_OFFSET	0x10
#define SJC_BISTCR3_OFFSET	0x11
#define SJC_BISTCR4_OFFSET	0x12
#define SJC_BISTCR5_OFFSET	0x13
#define SJC_BISTCR6_OFFSET	0x14
#define SJC_BISTCR7_OFFSET	0x15
#define SJC_MBISTPASSR1_OFFSET	0x16
#define SJC_MBISTPASSR2_OFFSET	0x17
#define SJC_MBISTDONER1_OFFSET	0x18
#define SJC_MBISTDONER2_OFFSET	0x19
#define SJC_MBISTMASKR1_OFFSET	0x1A
#define SJC_MBISTMASKR2_OFFSET	0x1B
#define SJC_BISTPASSR_OFFSET	0x1C
#define SJC_BISTDONER_OFFSET	0x1D
#define SJC_MONBISTSELR_OFFSET	0x1E
#define SJC_RWVALCR_OFFSET	0x1F
//#########################################
//# CCM
//#########################################
#define CCM_CCR_OFFSET  0x00
#define CCM_CCDR_OFFSET 0x04
#define CCM_CSR_OFFSET  0x08
#define CCM_CCSR_OFFSET 0x0C
#define CCM_CACRR_OFFSET        0x10
#define CCM_CBCDR_OFFSET        0x14
#define CCM_CBCMR_OFFSET 0X18
#define CCM_CSCMR1_OFFSET 0x1c
#define CCM_CSCMR2_OFFSET 0x20
#define CCM_CSCDR1_OFFSET 0x24
#define CCM_CS1CDR_OFFSET 0x28
#define CCM_CS2CDR_OFFSET 0x2c
#define CCM_CDCDR_OFFSET 0x30
#define CCM_CHSCCDR_OFFSET 0x34
#define CCM_CSCDR2_OFFSET 0x38
#define CCM_CSCDR3_OFFSET 0x3c
#define CCM_CSCDR4_OFFSET 0x40
#define CCM_CWDR_OFFSET   0x44
#define CCM_CDHIPR_OFFSET 0x48
#define CCM_CDCR_OFFSET   0x4c
#define CCM_CTOR_OFFSET   0x50
#define CCM_CLPCR_OFFSET  0x54
#define CCM_CISR_OFFSET 0x58
#define CCM_CIMR_OFFSET 0x5c
#define CCM_CCOSR_OFFSET 0x60
#define CCM_CGPR_OFFSET 0x64
#define CCM_CCGR0_OFFSET 0x68
#define CCM_CCGR1_OFFSET 0x6c
#define CCM_CCGR2_OFFSET 0x70
#define CCM_CCGR3_OFFSET 0x74
#define CCM_CCGR4_OFFSET 0x78
#define CCM_CCGR5_OFFSET 0x7c
#define CCM_CCGR6_OFFSET 0x80
#define CCM_CCGR7_OFFSET 0x84
#define CCM_CMEOR_OFFSET 0x88

//#########################################
//# GPC
//#########################################
#define GPC_CNTR_OFFSET	0x0
#define GPC_PGR_OFFSET	0x4
#define GPC_IMR1_OFFSET	0x8
#define GPC_IMR2_OFFSET	0xc
#define GPC_IMR3_OFFSET	0x10
#define GPC_IMR4_OFFSET	0x14
#define GPC_ISR1_OFFSET	0x18
#define GPC_ISR2_OFFSET	0x1c
#define GPC_ISR3_OFFSET	0x20
#define GPC_ISR4_OFFSET	0x24
//#define GPC_VCR_OFFSET    0x8
//#define GPC_ALL_PU_OFFSET 0xc
//#define GPC_NEON_OFFSET   0x10

// GPC_DPTC_LP
#define GPC_DPTC_LP_DPTCCR_OFFSET 0x80
#define GPC_DPTC_LP_DPTCDBG_OFFSET 0x84
#define GPC_DPTC_LP_DCVR0_OFFSET 0x88
#define GPC_DPTC_LP_DCVR1_OFFSET 0x8c
#define GPC_DPTC_LP_DCVR2_OFFSET 0x90
#define GPC_DPTC_LP_DCVR3_OFFSET 0x94

// GPC_DPTC_GP
#define GPC_DPTC_GP_DPTCCR_OFFSET 0x100
#define GPC_DPTC_GP_DPTCDBG_OFFSET 0x104
#define GPC_DPTC_GP_DCVR0_OFFSET 0x108
#define GPC_DPTC_GP_DCVR1_OFFSET 0x10c
#define GPC_DPTC_GP_DCVR2_OFFSET 0x110
#define GPC_DPTC_GP_DCVR3_OFFSET 0x114

// GPC_DVFS_CORE
#define GPC_DVFS_CORE_DVFSTHRS_OFFSET 0x180
#define GPC_DVFS_CORE_DVFSCOUN_OFFSET 0x184
#define GPC_DVFS_CORE_DVFSSIG1_OFFSET 0x188
#define GPC_DVFS_CORE_DVFDDIG0_OFFSET 0x18c
#define GPC_DVFS_CORE_DVFSGPC0_OFFSET 0x190
#define GPC_DVFS_CORE_DVFSGPC1_OFFSET 0x194
#define GPC_DVFS_CORE_DVFSGPBT_OFFSET 0x198
#define GPC_DVFS_CORE_DVFSEMAC_OFFSET 0x19c
#define GPC_DVFS_CORE_DVFSCNTR_OFFSET 0x1a0
#define GPC_DVFS_CORE_DVFSSLTR0_0_OFFSET 0x1a4
#define GPC_DVFS_CORE_DVFSSLTR0_1_OFFSET 0x1a8
#define GPC_DVFS_CORE_DVFSSLTR1_0_OFFSET 0x1ac
#define GPC_DVFS_CORE_DVFSSLTR1_1_OFFSET 0x1b0
#define GPC_DVFS_CORE_DVFSPT0_OFFSET 0x1b4
#define GPC_DVFS_CORE_DVFSPT1_OFFSET 0x1b8
#define GPC_DVFS_CORE_DVFSPT2_OFFSET 0x1bc
#define GPC_DVFS_CORE_DVFSPT3_OFFSET 0x1c0

// GPC_DVFS_PER
#define GPC_DVFS_PER_LTR0_OFFSET 0x1c4
#define GPC_DVFS_PER_LTR1_OFFSET 0x1c8
#define GPC_DVFS_PER_LTR2_OFFSET 0x1cc
#define GPC_DVFS_PER_LTR3_OFFSET 0x1d0
#define GPC_DVFS_PER_LTBR0_OFFSET 0x1d4
#define GPC_DVFS_PER_LTBR1_OFFSET 0x1d8
#define GPC_DVFS_PER_PMCR0_OFFSET 0x1dc
#define GPC_DVFS_PER_PMCR1_OFFSET 0x1e0

//GPC_PGC_GPU2D
#define GPC_PGC_GPU2D_PGCR_OFFSET   0x200
#define GPC_PGC_GPU2D_PUPSCR_OFFSET 0x204
#define GPC_PGC_GPU2D_PDNSCR_OFFSET 0x208
#define GPC_PGC_GPU2D_PGSR_OFFSET   0x20c

//GPC_PGC_IPU
#define GPC_PGC_IPU_PGCR_OFFSET   0x220
#define GPC_PGC_IPU_PUPSCR_OFFSET 0x224
#define GPC_PGC_IPU_PDNSCR_OFFSET 0x228
#define GPC_PGC_IPU_PGSR_OFFSET   0x22c

//GPC_PGC_VPU
#define GPC_PGC_VPU_PGCR_OFFSET   0x240
#define GPC_PGC_VPU_PUPSCR_OFFSET 0x244
#define GPC_PGC_VPU_PDNSCR_OFFSET 0x248
#define GPC_PGC_VPU_PGSR_OFFSET   0x24c

//GPC_PGC_GPU
#define GPC_PGC_GPU_PGCR_OFFSET   0x260
#define GPC_PGC_GPU_PUPSCR_OFFSET 0x264
#define GPC_PGC_GPU_PDNSCR_OFFSET 0x268
#define GPC_PGC_GPU_PGSR_OFFSET   0x26c

//GPC_SRPGC_NEON
#define GPC_SRPGC_NEON_SRPGCR_OFFSET 0x280
#define GPC_SRPGC_NEON_PUPSCR_OFFSET 0x284
#define GPC_SRPGC_NEON_PDNSCR_OFFSET 0x288
#define GPC_SRPGC_NEON_SRPGSR_OFFSET 0x28c
#define GPC_SRPGC_NEON_SRPGDR_OFFSET 0x290

//GPC_SRPGC_TIGER
#define GPC_SRPGC_TIGER_SRPGCR_OFFSET 0x2a0
#define GPC_SRPGC_TIGER_PUPSCR_OFFSET 0x2a4
#define GPC_SRPGC_TIGER_PDNSCR_OFFSET 0x2a8
#define GPC_SRPGC_TIGER_SRPGSR_OFFSET 0x2ac
#define GPC_SRPGC_TIGER_SRPGDR_OFFSET 0x2b0

//GPC_EMPGC0_TIGER
#define GPC_EMPGC0_TIGER_EMPGCR_OFFSET 0x2c0
#define GPC_EMPGC0_TIGER_PUPSCR_OFFSET 0x2c4
#define GPC_EMPGC0_TIGER_PDNSCR_OFFSET 0x2c8
#define GPC_EMPGC0_TIGER_EMPGSR_OFFSET 0x2cc

//GPC_EMPGC1_TIGER
#define GPC_EMPGC1_TIGER_EMPGCR_OFFSET 0x2d0
#define GPC_EMPGC1_TIGER_PUPSCR_OFFSET 0x2d4
#define GPC_EMPGC1_TIGER_PDNSCR_OFFSET 0x2d8
#define GPC_EMPGC1_TIGER_EMPGSR_OFFSET 0x2dc

//GPC_SRPGC_MEGAMIX
#define GPC_SRPGC_MEGAMIX_SRPGCR_OFFSET 0x2e0
#define GPC_SRPGC_MEGAMIX_PUPSCR_OFFSET 0x2e4
#define GPC_SRPGC_MEGAMIX_PDNSCR_OFFSET 0x2e8
#define GPC_SRPGC_MEGAMIX_SRPGSR_OFFSET 0x2ec
#define GPC_SRPGC_MEGAMIX_SRPGDR_OFFSET 0x2f0

//GPC_SRPGC_EMI
#define GPC_SRPGC_EMI_SRPGCR_OFFSET 0x300
#define GPC_SRPGC_EMI_PUPSCR_OFFSET 0x304
#define GPC_SRPGC_EMI_PDNSCR_OFFSET 0x308
#define GPC_SRPGC_EMI_SRPGSR_OFFSET 0x30c
#define GPC_SRPGC_EMI_SRPGDR_OFFSET 0x310

//#########################################
//# SRC
//#########################################
#define SRC_SCR_OFFSET	0x000
#define SRC_SBMR_OFFSET	0x004
#define SRC_SRSR_OFFSET	0x008
#define SRC_SAIAR_OFFSET	0x00c
#define SRC_SAIRAR_OFFSET	0x010
#define SRC_SISR_OFFSET	0x014
#define SRC_SIMR_OFFSET	0x018
#define SRC_SBMR2_OFFSET	0x01c
#define SRC_GPR1_OFFSET	0x020
#define SRC_GPR2_OFFSET	0x024
#define SRC_GPR3_OFFSET	0x028
#define SRC_GPR4_OFFSET	0x02c
#define SRC_GPR5_OFFSET	0x030
#define SRC_GPR6_OFFSET	0x034
#define SRC_GPR7_OFFSET	0x038
#define SRC_GPR8_OFFSET	0x03c
#define SRC_GPR9_OFFSET	0x040
#define SRC_GPR10_OFFSET	0x044
//#########################################
//# SPDIF
//#########################################
#define SPDIF_SCR_OFFSET	0x00
#define SPDIF_SRCD_OFFSET	0x04
#define SPDIF_SRPC_OFFSET	0x08
#define SPDIF_SIE_OFFSET	0x0C
#define SPDIF_SIS_OFFSET	0x10
#define SPDIF_SIC_OFFSET	0x10
#define SPDIF_SRL_OFFSET	0x14
#define SPDIF_SRR_OFFSET	0x18
#define SPDIF_SRCSH_OFFSET	0x1C
#define SPDIF_SRCSL_OFFSET	0x20
#define SPDIF_SRU_OFFSET	0x24
#define SPDIF_SRQ_OFFSET	0x28
#define SPDIF_STL_OFFSET	0x2C
#define SPDIF_STR_OFFSET	0x30
#define SPDIF_STCSH_OFFSET	0x34
#define SPDIF_STCSL_OFFSET	0x38
#define SPDIF_STUH_OFFSET	0x3C
#define SPDIF_STUL_OFFSET	0x40
#define SPDIF_SRFM_OFFSET	0x44
#define SPDIF_STC_OFFSET	0x50

//#########################################
//# ARM_DEBUG_UNIT
//#########################################
#define ARM_DEBUG_DRCR_REG (CORTEX_DEBUG_UNIT + 0x090)
#define ARM_DEBUG_LAR_REG  (CORTEX_DEBUG_UNIT + 0xFB0)
#define ARM_DEBUG_PRSR_REG (CORTEX_DEBUG_UNIT + 0x314)

//#########################################
//# ARM INTERRUPT CONTROLLER
//#########################################
#define ICDDCR          (IC_DISTRIBUTOR_BASE_ADDR + 0x000)
#define ICDICTR         (IC_DISTRIBUTOR_BASE_ADDR + 0x004)
#define ICDIIDR         (IC_DISTRIBUTOR_BASE_ADDR + 0x008)
#define ICDISR          (IC_DISTRIBUTOR_BASE_ADDR + 0x080)
#define ICDISER         (IC_DISTRIBUTOR_BASE_ADDR + 0x100)
#define ICDICER         (IC_DISTRIBUTOR_BASE_ADDR + 0x180)
#define ICDISPR         (IC_DISTRIBUTOR_BASE_ADDR + 0x200)
#define ICDICPR         (IC_DISTRIBUTOR_BASE_ADDR + 0x280)
#define ICDABR          (IC_DISTRIBUTOR_BASE_ADDR + 0x300)
#define ICDIPR          (IC_DISTRIBUTOR_BASE_ADDR + 0x400)
#define ICDIPTR         (IC_DISTRIBUTOR_BASE_ADDR + 0x800)
#define ICDICFR         (IC_DISTRIBUTOR_BASE_ADDR + 0xC00)
#define ICD_PPI_STAT    (IC_DISTRIBUTOR_BASE_ADDR + 0xD00)
#define ICD_SPI_STAT_0  (IC_DISTRIBUTOR_BASE_ADDR + 0xD04)
#define ICD_SPI_STAT_1  (IC_DISTRIBUTOR_BASE_ADDR + 0xD08)
#define ICD_SPI_STAT_2  (IC_DISTRIBUTOR_BASE_ADDR + 0xD0C)
#define ICD_SPI_STAT_3  (IC_DISTRIBUTOR_BASE_ADDR + 0xD10)
#define ICDSGIR         (IC_DISTRIBUTOR_BASE_ADDR + 0xF00)
#define ICCICR          (IC_INTERFACES_BASE_ADDR + 0x000)
#define ICCPMR          (IC_INTERFACES_BASE_ADDR + 0x004)
#define ICCBPR          (IC_INTERFACES_BASE_ADDR + 0x008)
#define ICCIAR          (IC_INTERFACES_BASE_ADDR + 0x00c)
#define ICCEOIR         (IC_INTERFACES_BASE_ADDR + 0x010)
#define ICCRPR          (IC_INTERFACES_BASE_ADDR + 0x014)
#define ICCHPIR         (IC_INTERFACES_BASE_ADDR + 0x018)
#define ICCABPR         (IC_INTERFACES_BASE_ADDR + 0x01C)
#define ICCIIDR         (IC_INTERFACES_BASE_ADDR + 0x0FC)

//#########################################
//# ENET REGISTERS
//#########################################
//base is probably 0x400c0000
#define ENET_OPD	0x00EC      //writable bits 0..15, 16..31 are readonly==0x0001

//#########################################
//# SNVS REGISTERS  后续这种控制器寄存器定义  转移到各个外设驱动文件中
//#########################################
//#define SNVS_HPLR   	0x0000  //writable bits 0..18
//#define SNVS_HPCOMR   	0x0004  //writable bits 0..31
//#define SNVS_HPCR   	0x0008  //writable bits 0..16
//#define SNVS_HPSICR   	0x000C  //writable bits 0..5
//#define SNVS_HPSVCR   	0x0010  //writable bits 0..6
//#define SNVS_HPSR   	0x0014  //writable bits 0..31
//#define SNVS_HPVSR   	0x0018  //writable bits 0..27
//#define SNVS_HPHACIVR 	0x001c  //writable bits 0..31
//#define SNVS_HPHACR   	0x0020  //no writable bits
//#define SNVS_HPRTCMR   	0x0024  //writable bits 0..14
//#define SNVS_HPRTCLR   	0x0028  //writable bits 0..31
//#define SNVS_HPTAMR   	0x002C  //writable bits 0..14
//#define SNVS_HPTALR   	0x0030  //writable bits 0..31
//#define SNVS_LPLR   	0x0034  //writable bits 0..8
//#define SNVS_LPCR   	0x0038  //writable bits 0..14
//#define SNVS_LPMKCR   	0x003c  //writable bits 0..4
//#define SNVS_LPSVCR   	0x0040  //writable bits 0..5
//#define SNVS_LPTGFCR   	0x0044  //writable bits 0..31
//#define SNVS_LPTDCR   	0x0048  //writable bits 0..28
//#define SNVS_LPSR   	0x004c  //writable bits 0..20
//#define SNVS_LPSRTCMR  	0x0050  //writable bits 0..14
//#define SNVS_LPSRTCLR  	0x0054  //writable bits 0..31
//#define SNVS_LPTAR   	0x0058  //writable bits 0..31
//#define SNVS_LPSMCMR   	0x005C  //no writable bits
//#define SNVS_LPSMCLR   	0x0060  //no writable bits
//#define SNVS_LPPGDR   	0x0064  //writable bits 0..31
//#define SNVS_LPGPR  	0x0068  //writable bits 0..31
//#define SNVS_LPZMKR0   	0x006C  //writable bits 0..31
//#define SNVS_LPZKMR1   	0x0070  //writable bits 0..31
//#define SNVS_LPZKMR2   	0x0074  //writable bits 0..31
//#define SNVS_LPZKMR3   	0x0078  //writable bits 0..31
//#define SNVS_LPZKMR4   	0x007C  //writable bits 0..31
//#define SNVS_LPZKMR5   	0x0080  //writable bits 0..31
//#define SNVS_LPZKMR6   	0x0084  //writable bits 0..31
//#define SNVS_LPZKMR7   	0x0088  //writable bits 0..31
//#define SNVS_HPVIDR1   	0x0BF8  //no writable bits
//#define SNVS_HPVIDR2   	0x0BFC  //no writable bits

//#########################################
//# MIPI_CSI REGISTERS
//#########################################
#define    MIPI_CSI2_VERSION                      0x000
#define    MIPI_CSI2_N_LANES                      0x004
#define    MIPI_CSI2_PHY_SHUTDOWNZ                0x008
#define    MIPI_CSI2_DPHY_RSTZ                    0x00c
#define    MIPI_CSI2_RESETN                       0x010
#define    MIPI_CSI2_PHY_STATE                    0x014
#define    MIPI_CSI2_DATA_IDS_1                   0x018
#define    MIPI_CSI2_DATA_IDS_2                   0x01c
#define    MIPI_CSI2_ERR1                         0x020
#define    MIPI_CSI2_ERR2                         0x024
#define    MIPI_CSI2_MSK1                         0x028
#define    MIPI_CSI2_MSK2                         0x02c
#define    MIPI_CSI2_PHY_TST_CTRL0                0x030
#define    MIPI_CSI2_PHY_TST_CTRL1                0x034
#define    MIPI_CSI2_SFT_RESET	                  0xf00

//#########################################
//# MIPI_DSI REGISTERS
//#########################################
#define    MIPI_DSI_VERSION                      0x000
#define    MIPI_DSI_PWR_UP                       0x004
#define    MIPI_DSI_CLKMGR_CFG                   0x008
#define    MIPI_DSI_DPI_CFG                      0x00c
#define    MIPI_DSI_DBI_CFG                      0x010
#define    MIPI_DSI_DBI_CMDSIZE                  0x014
#define    MIPI_DSI_PCKHDL_CFG                   0x018
#define    MIPI_DSI_VID_MODE_CFG                 0x01c
#define    MIPI_DSI_VID_PKT_CFG                  0x020
#define    MIPI_DSI_CMD_MODE_CFG                 0x024
#define    MIPI_DSI_TMR_LINE_CFG                 0x028
#define    MIPI_DSI_VTIMING_CFG                  0x02c
#define    MIPI_DSI_TMR_CFG                      0x030
#define    MIPI_DSI_GEN_HDR                      0x034
#define    MIPI_DSI_GEN_PLD_DATA                 0x038
#define    MIPI_DSI_CMD_PKT_STATUS               0x03c
#define    MIPI_DSI_TO_CNT_CFG                   0x040
#define    MIPI_DSI_ERROR_ST0                    0x044
#define    MIPI_DSI_ERROR_ST1                    0x048
#define    MIPI_DSI_ERROR_MSK0                   0x04c
#define    MIPI_DSI_ERROR_MSK1                   0x050
#define    MIPI_DSI_PHY_RSTZ                     0x054
#define    MIPI_DSI_PHY_IF_CFG                   0x058
#define    MIPI_DSI_PHY_IF_CTRL                  0x05c
#define    MIPI_DSI_PHY_STATUS                   0x060
#define    MIPI_DSI_PHY_TST_CTRL0                0x064
#define    MIPI_DSI_PHY_TST_CTRL1                0x068

//#########################################
//# MIPI_HSI REGISTERS
//#########################################

#define    MIPI_HW_HSI_CTRL                      0x000
#define    MIPI_HW_HSI_TX_CONF                   0x004
#define    MIPI_HW_HSI_RX_CONF                   0x008
#define    MIPI_HW_HSI_HSI_CAP                   0x00c
#define    MIPI_HW_HSI_TX_WML0                   0x010
#define    MIPI_HW_HSI_TX_WML1                   0x014
#define    MIPI_HW_HSI_TX_ARB_PRI0               0x018
#define    MIPI_HW_HSI_TX_ARB_PRI1               0x01c
#define    MIPI_HW_HSI_LINE_ST                   0x020
#define    MIPI_HW_HSI_ID_BIT                    0x024
#define    MIPI_HW_HSI_FIFO_THR_CONF             0x028
#define    MIPI_HW_HSI_CH_SFTRST                 0x02c
#define    MIPI_HW_HSI_IRQSTAT                   0x030
#define    MIPI_HW_HSI_IRQSTAT_EN                0x034
#define    MIPI_HW_HSI_IRQSIG_EN                 0x038
#define    MIPI_HW_HSI_FIFO_THR_IRQSTAT          0x03c
#define    MIPI_HW_HSI_FIFO_THR_IRQSTAT_EN       0x040
#define    MIPI_HW_HSI_FIFO_IRQSIG_EN            0x044
#define    MIPI_HW_HSI_TX_CH0_DP                 0x050
#define    MIPI_HW_HSI_TX_CH1_DP                 0x054
#define    MIPI_HW_HSI_TX_CH2_DP                 0x058
#define    MIPI_HW_HSI_TX_CH3_DP                 0x05c
#define    MIPI_HW_HSI_TX_CH4_DP                 0x060
#define    MIPI_HW_HSI_TX_CH5_DP                 0x064
#define    MIPI_HW_HSI_TX_CH6_DP                 0x068
#define    MIPI_HW_HSI_TX_CH7_DP                 0x06c
#define    MIPI_HW_HSI_TX_CH8_DP                 0x070
#define    MIPI_HW_HSI_TX_CH9_DP                 0x074
#define    MIPI_HW_HSI_TX_CH10_DP                0x078
#define    MIPI_HW_HSI_TX_CH11_DP                0x07c
#define    MIPI_HW_HSI_TX_CH12_DP                0x080
#define    MIPI_HW_HSI_TX_CH13_DP                0x084
#define    MIPI_HW_HSI_TX_CH14_DP                0x088
#define    MIPI_HW_HSI_TX_CH15_DP                0x08c
#define    MIPI_HW_HSI_RX_CH0_DP                 0x090
#define    MIPI_HW_HSI_RX_CH1_DP                 0x094
#define    MIPI_HW_HSI_RX_CH2_DP                 0x098
#define    MIPI_HW_HSI_RX_CH3_DP                 0x09c
#define    MIPI_HW_HSI_RX_CH4_DP                 0x0a0
#define    MIPI_HW_HSI_RX_CH5_DP                 0x0a4
#define    MIPI_HW_HSI_RX_CH6_DP                 0x0a8
#define    MIPI_HW_HSI_RX_CH7_DP                 0x0ac
#define    MIPI_HW_HSI_RX_CH8_DP                 0x0b0
#define    MIPI_HW_HSI_RX_CH9_DP                 0x0b4
#define    MIPI_HW_HSI_RX_CH10_DP                0x0b8
#define    MIPI_HW_HSI_RX_CH11_DP                0x0bc
#define    MIPI_HW_HSI_RX_CH12_DP                0x0c0
#define    MIPI_HW_HSI_RX_CH13_DP                0x0c4
#define    MIPI_HW_HSI_RX_CH14_DP                0x0c8
#define    MIPI_HW_HSI_RX_CH15_DP                0x0cc
#define    MIPI_HW_HSI_ERR_IRQSTAT               0x0d0
#define    MIPI_HW_HSI_ERR_IRQSTAT_EN            0x0d4
#define    MIPI_HW_HSI_ERR_IRQSIG_EN             0x0d8
#define    MIPI_HW_HSI_TDMA0_CONF                0x0dc
#define    MIPI_HW_HSI_TDMA1_CONF                0x0e0
#define    MIPI_HW_HSI_TDMA2_CONF                0x0e4
#define    MIPI_HW_HSI_TDMA3_CONF                0x0e8
#define    MIPI_HW_HSI_TDMA4_CONF                0x0ec
#define    MIPI_HW_HSI_TDMA5_CONF                0x0f0
#define    MIPI_HW_HSI_TDMA6_CONF                0x0f4
#define    MIPI_HW_HSI_TDMA7_CONF                0x0f8
#define    MIPI_HW_HSI_TDMA8_CONF                0x0fc
#define    MIPI_HW_HSI_TDMA9_CONF                0x100
#define    MIPI_HW_HSI_TDMA10_CONF               0x104
#define    MIPI_HW_HSI_TDMA11_CONF               0x108
#define    MIPI_HW_HSI_TDMA12_CONF               0x10c
#define    MIPI_HW_HSI_TDMA13_CONF               0x110
#define    MIPI_HW_HSI_TDMA14_CONF               0x114
#define    MIPI_HW_HSI_TDMA15_CONF               0x118
#define    MIPI_HW_HSI_RDMA0_CONF                0x11c
#define    MIPI_HW_HSI_RDMA1_CONF                0x120
#define    MIPI_HW_HSI_RDMA2_CONF                0x124
#define    MIPI_HW_HSI_RDMA3_CONF                0x128
#define    MIPI_HW_HSI_RDMA4_CONF                0x12c
#define    MIPI_HW_HSI_RDMA5_CONF                0x130
#define    MIPI_HW_HSI_RDMA6_CONF                0x134
#define    MIPI_HW_HSI_RDMA7_CONF                0x138
#define    MIPI_HW_HSI_RDMA8_CONF                0x13c
#define    MIPI_HW_HSI_RDMA9_CONF                0x140
#define    MIPI_HW_HSI_RDMA10_CONF               0x144
#define    MIPI_HW_HSI_RDMA11_CONF               0x148
#define    MIPI_HW_HSI_RDMA12_CONF               0x14c
#define    MIPI_HW_HSI_RDMA13_CONF               0x150
#define    MIPI_HW_HSI_RDMA14_CONF               0x154
#define    MIPI_HW_HSI_RDMA15_CONF               0x158
#define    MIPI_HW_HSI_TDMA0_STA_ADDR            0x15c
#define    MIPI_HW_HSI_TDMA1_STA_ADDR            0x160
#define    MIPI_HW_HSI_TDMA2_STA_ADDR            0x164
#define    MIPI_HW_HSI_TDMA3_STA_ADDR            0x168
#define    MIPI_HW_HSI_TDMA4_STA_ADDR            0x16c
#define    MIPI_HW_HSI_TDMA5_STA_ADDR            0x170
#define    MIPI_HW_HSI_TDMA6_STA_ADDR            0x174
#define    MIPI_HW_HSI_TDMA7_STA_ADDR            0x178
#define    MIPI_HW_HSI_TDMA8_STA_ADDR            0x17c
#define    MIPI_HW_HSI_TDMA9_STA_ADDR            0x180
#define    MIPI_HW_HSI_TDMA10_STA_ADDR           0x184
#define    MIPI_HW_HSI_TDMA11_STA_ADDR           0x188
#define    MIPI_HW_HSI_TDMA12_STA_ADDR           0x18c
#define    MIPI_HW_HSI_TDMA13_STA_ADDR           0x190
#define    MIPI_HW_HSI_TDMA14_STA_ADDR           0x194
#define    MIPI_HW_HSI_TDMA15_STA_ADDR           0x198
#define    MIPI_HW_HSI_RDMA0_STA_ADDR            0x19c
#define    MIPI_HW_HSI_RDMA1_STA_ADDR            0x1a0
#define    MIPI_HW_HSI_RDMA2_STA_ADDR            0x1a4
#define    MIPI_HW_HSI_RDMA3_STA_ADDR            0x1a8
#define    MIPI_HW_HSI_RDMA4_STA_ADDR            0x1ac
#define    MIPI_HW_HSI_RDMA5_STA_ADDR            0x1b0
#define    MIPI_HW_HSI_RDMA6_STA_ADDR            0x1b4
#define    MIPI_HW_HSI_RDMA7_STA_ADDR            0x1b8
#define    MIPI_HW_HSI_RDMA8_STA_ADDR            0x1bc
#define    MIPI_HW_HSI_RDMA9_STA_ADDR            0x1c0
#define    MIPI_HW_HSI_RDMA10_STA_ADDR           0x1c4
#define    MIPI_HW_HSI_RDMA11_STA_ADDR           0x1c8
#define    MIPI_HW_HSI_RDMA12_STA_ADDR           0x1cc
#define    MIPI_HW_HSI_RDMA13_STA_ADDR           0x1d0
#define    MIPI_HW_HSI_RDMA14_STA_ADDR           0x1d4
#define    MIPI_HW_HSI_RDMA15_STA_ADDR           0x1d8
#define    MIPI_HW_HSI_DMA_IRQSTAT               0x1dc
#define    MIPI_HW_HSI_DMA_IRQSTAT_EN            0x1e0
#define    MIPI_HW_HSI_DMA_IRQSIG_EN             0x1e4
#define    MIPI_HW_HSI_DMA_ERR_IRQSTAT           0x1e8
#define    MIPI_HW_HSI_DMA_ERR_IRQSTAT_EN        0x1ec
#define    MIPI_HW_HSI_DMA_ERR_IRQSIG_EN         0x1f0
#define    MIPI_HW_HSI_DMA_SINGLE_REQ_EN         0x1f4
#define    MIPI_HW_HSI_TX_FIFO_SIZE_CONF0        0x200
#define    MIPI_HW_HSI_TX_FIFO_SIZE_CONF1        0x204
#define    MIPI_HW_HSI_RX_FIFO_SIZE_CONF0        0x208
#define    MIPI_HW_HSI_RX_FIFO_SIZE_CONF1        0x20c
#define    MIPI_HW_HSI_TX_FIFO_STAT              0x210
#define    MIPI_HW_HSI_RX_FIFO_STAT              0x214
#define    MIPI_HW_HSI_DLL_CTRL                  0x220
#define    MIPI_HW_HSI_DLL_STAT                  0x224

#endif
