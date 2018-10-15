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
** 文   件   名: imx6q_hdmi.c
**
** 创   建   人: zhang.xu (张旭)
**
** 文件创建日期: 2015 年 12 月 13 日
**
** 描        述: 实现 HDMI 的驱动，使用的 I2C 接口均为 I2C2，使用信号管脚相同。
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include "hdmi/imx6q_hdmi.h"

#include "sdk.h"
#include "hdmi/hdmi_tx.h"
#include "ipu_fb/ipu_common.h"
#include "regshdmi.h"
#include "regsccm.h"
#include "regsccmanalog.h"
/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#define  HDMI_MODE_1080P          (1080)
#define  HDMI_MODE_720P           (720)
#define  CFG_HDMI_MODE            (HDMI_MODE_1080P)                     /*  目前支持 1080P 720P         */

#if (CFG_HDMI_MODE == HDMI_MODE_1080P)
#define  HDMI_PANEL_WIDTH         (1920)
#define  HDMI_PANEL_HEIGHT        (1080)
#endif

#if (CFG_HDMI_MODE == HDMI_MODE_720P)
#define  HDMI_PANEL_WIDTH         (1280)
#define  HDMI_PANEL_HEIGHT        (720)
#endif
/*********************************************************************************************************
  LCD 设备结构
*********************************************************************************************************/
typedef struct lcd_dev {
    LW_GM_DEVICE            LCDD_gmdev;
    LW_GM_FILEOPERATIONS    LCDD_gmfo;
    PVOID                   LCDD_pvFbBase;
    addr_t                  LCDD_atReg;
    INT                     LCDD_iX;
    INT                     LCDD_iY;
    INT                     LCDD_iBpp;
    INT                     LCDD_iLcdMode;
    struct clk             *LCDD_clk;
    struct clk             *LCDD_clkBl;
    INT                     LCDD_iClkFreq;
    INT                     LCDD_iLock;                                 /*  标记设备是否已经打开        */
} LCD_DEV;
/*********************************************************************************************************
  显示信息
*********************************************************************************************************/
static  LCD_DEV     _G_ldevHdmiDev;
/*********************************************************************************************************
  FrameBuffer 地址
*********************************************************************************************************/
static  PVOID       LCD_pFbBase = NULL;
/*********************************************************************************************************
** 函数名称: imx6qHdmiInit
** 功能描述: HDMI 显示参数初始化
** 输　入  : NONE
** 输　出  : 错误代码.
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT32  imx6qHdmiInit (INT32  *iIpuIndex, UINT32  uiClk)
{
    hdmi_pgm_iomux();
    hdmi_clock_set(*iIpuIndex, uiClk);

    return  (true);
}
/*********************************************************************************************************
** 函数名称: imx6qHdmiDeInit
** 功能描述: HDMI 显示参数
** 输　入  : NONE
** 输　出  : 错误代码.
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT32  imx6qHdmiDeInit (VOID)
{
    return  (true);
}
/*********************************************************************************************************
   显示器参数设置结构体
*********************************************************************************************************/
ips_dev_panel_t  disp_hdmi[] = {
    {
        "HDMI 720P 60Hz",                                               /*  name                        */
        HDMI_720P60,                                                    /*  name flag                   */
        DISP_DEV_HDMI,                                                  /*  panel type                  */
        DCMAP_RGB888,                                                   /*  原有设置 DCMAP_RGB888       */
        60,                                                             /*  refresh rate                */
        1280,                                                           /*  panel width                 */
        720,                                                            /*  panel height                */
        74250000,                                                       /*  pixel clock frequency       */
        260,                                                            /*  hsync start width           */
        40,                                                             /*  hsync width                 */
        110,                                                            /*  hsyn back width             */
        25,                                                             /*  vysnc start width           */
        5,                                                              /*  vsync width                 */
        5,                                                              /*  vsync back width            */
        0,                                                              /*  delay from hsync to vsync   */
        0,                                                              /*  interlaced mode             */
        1,                                                              /*  clock selection             */
        0,                                                              /*  clock polarity              */
        1,                                                              /*  hsync selection             */
        1,                                                              /*  hsync polarity              */
        1,                                                              /*  drdy polarity               */
        0,                                                              /*  data polarity               */
        imx6qHdmiInit,                                                  /*  initialization              */
        imx6qHdmiDeInit,                                                /*  deinit                      */
    }, {
        "HDMI 1080P 60Hz",
        HDMI_1080P60,
        DISP_DEV_HDMI,
        DCMAP_RGB888,
        60,
        1920,
        1080,
        148500000,
        192,
        44,
        80,
        41,
        5,
        4,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        imx6qHdmiInit,
        imx6qHdmiDeInit,
    }
};
/*********************************************************************************************************
** 函数名称: imx6qHdmiFindPanel
** 功能描述: 找一个满足条件的模式
** 输　入  : uiModeId  :  模式ID (目前支持 1080P 720P)
** 输　出  : 错误代码.
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ips_dev_panel_t  *imx6qHdmiFindPanel (UINT32  uiModeId)
{
    INT      i;
    UINT32   panel_id;

    if (uiModeId == HDMI_MODE_1080P) {
        panel_id = HDMI_1080P60;

    } else if (uiModeId == HDMI_MODE_720P) {
        panel_id = HDMI_720P60;

    } else {
        printk(KERN_ERR "Mode isn't supported.\n");
        return  (NULL);

    }

    for (i = 0; i < ARRAY_SIZE(disp_hdmi); ++i) {
        if (disp_hdmi[i].panel_id == panel_id) {
            return  (&disp_hdmi[i]);
        }
    }

    return  (NULL);
}
/*********************************************************************************************************
** 函数名称: imx6qSylixOSBmp
** 功能描述: SylixOS 启动画面
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static  VOID  imx6qSylixOSBmp (VOID)
{

}
/*********************************************************************************************************
** 函数名称: imx6qHdmiPhyInit
** 功能描述: imx6q HDMI PHY 初始化
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qHdmiPhyInit (VOID)
{
    hdmi_data_info_s    myHDMI_info       = {0};
    hdmi_vmode_s        myHDMI_vmode_info = {0};

    myHDMI_info.video_mode = &myHDMI_vmode_info;

    myHDMI_info.enc_in_format       = eRGB;
    myHDMI_info.enc_out_format      = eRGB;
    myHDMI_info.enc_color_depth     = 8;
    myHDMI_info.colorimetry         = eITU601;
    myHDMI_info.pix_repet_factor    = 0;
    myHDMI_info.hdcp_enable         = 0;

#if (CFG_HDMI_MODE == HDMI_MODE_1080P)
    myHDMI_info.video_mode->mCode = 16;                                 /* 1920x1080p @ 59.94/60Hz 16:9 */
#endif

#if (CFG_HDMI_MODE == HDMI_MODE_720P)
    myHDMI_info.video_mode->mCode = 4;                                  /* 1280x720p @ 59.94/60Hz 16:9  */
#endif

    myHDMI_info.video_mode->mHdmiDviSel         = TRUE;
    myHDMI_info.video_mode->mRVBlankInOSC       = FALSE;
    myHDMI_info.video_mode->mRefreshRate        = 60000;
    myHDMI_info.video_mode->mDataEnablePolarity = TRUE;

    hdmi_av_frame_composer(&myHDMI_info);
    hdmi_video_packetize(myHDMI_info);
    hdmi_video_csc(myHDMI_info);
    hdmi_video_sample(myHDMI_info);

    hdmi_audio_mute(FALSE);                                             /*  禁止音频输出设置            */
    hdmi_tx_hdcp_config(myHDMI_info.video_mode->mDataEnablePolarity);

    hdmi_phy_init(TRUE, myHDMI_info.video_mode->mPixelClock);

    hdmi_config_input_source(IPU2_DI0);                                 /* cfg source to HDMI port      */
}
/*********************************************************************************************************
** 函数名称: imx6qHdmiCreate
** 功能描述: 创建 HDMI framebuffer
** 输　入  : NONE
** 输　出  : 错误代码.
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  imx6qHdmiCreate (VOID)
{
    INT32                iIpuIndex = 2;                                 /*  use ipu 2                   */
    UINT32               uiFbBase;
    ips_dev_panel_t     *pPanel;

    if (!LCD_pFbBase) {                                                 /* 分配一块内存做framebuffer    */
       LCD_pFbBase = API_VmmDmaAllocAlign(HDMI_PANEL_WIDTH * HDMI_PANEL_HEIGHT * 4,
                                          64 * LW_CFG_KB_SIZE);
       if (LCD_pFbBase == LW_NULL) {
           return  (PX_ERROR);
       }
    }

    pPanel = imx6qHdmiFindPanel(CFG_HDMI_MODE);
    if (!pPanel) {
        printk(KERN_ERR "Don't find a panel mode.\n");
        return  (PX_ERROR);
    }

    memset(LCD_pFbBase, 0xDD, HDMI_PANEL_WIDTH * HDMI_PANEL_HEIGHT * 4);
    uiFbBase = (UINT32)LCD_pFbBase;                                     /*  设置 DMA 控制器地址         */

    pPanel->panel_init(&iIpuIndex, pPanel->pixel_clock);

    ipu_sw_reset(iIpuIndex, 1000);                                      /*  reset IPU                   */

    ipu_display_setup(iIpuIndex, uiFbBase, (UINT32)NULL,                /*  setup IPU display channel   */
                      INTERLEAVED_ARGB8888, pPanel);

    imx6qHdmiPhyInit();

    ipu_enable_display(iIpuIndex);                                      /*  enable ipu display channel  */

    /*
     * 可以添加需要显示的自定义启动画面
     */
    imx6qSylixOSBmp();

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __lcdOpen
** 功能描述: 打开 FB
** 输　入  : pgmdev    :  fb 设备
**           iFlag     :  打开设备标志
**           iMode     :  打开设备模式
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __lcdOpen (PLW_GM_DEVICE  pgmdev, INT  iFlag, INT  iMode)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __lcdClose
** 功能描述: FB 关闭函数  在 Linux 的驱动下也没有实现 framebuffer 的关闭操作，
** 输　入  : pgmdev    :  fb 设备
** 输　出  : 错误号
** 全局变量:
** 调用模块: 这里参考s5pv210的设计，填充 close 内容
*********************************************************************************************************/
static INT  __lcdClose (PLW_GM_DEVICE  pgmdev)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __lcdGetVarInfo
** 功能描述: 获得 FB 信息  此部分内容需要核对    --sylixos
** 输　入  : pgmdev    :  fb 设备
**           pgmsi     :  fb 屏幕信息
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __lcdGetVarInfo (PLW_GM_DEVICE  pgmdev, PLW_GM_VARINFO  pgmvi)
{
    LCD_DEV     *plcddev;

    plcddev = _LIST_ENTRY(pgmdev, LCD_DEV, LCDD_gmdev);
    if (!plcddev) {
        return  (PX_ERROR);
    }

    if (pgmvi) {
        /*
         * imx6q 中使用HDMI调试，默认使用720p = 1280 * 720, 对应framebuffer参数，宽-X，高-Y
         */
        pgmvi->GMVI_ulXRes = HDMI_PANEL_WIDTH;
        pgmvi->GMVI_ulYRes = HDMI_PANEL_HEIGHT;

        pgmvi->GMVI_ulXResVirtual = HDMI_PANEL_WIDTH;
        pgmvi->GMVI_ulYResVirtual = HDMI_PANEL_HEIGHT;

        pgmvi->GMVI_ulXOffset = 0;
        pgmvi->GMVI_ulYOffset = 0;
        /*
         * hdmi 使用 24bits 显示，每个像素点使用 4 个字节，其中最后一个字节为空。
         */
        pgmvi->GMVI_ulBitsPerPixel  = 32;
        pgmvi->GMVI_ulBytesPerPixel = 4;
        /*
         * 不清楚有何用,无实际意义
         */
        pgmvi->GMVI_ulGrayscale = 0;
        /*
         * RGB888 RED 0xFF000000, GREEN 0x00FF0000, BLUE 0x0000FF00
         * RGB565 RED 0xF800,     GREEN 0x07E0,     BLUE 0x001F
         */
        pgmvi->GMVI_ulRedMask   = 0x00FF0000;
        pgmvi->GMVI_ulGreenMask = 0x0000FF00;
        pgmvi->GMVI_ulBlueMask  = 0x000000FF;
        pgmvi->GMVI_ulTransMask = 0xFF000000;

        pgmvi->GMVI_bHardwareAccelerate = LW_FALSE;
        pgmvi->GMVI_ulMode              = LW_GM_SET_MODE;
        pgmvi->GMVI_ulStatus            = 0;
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __lcdGetScrInfo
** 功能描述: 获得 LCD 屏幕信息
** 输　入  : pgmdev    :  fb 设备
**           pgmsi     :  fb 屏幕信息
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __lcdGetScrInfo (PLW_GM_DEVICE  pgmdev, PLW_GM_SCRINFO  pgmsi)
{
    LCD_DEV     *plcddev;

    plcddev = _LIST_ENTRY(pgmdev, LCD_DEV, LCDD_gmdev);
    if (!plcddev) {
        return  (PX_ERROR);
    }

    /*
     * imx6q 中使用HDMI调试，默认使用720p = 1280 * 720, 对应framebuffer参数，宽-X，高-Y
     */
    if (pgmsi) {
        pgmsi->GMSI_pcName           = "/dev/fb1";
        pgmsi->GMSI_ulId             = 0;
        pgmsi->GMSI_stMemSize        = HDMI_PANEL_WIDTH * HDMI_PANEL_HEIGHT * 4;
        pgmsi->GMSI_stMemSizePerLine = HDMI_PANEL_WIDTH * 4;
        pgmsi->GMSI_pcMem            = (caddr_t)plcddev->LCDD_pvFbBase;
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: lcdDevCreate
** 功能描述: 创建 LCD 设备 (fb0)
** 输　入  : NONE
** 输　出  : 错误代码.
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  imx6qHdmiDevCreate (VOID)
{
    static INT  iLock = 0;

    if (iLock) {
        return  (ERROR_NONE);
    }

    _G_ldevHdmiDev.LCDD_pvFbBase  = LCD_pFbBase;

    /*
     *  仅支持 framebuffer 模式.
     */
    _G_ldevHdmiDev.LCDD_gmfo.GMFO_pfuncOpen       = __lcdOpen;
    _G_ldevHdmiDev.LCDD_gmfo.GMFO_pfuncClose      = __lcdClose;
    _G_ldevHdmiDev.LCDD_gmfo.GMFO_pfuncGetVarInfo = (INT (*)(LONG, PLW_GM_VARINFO))__lcdGetVarInfo;
    _G_ldevHdmiDev.LCDD_gmfo.GMFO_pfuncGetScrInfo = (INT (*)(LONG, PLW_GM_SCRINFO))__lcdGetScrInfo;
    _G_ldevHdmiDev.LCDD_gmdev.GMDEV_gmfileop      = &_G_ldevHdmiDev.LCDD_gmfo;

    _G_ldevHdmiDev.LCDD_iClkFreq = 2700;
    _G_ldevHdmiDev.LCDD_iLcdMode = 1;

    return  (gmemDevAdd("/dev/fb1", &_G_ldevHdmiDev.LCDD_gmdev));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
