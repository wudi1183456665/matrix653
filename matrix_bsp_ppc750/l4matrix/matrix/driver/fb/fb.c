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
** 文   件   名: fb.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2016 年 11 月 04 日
**
** 描        述: DUMMY Framebuffer 驱动源文件
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "Matrix.h"
/*********************************************************************************************************
  FB 配置
*********************************************************************************************************/
#define FB_WIDTH            (800)
#define FB_HEIGHT           (600)
#define FB_BPP              (16)
#define FB_BASE             (_G_fbDev.FB_ulFbAddr)
#define FB_NAME             "/dev/fb0"
/*********************************************************************************************************
  FB 设备结构
*********************************************************************************************************/
typedef struct {
    LW_GM_DEVICE            FB_gmdev;
    LW_GM_FILEOPERATIONS    FB_gmfo;
    addr_t                  FB_ulFbAddr;
} FB_DEV;

static FB_DEV   _G_fbDev;
/*********************************************************************************************************
** 函数名称: __fbOpen
** 功能描述: 打开 FB
** 输　入  : pgmdev        fb 设备
**           iFlag         打开设备标志
**           iMode         打开设备模式
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __fbOpen (PLW_GM_DEVICE  pgmdev, INT  iFlag, INT  iMode)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __fbClose
** 功能描述: FB 关闭函数
** 输　入  : pgmdev        fb 设备
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __fbClose (PLW_GM_DEVICE  pgmdev)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __fbGetVarInfo
** 功能描述: 获得 FB 可变信息
** 输　入  : pgmdev        fb 设备
**           pgmvi         fb 可变信息
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __fbGetVarInfo (PLW_GM_DEVICE  pgmdev, PLW_GM_VARINFO  pgmvi)
{
    if (pgmvi) {
        pgmvi->GMVI_ulXRes = FB_WIDTH;
        pgmvi->GMVI_ulYRes = FB_HEIGHT;

        pgmvi->GMVI_ulXResVirtual = FB_WIDTH;
        pgmvi->GMVI_ulYResVirtual = FB_HEIGHT;

        pgmvi->GMVI_ulXOffset = 0;
        pgmvi->GMVI_ulYOffset = 0;

        pgmvi->GMVI_ulBitsPerPixel  = FB_BPP;
        pgmvi->GMVI_ulBytesPerPixel = FB_BPP >> 3;
        pgmvi->GMVI_ulGrayscale     = 0;

        pgmvi->GMVI_ulRedMask   = 0xF800;
        pgmvi->GMVI_ulGreenMask = 0x07E0;
        pgmvi->GMVI_ulBlueMask  = 0x001F;
        pgmvi->GMVI_ulTransMask = 0x0000;

        pgmvi->GMVI_bHardwareAccelerate = LW_FALSE;
        pgmvi->GMVI_ulMode              = LW_GM_SET_MODE;
        pgmvi->GMVI_ulStatus            = 0;
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __fbGetScrInfo
** 功能描述: 获得 FB 屏幕信息
** 输　入  : pgmdev        fb 设备
**           pgmsi         fb 屏幕信息
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __fbGetScrInfo (PLW_GM_DEVICE  pgmdev, PLW_GM_SCRINFO  pgmsi)
{
    if (pgmsi) {
        pgmsi->GMSI_pcName           = FB_NAME;
        pgmsi->GMSI_ulId             = 0;
        pgmsi->GMSI_stMemSize        = FB_WIDTH * FB_HEIGHT * 2;
        pgmsi->GMSI_stMemSizePerLine = FB_WIDTH * 2;
        pgmsi->GMSI_pcMem            = (caddr_t)FB_BASE;
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: fbDevCreate
** 功能描述: 创建 FB 设备
** 输　入  : NONE
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  fbDevCreate (VOID)
{
    /*
     *  仅支持 framebuffer 模式.
     */
    _G_fbDev.FB_gmfo.GMFO_pfuncOpen       = __fbOpen;
    _G_fbDev.FB_gmfo.GMFO_pfuncClose      = __fbClose;
    _G_fbDev.FB_gmfo.GMFO_pfuncGetVarInfo = (INT (*)(LONG, PLW_GM_VARINFO))__fbGetVarInfo;
    _G_fbDev.FB_gmfo.GMFO_pfuncGetScrInfo = (INT (*)(LONG, PLW_GM_SCRINFO))__fbGetScrInfo;
    _G_fbDev.FB_gmdev.GMDEV_gmfileop      = &_G_fbDev.FB_gmfo;

    _G_fbDev.FB_ulFbAddr = (addr_t)API_VmmPhyAlloc(FB_WIDTH * FB_HEIGHT * 2);

    return  (gmemDevAdd(FB_NAME, &_G_fbDev.FB_gmdev));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
