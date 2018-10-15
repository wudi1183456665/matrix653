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
** 文   件   名: imx6q_fb.c
**
** 创   建   人: zhang.xu (张旭)
**
** 文件创建日期: 2015 年 12 月 13 日
**
** 描        述: 实现 ipu 的驱动，官方 ipu 测试例程默认是 lvds，本驱动中实现 lvds 和 rgb888 两种配置。
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
  LCD 设备结构
*********************************************************************************************************/
typedef struct lcd_dev {
    LW_GM_DEVICE            LCDD_gmdev;
    LW_GM_FILEOPERATIONS    LCDD_gmfo;
    PVOID                   LCDD_pvFbBase;
    INT                     LCDD_uiPwrFB;                               /*  LCD 使能                    */
    INT                     LCDD_iBlkFB;                                /*  背光灯的变化                */
    INT                     LCDD_iLock;                                 /*  标记设备是否已经打开        */
    UINT32                  LCDD_uiLcdWidth;                            /*  fb 设备的行像素个数         */
    UINT32                  LCDD_uiLcdHight;                            /*  fb 设备的列像素个数         */
    CPCHAR                  LCDD_ucpPwrDev;                             /*  pwm 控制设备名称"dev/pwm0"  */
    CPCHAR                  LCDD_ucpBlkDev;                             /*  pwm 控制设备名称"dev/pwm3"  */
    ips_dev_panel_t        *disp_panel;
} LCD_DEV;
typedef LCD_DEV          *__PLCD_DEV;
/*********************************************************************************************************
  显示信息
*********************************************************************************************************/
static LCD_DEV  _gImx6qFbDev;

PVOID  VGA_pFbBase = NULL;
/*
 * PWM ioctl 函数命令字
 */
#define  PWM_FREN               (105)                                   /*  0 ~ 4069, 260 = 1Khz        */
#define  PWM_DUTY               (106)                                   /*  0 ~ 255,  255 = 100%        */
/*********************************************************************************************************
  外部引用函数
*********************************************************************************************************/
VOID lcdif_iomux_config();
/*********************************************************************************************************
** 函数名称: __lvdsPanelInit
** 功能描述: __lvds 接口初始化
** 输　入  : iIpuIndex   使用的 IPU 设备
** 输　出  : 错误号
** 全局变量:
** 调用模块:
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
** 函数名称: imx6qIpuFbCreate
** 功能描述: 初始化 IPU 控制器的显示
** 输　入  : NONE
** 输　出  : 错误代码.
** 全局变量:
** 调用模块:
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
     * 分配物理页表, 这里在驱动正式使用的时候应用
     */
#if (BOARD_TQIMX6_BASEC == 1) || (BOARD_SYLIXOS_EVM == 1)
    VGA_pFbBase = (PVOID)BSP_CFG_RAM_RSV_ADDR;
#else
    if (!VGA_pFbBase) {                                                 /* 分配一块内存用来做framebuffer*/
      	 VGA_pFbBase = API_VmmPhyAllocAlign(disp_panel->height * disp_panel->width * 4,
                                           4 * 1024, LW_ZONE_ATTR_DMA); /* 4K 对齐                      */
       if (VGA_pFbBase == LW_NULL) {
           return  (PX_ERROR);
       }
    }
#endif

    uiDispMem = (UINT32)VGA_pFbBase;

    lcdif_iomux_config();                                               /*  RGB 并行接口显示            */

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
** 函数名称: __lcdClose
** 功能描述: FB 关闭函数  在 Linux 的驱动下也没有实现 framebuffer 的关闭操作，
** 输　入  : pgmdev    :  fb 设备
** 输　出  : 错误号
** 全局变量:
** 调用模块: 这里参考s5pv210的设计，填充 close 内容
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
	__PLCD_DEV  plcddev = (__PLCD_DEV)pgmdev;

    if (!plcddev) {
        return  (PX_ERROR);
    }

    if (pgmvi) {

    	/*
    	 * imx6q 中使用LCD 调试，默认使用7寸触摸屏，分辨率LCD_WIDTHxLCD_HEIGHT
    	 */
        pgmvi->GMVI_ulXRes = plcddev->disp_panel->width;
        pgmvi->GMVI_ulYRes = plcddev->disp_panel->height;

        pgmvi->GMVI_ulXResVirtual = plcddev->disp_panel->width;
        pgmvi->GMVI_ulYResVirtual = plcddev->disp_panel->height;

        pgmvi->GMVI_ulXOffset = 0;
        pgmvi->GMVI_ulYOffset = 0;

    	/*
    	 * lcd 使用 24bits 显示，每个像素点使用 4 个字节，其中最后一个字节为空。
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
	__PLCD_DEV  plcddev = (__PLCD_DEV)pgmdev;

    if (!plcddev) {
        return  (PX_ERROR);
    }

	/*
	 * imx6q 中使用lcd调试，默认使用7 寸触摸屏
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
** 函数名称: lcdDevCreate
** 功能描述: 创建 LCD 设备 (fb0)
** 输　入  : ucpPwrDev:  LCD 设备电源控制管脚相应设备名称    如果是 GPIO 需要编写对应的字符设备驱动
**           ucpBlkDev:  LCD 设备背光灯控制管脚相应设备名称  背光灯控制一般会使用 PWM 以便调节亮度
**           disp_panel: 参数控制器结构体
** 输　出  : 错误代码.
** 全局变量:
** 调用模块:
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
     *  仅支持 framebuffer 模式.
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
