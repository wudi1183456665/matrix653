/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: imx6q_hdmi.c
**
** ��   ��   ��: zhang.xu (����)
**
** �ļ���������: 2015 �� 12 �� 13 ��
**
** ��        ��: ʵ�� HDMI ��������ʹ�õ� I2C �ӿھ�Ϊ I2C2��ʹ���źŹܽ���ͬ��
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
  �궨��
*********************************************************************************************************/
#define  HDMI_MODE_1080P          (1080)
#define  HDMI_MODE_720P           (720)
#define  CFG_HDMI_MODE            (HDMI_MODE_1080P)                     /*  Ŀǰ֧�� 1080P 720P         */

#if (CFG_HDMI_MODE == HDMI_MODE_1080P)
#define  HDMI_PANEL_WIDTH         (1920)
#define  HDMI_PANEL_HEIGHT        (1080)
#endif

#if (CFG_HDMI_MODE == HDMI_MODE_720P)
#define  HDMI_PANEL_WIDTH         (1280)
#define  HDMI_PANEL_HEIGHT        (720)
#endif
/*********************************************************************************************************
  LCD �豸�ṹ
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
    INT                     LCDD_iLock;                                 /*  ����豸�Ƿ��Ѿ���        */
} LCD_DEV;
/*********************************************************************************************************
  ��ʾ��Ϣ
*********************************************************************************************************/
static  LCD_DEV     _G_ldevHdmiDev;
/*********************************************************************************************************
  FrameBuffer ��ַ
*********************************************************************************************************/
static  PVOID       LCD_pFbBase = NULL;
/*********************************************************************************************************
** ��������: imx6qHdmiInit
** ��������: HDMI ��ʾ������ʼ��
** �䡡��  : NONE
** �䡡��  : �������.
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT32  imx6qHdmiInit (INT32  *iIpuIndex, UINT32  uiClk)
{
    hdmi_pgm_iomux();
    hdmi_clock_set(*iIpuIndex, uiClk);

    return  (true);
}
/*********************************************************************************************************
** ��������: imx6qHdmiDeInit
** ��������: HDMI ��ʾ����
** �䡡��  : NONE
** �䡡��  : �������.
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT32  imx6qHdmiDeInit (VOID)
{
    return  (true);
}
/*********************************************************************************************************
   ��ʾ���������ýṹ��
*********************************************************************************************************/
ips_dev_panel_t  disp_hdmi[] = {
    {
        "HDMI 720P 60Hz",                                               /*  name                        */
        HDMI_720P60,                                                    /*  name flag                   */
        DISP_DEV_HDMI,                                                  /*  panel type                  */
        DCMAP_RGB888,                                                   /*  ԭ������ DCMAP_RGB888       */
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
** ��������: imx6qHdmiFindPanel
** ��������: ��һ������������ģʽ
** �䡡��  : uiModeId  :  ģʽID (Ŀǰ֧�� 1080P 720P)
** �䡡��  : �������.
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6qSylixOSBmp
** ��������: SylixOS ��������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static  VOID  imx6qSylixOSBmp (VOID)
{

}
/*********************************************************************************************************
** ��������: imx6qHdmiPhyInit
** ��������: imx6q HDMI PHY ��ʼ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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

    hdmi_audio_mute(FALSE);                                             /*  ��ֹ��Ƶ�������            */
    hdmi_tx_hdcp_config(myHDMI_info.video_mode->mDataEnablePolarity);

    hdmi_phy_init(TRUE, myHDMI_info.video_mode->mPixelClock);

    hdmi_config_input_source(IPU2_DI0);                                 /* cfg source to HDMI port      */
}
/*********************************************************************************************************
** ��������: imx6qHdmiCreate
** ��������: ���� HDMI framebuffer
** �䡡��  : NONE
** �䡡��  : �������.
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qHdmiCreate (VOID)
{
    INT32                iIpuIndex = 2;                                 /*  use ipu 2                   */
    UINT32               uiFbBase;
    ips_dev_panel_t     *pPanel;

    if (!LCD_pFbBase) {                                                 /* ����һ���ڴ���framebuffer    */
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
    uiFbBase = (UINT32)LCD_pFbBase;                                     /*  ���� DMA ��������ַ         */

    pPanel->panel_init(&iIpuIndex, pPanel->pixel_clock);

    ipu_sw_reset(iIpuIndex, 1000);                                      /*  reset IPU                   */

    ipu_display_setup(iIpuIndex, uiFbBase, (UINT32)NULL,                /*  setup IPU display channel   */
                      INTERLEAVED_ARGB8888, pPanel);

    imx6qHdmiPhyInit();

    ipu_enable_display(iIpuIndex);                                      /*  enable ipu display channel  */

    /*
     * ���������Ҫ��ʾ���Զ�����������
     */
    imx6qSylixOSBmp();

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __lcdOpen
** ��������: �� FB
** �䡡��  : pgmdev    :  fb �豸
**           iFlag     :  ���豸��־
**           iMode     :  ���豸ģʽ
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __lcdOpen (PLW_GM_DEVICE  pgmdev, INT  iFlag, INT  iMode)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __lcdClose
** ��������: FB �رպ���  �� Linux ��������Ҳû��ʵ�� framebuffer �Ĺرղ�����
** �䡡��  : pgmdev    :  fb �豸
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��: ����ο�s5pv210����ƣ���� close ����
*********************************************************************************************************/
static INT  __lcdClose (PLW_GM_DEVICE  pgmdev)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __lcdGetVarInfo
** ��������: ��� FB ��Ϣ  �˲���������Ҫ�˶�    --sylixos
** �䡡��  : pgmdev    :  fb �豸
**           pgmsi     :  fb ��Ļ��Ϣ
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
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
         * imx6q ��ʹ��HDMI���ԣ�Ĭ��ʹ��720p = 1280 * 720, ��Ӧframebuffer��������-X����-Y
         */
        pgmvi->GMVI_ulXRes = HDMI_PANEL_WIDTH;
        pgmvi->GMVI_ulYRes = HDMI_PANEL_HEIGHT;

        pgmvi->GMVI_ulXResVirtual = HDMI_PANEL_WIDTH;
        pgmvi->GMVI_ulYResVirtual = HDMI_PANEL_HEIGHT;

        pgmvi->GMVI_ulXOffset = 0;
        pgmvi->GMVI_ulYOffset = 0;
        /*
         * hdmi ʹ�� 24bits ��ʾ��ÿ�����ص�ʹ�� 4 ���ֽڣ��������һ���ֽ�Ϊ�ա�
         */
        pgmvi->GMVI_ulBitsPerPixel  = 32;
        pgmvi->GMVI_ulBytesPerPixel = 4;
        /*
         * ������к���,��ʵ������
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
** ��������: __lcdGetScrInfo
** ��������: ��� LCD ��Ļ��Ϣ
** �䡡��  : pgmdev    :  fb �豸
**           pgmsi     :  fb ��Ļ��Ϣ
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __lcdGetScrInfo (PLW_GM_DEVICE  pgmdev, PLW_GM_SCRINFO  pgmsi)
{
    LCD_DEV     *plcddev;

    plcddev = _LIST_ENTRY(pgmdev, LCD_DEV, LCDD_gmdev);
    if (!plcddev) {
        return  (PX_ERROR);
    }

    /*
     * imx6q ��ʹ��HDMI���ԣ�Ĭ��ʹ��720p = 1280 * 720, ��Ӧframebuffer��������-X����-Y
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
** ��������: lcdDevCreate
** ��������: ���� LCD �豸 (fb0)
** �䡡��  : NONE
** �䡡��  : �������.
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qHdmiDevCreate (VOID)
{
    static INT  iLock = 0;

    if (iLock) {
        return  (ERROR_NONE);
    }

    _G_ldevHdmiDev.LCDD_pvFbBase  = LCD_pFbBase;

    /*
     *  ��֧�� framebuffer ģʽ.
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
