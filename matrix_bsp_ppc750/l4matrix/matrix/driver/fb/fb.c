/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: fb.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2016 �� 11 �� 04 ��
**
** ��        ��: DUMMY Framebuffer ����Դ�ļ�
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "Matrix.h"
/*********************************************************************************************************
  FB ����
*********************************************************************************************************/
#define FB_WIDTH            (800)
#define FB_HEIGHT           (600)
#define FB_BPP              (16)
#define FB_BASE             (_G_fbDev.FB_ulFbAddr)
#define FB_NAME             "/dev/fb0"
/*********************************************************************************************************
  FB �豸�ṹ
*********************************************************************************************************/
typedef struct {
    LW_GM_DEVICE            FB_gmdev;
    LW_GM_FILEOPERATIONS    FB_gmfo;
    addr_t                  FB_ulFbAddr;
} FB_DEV;

static FB_DEV   _G_fbDev;
/*********************************************************************************************************
** ��������: __fbOpen
** ��������: �� FB
** �䡡��  : pgmdev        fb �豸
**           iFlag         ���豸��־
**           iMode         ���豸ģʽ
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __fbOpen (PLW_GM_DEVICE  pgmdev, INT  iFlag, INT  iMode)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __fbClose
** ��������: FB �رպ���
** �䡡��  : pgmdev        fb �豸
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __fbClose (PLW_GM_DEVICE  pgmdev)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __fbGetVarInfo
** ��������: ��� FB �ɱ���Ϣ
** �䡡��  : pgmdev        fb �豸
**           pgmvi         fb �ɱ���Ϣ
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __fbGetScrInfo
** ��������: ��� FB ��Ļ��Ϣ
** �䡡��  : pgmdev        fb �豸
**           pgmsi         fb ��Ļ��Ϣ
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: fbDevCreate
** ��������: ���� FB �豸
** �䡡��  : NONE
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  fbDevCreate (VOID)
{
    /*
     *  ��֧�� framebuffer ģʽ.
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
