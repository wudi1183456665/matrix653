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
** 文   件   名: ccm_pll.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 01 月 21 日
**
** 描        述: imx6q 处理器的系统时钟设置
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "sdk.h"                                                        /*  借用第三方软件头文件        */
#include "ccm_pll/ccm_pll.h"                                            /*  驱动头文件                  */
/*********************************************************************************************************
  系统各个模块分支时钟定义
*********************************************************************************************************/
const UINT32  PLL1_OUTPUT   = 792000000;
const UINT32  PLL2_OUTPUT[] = { 528000000, 396000000, 352000000, 198000000, 594000000 };
const UINT32  PLL3_OUTPUT[] = { 480000000, 720000000, 540000000, 508235294, 454736842 };
const UINT32  PLL4_OUTPUT   = 650000000;
const UINT32  PLL5_OUTPUT   = 650000000;
/*********************************************************************************************************
** 函数名称: imx6qCcmPLLInit
** 功能描述: 系统时钟初始化设置，
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qCcmPLLInit (VOID)
{
    UINT32  uiRegVal;

	writel(BM_CCM_ANALOG_PLL_ENET_POWERDOWN, HW_CCM_ANALOG_PLL_ENET_CLR_ADDR);
	writel(BM_CCM_ANALOG_PLL_ENET_ENABLE, HW_CCM_ANALOG_PLL_ENET_SET_ADDR);
	writel(BM_CCM_ANALOG_PLL_ENET_BYPASS, HW_CCM_ANALOG_PLL_ENET_CLR_ADDR);
	uiRegVal = readl(HW_CCM_ANALOG_PLL_ENET_ADDR) & 0xFFFFFFFC;
	writel(uiRegVal | 0x3, HW_CCM_ANALOG_PLL_ENET_ADDR);

    /*
     * 将不使用的系统外设时钟禁用，需要使用的系统外设时钟使能
     */
#if 0
	writel(0xffffffff, HW_CCM_CCGR0_ADDR);
	writel(0xFFCF0FFF, HW_CCM_CCGR1_ADDR);
	writel(0xFFFFF03F, HW_CCM_CCGR2_ADDR);
	writel(0xffffffff, HW_CCM_CCGR3_ADDR);
	writel(0x00FFFF03, HW_CCM_CCGR4_ADDR);
	writel(0xF0FFFFCF, HW_CCM_CCGR5_ADDR);
	writel(0xffffffff, HW_CCM_CCGR6_ADDR);
#endif

	writel(0xFFFFC03F, HW_CCM_CCGR0_ADDR);								/* disable CG3-CG13 			*/
	writel(0xFFFCFFFF, HW_CCM_CCGR1_ADDR);								/* disable GPU-CG12 CG13 ESAI-CG8 */
	writel(0xFFFFFFFF, HW_CCM_CCGR2_ADDR);
	writel(0xFFFFFFFF, HW_CCM_CCGR3_ADDR);  							/* disable GPU 2D-CG15 			*/
	writel(0xFFFFFFFF, HW_CCM_CCGR4_ADDR);
	writel(0xFF033FFF, HW_CCM_CCGR5_ADDR);								/* disable SPDIF-CG7 SSI-CG9 CG10 CG11 */
	writel(0xFFFF33FF, HW_CCM_CCGR6_ADDR);								/* disable CG5 	VPU-CG7			*/

    /*
     * Keep default settings at reset.
     * uiPrePeriphClkSel is by default at 0, so the selected output
     * of PLL2 is the main output at 528MHz.
     * => by default, ahb_podf divides by 4 => AHB_CLK@132MHz.
     * => by default, ipg_podf divides by 2 => IPG_CLK@66MHz.
     */
    uiRegVal = 3 << 10 | 1 << 16 | 1 << 8 ;
    writel(uiRegVal, HW_CCM_CBCDR_ADDR);
    writel(0xFFFFFFFF, HW_GPC_IMR1_ADDR);
    writel(0xFFFFFFFF, HW_GPC_IMR2_ADDR);
    writel(0xFFFFFFFF, HW_GPC_IMR3_ADDR);
    writel(0xFFFFFFFF, HW_GPC_IMR4_ADDR);
}
/*********************************************************************************************************
** 函数名称: imx6qUsdhcClkSet
** 功能描述: 设置系统的 SD/SDIO 控制器时钟
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT32  imx6qUsdhcClkSet (VOID)
{
	UINT32  uiRegVal;

    /*
     * 关闭 usdhc 控制器时钟
     */
    writel(0xfffffc00, HW_CCM_CCGR6_ADDR);

    /*
     * 设置 usdhc 控制器时钟
     */
	uiRegVal = readl(HW_CCM_CSCDR1_ADDR) | 0x00FFF380;
	writel(uiRegVal , HW_CCM_CSCDR1_ADDR);

    /*
     * 打开 usdhc 控制器时钟
     */
	writel(0xffffffff , HW_CCM_CCGR6_ADDR);

    return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imx6qMainClkGet
** 功能描述: 获取系统的各个时钟频率
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT32  imx6qMainClkGet (INT iClk)
{
    UINT32 uiRetVal          = 0;
    UINT32 uiPrePeriphClkSel = HW_CCM_CBCMR.B.PRE_PERIPH_CLK_SEL;

    switch (iClk) {
    case CPU_CLK:
        uiRetVal = PLL1_OUTPUT;
        break;

    case AXI_CLK:
        uiRetVal = PLL2_OUTPUT[uiPrePeriphClkSel] / (HW_CCM_CBCDR.B.AXI_PODF + 1);
        break;

    case MMDC_CH0_AXI_CLK:
        uiRetVal = PLL2_OUTPUT[uiPrePeriphClkSel] / (HW_CCM_CBCDR.B.MMDC_CH0_AXI_PODF + 1);
        break;

    case AHB_CLK:
        uiRetVal = PLL2_OUTPUT[uiPrePeriphClkSel] / (HW_CCM_CBCDR.B.AHB_PODF + 1);
        break;

    case IPG_CLK:
        uiRetVal =
            PLL2_OUTPUT[uiPrePeriphClkSel] / (HW_CCM_CBCDR.B.AHB_PODF + 1) /
            (HW_CCM_CBCDR.B.IPG_PODF + 1);
        break;

    case IPG_PER_CLK:
        uiRetVal =
            PLL2_OUTPUT[uiPrePeriphClkSel] / (HW_CCM_CBCDR.B.AHB_PODF + 1) /
            (HW_CCM_CBCDR.B.IPG_PODF + 1)  / (HW_CCM_CSCMR1.B.PERCLK_PODF + 1);
        break;

    case MMDC_CH1_AXI_CLK:
        uiRetVal = PLL2_OUTPUT[uiPrePeriphClkSel] / (HW_CCM_CBCDR.B.MMDC_CH1_AXI_PODF + 1);
        break;

    default:
        break;
    }

    return (uiRetVal);
}
/*********************************************************************************************************
** 函数名称: imx6qPeriClkGet
** 功能描述: 获取系统外设的时钟频率
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT32  imx6qPeriClkGet (peri_clocks_t clock)
{
    UINT32 uiRetVal = 0;

    switch (clock)
    {
        case UART1_MODULE_CLK:
        case UART2_MODULE_CLK:
        case UART3_MODULE_CLK:
        case UART4_MODULE_CLK:
            /*
             * UART source clock is a fixed PLL3 / 6
             */
            uiRetVal = PLL3_OUTPUT[0] / 6 / (HW_CCM_CSCDR1.B.UART_CLK_PODF + 1);
            break;
        
            /*
             * eCSPI clock: PLL3(480) -> /8 -> CSCDR2[ECSPI_CLK_PODF]
             */
        case SPI_CLK:
            uiRetVal = PLL3_OUTPUT[0] / 8 / (HW_CCM_CSCDR2.B.ECSPI_CLK_PODF + 1);
            break;

        case RAWNAND_CLK:
            uiRetVal = PLL3_OUTPUT[0] / (HW_CCM_CS2CDR.B.ENFC_CLK_PRED + 1) /
                        (HW_CCM_CS2CDR.B.ENFC_CLK_PODF + 1);
            break;
            
        case CAN_CLK:
            /*
             *  For i.mx6dq/sdl CAN source clock is a fixed PLL3 / 8
             */
            uiRetVal = PLL3_OUTPUT[0] / 8 / (HW_CCM_CSCMR2.B.CAN_CLK_PODF + 1);
            break;

        case EIM_CLK:
            /*
             *  For i.mx6dq/sdl EIM source clock
             */
            uiRetVal = PLL3_OUTPUT[0] / 8/ (HW_CCM_CSCMR1.B.ACLK_EIM_SLOW_PODF + 1);
            break;

        default:
            break;
    }

    return (uiRetVal);
}
/*********************************************************************************************************
** 函数名称: imx6qCcgrCfg
** 功能描述: Set/unset clock gating for a peripheral.
** 输　入  : uiCcmCcgRx Address of the clock gating register: CCM_CCGR1,...
**           uiCgxOffSet Offset of the clock gating field: CG(x).
**           uiGateMode Clock gating mode: CLOCK_ON or CLOCK_OFF.
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qCcgrCfg (UINT32 uiCcmCcgRx, UINT32 uiCgxOffSet, UINT32 uiGateMode)
{
    if (uiGateMode == CLOCK_ON) {
        *(volatile UINT32 *)(uiCcmCcgRx) |= uiCgxOffSet;
    } else {
        *(volatile UINT32 *)(uiCcmCcgRx) &= ~uiCgxOffSet;
    }
}
/*********************************************************************************************************
** 函数名称: imx6qClkGateSet
** 功能描述: 打开或关闭特定外设的时钟
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qClkGateSet (addr_t atBaseAddr, UINT32 uiGateMode)
{
    UINT32  uiCcmCcgRx  = 0;
    UINT32  uiCgxOffSet = 0;

    switch (atBaseAddr) {
    case REGS_UART1_BASE:
    case REGS_UART2_BASE:
    case REGS_UART3_BASE:
    case REGS_UART4_BASE:
    case REGS_UART5_BASE:
        uiCcmCcgRx = HW_CCM_CCGR5_ADDR;
        uiCgxOffSet = CG(13) | CG(12);
        break;

    case REGS_SSI3_BASE:
        uiCcmCcgRx = HW_CCM_CCGR5_ADDR;
        uiCgxOffSet = CG(11);
        break;

    case REGS_SSI2_BASE:
        uiCcmCcgRx = HW_CCM_CCGR5_ADDR;
        uiCgxOffSet = CG(10);
        break;

    case REGS_SSI1_BASE:
        uiCcmCcgRx = HW_CCM_CCGR5_ADDR;
        uiCgxOffSet = CG(9);
        break;

    case REGS_SPDIF_BASE:
        uiCcmCcgRx = HW_CCM_CCGR5_ADDR;
        uiCgxOffSet = CG(7);
        break;

    case REGS_SPBA_BASE:
        uiCcmCcgRx = HW_CCM_CCGR5_ADDR;
        uiCgxOffSet = CG(6);
        break;

    case REGS_SDMAARM_BASE:
        uiCcmCcgRx = HW_CCM_CCGR5_ADDR;
        uiCgxOffSet = CG(3);
        break;

    case REGS_SATA_BASE:
        uiCcmCcgRx = HW_CCM_CCGR5_ADDR;
        uiCgxOffSet = CG(2);
        break;

    case REGS_EPIT1_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(6);
        break;

    case REGS_EPIT2_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(7);
        break;

    case REGS_GPT_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(10);
        break;

    case REGS_I2C1_BASE:
        uiCcmCcgRx = HW_CCM_CCGR2_ADDR;
        uiCgxOffSet = CG(3);
        break;

    case REGS_I2C2_BASE:
        uiCcmCcgRx = HW_CCM_CCGR2_ADDR;
        uiCgxOffSet = CG(4);
        break;

    case REGS_I2C3_BASE:
        uiCcmCcgRx = HW_CCM_CCGR2_ADDR;
        uiCgxOffSet = CG(5);
        break;

    case REGS_ECSPI1_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(0);
        break;

    case REGS_ECSPI2_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(1);
        break;

    case REGS_ECSPI3_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(2);
        break;

    case REGS_ECSPI4_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(3);
        break;

    case REGS_ECSPI5_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(4);
        break;

    case REGS_GPMI_BASE:
        uiCcmCcgRx = HW_CCM_CCGR4_ADDR;
        uiCgxOffSet = CG(15) | CG(14) | CG(13) | CG(12);
        break;

    case REGS_ESAI_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(8);
        break;

    case CAAM_BASE_ADDR:
        uiCcmCcgRx = HW_CCM_CCGR0_ADDR;
        uiCgxOffSet = CG(6) | CG(5) | CG(4);
        break;

    case REGS_EIM_BASE:
        uiCcmCcgRx = HW_CCM_CCGR6_ADDR;
        uiCgxOffSet = CG(5);
        break;
        /* CLKCTL_CCGR6: Set emi_slow_clock to be on in all modes */

    case REGS_GPU2D_BASE:
        uiCcmCcgRx = HW_CCM_CCGR3_ADDR;
        uiCgxOffSet = CG(15);
        break;

    case REGS_GPU3D_BASE:
        uiCcmCcgRx = HW_CCM_CCGR1_ADDR;
        uiCgxOffSet = CG(12) | CG(13);
        break;

    default:
        break;
    }

    /*
     * apply changes only if a valid address was found
     */
    if (uiCcmCcgRx != 0) {
    	imx6qCcgrCfg(uiCcmCcgRx, uiCgxOffSet, uiGateMode);
    }
}
/*********************************************************************************************************
** 函数名称: imx6qClkGateSetOnAll
** 功能描述: 打开或关闭特定外设的时钟
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qClkGateSetOnAll (VOID)
{
    imx6qClkGateSet(REGS_UART1_BASE,   CLOCK_ON);
    imx6qClkGateSet(REGS_SSI3_BASE,    CLOCK_ON);
    imx6qClkGateSet(REGS_SSI2_BASE,    CLOCK_ON);
    imx6qClkGateSet(REGS_SSI1_BASE,    CLOCK_ON);
    imx6qClkGateSet(REGS_SPDIF_BASE,   CLOCK_ON);
    imx6qClkGateSet(REGS_SPBA_BASE,    CLOCK_ON);
    imx6qClkGateSet(REGS_SDMAARM_BASE, CLOCK_ON);
    imx6qClkGateSet(REGS_SATA_BASE,    CLOCK_ON);
	imx6qClkGateSet(REGS_EPIT1_BASE,   CLOCK_ON);
	imx6qClkGateSet(REGS_EPIT2_BASE,   CLOCK_ON);
	imx6qClkGateSet(REGS_GPT_BASE,     CLOCK_ON);
	imx6qClkGateSet(REGS_I2C1_BASE,    CLOCK_ON);
	imx6qClkGateSet(REGS_I2C2_BASE,    CLOCK_ON);
	imx6qClkGateSet(REGS_I2C3_BASE,    CLOCK_ON);
	imx6qClkGateSet(REGS_ECSPI1_BASE,  CLOCK_ON);
	imx6qClkGateSet(REGS_ECSPI2_BASE,  CLOCK_ON);
	imx6qClkGateSet(REGS_ECSPI3_BASE,  CLOCK_ON);
	imx6qClkGateSet(REGS_GPMI_BASE,    CLOCK_ON);
	imx6qClkGateSet(REGS_ESAI_BASE,    CLOCK_ON);
	imx6qClkGateSet(CAAM_BASE_ADDR,    CLOCK_ON);
    imx6qClkGateSet(REGS_EIM_BASE,     CLOCK_ON);
    imx6qClkGateSet(REGS_GPU2D_BASE,   CLOCK_ON);
    imx6qClkGateSet(REGS_GPU3D_BASE,   CLOCK_ON);
}

/*********************************************************************************************************
** 函数名称: imx6qLpmWakeUpSrc
** 功能描述: Mask/unmask an interrupt source that can wake up the processor when in a low power mode.
** 输　入  : uiIrqNum ID of the interrupt to mask/unmask.
**           bEnable Pass true to unmask the source ID.
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qLpmWakeUpSrc (UINT32 uiIrqNum, BOOL bEnable)
{
    UINT32  uiRegOffset = 0;
    UINT32  uiBitOffset = 0;
    UINT32  uiGprImr    = 0;

    /*
     *  calculate the offset of the register handling that interrupt ID
     *  ID starts at 32, so for instance ID=89 is handled by IMR2 because
     *  the integer part of the division is uiRegOffset = 2
     */
    uiRegOffset = (uiIrqNum / 32);
    /*
     * and the rest of the previous division is used to calculate the bit
     * offset in the register, so for ID=89 this is uiBitOffset = 25
     */
    uiBitOffset = uiIrqNum - 32 * uiRegOffset;

    /*
     * get the current value of the corresponding uiGprImrx register
     */
    uiGprImr = readl(HW_GPC_IMR1_ADDR + (uiRegOffset - 1) * 4);

    if (bEnable) {
        /*
         * clear the corresponding bit to unmask the interrupt source
         */
        uiGprImr &= ~(1 << uiBitOffset);
        /*
         * write the new mask
         */
        writel(uiGprImr, HW_GPC_IMR1_ADDR + (uiRegOffset - 1) * 4);
    } else {
        /*
         * set the corresponding bit to mask the interrupt source
         */
        uiGprImr |= (1 << uiBitOffset);
        /*
         * write the new mask
         */
        writel(uiGprImr, HW_GPC_IMR1_ADDR + (uiRegOffset - 1) * 4);
    }
}
/*********************************************************************************************************
** 函数名称: imx6qCcmLowPower
** 功能描述: Prepare and enter in a low power mode.
** 输　入  : lp_mode low power mode : WAIT_MODE or STOP_MODE.
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qCcmLowPower (lp_modes_t lp_mode)
{
    UINT32 uiCcmClpcr = 0;

    /*
     * if MMDC channel 1 is not used, the handshake must be masked
     * set disable core clock in wait - set disable oscillator in stop
     */
    uiCcmClpcr = BM_CCM_CLPCR_BYPASS_MMDC_CH1_LPM_HS |
    		     BM_CCM_CLPCR_SBYOS                  |
    		     BM_CCM_CLPCR_ARM_CLK_DIS_ON_LPM     |
    		     lp_mode;

    if (lp_mode == STOP_MODE) {
        /*
         * enable peripherals well-biased
         */
        uiCcmClpcr |= BM_CCM_CLPCR_WB_PER_AT_LPM;
    }

    HW_CCM_CLPCR_WR(uiCcmClpcr);

    __asm("dsb;"
          "wfi;"
          "isb;");
}
/*********************************************************************************************************
** 函数名称: imx6qIpuClkCfg
** 功能描述: Configure ipu 1 and 2 hsp clk to default 264MHz
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qIpuClkCfg (VOID)
{
    /*
     * ipu_hsp_clk is derived from mmdc_ch0 divided by 2.
     * clk_sel from mmdc_ch0, podf=1
     */
    HW_CCM_CSCDR3_WR(BF_CCM_CSCDR3_IPU1_HSP_CLK_SEL(0) | BF_CCM_CSCDR3_IPU1_HSP_PODF(1) |
                     BF_CCM_CSCDR3_IPU2_HSP_CLK_SEL(0) | BF_CCM_CSCDR3_IPU2_HSP_PODF(1));
}
/*********************************************************************************************************
** 函数名称: imx6qGpuClkCfg
** 功能描述: GPU 设备时钟设置
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qGpuClkCfg (VOID)
{
    HW_CCM_ANALOG_PLL_VIDEO_NUM_WR(0xFF0D6C3);

    HW_CCM_ANALOG_PLL_VIDEO_WR(BF_CCM_ANALOG_PLL_VIDEO_DIV_SELECT(2) |
                               BF_CCM_ANALOG_PLL_VIDEO_ENABLE(1)     |
                               BF_CCM_ANALOG_PLL_VIDEO_BYPASS(1));

    while (!HW_CCM_ANALOG_PLL_VIDEO.B.LOCK) ;                           /*  waiting for PLL lock        */
    BF_CLR(CCM_ANALOG_PLL_VIDEO, BYPASS);

    HW_CCM_CS2CDR.B.LDB_DI0_CLK_SEL = 0;                                /*  ldb_di0_clk select PLL5     */

    HW_IOMUXC_GPR3.B.LVDS1_MUX_CTL = 0;                                 /*  LVDS1 src is IPU1 DI0 port  */
    HW_IOMUXC_GPR3.B.LVDS0_MUX_CTL = 2;                                 /*  LVDS0 src is IPU2 DI0 port  */

    HW_CCM_CHSCCDR.B.IPU1_DI0_CLK_SEL = 3;                              /*  drive clock from ldb_di0_clk*/

    HW_CCM_CSCMR2_SET(BM_CCM_CSCMR2_LDB_DI0_IPU_DIV |                   /*  ldb_di0 divided by 3.5      */
    		          BM_CCM_CSCMR2_LDB_DI1_IPU_DIV);

    HW_CCM_CSCDR2.B.IPU2_DI0_CLK_SEL = 3;                               /*  drive clock from ldb_di0_clk*/
    HW_CCM_CSCDR2.B.IPU2_DI1_CLK_SEL = 3;                               /*  drive clock from 352M PFD   */
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
