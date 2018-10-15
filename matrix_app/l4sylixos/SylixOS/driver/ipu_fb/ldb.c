
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include <stdio.h>
#include <string.h>
#include "sdk.h"
#include "ipu_fb/ldb_def.h"
#include "ipu_fb/ipu_common.h"
#include "regsldb.h"
#include "regsccmanalog.h"
#include "regsccm.h"
/////////////////////////////////////////////////////////////////////////////
// CODE
/////////////////////////////////////////////////////////////////////////////

//! @brief select the LDB port and data source
static void ldb_set_channel_route(int ipu_port, int lvds_port)
{
    if (ipu_port == IPU1_DI0 || ipu_port == IPU2_DI0) {
        if (lvds_port == LVDS_PORT0)
            HW_LDB_CTRL.B.CH0_MODE = 1;
        else if (lvds_port == LVDS_PORT1)
            HW_LDB_CTRL.B.CH1_MODE = 1;
        else {
            HW_LDB_CTRL.B.CH0_MODE = 1;
            HW_LDB_CTRL.B.CH1_MODE = 1;
        }
    } else if (ipu_port == IPU1_DI1 || ipu_port == IPU2_DI1) {
        if (lvds_port == LVDS_PORT0)
            HW_LDB_CTRL.B.CH0_MODE = 3;
        else if (lvds_port == LVDS_PORT1)
            HW_LDB_CTRL.B.CH1_MODE = 3;
        else {
            HW_LDB_CTRL.B.CH0_MODE = 3;
            HW_LDB_CTRL.B.CH1_MODE = 3;
        }
    } else {
        printk("Wrong IPU display port %d input!!\n", ipu_port);
        return;
    }

    if (ipu_port == IPU1_DI0 || ipu_port == IPU1_DI1) {
        HW_IOMUXC_GPR3.B.LVDS0_MUX_CTL = 0;
        HW_IOMUXC_GPR3.B.LVDS1_MUX_CTL = 0;
    } else {
        HW_IOMUXC_GPR3.B.LVDS0_MUX_CTL = 2;
        HW_IOMUXC_GPR3.B.LVDS1_MUX_CTL = 2;
    }
}

void ldb_config(int ipu_port, int lvds_port, int data_width, int bit_map)
{
    switch (lvds_port) {
    case LVDS_PORT0:
        HW_LDB_CTRL.B.BIT_MAPPING_CH0 = bit_map;
        HW_LDB_CTRL.B.DATA_WIDTH_CH0 = data_width;
//#if defined(CHIP_MX6DQ) || defined(CHIP_MX6SDL)
        HW_IOMUXC_GPR3.B.LVDS0_MUX_CTL = ipu_port;
//#endif
        break;
    case LVDS_PORT1:
        HW_LDB_CTRL.B.BIT_MAPPING_CH1 = bit_map;
        HW_LDB_CTRL.B.DATA_WIDTH_CH1 = data_width;
//#if defined(CHIP_MX6DQ) || defined(CHIP_MX6SDL)
        HW_IOMUXC_GPR3.B.LVDS1_MUX_CTL = ipu_port;
//#endif
        break;
    case LVDS_SPLIT_PORT:
        HW_LDB_CTRL.B.SPLIT_MODE_EN = 1;
        break;
    case LVDS_DUAL_PORT:
        HW_LDB_CTRL.B.BIT_MAPPING_CH0 = bit_map;
        HW_LDB_CTRL.B.DATA_WIDTH_CH0 = data_width;
        HW_LDB_CTRL.B.BIT_MAPPING_CH1 = bit_map;
        HW_LDB_CTRL.B.DATA_WIDTH_CH1 = data_width;
//#if defined(CHIP_MX6DQ) || defined(CHIP_MX6SDL)
        HW_IOMUXC_GPR3.B.LVDS0_MUX_CTL = ipu_port;
        HW_IOMUXC_GPR3.B.LVDS1_MUX_CTL = ipu_port;
//#endif
        break;
    default:
        printk("Wrong LVDS port input!!\n");
        return;
    }
    ldb_set_channel_route(ipu_port, lvds_port);
}

void ldb_set_vs_polarity(int ipu_port, int vs_pol)
{
    switch (ipu_port) {
    case IPU1_DI0:
    case IPU2_DI0:
        HW_LDB_CTRL.B.DI0_VS_POLARITY = vs_pol;
        break;
    case IPU1_DI1:
    case IPU2_DI1:
        HW_LDB_CTRL.B.DI1_VS_POLARITY = vs_pol;
        break;
    default:
        printk("Unkown display port %d!! please check.\n", ipu_port);
        break;
    }
}
/*此函数从board_display.c中拷贝过来*/
/*!
 * @brief Configure ldb clock as per the display resolution.
 *
 * ldb clock is derived from PLL5, ldb on ipu1
 */
void ldb_clock_config(int freq, int ipu_index)
{
    if (freq == 65000000)       //for XGA resolution
    {
        //config pll3 PFD1 to 455M. pll3 is 480M
        BW_CCM_ANALOG_PFD_480_PFD1_FRAC(19);

        // set ldb_di0_clk_sel to PLL3 PFD1
        HW_CCM_CS2CDR.B.LDB_DI0_CLK_SEL = 3;
        HW_CCM_CS2CDR.B.LDB_DI1_CLK_SEL = 3;

        // set clk_div to 7
        HW_CCM_CSCMR2.B.LDB_DI0_IPU_DIV = 1;
        HW_CCM_CSCMR2.B.LDB_DI1_IPU_DIV = 1;

        if (ipu_index == 1) {
            //set ipu1_di0_clk_sel from ldb_di0_clk
            HW_CCM_CHSCCDR.B.IPU1_DI0_CLK_SEL = 3;  // ldb_di0_clk
            HW_CCM_CHSCCDR.B.IPU1_DI1_CLK_SEL = 3;  // ldb_di0_clk
        } else {
            //set ipu2_di0_clk_sel from ldb_di0_clk
            HW_CCM_CSCDR2.B.IPU2_DI0_CLK_SEL = 3;
            HW_CCM_CSCDR2.B.IPU2_DI1_CLK_SEL = 3;
        }

    } else if (freq == 27000000) {                                      /*  for 800*480 resolution      */

        //config pll3 PFD1 to 455M. pll3 is 480M
        //BW_CCM_ANALOG_PFD_480_PFD1_FRAC(19);

        // set ldb_di0_clk_sel to PLL3 PFD1
        HW_CCM_CS2CDR.B.LDB_DI0_CLK_SEL = 3;
        HW_CCM_CS2CDR.B.LDB_DI1_CLK_SEL = 3;

        // set clk_div to 7
        HW_CCM_CSCMR2.B.LDB_DI0_IPU_DIV = 1;
        HW_CCM_CSCMR2.B.LDB_DI1_IPU_DIV = 1;

        if (ipu_index == 1) {
            //set ipu1_di0_clk_sel from ldb_di0_clk
            HW_CCM_CHSCCDR.B.IPU1_DI0_CLK_SEL = 3;  // ldb_di0_clk
            HW_CCM_CHSCCDR.B.IPU1_DI1_CLK_SEL = 3;  // ldb_di0_clk
        } else {
            //set ipu2_di0_clk_sel from ldb_di0_clk
            HW_CCM_CSCDR2.B.IPU2_DI0_CLK_SEL = 3;
            HW_CCM_CSCDR2.B.IPU2_DI1_CLK_SEL = 3;
        }
    }
    else {
        printk("The frequency %d for LDB is not supported yet.", freq);
    }
}
/////////////////////////////////////////////////////////////////////////////
// CODE
/////////////////////////////////////////////////////////////////////////////
