#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "sdk.h"
#include "hdmi/hdmi_tx.h"
#include "regsccm.h"
#include "regshdmi.h"

////////////////////////////////////////////////////////////////////////////////
// CODE
////////////////////////////////////////////////////////////////////////////////

void ipu1_clock_show()
{
    printk(KERN_WARNING "IPU1_DI0_PRE_CLK_SEL:  %0x \n", HW_CCM_CHSCCDR.B.IPU1_DI0_PRE_CLK_SEL);
    printk(KERN_WARNING "IPU1_DI0_CLK_SEL:      %0x \n", HW_CCM_CHSCCDR.B.IPU1_DI0_CLK_SEL);
    printk(KERN_WARNING "IPU1_DI0_PODF:         %0x \n", HW_CCM_CHSCCDR.B.IPU1_DI0_PODF);
    printk(KERN_WARNING "IPU1_DI1_PRE_CLK_SEL:  %0x \n", HW_CCM_CHSCCDR.B.IPU1_DI1_PRE_CLK_SEL);
    printk(KERN_WARNING "IPU1_DI1_CLK_SEL:      %0x \n", HW_CCM_CHSCCDR.B.IPU1_DI1_CLK_SEL);
    printk(KERN_WARNING "IPU1_DI1_PODF:         %0x \n", HW_CCM_CHSCCDR.B.IPU1_DI1_PODF);

    printk(KERN_WARNING "IPU1_DI0_CLK_SEL --> ");
    switch (HW_CCM_CHSCCDR.B.IPU1_DI0_CLK_SEL) {
    case 0:                                                             /* IPU1_DI0_PRE_CLK_SEL         */
    {
        switch (HW_CCM_CHSCCDR.B.IPU1_DI0_PRE_CLK_SEL) {
        case 0:
            printk(KERN_WARNING "IPU1_DI0_PRE_CLK --> MMDC_CH0_AXI_CLK_ROOT\n");
            break;
        case 1:
            printk(KERN_WARNING "IPU1_DI0_PRE_CLK --> pll3_sw_clk\n");
            break;
        case 2:
            printk(KERN_WARNING "IPU1_DI0_PRE_CLK --> PLL5\n");
            break;
        case 3:
            printk(KERN_WARNING "IPU1_DI0_PRE_CLK --> PLL2 PFD0\n");
            break;
        case 4:
            printk(KERN_WARNING "IPU1_DI0_PRE_CLK --> PLL2 PFD2\n");
            break;
        case 5:
            printk(KERN_WARNING "IPU1_DI0_PRE_CLK --> PLL3 PFD1\n");
            break;
        default:
            printk(KERN_WARNING "IPU1_DI0_PRE_CLK --> UNKOWN SOURCE\n");
            break;
        }
        break;
    }
    case 1:
        printk(KERN_WARNING "ipp_di0_clk\n");
        break;
    case 2:
        printk(KERN_WARNING "ipp_di1_clk\n");
        break;
    case 3:
        printk(KERN_WARNING "ldb_di0_ipu\n");
        break;
    case 4:
        printk(KERN_WARNING "ldb_di1_ipu\n");
        break;
    default :
        printk(KERN_WARNING "UNKOWN SOURCE\n");
        break;
    }

    printk(KERN_WARNING "IPU1_DI1_CLK_SEL --> ");
    switch (HW_CCM_CHSCCDR.B.IPU1_DI1_CLK_SEL) {
    case 0:                                                             /* IPU1_DI0_PRE_CLK_SEL         */
    {
        switch (HW_CCM_CHSCCDR.B.IPU1_DI1_PRE_CLK_SEL) {
        case 0:
            printk(KERN_WARNING "IPU1_DI1_PRE_CLK --> MMDC_CH0_AXI_CLK_ROOT\n");
            break;
        case 1:
            printk(KERN_WARNING "IPU1_DI1_PRE_CLK --> pll3_sw_clk\n");
            break;
        case 2:
            printk(KERN_WARNING "IPU1_DI1_PRE_CLK --> PLL5\n");
            break;
        case 3:
            printk(KERN_WARNING "IPU1_DI1_PRE_CLK --> PLL2 PFD0\n");
            break;
        case 4:
            printk(KERN_WARNING "IPU1_DI1_PRE_CLK --> PLL2 PFD2\n");
            break;
        case 5:
            printk(KERN_WARNING "IPU1_DI1_PRE_CLK --> PLL3 PFD1\n");
            break;
        default:
            printk(KERN_WARNING "IPU1_DI1_PRE_CLK --> UNKOWN SOURCE\n");
            break;
        }
        break;
    }
    case 1:
        printk(KERN_WARNING "ipp_di0_clk\n");
        break;
    case 2:
        printk(KERN_WARNING "ipp_di1_clk\n");
        break;
    case 3:
        printk(KERN_WARNING "ldb_di0_ipu\n");
        break;
    case 4:
        printk(KERN_WARNING "ldb_di1_ipu\n");
        break;
    default :
        printk(KERN_WARNING "UNKOWN SOURCE\n");
        break;
    }

}

void ipu2_clock_show()
{
    printk(KERN_WARNING "IPU2_DI0_PRE_CLK_SEL:  %0x \n", HW_CCM_CSCDR2.B.IPU2_DI0_PRE_CLK_SEL);
    printk(KERN_WARNING "IPU2_DI0_CLK_SEL:      %0x \n", HW_CCM_CSCDR2.B.IPU2_DI0_CLK_SEL);
    printk(KERN_WARNING "IPU2_DI0_PODF:         %0x \n", HW_CCM_CSCDR2.B.IPU2_DI0_PODF);
    printk(KERN_WARNING "IPU2_DI1_PRE_CLK_SEL:  %0x \n", HW_CCM_CSCDR2.B.IPU2_DI1_PRE_CLK_SEL);
    printk(KERN_WARNING "IPU2_DI1_CLK_SEL:      %0x \n", HW_CCM_CSCDR2.B.IPU2_DI1_CLK_SEL);
    printk(KERN_WARNING "IPU2_DI1_PODF:         %0x \n", HW_CCM_CSCDR2.B.IPU2_DI1_PODF);

    printk(KERN_WARNING "IPU2_DI0_CLK_SEL --> ");
    switch (HW_CCM_CSCDR2.B.IPU2_DI0_CLK_SEL) {
    case 0:                                                             /* IPU2_DI0_PRE_CLK_SEL         */
    {
        switch (HW_CCM_CSCDR2.B.IPU2_DI0_PRE_CLK_SEL) {
        case 0:
            printk(KERN_WARNING "IPU2_DI0_PRE_CLK --> MMDC_CH0_AXI_CLK_ROOT\n");
            break;
        case 1:
            printk(KERN_WARNING "IPU2_DI0_PRE_CLK --> pll3_sw_clk\n");
            break;
        case 2:
            printk(KERN_WARNING "IPU2_DI0_PRE_CLK --> PLL5\n");
            break;
        case 3:
            printk(KERN_WARNING "IPU2_DI0_PRE_CLK --> PLL2 PFD0\n");
            break;
        case 4:
            printk(KERN_WARNING "IPU2_DI0_PRE_CLK --> PLL2 PFD2\n");
            break;
        case 5:
            printk(KERN_WARNING "IPU2_DI0_PRE_CLK --> PLL3 PFD1\n");
            break;
        default:
            printk(KERN_WARNING "IPU2_DI0_PRE_CLK --> UNKOWN SOURCE\n");
            break;
        }
        break;
    }
    case 1:
        printk(KERN_WARNING "ipp_di0_clk\n");
        break;
    case 2:
        printk(KERN_WARNING "ipp_di1_clk\n");
        break;
    case 3:
        printk(KERN_WARNING "ldb_di0_ipu\n");
        break;
    case 4:
        printk(KERN_WARNING "ldb_di1_ipu\n");
        break;
    default :
        printk(KERN_WARNING "UNKOWN SOURCE\n");
        break;
    }

    printk(KERN_WARNING "IPU2_DI1_CLK_SEL --> ");
    switch (HW_CCM_CSCDR2.B.IPU2_DI1_CLK_SEL) {
    case 0:                                                             /* IPU2_DI1_PRE_CLK_SEL         */
    {
        switch (HW_CCM_CSCDR2.B.IPU2_DI1_PRE_CLK_SEL) {
        case 0:
            printk(KERN_WARNING "IPU2_DI1_PRE_CLK --> MMDC_CH0_AXI_CLK_ROOT\n");
            break;
        case 1:
            printk(KERN_WARNING "IPU2_DI1_PRE_CLK --> pll3_sw_clk\n");
            break;
        case 2:
            printk(KERN_WARNING "IPU2_DI1_PRE_CLK --> PLL5\n");
            break;
        case 3:
            printk(KERN_WARNING "IPU2_DI1_PRE_CLK --> PLL2 PFD0\n");
            break;
        case 4:
            printk(KERN_WARNING "IPU2_DI1_PRE_CLK --> PLL2 PFD2\n");
            break;
        case 5:
            printk(KERN_WARNING "IPU2_DI1_PRE_CLK --> PLL3 PFD1\n");
            break;
        default:
            printk(KERN_WARNING "IPU2_DI1_PRE_CLK --> UNKOWN SOURCE\n");
            break;
        }
        break;
    }
    case 1:
        printk(KERN_WARNING "ipp_di0_clk\n");
        break;
    case 2:
        printk(KERN_WARNING "ipp_di1_clk\n");
        break;
    case 3:
        printk(KERN_WARNING "ldb_di0_ipu\n");
        break;
    case 4:
        printk(KERN_WARNING "ldb_di1_ipu\n");
        break;
    default :
        printk(KERN_WARNING "UNKOWN SOURCE\n");
        break;
    }

}
/*********************************************************************************************************
** 函数名称: hdmi_clock_setting_show
** 功能描述: 显示HDMI时钟设置
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
void hdmi_clock_setting_show()
{
    ipu1_clock_show();
    ipu2_clock_show();
}
/*********************************************************************************************************
** 函数名称: hdmi_clock_set
** 功能描述: 设置HDMI时钟
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
void hdmi_clock_set(int ipu_index, uint32_t pclk)
{
    /*
     * Should enable these clocks
     * 1. AHB_CLK_ROOT
     *   CBCMR[PRE_PERIPH_CLK_SEL] -  CBCDR[periph_clk_sel] -  Gate   -  CBCDR[AHB_PODF]
     *  00 derive clock from PLL2
        01 derive clock from PLL2 PFD2
        10 derive clock from PLL2 PFD0
        11 derive clock from divided (/2) PLL2 PFD2
     */
    HW_CCM_CBCMR.B.PRE_PERIPH_CLK_SEL   = 0x0;                          /* PLL2 528M                    */
    /*
     *  0 PLL2 (pll2_main_clk)
        1 derive clock from periph_clk2_clk clock source.
     */
    HW_CCM_CBCDR.B.PERIPH_CLK_SEL       = 0x0;                          /* PLL2 main clk                */
    HW_CCM_CBCDR.B.AHB_PODF             = 0x3;                          /* /4  -> 132M                  */

    /*
     *  unsigned CG0 : 2; //!< [1:0] hdmi_tx_iahbclk, hdmi_tx_ihclk clock (hdmi_tx_iahbclk_enable)
        unsigned CG2 : 2; //!< [5:4] hdmi_tx_isfrclk clock (hdmi_tx_isfrclk_enable)
     */
    HW_CCM_CCGR2.B.CG0  = 0x3;
    HW_CCM_CCGR2.B.CG2  = 0x3;
}
/*!
 * @brief Mute or un-mute the audio of HDMI output
 *
 * @param en Enable or Disable
 * @return TRUE
 */
uint32_t hdmi_audio_mute(uint32_t en)
{
    HW_HDMI_FC_AUDSCONF.B.AUD_PACKET_SAMPFIT = (en == TRUE) ? 0xF : 0;
    return TRUE;
}
/*!
 * @brief Check if color space conversion is needed
 *
 * @param hdmi_instance HDMI instance information containing all the parameters
 * of input and output
 */
int isColorSpaceConversion(hdmi_data_info_s hdmi_instance)
{
    return (hdmi_instance.enc_in_format != hdmi_instance.enc_out_format) ? TRUE : FALSE;
}
/*!
 * @brief Check if color space decimition is needed
 *
 * @param hdmi_instance HDMI instance information containing all the parameters
 * of input and output
 */
int isColorSpaceDecimation(hdmi_data_info_s hdmi_instance)
{
    return (hdmi_instance.enc_in_format != hdmi_instance.enc_out_format) ? TRUE : FALSE;
}
/*!
 * @brief Check if color space interpolation is needed
 *
 * @param hdmi_instance HDMI instance information containing all the parameters
 * of input and output
 */
int isColorSpaceInterpolation(hdmi_data_info_s hdmi_instance)
{
    return ((hdmi_instance.enc_in_format == eYCC422) &&
            (hdmi_instance.enc_out_format == eRGB
             || hdmi_instance.enc_out_format == eYCC444)) ? TRUE : FALSE;
}

/*!
 * @brief Check if pixel repetition is needed
 *
 * @param hdmi_instance HDMI instance information containing all the parameters
 * of input and output
 */
int isPixelRepetition(hdmi_data_info_s hdmi_instance)
{
    return (hdmi_instance.pix_repet_factor > 0) ? TRUE : FALSE;
}
/*!
 * @brief This submodule is responsible for the video/audio data composition
 *
 * @param vmode Video mode parameters
 */
void hdmi_set_video_mode(hdmi_vmode_s * vmode)
{
    vmode->mHBorder = 0;
    vmode->mVBorder = 0;
    vmode->mPixelRepetitionInput = 0;
    vmode->mHImageSize = 16;
    vmode->mVImageSize = 9;

    switch (vmode->mCode) {
    case 1:                    //640x480p @ 59.94/60Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
        vmode->mHActive = 640;
        vmode->mVActive = 480;
        vmode->mHBlanking = 160;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 16;
        vmode->mVSyncOffset = 10;
        vmode->mHSyncPulseWidth = 96;
        vmode->mVSyncPulseWidth = 2;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE; // not(progressive_nI)
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 2517 : 2520;
        break;
    case 2:                    //720x480p @ 59.94/60Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 3:                    //720x480p @ 59.94/60Hz 16:9
        vmode->mHActive = 720;
        vmode->mVActive = 480;
        vmode->mHBlanking = 138;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 16;
        vmode->mVSyncOffset = 9;
        vmode->mHSyncPulseWidth = 62;
        vmode->mVSyncPulseWidth = 6;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 2700 : 2702;
        break;
    case 4:                    //1280x720p @ 59.94/60Hz 16:9
        vmode->mHActive = 1280;
        vmode->mVActive = 720;
        vmode->mHBlanking = 370;
        vmode->mVBlanking = 30;
        vmode->mHSyncOffset = 110;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 40;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 7417 : 7425;
        break;
    case 5:                    //1920x1080i @ 59.94/60Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 540;
        vmode->mHBlanking = 280;
        vmode->mVBlanking = 22;
        vmode->mHSyncOffset = 88;
        vmode->mVSyncOffset = 2;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 7417 : 7425;
        break;
    case 6:                    //720(1440)x480i @ 59.94/60Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 7:                    //720(1440)x480i @ 59.94/60Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 240;
        vmode->mHBlanking = 276;
        vmode->mVBlanking = 22;
        vmode->mHSyncOffset = 38;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 124;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 2700 : 2702;
        vmode->mPixelRepetitionInput = 1;
        break;
    case 8:                    //720(1440)x240p @ 59.826/60.054/59.886/60.115Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 9:                    //720(1440)x240p @ 59.826/60.054/59.886/60.115Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 240;
        vmode->mHBlanking = 276;
        vmode->mVBlanking = (vmode->mRefreshRate > 60000) ? 22 : 23;
        vmode->mHSyncOffset = 38;
        vmode->mVSyncOffset = (vmode->mRefreshRate > 60000) ? 4 : 5;
        vmode->mHSyncPulseWidth = 124;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = ((vmode->mRefreshRate == 60054) || vmode->mRefreshRate == 59826) ? 2700 : 2702;    // else 60.115/59.886 Hz
        vmode->mPixelRepetitionInput = 1;
        break;
    case 10:                   //2880x480i @ 59.94/60Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 11:                   //2880x480i @ 59.94/60Hz 16:9
        vmode->mHActive = 2880;
        vmode->mVActive = 240;
        vmode->mHBlanking = 552;
        vmode->mVBlanking = 22;
        vmode->mHSyncOffset = 76;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 248;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 5400 : 5405;
        break;
    case 12:                   //2880x240p @ 59.826/60.054/59.886/60.115Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 13:                   //2880x240p @ 59.826/60.054/59.886/60.115Hz 16:9
        vmode->mHActive = 2880;
        vmode->mVActive = 240;
        vmode->mHBlanking = 552;
        vmode->mVBlanking = (vmode->mRefreshRate > 60000) ? 22 : 23;
        vmode->mHSyncOffset = 76;
        vmode->mVSyncOffset = (vmode->mRefreshRate > 60000) ? 4 : 5;
        vmode->mHSyncPulseWidth = 248;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = ((vmode->mRefreshRate == 60054) || vmode->mRefreshRate == 59826) ? 5400 : 5405;    // else 60.115/59.886 Hz
        break;
    case 14:                   //1440x480p @ 59.94/60Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 15:                   //1440x480p @ 59.94/60Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 480;
        vmode->mHBlanking = 276;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 32;
        vmode->mVSyncOffset = 9;
        vmode->mHSyncPulseWidth = 124;
        vmode->mVSyncPulseWidth = 6;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 5400 : 5405;
        break;
    case 16:                   //1920x1080p @ 59.94/60Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 1080;
        vmode->mHBlanking = 280;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 88;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 14835 : 14850;
        break;
    case 17:                   //720x576p @ 50Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 18:                   //720x576p @ 50Hz 16:9
        vmode->mHActive = 720;
        vmode->mVActive = 576;
        vmode->mHBlanking = 144;
        vmode->mVBlanking = 49;
        vmode->mHSyncOffset = 12;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 64;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 2700;
        break;
    case 19:                   //1280x720p @ 50Hz 16:9
        vmode->mHActive = 1280;
        vmode->mVActive = 720;
        vmode->mHBlanking = 700;
        vmode->mVBlanking = 30;
        vmode->mHSyncOffset = 440;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 40;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 7425;
        break;
    case 20:                   //1920x1080i @ 50Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 540;
        vmode->mHBlanking = 720;
        vmode->mVBlanking = 22;
        vmode->mHSyncOffset = 528;
        vmode->mVSyncOffset = 2;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = 7425;
        break;
    case 21:                   //720(1440)x576i @ 50Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 22:                   //720(1440)x576i @ 50Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 288;
        vmode->mHBlanking = 288;
        vmode->mVBlanking = 24;
        vmode->mHSyncOffset = 24;
        vmode->mVSyncOffset = 2;
        vmode->mHSyncPulseWidth = 126;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = 2700;
        vmode->mPixelRepetitionInput = 1;
        break;
    case 23:                   //720(1440)x288p @ 50Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 24:                   //720(1440)x288p @ 50Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 288;
        vmode->mHBlanking = 288;
        vmode->mVBlanking =
            (vmode->mRefreshRate == 50080) ? 24 : ((vmode->mRefreshRate == 49920) ? 25 : 26);
        vmode->mHSyncOffset = 24;
        vmode->mVSyncOffset =
            (vmode->mRefreshRate == 50080) ? 2 : ((vmode->mRefreshRate == 49920) ? 3 : 4);
        vmode->mHSyncPulseWidth = 126;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 2700;
        vmode->mPixelRepetitionInput = 1;
        break;
    case 25:                   //2880x576i @ 50Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 26:                   //2880x576i @ 50Hz 16:9
        vmode->mHActive = 2880;
        vmode->mVActive = 288;
        vmode->mHBlanking = 576;
        vmode->mVBlanking = 24;
        vmode->mHSyncOffset = 48;
        vmode->mVSyncOffset = 2;
        vmode->mHSyncPulseWidth = 252;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = 5400;
        break;
    case 27:                   //2880x288p @ 50Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 28:                   //2880x288p @ 50Hz 16:9
        vmode->mHActive = 2880;
        vmode->mVActive = 288;
        vmode->mHBlanking = 576;
        vmode->mVBlanking =
            (vmode->mRefreshRate == 50080) ? 24 : ((vmode->mRefreshRate == 49920) ? 25 : 26);
        vmode->mHSyncOffset = 48;
        vmode->mVSyncOffset =
            (vmode->mRefreshRate == 50080) ? 2 : ((vmode->mRefreshRate == 49920) ? 3 : 4);
        vmode->mHSyncPulseWidth = 252;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 5400;
        break;
    case 29:                   //1440x576p @ 50Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 30:                   //1440x576p @ 50Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 576;
        vmode->mHBlanking = 288;
        vmode->mVBlanking = 49;
        vmode->mHSyncOffset = 24;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 128;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 5400;
        break;
    case 31:                   //1920x1080p @ 50Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 1080;
        vmode->mHBlanking = 720;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 528;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 14850;
        break;
    case 32:                   //1920x1080p @ 23.976/24Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 1080;
        vmode->mHBlanking = 830;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 638;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 23976) ? 7417 : 7425;
        break;
    case 33:                   //1920x1080p @ 25Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 1080;
        vmode->mHBlanking = 720;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 528;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 7425;
        break;
    case 34:                   //1920x1080p @ 29.97/30Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 1080;
        vmode->mHBlanking = 280;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 88;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 29970) ? 7417 : 7425;
        break;
    case 35:                   //2880x480p @ 60Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 36:                   //2880x480p @ 60Hz 16:9
        vmode->mHActive = 2880;
        vmode->mVActive = 480;
        vmode->mHBlanking = 552;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 64;
        vmode->mVSyncOffset = 9;
        vmode->mHSyncPulseWidth = 248;
        vmode->mVSyncPulseWidth = 6;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 59940) ? 10800 : 10810;
        break;
    case 37:                   //2880x576p @ 50Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 38:                   //2880x576p @ 50Hz 16:9
        vmode->mHActive = 2880;
        vmode->mVActive = 576;
        vmode->mHBlanking = 576;
        vmode->mVBlanking = 49;
        vmode->mHSyncOffset = 48;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 256;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 10800;
        break;
    case 39:                   //1920x1080i (1250 total) @ 50Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 540;
        vmode->mHBlanking = 384;
        vmode->mVBlanking = 85;
        vmode->mHSyncOffset = 32;
        vmode->mVSyncOffset = 23;
        vmode->mHSyncPulseWidth = 168;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = 7200;
        break;
    case 40:                   //1920x1080i @ 100Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 540;
        vmode->mHBlanking = 720;
        vmode->mVBlanking = 22;
        vmode->mHSyncOffset = 528;
        vmode->mVSyncOffset = 2;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = 14850;
        break;
    case 41:                   //1280x720p @ 100Hz 16:9
        vmode->mHActive = 1280;
        vmode->mVActive = 720;
        vmode->mHBlanking = 700;
        vmode->mVBlanking = 30;
        vmode->mHSyncOffset = 440;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 40;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 14850;
        break;
    case 42:                   //720x576p @ 100Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 43:                   //720x576p @ 100Hz 16:9
        vmode->mHActive = 720;
        vmode->mVActive = 576;
        vmode->mHBlanking = 144;
        vmode->mVBlanking = 49;
        vmode->mHSyncOffset = 12;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 64;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 5400;
        break;
    case 44:                   //720(1440)x576i @ 100Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 45:                   //720(1440)x576i @ 100Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 288;
        vmode->mHBlanking = 288;
        vmode->mVBlanking = 24;
        vmode->mHSyncOffset = 24;
        vmode->mVSyncOffset = 2;
        vmode->mHSyncPulseWidth = 126;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = 5400;
        vmode->mPixelRepetitionInput = 1;
        break;
    case 46:                   //1920x1080i @ 119.88/120Hz 16:9
        vmode->mHActive = 1920;
        vmode->mVActive = 540;
        vmode->mHBlanking = 288;
        vmode->mVBlanking = 22;
        vmode->mHSyncOffset = 88;
        vmode->mVSyncOffset = 2;
        vmode->mHSyncPulseWidth = 44;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = (vmode->mRefreshRate == 119880) ? 14835 : 14850;
        break;
    case 47:                   //1280x720p @ 119.88/120Hz 16:9
        vmode->mHActive = 1280;
        vmode->mVActive = 720;
        vmode->mHBlanking = 370;
        vmode->mVBlanking = 30;
        vmode->mHSyncOffset = 110;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 40;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = TRUE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 119880) ? 14835 : 14850;
        break;
    case 48:                   //720x480p @ 119.88/120Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 49:                   //720x480p @ 119.88/120Hz 16:9
        vmode->mHActive = 720;
        vmode->mVActive = 480;
        vmode->mHBlanking = 138;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 16;
        vmode->mVSyncOffset = 9;
        vmode->mHSyncPulseWidth = 62;
        vmode->mVSyncPulseWidth = 6;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 119880) ? 5400 : 5405;
        break;
    case 50:                   //720(1440)x480i @ 119.88/120Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 51:                   //720(1440)x480i @ 119.88/120Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 240;
        vmode->mHBlanking = 276;
        vmode->mVBlanking = 22;
        vmode->mHSyncOffset = 38;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 124;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = (vmode->mRefreshRate == 119880) ? 5400 : 5405;
        vmode->mPixelRepetitionInput = 1;
        break;
    case 52:                   //720X576p @ 200Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 53:                   //720X576p @ 200Hz 16:9
        vmode->mHActive = 720;
        vmode->mVActive = 576;
        vmode->mHBlanking = 144;
        vmode->mVBlanking = 49;
        vmode->mHSyncOffset = 12;
        vmode->mVSyncOffset = 5;
        vmode->mHSyncPulseWidth = 64;
        vmode->mVSyncPulseWidth = 5;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = 10800;
        break;
    case 54:                   //720(1440)x576i @ 200Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 55:                   //720(1440)x576i @ 200Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 288;
        vmode->mHBlanking = 288;
        vmode->mVBlanking = 24;
        vmode->mHSyncOffset = 24;
        vmode->mVSyncOffset = 2;
        vmode->mHSyncPulseWidth = 126;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = 10800;
        vmode->mPixelRepetitionInput = 1;
        break;
    case 56:                   //720x480p @ 239.76/240Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 57:                   //720x480p @ 239.76/240Hz 16:9
        vmode->mHActive = 720;
        vmode->mVActive = 480;
        vmode->mHBlanking = 138;
        vmode->mVBlanking = 45;
        vmode->mHSyncOffset = 16;
        vmode->mVSyncOffset = 9;
        vmode->mHSyncPulseWidth = 62;
        vmode->mVSyncPulseWidth = 6;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = FALSE;
        vmode->mPixelClock = (vmode->mRefreshRate == 239760) ? 10800 : 10810;
        break;
    case 58:                   //720(1440)x480i @ 239.76/240Hz 4:3
        vmode->mHImageSize = 4;
        vmode->mVImageSize = 3;
    case 59:                   //720(1440)x480i @ 239.76/240Hz 16:9
        vmode->mHActive = 1440;
        vmode->mVActive = 240;
        vmode->mHBlanking = 276;
        vmode->mVBlanking = 22;
        vmode->mHSyncOffset = 38;
        vmode->mVSyncOffset = 4;
        vmode->mHSyncPulseWidth = 124;
        vmode->mVSyncPulseWidth = 3;
        vmode->mHSyncPolarity = vmode->mVSyncPolarity = FALSE;
        vmode->mInterlaced = TRUE;
        vmode->mPixelClock = (vmode->mRefreshRate == 239760) ? 10800 : 10810;
        vmode->mPixelRepetitionInput = 1;
        break;
    default:
        vmode->mCode = -1;
        printk("invalid code\n");
        return;
    }
    return;
}
/*!
 * @brief program the input source mux for the hdmi input
 * this is set up in IOMUXC_GPR3 register
 *
 * @param mux_value HDMI input source slection between the mux options
 */
void hdmi_config_input_source(uint32_t mux_value)
{
    HW_IOMUXC_GPR3.B.HDMI_MUX_CTL = mux_value;
}
/*!
 * @brief This is responsible for the video data synchronization.
 * for example, for RGB 4:4:4 input, the data map is defined as
 * 			pin{47~40} <==> R[7:0]
 * 			pin{31~24} <==> G[7:0]
 * 			pin{15~8}  <==> B[7:0]
 *
 * @param hdmi_instance Instance of the HDMI
 */
void hdmi_video_sample(hdmi_data_info_s hdmi_instance)
{
    int color_format = 0;
    if (hdmi_instance.enc_in_format == eRGB) {
        if (hdmi_instance.enc_color_depth == 8) {
            color_format = 0x01;
        } else if (hdmi_instance.enc_color_depth == 10) {
            color_format = 0x03;
        } else if (hdmi_instance.enc_color_depth == 12) {
            color_format = 0x05;
        } else if (hdmi_instance.enc_color_depth == 16) {
            color_format = 0x07;
        } else {
            printk("Invalid color depth %d!!", hdmi_instance.enc_color_depth);
            return;
        }
    } else if (hdmi_instance.enc_in_format == eYCC444) {
        if (hdmi_instance.enc_color_depth == 8) {
            color_format = 0x09;
        } else if (hdmi_instance.enc_color_depth == 10) {
            color_format = 0x0B;
        } else if (hdmi_instance.enc_color_depth == 12) {
            color_format = 0x0D;
        } else if (hdmi_instance.enc_color_depth == 16) {
            color_format = 0x0F;
        } else {
            printk("Invalid color depth %d!!", hdmi_instance.enc_color_depth);
            return;
        }
    } else if (hdmi_instance.enc_in_format == eYCC422) {
        if (hdmi_instance.enc_color_depth == 8) {
            color_format = 0x16;
        } else if (hdmi_instance.enc_color_depth == 10) {
            color_format = 0x14;
        } else if (hdmi_instance.enc_color_depth == 12) {
            color_format = 0x12;
        } else {
            printk("Invalid color depth %d!!", hdmi_instance.enc_color_depth);
            return;
        }
    }

    HW_HDMI_TX_INVID0.B.VIDEO_MAPPING = color_format;
    HW_HDMI_TX_INVID0.B.INTERNAL_DE_GENERATOR = 0;

    /*enable tx stuffing, when DE is inactive, fix the output data to 0 */
    HW_HDMI_TX_INSTUFFING.U = 0x7;
    HW_HDMI_TX_GYDATA0.U = 0x0;
    HW_HDMI_TX_GYDATA1.U = 0x0;
    HW_HDMI_TX_RCRDATA0.U = 0x0;
    HW_HDMI_TX_RCRDATA1.U = 0x0;
    HW_HDMI_TX_BCBDATA0.U = 0x0;
    HW_HDMI_TX_BCBDATA1.U = 0x0;
}
/*!
 * @brief Update the color space conversion coefficients.
 *
 * @param hdmi_instance Instance of the HDMI
 */
void update_csc_coeffs(hdmi_data_info_s hdmi_instance)
{
    uint16_t csc_coeff[3][4];
    uint32_t csc_scale = 1;

    /* default initialization (mainly to workaround
     * uninitialized warning during build) */
    csc_coeff[0][0] = 0x2000;
    csc_coeff[0][1] = 0x0000;
    csc_coeff[0][2] = 0x0000;
    csc_coeff[0][3] = 0x0000;

    csc_coeff[1][0] = 0x0000;
    csc_coeff[1][1] = 0x2000;
    csc_coeff[1][2] = 0x0000;
    csc_coeff[1][3] = 0x0000;

    csc_coeff[2][0] = 0x0000;
    csc_coeff[2][1] = 0x0000;
    csc_coeff[2][2] = 0x2000;
    csc_coeff[2][3] = 0x0000;

    csc_scale = 1;

    if (isColorSpaceConversion(hdmi_instance) == TRUE)  // csc needed
    {
        if (hdmi_instance.enc_out_format == eRGB) {
            if (hdmi_instance.colorimetry == eITU601) {
                csc_coeff[0][0] = 0x2000;
                csc_coeff[0][1] = 0x6926;
                csc_coeff[0][2] = 0x74fd;
                csc_coeff[0][3] = 0x010e;

                csc_coeff[1][0] = 0x2000;
                csc_coeff[1][1] = 0x2cdd;
                csc_coeff[1][2] = 0x0000;
                csc_coeff[1][3] = 0x7e9a;

                csc_coeff[2][0] = 0x2000;
                csc_coeff[2][1] = 0x0000;
                csc_coeff[2][2] = 0x38b4;
                csc_coeff[2][3] = 0x7e3b;

                csc_scale = 1;
            } else if (hdmi_instance.colorimetry == eITU709) {
                csc_coeff[0][0] = 0x2000;
                csc_coeff[0][1] = 0x7106;
                csc_coeff[0][2] = 0x7a02;
                csc_coeff[0][3] = 0x00a7;

                csc_coeff[1][0] = 0x2000;
                csc_coeff[1][1] = 0x3264;
                csc_coeff[1][2] = 0x0000;
                csc_coeff[1][3] = 0x7e6d;

                csc_coeff[2][0] = 0x2000;
                csc_coeff[2][1] = 0x0000;
                csc_coeff[2][2] = 0x3b61;
                csc_coeff[2][3] = 0x7e25;

                csc_scale = 1;
            }
        } else if (hdmi_instance.enc_in_format == eRGB) {
            if (hdmi_instance.colorimetry == eITU601) {
                csc_coeff[0][0] = 0x2591;
                csc_coeff[0][1] = 0x1322;
                csc_coeff[0][2] = 0x074b;
                csc_coeff[0][3] = 0x0000;

                csc_coeff[1][0] = 0x6535;
                csc_coeff[1][1] = 0x2000;
                csc_coeff[1][2] = 0x7acc;
                csc_coeff[1][3] = 0x0200;

                csc_coeff[1][0] = 0x6acd;
                csc_coeff[1][1] = 0x7534;
                csc_coeff[1][2] = 0x2000;
                csc_coeff[1][3] = 0x0200;

                csc_scale = 1;
            } else if (hdmi_instance.colorimetry == eITU709) {
                csc_coeff[0][0] = 0x2dc5;
                csc_coeff[0][1] = 0x0d9b;
                csc_coeff[0][2] = 0x049e;
                csc_coeff[0][3] = 0x0000;

                csc_coeff[1][0] = 0x63f0;
                csc_coeff[1][1] = 0x2000;
                csc_coeff[1][2] = 0x7d11;
                csc_coeff[1][3] = 0x0200;

                csc_coeff[2][0] = 0x6756;
                csc_coeff[2][1] = 0x78ab;
                csc_coeff[2][2] = 0x2000;
                csc_coeff[2][3] = 0x0200;

                csc_scale = 1;
            }
        }
    }

    /*update csc parameters in hdmi csc registers */
    HW_HDMI_CSC_COEF_A1_LSB.U = (uint8_t) (csc_coeff[0][0] & 0xFF);
    HW_HDMI_CSC_COEF_A1_MSB.U = (uint8_t) (csc_coeff[0][0] >> 8);
    HW_HDMI_CSC_COEF_A2_LSB.U = (uint8_t) (csc_coeff[0][1] & 0xFF);
    HW_HDMI_CSC_COEF_A2_MSB.U = (uint8_t) (csc_coeff[0][1] >> 8);
    HW_HDMI_CSC_COEF_A3_LSB.U = (uint8_t) (csc_coeff[0][2] & 0xFF);
    HW_HDMI_CSC_COEF_A3_MSB.U = (uint8_t) (csc_coeff[0][2] >> 8);
    HW_HDMI_CSC_COEF_A4_LSB.U = (uint8_t) (csc_coeff[0][3] & 0xFF);
    HW_HDMI_CSC_COEF_A4_MSB.U = (uint8_t) (csc_coeff[0][3] >> 8);

    HW_HDMI_CSC_COEF_B1_LSB.U = (uint8_t) (csc_coeff[1][0] & 0xFF);
    HW_HDMI_CSC_COEF_B1_MSB.U = (uint8_t) (csc_coeff[1][0] >> 8);
    HW_HDMI_CSC_COEF_B2_LSB.U = (uint8_t) (csc_coeff[1][1] & 0xFF);
    HW_HDMI_CSC_COEF_B2_MSB.U = (uint8_t) (csc_coeff[1][1] >> 8);
    HW_HDMI_CSC_COEF_B3_LSB.U = (uint8_t) (csc_coeff[1][2] & 0xFF);
    HW_HDMI_CSC_COEF_B3_MSB.U = (uint8_t) (csc_coeff[1][2] >> 8);
    HW_HDMI_CSC_COEF_B4_LSB.U = (uint8_t) (csc_coeff[1][3] & 0xFF);
    HW_HDMI_CSC_COEF_B4_MSB.U = (uint8_t) (csc_coeff[1][3] >> 8);

    HW_HDMI_CSC_COEF_C1_LSB.U = (uint8_t) (csc_coeff[2][0] & 0xFF);
    HW_HDMI_CSC_COEF_C1_MSB.U = (uint8_t) (csc_coeff[2][0] >> 8);
    HW_HDMI_CSC_COEF_C2_LSB.U = (uint8_t) (csc_coeff[2][1] & 0xFF);
    HW_HDMI_CSC_COEF_C2_MSB.U = (uint8_t) (csc_coeff[2][1] >> 8);
    HW_HDMI_CSC_COEF_C3_LSB.U = (uint8_t) (csc_coeff[2][2] & 0xFF);
    HW_HDMI_CSC_COEF_C3_MSB.U = (uint8_t) (csc_coeff[2][2] >> 8);
    HW_HDMI_CSC_COEF_C4_LSB.U = (uint8_t) (csc_coeff[2][3] & 0xFF);
    HW_HDMI_CSC_COEF_C4_MSB.U = (uint8_t) (csc_coeff[2][3] >> 8);

    HW_HDMI_CSC_SCALE.B.CSC_SCALE = csc_scale;
}
/*!
 * @brief Set HDMI color space conversion module.
 *
 * @param hdmi_instance Instance of the HDMI
 */
void hdmi_video_csc(hdmi_data_info_s hdmi_instance)
{
    int color_depth = 0;
    int interpolation = 0;
    int decimation = 0;

    /*YCC422 interpolation to 444 mode */
    if (isColorSpaceInterpolation(hdmi_instance) == TRUE) {
        interpolation = 0x01;
    } else if (isColorSpaceDecimation(hdmi_instance) == TRUE) {
        decimation = 0x01;
    }

    if (hdmi_instance.enc_color_depth == 8) {
        color_depth = 4;
    } else if (hdmi_instance.enc_color_depth == 10) {
        color_depth = 5;
    } else if (hdmi_instance.enc_color_depth == 12) {
        color_depth = 6;
    } else if (hdmi_instance.enc_color_depth == 16) {
        color_depth = 7;
    } else {
        printk("Invalid color depth %d!!", hdmi_instance.enc_color_depth);
        return;
    }

    /*configure the CSC registers */
    HW_HDMI_CSC_CFG.B.DECMODE = decimation;
    HW_HDMI_CSC_CFG.B.INTMODE = interpolation;
    HW_HDMI_CSC_SCALE.B.CSC_COLORDEPTH = color_depth;
    update_csc_coeffs(hdmi_instance);
}
/*!
 * @brief HDMI video packetizer is used to packetize the data.
 * for example, if input is YCC422 mode or repeater is used, data should be repacked
 * this module can be bypassed.
 *
 * @param hdmi_instance Instance of the HDMI
 */
void hdmi_video_packetize(hdmi_data_info_s hdmi_instance)
{
    unsigned int color_depth = 0;
    unsigned int remap_size = 0;
    unsigned int output_select = 0;

    if (hdmi_instance.enc_out_format == eRGB || hdmi_instance.enc_out_format == eYCC444) {
        if (hdmi_instance.enc_color_depth == 0) {
            output_select = 3;
        } else if (hdmi_instance.enc_color_depth == 8) {
            color_depth = 4;
            output_select = 3;
        } else if (hdmi_instance.enc_color_depth == 10) {
            color_depth = 5;
        } else if (hdmi_instance.enc_color_depth == 12) {
            color_depth = 6;
        } else if (hdmi_instance.enc_color_depth == 16) {
            color_depth = 7;
        } else {
            printk("Invalid color depth %d!!\n", hdmi_instance.enc_color_depth);
            return;
        }
    } else if (hdmi_instance.enc_out_format == eYCC422) {
        if (hdmi_instance.enc_color_depth == 0 || hdmi_instance.enc_color_depth == 8) {
            remap_size = 0;
        } else if (hdmi_instance.enc_color_depth == 10) {
            remap_size = 1;
        } else if (hdmi_instance.enc_color_depth == 12) {
            remap_size = 2;
        } else {
            printk("Invalid color remap size %d!!\n", hdmi_instance.enc_color_depth);
            return;
        }
        output_select = 1;
    } else {
        printk("Invalid output encoding type %d!!\n", hdmi_instance.enc_out_format);
        return;
    }

    /*set the packetizer registers. */
    HW_HDMI_VP_PR_CD.B.COLOR_DEPTH = color_depth;
    HW_HDMI_VP_PR_CD.B.DESIRED_PR_FACTOR = hdmi_instance.pix_repet_factor;

    HW_HDMI_VP_STUFF.B.PR_STUFFING = 0x1;
    if (hdmi_instance.pix_repet_factor > 1) // data from repeter block
    {
        HW_HDMI_VP_CONF.B.PR_EN = 1;
        HW_HDMI_VP_CONF.B.BYPASS_SELECT = 0;
    } else {                    //data from packetizer block
        HW_HDMI_VP_CONF.B.PR_EN = 0;
        HW_HDMI_VP_CONF.B.BYPASS_SELECT = 1;
    }
    HW_HDMI_VP_STUFF.B.IDEFAULT_PHASE = 0x1;
    HW_HDMI_VP_REMAP.B.YCC422_SIZE = remap_size;

    if (output_select == 0) {
        HW_HDMI_VP_CONF.B.BYPASS_EN = 0;
        HW_HDMI_VP_CONF.B.PP_EN = 1;
        HW_HDMI_VP_CONF.B.YCC422_EN = 0;
    } else if (output_select == 1) {
        HW_HDMI_VP_CONF.B.BYPASS_EN = 0;
        HW_HDMI_VP_CONF.B.PP_EN = 0;
        HW_HDMI_VP_CONF.B.YCC422_EN = 1;
    } else if (output_select == 2 || output_select == 3) {
        HW_HDMI_VP_CONF.B.BYPASS_EN = 1;
        HW_HDMI_VP_CONF.B.PP_EN = 0;
        HW_HDMI_VP_CONF.B.YCC422_EN = 0;
    } else {
        printk("Invalid output option %d\n", output_select);
        return;
    }

    HW_HDMI_VP_STUFF.B.YCC422_STUFFING = 0x1;
    HW_HDMI_VP_STUFF.B.PP_STUFFING = 0x1;
    HW_HDMI_VP_CONF.B.OUTPUT_SELECTOR = output_select;
}
/*!
 * @brief Preamble filter setting. this is used to indicate whether the
 * upcoming data period is a VDP(video data period) or DI(data island)
 *
 * @param	value Data value for configuration
 * @param	channel Channel number, 0~2
 */
void preamble_filter_set(uint8_t value, uint8_t channel)
{
    if (channel == 0) {
        HW_HDMI_FC_CH0PREAM.B.CH0_PREAMBLE_FILTER = value;
    } else if (channel == 1) {
        HW_HDMI_FC_CH1PREAM.B.CH1_PREAMBLE_FILTER = value;
    } else if (channel == 2) {
        HW_HDMI_FC_CH2PREAM.B.CH2_PREAMBLE_FILTER = value;
    } else {
        printk("Invalid channel number %d\n", channel);
    }
    return;
}
/*!
 * @brief This function is responsible for the video/audio data composition.
 * video mode is set here, but the actual flow is determined by the input.
 *
 * @param hdmi_instance Instance of the HDMI
 */
void hdmi_av_frame_composer(hdmi_data_info_s * hdmi_instance)
{
    uint8_t i = 0;

    hdmi_set_video_mode(hdmi_instance->video_mode);

    // configure HDMI_FC_INVIDCONF register
    HW_HDMI_FC_INVIDCONF.B.HDCP_KEEPOUT = (hdmi_instance->hdcp_enable == TRUE) ? 1 : 0;
    HW_HDMI_FC_INVIDCONF.B.VSYNC_IN_POLARITY =
        (hdmi_instance->video_mode->mVSyncPolarity == TRUE) ? 1 : 0;
    HW_HDMI_FC_INVIDCONF.B.HSYNC_IN_POLARITY =
        (hdmi_instance->video_mode->mHSyncPolarity == TRUE) ? 1 : 0;
    HW_HDMI_FC_INVIDCONF.B.DE_IN_POLARITY =
        (hdmi_instance->video_mode->mDataEnablePolarity == TRUE) ? 1 : 0;
    HW_HDMI_FC_INVIDCONF.B.DVI_MODEZ = (hdmi_instance->video_mode->mHdmiDviSel == TRUE) ? 1 : 0;
    if (hdmi_instance->video_mode->mCode == 39) {
        HW_HDMI_FC_INVIDCONF.B.R_V_BLANK_IN_OSC = 0;
    } else {
        HW_HDMI_FC_INVIDCONF.B.R_V_BLANK_IN_OSC =
            (hdmi_instance->video_mode->mInterlaced == TRUE) ? 1 : 0;
    }
    HW_HDMI_FC_INVIDCONF.B.IN_I_P = (hdmi_instance->video_mode->mInterlaced == TRUE) ? 1 : 0;

    HW_HDMI_FC_INHACTIV0.U = hdmi_instance->video_mode->mHActive;
    HW_HDMI_FC_INHACTIV1.B.H_IN_ACTIV = (hdmi_instance->video_mode->mHActive >> 8);
    HW_HDMI_FC_INHBLANK0.U = hdmi_instance->video_mode->mHBlanking;
    HW_HDMI_FC_INHBLANK1.B.H_IN_BLANK = (hdmi_instance->video_mode->mHBlanking >> 8);
    HW_HDMI_FC_INVACTIV0.U = hdmi_instance->video_mode->mVActive;
    HW_HDMI_FC_INVACTIV1.B.V_IN_ACTIV = (hdmi_instance->video_mode->mVActive >> 8);
    HW_HDMI_FC_INVBLANK.U = (uint8_t) hdmi_instance->video_mode->mVBlanking;
    HW_HDMI_FC_HSYNCINDELAY0.U = (uint8_t) hdmi_instance->video_mode->mHSyncOffset;
    HW_HDMI_FC_HSYNCINDELAY1.B.H_IN_DELAY =
        (uint8_t) (hdmi_instance->video_mode->mHSyncOffset >> 8);
    HW_HDMI_FC_HSYNCINWIDTH0.U = hdmi_instance->video_mode->mHSyncPulseWidth;
    HW_HDMI_FC_HSYNCINWIDTH1.B.H_IN_WIDTH = (hdmi_instance->video_mode->mHSyncPulseWidth >> 8);
    HW_HDMI_FC_VSYNCINDELAY.U = hdmi_instance->video_mode->mVSyncOffset;
    HW_HDMI_FC_VSYNCINWIDTH.B.V_IN_WIDTH = hdmi_instance->video_mode->mVSyncPulseWidth;

    HW_HDMI_FC_CTRLDUR.U = 12;  //control period minimum duration
    HW_HDMI_FC_EXCTRLDUR.U = 32;
    HW_HDMI_FC_EXCTRLSPAC.U = 1;

    for (i = 0; i < 3; i++) {
        preamble_filter_set((i + 1) * 11, i);
    }

    /*pixel repetition setting. */
    HW_HDMI_FC_PRCONF.B.INCOMING_PR_FACTOR = hdmi_instance->video_mode->mPixelRepetitionInput + 1;
}
/*!
 * @brief HDCP configuration, disabled here
 *
 * @param de Data enable polarity
 */
void hdmi_tx_hdcp_config(uint32_t de)
{
    HW_HDMI_A_HDCPCFG0.B.RXDETECT = 0;
    HW_HDMI_A_VIDPOLCFG.B.DATAENPOL = (de == TRUE) ? 1 : 0;
    HW_HDMI_A_HDCPCFG1.B.ENCRYPTIONDISABLE = 1;
}
/*!
 * @brief Force the HDMI video output to be fixed value! from FC_DBGTMDSx registers
 *
 * @param force True/false or enable/disable, if true enabled force video setting, if false disable it
 */
void hdmi_config_force_video(uint8_t force)
{
    if (force) {
        HW_HDMI_FC_DBGFORCE.B.FORCEVIDEO = 0x1;
    } else {
        HW_HDMI_FC_DBGFORCE.B.FORCEVIDEO = 0x0;
    }
}
/*!
 * configure the RGB pixel settings to be used when fixed video mode is enabled
 *
 * @param red Pixel settings for red component.
 * @param green Pixel settings for green component.
 * @param blue Pixel settings for blue component.
 */
void hdmi_config_forced_pixels(uint8_t red, uint8_t green, uint8_t blue)
{
    HW_HDMI_FC_DBGTMDS2.U = red;
    HW_HDMI_FC_DBGTMDS1.U = green;
    HW_HDMI_FC_DBGTMDS0.U = blue;
}
void hdmi_phy_int_mask(int mask)
{
    HW_HDMI_PHY_MASK0.U = mask;
}

/*!
 * @brief Set the DE polarity
 *
 * @param	pol High or low for DE active
 */
static void hdmi_phy_de_pol(int pol)
{
    HW_HDMI_PHY_CONF0.B.SELDATAENPOL = pol;
}

/*!
 * @brief Select the interface control
 *
 * @param seldipif Interface selection
 */
static void hdmi_phy_if_sel(int seldipif)
{
    HW_HDMI_PHY_CONF0.B.SELDIPIF = seldipif;
}

/*!
 * @brief Enable TMDS output
 *
 * @param	en Enable or disable the TMDS output
 */
static void hdmi_phy_tmds(int en)
{
    HW_HDMI_PHY_CONF0.B.ENTMDS = en;
}

/*!
 * @brief PHY power down enable
 *
 * @param en Enable or disable PHY
 * 1 - power down disable
 * 0 - power down enable
 */
static void hdmi_phy_pdown(int en)
{
    HW_HDMI_PHY_CONF0.B.PDZ = en;
}

/*!
 * @brief PHY reset
 *
 * @param 	bit: reset PHY
 * 1 - reset PHY
 * 0 - turn PHY to normal
 */
static void hdmi_phy_reset(uint8_t bit)
{
    HW_HDMI_MC_PHYRSTZ.B.PHYRSTZ = bit ? 0 : 1;
}

/*!
 * @brief HDMI TEST mode clear
 *
 * @param bit Enable or disable
 */
static inline void hdmi_phy_test_clear(uint8_t bit)
{
    HW_HDMI_PHY_TST0.B.TESTCLR = bit;
}

/*!
 * @brief HDMI TEST mode enable
 *
 * @param bit Enable or disable
 */
static inline void hdmi_phy_test_enable(uint8_t bit)
{
    HW_HDMI_PHY_TST0.B.TESTEN = bit;
}

/*!
 * @brief HDMI PHY TEST clock enable
 *
 * @param bit Enable or disable
 */
static inline void hdmi_phy_test_clock(uint8_t bit)
{
    HW_HDMI_PHY_TST0.B.TESTCLK = bit;
}

/*!
 * @brief HDMI PHY TEST data input
 *
 * @param bit Data value
 */
static inline void hdmi_phy_test_din(uint8_t bit)
{
    HW_HDMI_PHY_TST1.U = bit;
}

/*!
 * @brief HDMI PHY TEST data output
 *
 * @param bit Data value
 */
static inline void hdmi_phy_test_dout(uint8_t bit)
{
    HW_HDMI_PHY_TST2.U = bit;
}

/*!
 * @brief HDMI PHY TEST mode control
 *
 * @param value Test Data input value
 */
static int hdmi_phy_test_control(uint8_t value)
{
    hdmi_phy_test_din(value);
    hdmi_phy_test_enable(1);
    hdmi_phy_test_clock(1);
    hdmi_phy_test_clock(0);
    hdmi_phy_test_enable(0);

    return TRUE;
}

/*!
 * @brief HDMI PHY TEST data set
 *
 * @param value Test Data input value
 */
static int hdmi_phy_test_data(uint8_t value)
{
    hdmi_phy_test_din(value);
    hdmi_phy_test_enable(0);
    hdmi_phy_test_clock(1);
    hdmi_phy_test_clock(0);

    return TRUE;
}

/*!
 * @brief HDMI PHY tx power down
 *
 * @param bit Enable or disable
 */
static void hdmi_phy_gen2_tx_pon(uint8_t bit)
{
    HW_HDMI_PHY_CONF0.B.GEN2_TXPWRON = bit;
}

/*!
 * @brief HDMI PHY tx PDDQ down
 *
 * @param bit Enable or disable
 */
static void hdmi_phy_gen2_pddq(uint8_t bit)
{
    HW_HDMI_PHY_CONF0.B.GEN2_PDDQ = bit;
}

/*!
 * @brief HDMI PHY reset
 *
 * @param bit Enable or disable
 */
static void hdmi_heacphy_reset(uint8_t bit)
{
    HW_HDMI_MC_HEACPHY_RST.B.HEACPHYRST = bit;
}

/*!
 * @brief Polling the i2c operation status bit to check if the i2c r/w
 * is done before the time run out.
 *
 * @param msec Time out value, using epit as the timer
 */
static int hdmi_phy_wait_i2c_done(int msec)
{
    unsigned char val = 0;
    val |= (HW_HDMI_IH_I2CMPHY_STAT0.B.I2CMPHYERROR << BP_HDMI_IH_I2CMPHY_STAT0_I2CMPHYERROR);
    val |= (HW_HDMI_IH_I2CMPHY_STAT0.B.I2CMPHYDONE << BP_HDMI_IH_I2CMPHY_STAT0_I2CMPHYDONE);
    while (val == 0) {
    	udelay(1000);
        if (msec-- == 0) {
            printk("HDMI PHY i2c operation time out!!\n");
            return FALSE;
        }
        val |= (HW_HDMI_IH_I2CMPHY_STAT0.B.I2CMPHYERROR << BP_HDMI_IH_I2CMPHY_STAT0_I2CMPHYERROR);
        val |= (HW_HDMI_IH_I2CMPHY_STAT0.B.I2CMPHYDONE << BP_HDMI_IH_I2CMPHY_STAT0_I2CMPHYDONE);
    }
    return TRUE;
}

/*!
 * @brief HDMI phy registers access through internal I2C bus
 *
 * @param	data Value to be send
 * @param	addr Sub-address of registers
 *
 * @return TRUE if I2C write succeed or FALSE if I2C write failed/timeout
 */
static int hdmi_phy_i2c_write(uint16_t data, uint8_t addr)
{
    HW_HDMI_IH_I2CMPHY_STAT0.U = 0xFF;
    HW_HDMI_PHY_I2CM_ADDRESS_ADDR.U = addr;
    HW_HDMI_PHY_I2CM_DATAO_1_ADDR.U = (uint8_t) (data >> 8);
    HW_HDMI_PHY_I2CM_DATAO_0_ADDR.U = (uint8_t) data;
    HW_HDMI_PHY_I2CM_OPERATION_ADDR.B.WRITE = 1;
    return hdmi_phy_wait_i2c_done(1000);
}

/*!
 * @brief HDMI phy registers read through internal I2C bus
 *
 * @param	addr Sub-address of register
 *
 * @return	value read back
 */
static uint16_t hdmi_phy_i2c_read(uint8_t addr)
{
    uint16_t data;
    uint8_t msb = 0, lsb = 0;

    HW_HDMI_IH_I2CMPHY_STAT0.U = 0xFF;
    HW_HDMI_PHY_I2CM_ADDRESS_ADDR.U = addr;
    HW_HDMI_PHY_I2CM_OPERATION_ADDR.B.READ = 1;
    hdmi_phy_wait_i2c_done(1000);
    msb = HW_HDMI_PHY_I2CM_DATAI_1_ADDR.U;
    lsb = HW_HDMI_PHY_I2CM_DATAI_0_ADDR.U;
    data = (msb << 8) | lsb;
    return data;
}

/*!
 * @brief HDMI phy registers access and verificate through internal I2C bus
 *
 * @param	data Value to be send
 * @param	addr Sub-address of registers
 *
 * @return TRUE if I2C write succeed, or False if the value read back does not
 * equal to the input
 */
static int hdmi_phy_i2c_write_verify(uint16_t data, uint8_t addr)
{
    unsigned short val = 0;
    //int repeat = 5;
    hdmi_phy_i2c_write(data, addr);
    val = hdmi_phy_i2c_read(addr);
    if (val != data) {
        printk("I2C data verification failed at REG 0x%02x : 0x%04x vs 0x%04x\n", addr, data, val);
        return FALSE;
    }
    return TRUE;
}

#define PHY_GEN2
#define PHY_TNP
/*!
 * @brief HDMI phy configuration
 *
 * @param	pClk Pixel clock input
 * @param	pRep Repetition setting
 * @param	cRes Color resolution per component
 * @param	cscOn Csc on/off switch
 * @param	audioOn Audio on/off switch
 * @param	cecOn CEC on/off switch
 * @param	hdcpOn HDCP on/off switch
 *
 * @return FALSE if configuration failed or TRUE ifconfiguration succeed
 */
static int hdmi_phy_configure(uint16_t pClk, uint8_t pRep, uint8_t cRes, int cscOn,
                              int audioOn, int cecOn, int hdcpOn)
{
    // colour resolution 0 is 8 bit colour depth
    if (cRes == 0) {
        cRes = 8;
    }
    if (pRep != 0) {
        printk("pixel repetition not supported %d", pRep);
        return FALSE;
    } else if (cRes != 8 && cRes != 12) {
        printk("color resolution not supported %d", cRes);
        return FALSE;
    }
#ifndef PHY_TNP
    uint16_t clk = 0, rep = 0;
    switch (pClk) {
    case 2520:
        clk = 0x93C1;
        rep = (cRes == 8) ? 0x6A4A : 0x6653;
        break;
    case 2700:
        clk = 0x96C1;
        rep = (cRes == 8) ? 0x6A4A : 0x6653;
        break;
    case 5400:
        clk = 0x8CC3;
        rep = (cRes == 8) ? 0x6A4A : 0x6653;
        break;
    case 7200:
        clk = 0x90C4;
        rep = (cRes == 8) ? 0x6A4A : 0x6654;
        break;
    case 7425:
        clk = 0x95C8;
        rep = (cRes == 8) ? 0x6A4A : 0x6654;
        break;
    case 10800:
        clk = 0x98C6;
        rep = (cRes == 8) ? 0x6A4A : 0x6653;
        break;
    case 14850:
        clk = 0x89C9;
        rep = (cRes == 8) ? 0x6A4A : 0x6654;
        break;
    default:
        printk("pixel clock not supported %d", pClk);
        return FALSE;
    }
#endif

    HW_HDMI_MC_FLOWCTRL.B.FEED_THROUGH_OFF = (cscOn == TRUE) ? 1 : 0;
    // clock gate == 0 => turn on modules
    HW_HDMI_MC_CLKDIS.B.PIXELCLK_DISABLE = 0;
    HW_HDMI_MC_CLKDIS.B.TMDSCLK_DISABLE = 0;
    HW_HDMI_MC_CLKDIS.B.PREPCLK_DISABLE = (pRep > 0) ? 1 : 0;
    HW_HDMI_MC_CLKDIS.B.AUDCLK_DISABLE = (audioOn == TRUE) ? 1 : 0;
    HW_HDMI_MC_CLKDIS.B.CSCCLK_DISABLE = (cscOn == TRUE) ? 1 : 0;
    HW_HDMI_MC_CLKDIS.B.CECCLK_DISABLE = (cecOn == TRUE) ? 1 : 0;
    HW_HDMI_MC_CLKDIS.B.HDCPCLK_DISABLE = (hdcpOn == TRUE) ? 1 : 0;

#ifdef PHY_GEN2
    hdmi_phy_gen2_tx_pon(0);
    hdmi_phy_gen2_pddq(1);
    hdmi_phy_reset(0);
    hdmi_phy_reset(1);
    hdmi_heacphy_reset(1);
    hdmi_phy_test_clear(1);
    HW_HDMI_PHY_I2CM_SLAVE_ADDR.U = 0x69;
    hdmi_phy_test_clear(0);

    switch (pClk) {
    case 2520:
        switch (cRes) {
        case 8:
            // PLL/MPLL Cfg
            hdmi_phy_i2c_write(0x01e0, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);   // CURRCTRL
            hdmi_phy_i2c_write(0x0000, 0x15);   // GMPCTRL
            hdmi_phy_i2c_write(0x0000, 0x13);   // PLLPHBYCTRL
            hdmi_phy_i2c_write(0x0006, 0x17);
            // RESISTANCE TERM 133Ohm Cfg
            hdmi_phy_i2c_write(0x0005, 0x19);   // TXTERM
            // PREEMP Cgf 0.00
            hdmi_phy_i2c_write(0x8009, 0x09);   // CKSYMTXCTRL
            // TX/CK LVL 10
            hdmi_phy_i2c_write(0x0210, 0x0E);   // VLEVCTRL
            // REMOVE CLK TERM
            hdmi_phy_i2c_write(0x8000, 0x05);   // CKCALCTRL
            break;
        case 10:
            hdmi_phy_i2c_write(0x21e1, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x0000, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 12:
            hdmi_phy_i2c_write(0x41e2, 0x06);
            hdmi_phy_i2c_write(0x06dc, 0x10);
            hdmi_phy_i2c_write(0x0000, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        default:
            printk("color depth not supported %d", pClk);
            return FALSE;
        }
        break;
    case 2700:
        switch (cRes) {
        case 8:
            hdmi_phy_i2c_write(0x01e0, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x0000, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 10:
            hdmi_phy_i2c_write(0x21e1, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x0000, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 12:
            hdmi_phy_i2c_write(0x41e2, 0x06);
            hdmi_phy_i2c_write(0x06dc, 0x10);
            hdmi_phy_i2c_write(0x0000, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        default:
            printk("color depth not supported %d", pClk);
            return FALSE;
        }
        break;
    case 5400:
        switch (cRes) {
        case 8:
            hdmi_phy_i2c_write(0x0140, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x0005, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 10:
            hdmi_phy_i2c_write(0x2141, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x0005, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 12:
            hdmi_phy_i2c_write(0x4142, 0x06);
            hdmi_phy_i2c_write(0x06dc, 0x10);
            hdmi_phy_i2c_write(0x0005, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        default:
            printk("color depth not supported %d", pClk);
            return FALSE;
        }
        break;
    case 7200:
        switch (cRes) {
        case 8:
            hdmi_phy_i2c_write(0x0140, 0x06);
            hdmi_phy_i2c_write(0x06dc, 0x10);
            hdmi_phy_i2c_write(0x0005, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 10:
            hdmi_phy_i2c_write(0x2141, 0x06);
            hdmi_phy_i2c_write(0x06dc, 0x10);
            hdmi_phy_i2c_write(0x0005, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 12:
            hdmi_phy_i2c_write(0x40a2, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x000a, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        default:
            printk("color depth not supported %d", pClk);
            return FALSE;
        }
        break;
    case 7425:
        switch (cRes) {
        case 8:
            hdmi_phy_i2c_write(0x0140, 0x06);
            hdmi_phy_i2c_write(0x06dc, 0x10);
            hdmi_phy_i2c_write(0x0005, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 10:
            hdmi_phy_i2c_write(0x20a1, 0x06);
            hdmi_phy_i2c_write(0x0b5c, 0x10);
            hdmi_phy_i2c_write(0x000a, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 12:
            hdmi_phy_i2c_write(0x40a2, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x000a, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        default:
            printk("color depth not supported %d", pClk);
            return FALSE;
        }
        break;
    case 10800:
        switch (cRes) {
        case 8:
            hdmi_phy_i2c_write(0x00a0, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x000a, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 10:
            hdmi_phy_i2c_write(0x20a1, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x000a, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 12:
            hdmi_phy_i2c_write(0x40a2, 0x06);
            hdmi_phy_i2c_write(0x06dc, 0x10);
            hdmi_phy_i2c_write(0x000a, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        default:
            printk("color depth not supported %d", pClk);
            return FALSE;
        }
        break;
    case 14850:
        switch (cRes) {
        case 8:
            hdmi_phy_i2c_write(0x00a0, 0x06);
            hdmi_phy_i2c_write(0x06dc, 0x10);
            hdmi_phy_i2c_write(0x000a, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 10:
            hdmi_phy_i2c_write(0x2001, 0x06);
            hdmi_phy_i2c_write(0x0b5c, 0x10);
            hdmi_phy_i2c_write(0x000f, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x8009, 0x09);
            hdmi_phy_i2c_write(0x0210, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        case 12:
            hdmi_phy_i2c_write(0x4002, 0x06);
            hdmi_phy_i2c_write(0x091c, 0x10);
            hdmi_phy_i2c_write(0x000f, 0x15);
            hdmi_phy_i2c_write(0x0000, 0x13);
            hdmi_phy_i2c_write(0x0006, 0x17);
            hdmi_phy_i2c_write(0x0005, 0x19);
            hdmi_phy_i2c_write(0x800b, 0x09);
            hdmi_phy_i2c_write(0x0129, 0x0E);
            hdmi_phy_i2c_write(0x8000, 0x05);
            break;
        default:
            printk("color depth not supported %d", pClk);
            return FALSE;
        }
        break;
    default:
        printk("pixel clock not supported %d", pClk);
        return FALSE;
    }
    hdmi_phy_gen2_tx_pon(1);
    hdmi_phy_gen2_pddq(0);
    udelay(1000);
#else //this is for test mode
    hdmi_phy_test_clear(0);
    hdmi_phy_test_clear(1);
    hdmi_phy_test_clear(0);
#define PHY_TNP
#ifdef PHY_TNP
    printk("TNP build\n");
#else
    printk("CNP build\n");
#endif
#ifndef PHY_TNP
    hdmi_phy_test_clock(0);
    hdmi_phy_test_enable(0);
    if (hdmi_phy_test_control(0x1B) == FALSE) {
        return FALSE;
    }
    hdmi_phy_test_data((unsigned char)(clk >> 8));
    hdmi_phy_test_data((unsigned char)(clk >> 0));
    if (hdmi_phy_test_control(0x1A) == FALSE) {
        return FALSE;
    }
    hdmi_phy_test_data((unsigned char)(rep >> 8));
    hdmi_phy_test_data((unsigned char)(rep >> 0));
#endif

    if (pClk == 14850 && cRes == 12) {
        printk("Applying Pre-Emphase");
        if (hdmi_phy_test_control(0x24) == FALSE) {
            return FALSE;
        }
        hdmi_phy_test_data(0x80);
        hdmi_phy_test_data(0x90);
        hdmi_phy_test_data(0xa0);
#ifndef PHY_TNP
        hdmi_phy_test_data(0xb0);
        if (hdmi_phy_test_control(0x20) == FALSE) { // +11.1ma 3.3 pe
            return FALSE;
        }
        hdmi_phy_test_data(0x04);
        if (hdmi_phy_test_control(0x21) == FALSE) { // +11.1 +2ma 3.3 pe
            return FALSE;
        }
        hdmi_phy_test_data(0x2a);
        // TODO verify
        if (hdmi_phy_test_control(0x11) == FALSE) {
            return FALSE;
        }
        hdmi_phy_test_data(0xf3);
        hdmi_phy_test_data(0x93);
#else
        if (hdmi_phy_test_control(0x20) == FALSE) {
            return FALSE;
        }
        hdmi_phy_test_data(0x00);
        if (hdmi_phy_test_control(0x21) == FALSE) {
            return FALSE;
        }
        hdmi_phy_test_data(0x00);
#endif
    }
    if (hdmi_phy_test_control(0x00) == FALSE) {
        return FALSE;
    }
    hdmi_phy_test_din(0x00);
    hdmi_phy_reset(1);          // reset PHY
    hdmi_phy_reset(0);          // for phy gen2 shoud be 0 => 1
    hdmi_phy_tmds(0);           // toggle TMDS
    hdmi_phy_tmds(1);
#endif
    if (HW_HDMI_IH_PHY_STAT0.B.TX_PHY_LOCK == 0) {
        //printk("PHY PLL not locked");
        return FALSE;
    }
    return TRUE;
}
/*!
 * @brief HDMI phy initialization
 *
 * @param	de Data enable polarity, 1 for positive and 0 for negative
 * @param	pclk Pixel clock
 */
void hdmi_phy_init(uint8_t de, uint16_t pclk)
{
    hdmi_phy_int_mask(0xFF);
    hdmi_phy_de_pol((de == TRUE) ? 1 : 0);
    hdmi_phy_if_sel(0);
    hdmi_phy_tmds(1);
    hdmi_phy_pdown(1);          //enable PHY

    hdmi_phy_configure(pclk, 0, 8, FALSE, FALSE, FALSE, FALSE);
}
