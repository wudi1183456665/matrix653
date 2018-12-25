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
** ��   ��   ��: imx6q_fb.c
**
** ��   ��   ��: zhang.xu (����)
**
** �ļ���������: 2015 �� 12 �� 13 ��
**
** ��        ��: ʵ�� ipu ���������ٷ� ipu ��������Ĭ���� lvds����������ʵ�� lvds �� rgb888 �������á�
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include "ipu_fb/imx6q_fb.h"
#include "pinmux/iomux_config.h"
#include "regsipu.h"
#include "ipu_fb/ldb_def.h"
#include "ipu_fb/ipu_common.h"
#include "pinmux/iomux_config.h"
#include "regsipu.h"
#include "ipu_fb/ldb_def.h"
#include "gpio/imx6q_gpio.h"

/*********************************************************************************************************
  LCD �豸�ṹ
*********************************************************************************************************/
typedef struct lcd_dev {
    LW_GM_DEVICE            LCDD_gmdev;
    LW_GM_FILEOPERATIONS    LCDD_gmfo;
    PVOID                   LCDD_pvFbBase;
    INT                     LCDD_uiPwrFB;                               /*  LCD ʹ��                    */
    INT                     LCDD_iBlkFB;                                /*  ����Ƶı仯                */
    INT                     LCDD_iLock;                                 /*  ����豸�Ƿ��Ѿ���        */
    UINT32                  LCDD_uiLcdWidth;                            /*  fb �豸�������ظ���         */
    UINT32                  LCDD_uiLcdHight;                            /*  fb �豸�������ظ���         */
    CPCHAR                  LCDD_ucpPwrDev;                             /*  pwm �����豸����"dev/pwm0"  */
    CPCHAR                  LCDD_ucpBlkDev;                             /*  pwm �����豸����"dev/pwm3"  */
    ips_dev_panel_t        *disp_panel;
} LCD_DEV;
typedef LCD_DEV          *__PLCD_DEV;
/*********************************************************************************************************
  ��ʾ��Ϣ
*********************************************************************************************************/
static LCD_DEV  _gImx6qFbDev;

PVOID  VGA_pFbBase = NULL;
/*
 * PWM ioctl ����������
 */
#define  PWM_FREN               (105)                                   /*  0 ~ 4069, 260 = 1Khz        */
#define  PWM_DUTY               (106)                                   /*  0 ~ 255,  255 = 100%        */
/*********************************************************************************************************
  �ⲿ���ú���
*********************************************************************************************************/
VOID lcdif_iomux_config();
/*********************************************************************************************************
** ��������: __lvdsPanelInit
** ��������: __lvds �ӿڳ�ʼ��
** �䡡��  : iIpuIndex   ʹ�õ� IPU �豸
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static int32_t __lvdsPanelInit (int32_t *iIpuIndex)
{
    ldb_iomux_config();
    ldb_clock_config(65000000, *iIpuIndex);

    if (*iIpuIndex == 1) {
        ldb_config(IPU1_DI0, LVDS_DUAL_PORT, LVDS_PANEL_18BITS_MODE, SPWG);
    } else {
        ldb_config(IPU2_DI0, LVDS_DUAL_PORT, LVDS_PANEL_18BITS_MODE, SPWG);
    }
    return  (true);
}
/*********************************************************************************************************
** ��������: imx6qIpuFbCreate
** ��������: ��ʼ�� IPU ����������ʾ
** �䡡��  : NONE
** �䡡��  : �������.
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qIpuFbCreate (ips_dev_panel_t  *disp_panel)
{
    UINT    uiDispMem;
    INT32   iIpuIndex = 1;                                              /* use ipu 1                    */

    ips_dev_panel_t  *panel;

    if (!disp_panel) {
        return  (PX_ERROR);
    }

    /*
     * ��������ҳ��, ������������ʽʹ�õ�ʱ��Ӧ��
     */
#if (BOARD_TQIMX6_BASEC == 1) || (BOARD_SYLIXOS_EVM == 1)
    VGA_pFbBase = (PVOID)BSP_CFG_RAM_RSV_ADDR;
#else
    if (!VGA_pFbBase) {                                                 /* ����һ���ڴ�������framebuffer*/
      	 VGA_pFbBase = API_VmmPhyAllocAlign(disp_panel->height * disp_panel->width * 4,
                                           4 * 1024, LW_ZONE_ATTR_DMA); /* 4K ����                      */
       if (VGA_pFbBase == LW_NULL) {
           return  (PX_ERROR);
       }
    }
#endif

    uiDispMem = (UINT32)VGA_pFbBase;

    lcdif_iomux_config();                                               /*  RGB ���нӿ���ʾ            */

    __lvdsPanelInit(&iIpuIndex);

    panel = disp_panel;

    /*
     * reset IPU
     */
    ipu_sw_reset(iIpuIndex, 1000);

    /*
     * setup IPU display channel
     */
//    ipu_display_setup(iIpuIndex, uiDispMem, (UINT32) NULL, INTERLEAVED_ARGB8888, panel);
    ipu_display_setup(iIpuIndex, uiDispMem, (UINT32) NULL, INTERLEAVED_RGB, panel);

    /*
     * enable ipu display channel
     */
    ipu_enable_display(iIpuIndex);

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
	__PLCD_DEV  plcddev = (__PLCD_DEV)pgmdev;

#if BSP_CFG_CLOSE_FB_EN
	plcddev->LCDD_iBlkFB   = -1;

	API_GpioSetValue(plcddev->LCDD_uiPwrFB, LW_GPIOF_INIT_HIGH);

	if (plcddev->LCDD_ucpBlkDev) {
	    plcddev->LCDD_iBlkFB  = open(plcddev->LCDD_ucpBlkDev, O_RDWR, 0666);

	    ioctl(plcddev->LCDD_iBlkFB, PWM_FREN, 4000);
	    ioctl(plcddev->LCDD_iBlkFB, PWM_DUTY, 56);
	}
#endif

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
	__PLCD_DEV  plcddev = (__PLCD_DEV)pgmdev;

#if BSP_CFG_CLOSE_FB_EN
	API_GpioSetValue(plcddev->LCDD_uiPwrFB, LW_GPIOF_INIT_LOW);

	if (plcddev->LCDD_iBlkFB != -1) {
	    close(plcddev->LCDD_iBlkFB);
	}
#endif

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
	__PLCD_DEV  plcddev = (__PLCD_DEV)pgmdev;

    if (!plcddev) {
        return  (PX_ERROR);
    }

    if (pgmvi) {

    	/*
    	 * imx6q ��ʹ��LCD ���ԣ�Ĭ��ʹ��7�紥�������ֱ���LCD_WIDTHxLCD_HEIGHT
    	 */
        pgmvi->GMVI_ulXRes = plcddev->disp_panel->width;
        pgmvi->GMVI_ulYRes = plcddev->disp_panel->height;

        pgmvi->GMVI_ulXResVirtual = plcddev->disp_panel->width;
        pgmvi->GMVI_ulYResVirtual = plcddev->disp_panel->height;

        pgmvi->GMVI_ulXOffset = 0;
        pgmvi->GMVI_ulYOffset = 0;

    	/*
    	 * lcd ʹ�� 24bits ��ʾ��ÿ�����ص�ʹ�� 4 ���ֽڣ��������һ���ֽ�Ϊ�ա�
    	 */
        pgmvi->GMVI_ulBitsPerPixel  = 32;
        pgmvi->GMVI_ulBytesPerPixel = 4;
        pgmvi->GMVI_ulGrayscale     = 0;

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

        pgmvi->GMVI_gmbfTrans.GMBF_uiOffset   = 24;
        pgmvi->GMVI_gmbfTrans.GMBF_uiLength   = 8;
        pgmvi->GMVI_gmbfTrans.GMBF_uiMsbRight = 0;

        pgmvi->GMVI_gmbfRed.GMBF_uiOffset     = 16;
        pgmvi->GMVI_gmbfRed.GMBF_uiLength     = 8;
        pgmvi->GMVI_gmbfRed.GMBF_uiMsbRight   = 0;

        pgmvi->GMVI_gmbfGreen.GMBF_uiOffset   = 8;
        pgmvi->GMVI_gmbfGreen.GMBF_uiLength   = 8;
        pgmvi->GMVI_gmbfGreen.GMBF_uiMsbRight = 0;

        pgmvi->GMVI_gmbfBlue.GMBF_uiOffset    = 0;
        pgmvi->GMVI_gmbfBlue.GMBF_uiLength    = 8;
        pgmvi->GMVI_gmbfBlue.GMBF_uiMsbRight  = 0;
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
	__PLCD_DEV  plcddev = (__PLCD_DEV)pgmdev;

    if (!plcddev) {
        return  (PX_ERROR);
    }

	/*
	 * imx6q ��ʹ��lcd���ԣ�Ĭ��ʹ��7 �紥����
	 */
    if (pgmsi) {
        pgmsi->GMSI_pcName           = "/dev/fb0";
        pgmsi->GMSI_ulId             = 0;
        pgmsi->GMSI_stMemSize        = plcddev->disp_panel->width * plcddev->disp_panel->height * 4;
        pgmsi->GMSI_stMemSizePerLine = plcddev->disp_panel->width * 4;
        pgmsi->GMSI_pcMem            = (caddr_t)plcddev->LCDD_pvFbBase;
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: lcdDevCreate
** ��������: ���� LCD �豸 (fb0)
** �䡡��  : ucpPwrDev:  LCD �豸��Դ���ƹܽ���Ӧ�豸����    ����� GPIO ��Ҫ��д��Ӧ���ַ��豸����
**           ucpBlkDev:  LCD �豸����ƿ��ƹܽ���Ӧ�豸����  ����ƿ���һ���ʹ�� PWM �Ա��������
**           disp_panel: �����������ṹ��
** �䡡��  : �������.
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qIpuDevCreate (UINT  uiPwrPin, CPCHAR  ucpBlkDev, ips_dev_panel_t  *disp_panel)
{
    INT  iLock = 0;

    if (iLock) {
        return  (ERROR_NONE);
    }

    imx6qIpuFbCreate(disp_panel);

    _gImx6qFbDev.LCDD_pvFbBase  = VGA_pFbBase;
    /*
     *  ��֧�� framebuffer ģʽ.
     */
    _gImx6qFbDev.LCDD_uiPwrFB                   = uiPwrPin;
    _gImx6qFbDev.LCDD_ucpBlkDev                 = ucpBlkDev;
    _gImx6qFbDev.disp_panel                     = disp_panel;
    _gImx6qFbDev.LCDD_gmfo.GMFO_pfuncOpen       = __lcdOpen;
    _gImx6qFbDev.LCDD_gmfo.GMFO_pfuncClose      = __lcdClose;
    _gImx6qFbDev.LCDD_gmfo.GMFO_pfuncGetVarInfo = (INT (*)(LONG, PLW_GM_VARINFO))__lcdGetVarInfo;
    _gImx6qFbDev.LCDD_gmfo.GMFO_pfuncGetScrInfo = (INT (*)(LONG, PLW_GM_SCRINFO))__lcdGetScrInfo;
    _gImx6qFbDev.LCDD_gmdev.GMDEV_gmfileop      = &_gImx6qFbDev.LCDD_gmfo;

#if (BSP_CFG_CLOSE_FB_EN == 0)
	if (_gImx6qFbDev.LCDD_ucpPwrDev) {
		_gImx6qFbDev.LCDD_uiPwrFB = open(_gImx6qFbDev.LCDD_ucpPwrDev, O_RDWR, 0666);
	}

	if (_gImx6qFbDev.LCDD_ucpBlkDev) {
		_gImx6qFbDev.LCDD_iBlkFB  = open(_gImx6qFbDev.LCDD_ucpBlkDev, O_RDWR, 0666);

	    ioctl(_gImx6qFbDev.LCDD_iBlkFB, PWM_FREN, 4000);
	    ioctl(_gImx6qFbDev.LCDD_iBlkFB, PWM_DUTY, 56);
	}
#endif
    return  (gmemDevAdd("/dev/fb0", &_gImx6qFbDev.LCDD_gmdev));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
