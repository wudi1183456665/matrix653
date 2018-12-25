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
** ��   ��   ��: imx6q_lcd.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2015 �� 01 �� 08 ��
**
** ��        ��: IMX6Q ������ LCD ����
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include "string.h"
#include "bsp/bspMap.h"

#include "linux/compat.h"
#include "imx6q_lcd.h"
/*********************************************************************************************************
  LCD ���������Ͷ���
*********************************************************************************************************/
typedef struct {
    addr_t                      LCDC_ulPhyAddrBase;                     /*  �����ַ����ַ              */
    size_t                      LCDC_stPhyAddrSize;                     /*  �����ַ�ռ��С            */
    ULONG                       LCDC_ulVector;                          /*  �ж�����                    */
    UINT                        LCDC_uiIntPriority;                     /*  �ж����ȼ�                  */
    BOOL                        LCDC_bDoubleFrameBufferEn;              /*  �Ƿ�ʹ��˫������            */
    BOOL                        LCDC_bInterruptEn;                      /*  �Ƿ�ʹ���ж�                */
    CPCHAR                      LCDC_pcName;                            /*  �豸��                      */

    addr_t                      LCDC_ulVirtAddrBase;                    /*  �����ַ����ַ              */
    VOID                       *LCDC_pvLcdMemBase;                      /*  LCD �ڴ����ַ              */
    BOOL                        LCDC_bIsInit;                           /*  �Ƿ��Ѿ���ʼ��              */
    LW_GM_DEVICE                LCDC_gmDev;                             /*  ͼ���豸                    */
    PIMX6Q_LCD_PANEL            LCDC_pPanel;
    PIMX6Q_LCD_DATA             LCDC_pData;
    PIMX6Q_LCD_CTRL_CONFIG      LCDC_pCtrlConfig;
} __IMX6Q_LCD_CONTROLER, *__PIMX6Q_LCD_CONTROLER;
/*********************************************************************************************************
  LCD ������
*********************************************************************************************************/
static __IMX6Q_LCD_CONTROLER  _G_imx6qLcdControler = {
        .LCDC_ulPhyAddrBase     = 0,
        .LCDC_stPhyAddrSize     = LW_CFG_VMM_PAGE_SIZE,
        .LCDC_ulVector          = 0,
        .LCDC_uiIntPriority     = 0,
        .LCDC_bDoubleFrameBufferEn  = LW_FALSE,
        .LCDC_bInterruptEn      = LW_FALSE,
        .LCDC_pcName            = "/dev/fb0"
};
/*********************************************************************************************************
  ��֪�� LCD ���
*********************************************************************************************************/
static IMX6Q_LCD_PANEL  __G_imx6qKnownLcdPanels[] = {
    /* TN92 XGA TFT */
    [0] = {
        .acName = "TN92_XGA_TFT",
        .iRefresh       = 60,
        .iWidth         = 800,
        .iHeight        = 480,
        .iHfp           = 75,
        .iHbp           = 25,
        .iHsw           = 20,
        .iVfp           = 10,
        .iVbp           = 10,
        .iVsw           = 10,
        .iPixFmt        = 0,
        .iBitsPerPix    = 16,
        .uiPixelClk     = 27000000,
        .bInvertPixelClk = 1,
        .bInterLace     = 0,
    },
};
/*********************************************************************************************************
  FrameBuffer ��ض���
*********************************************************************************************************/
#define __LCD_XSIZE                     pLcdControler->LCDC_pPanel->iWidth
#define __LCD_YSIZE                     pLcdControler->LCDC_pPanel->iHeight
#define __LCD_BITS_PER_PIXEL            pLcdControler->LCDC_pPanel->iBitsPerPix
#define __LCD_BYTES_PER_PIXEL           (__LCD_BITS_PER_PIXEL / 8)

#define __LCD_MEM_BASE                  ((ULONG)pLcdControler->LCDC_pvLcdMemBase)
#define __LCD_FRAMEBUFFER_BASE          (__LCD_MEM_BASE)
#define __LCD_FRAMEBUFFER_SIZE          (__LCD_XSIZE * __LCD_YSIZE * __LCD_BYTES_PER_PIXEL)
#define __LCD_FRAMEBUFFER_END           (__LCD_FRAMEBUFFER_BASE + __LCD_FRAMEBUFFER_SIZE)
#define __LCD_FRAMEBUFFER_TOTAL_SIZE    (__LCD_FRAMEBUFFER_SIZE * 2)
/*********************************************************************************************************
  ����
*********************************************************************************************************/
#ifndef container_of
#define container_of(ptr, type, member)     ({\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);\
        (type *)( (char *)__mptr - offsetof(type, member) );})
#endif
/*********************************************************************************************************
** ��������: __imx6qLcdHwInit
** ��������: ��ʼ�� LCD Ӳ��
** �䡡��  : pLcdControler         LCD ������
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __imx6qLcdHwInit (__PIMX6Q_LCD_CONTROLER  pLcdControler)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __imx6qLcdIsr
** ��������: LCD �жϷ�������
** �䡡��  : pLcdControler         LCD ������
**           ulVector              �ж�����
** �䡡��  : �жϷ���ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static irqreturn_t  __imx6qLcdIsr (__PIMX6Q_LCD_CONTROLER  pLcdControler, ULONG  ulVector)
{
    /* TODO: */
    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
** ��������: __imx6qLcdOn
** ��������: �� LCD ������
** �䡡��  : pLcdControler         LCD ������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  __imx6qLcdOn (__PIMX6Q_LCD_CONTROLER  pLcdControler)
{
    /* TODO: */
}
/*********************************************************************************************************
** ��������: __imx6qLcdOff
** ��������: �ر� LCD ������
** �䡡��  : pLcdControler         LCD ������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  __imx6qLcdOff (__PIMX6Q_LCD_CONTROLER  pLcdControler)
{
    /* TODO: */
}
/*********************************************************************************************************
** ��������: __imx6qLcdInit
** ��������: ��ʼ�� LCD
** �䡡��  : pLcdControler         LCD ������
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __imx6qLcdInit (__PIMX6Q_LCD_CONTROLER  pLcdControler)
{
    PVOID          pvVirtAddr;

    if (!pLcdControler->LCDC_bIsInit) {

        if (pLcdControler->LCDC_ulPhyAddrBase) {
            pLcdControler->LCDC_ulVirtAddrBase = (addr_t)API_VmmIoRemapNocache(
                                                        (PVOID)pLcdControler->LCDC_ulPhyAddrBase,
                                                        pLcdControler->LCDC_stPhyAddrSize);
            if (!pLcdControler->LCDC_ulVirtAddrBase) {
                printk(KERN_ERR "__imx6qLcdInit(): failed to remap!\n");
                goto  __error_handle;
            }
        }

#if 0
        pLcdControler->LCDC_pvLcdMemBase = API_VmmPhyAllocAlign(__LCD_FRAMEBUFFER_TOTAL_SIZE,
                                                                LW_CFG_VMM_PAGE_SIZE,
                                                                LW_ZONE_ATTR_DMA);
#else
        /*
         * Warning: Should reserv mem for LCD framebuffer. see EMBEDSKY_BASEC
         * Use uboot to config LCD controllor
         */
        pLcdControler->LCDC_pvLcdMemBase = (PVOID)BSP_CFG_RAM_RSV_ADDR;
#endif

        if (pLcdControler->LCDC_pvLcdMemBase == LW_NULL) {
            printk(KERN_ERR "__imx6qLcdInit(): low vmm memory!\n");
            goto  __error_handle;
        }

        pvVirtAddr = API_VmmIoRemapNocache((PVOID)__LCD_MEM_BASE, __LCD_FRAMEBUFFER_TOTAL_SIZE);
        if (pvVirtAddr == LW_NULL) {
            printk(KERN_ERR "__imx6qLcdInit(): failed to remap lcd mem!\n");
            goto  __error_handle;
        }

        bzero((PVOID)((ULONG)pvVirtAddr), __LCD_FRAMEBUFFER_TOTAL_SIZE);

        API_VmmIoUnmap((PVOID)pvVirtAddr);
        pvVirtAddr = LW_NULL;

        if (__imx6qLcdHwInit(pLcdControler) != ERROR_NONE) {
            printk(KERN_ERR "__imx6qLcdInit(): failed to remap lcd mem!\n");
            goto  __error_handle;
        }


        pLcdControler->LCDC_bIsInit = LW_TRUE;
    }

    return  (ERROR_NONE);

    __error_handle:

    if (pvVirtAddr) {
        API_VmmIoUnmap(pvVirtAddr);
        pvVirtAddr = 0;
    }

    if (pLcdControler->LCDC_pvLcdMemBase) {
        API_VmmPhyFree(pLcdControler->LCDC_pvLcdMemBase);
        pLcdControler->LCDC_pvLcdMemBase = LW_NULL;
    }

    if (pLcdControler->LCDC_ulVirtAddrBase) {
        API_VmmIoUnmap((PVOID)pLcdControler->LCDC_ulVirtAddrBase);
        pLcdControler->LCDC_ulVirtAddrBase = 0;
    }

    return  (PX_ERROR);
}
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
static LW_GM_FILEOPERATIONS     _G_imx6qGmFileOper;
/*********************************************************************************************************
  ��������
*********************************************************************************************************/
static INT      __imx6qLcdOpen(PLW_GM_DEVICE  pGmDev, INT  iFlag, INT  iMode);
static INT      __imx6qLcdClose(PLW_GM_DEVICE  pGmDev);
static INT      __imx6qLcdGetVarInfo(PLW_GM_DEVICE  pGmDev, PLW_GM_VARINFO  pGmVi);
static INT      __imx6qLcdGetScrInfo(PLW_GM_DEVICE  pGmDev, PLW_GM_SCRINFO  pGmSi);
/*********************************************************************************************************
** ��������: __imx6qLcdOpen
** ��������: �� LCD �豸
** �䡡��  : pGmDev            ͼ���豸
**           iFlag             ��־
**           iMode             ģʽ
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __imx6qLcdOpen (PLW_GM_DEVICE  pGmDev, INT  iFlag, INT  iMode)
{
    __PIMX6Q_LCD_CONTROLER  pLcdControler  = container_of(pGmDev, __IMX6Q_LCD_CONTROLER, LCDC_gmDev);

    if (__imx6qLcdInit(pLcdControler) != ERROR_NONE) {
        printk(KERN_ERR "__imx6qLcdOpen(): failed to init!\n");
        return  (PX_ERROR);
    }

    if (pLcdControler->LCDC_bInterruptEn > 0) {
        API_InterVectorEnable(pLcdControler->LCDC_ulVector);
    }

    if (pLcdControler->LCDC_pData->pfuncPanelPowerCtrl) {
        pLcdControler->LCDC_pData->pfuncPanelPowerCtrl(LW_TRUE);
    }

    __imx6qLcdOn(pLcdControler);
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __imx6qLcdClose
** ��������: �ر� LCD �豸
** �䡡��  : pGmDev            ͼ���豸
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT      __imx6qLcdClose (PLW_GM_DEVICE  pGmDev)
{
    __PIMX6Q_LCD_CONTROLER  pLcdControler = container_of(pGmDev, __IMX6Q_LCD_CONTROLER, LCDC_gmDev);

    if (pLcdControler->LCDC_pData->pfuncPanelPowerCtrl) {
        pLcdControler->LCDC_pData->pfuncPanelPowerCtrl(LW_FALSE);
    }

    if (pLcdControler->LCDC_bInterruptEn > 0) {
        API_InterVectorDisable(pLcdControler->LCDC_ulVector);
    }

    __imx6qLcdOff(pLcdControler);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __imx6qLcdGetVarInfo
** ��������: ��� LCD �豸 LW_GM_VARINFO ����
** �䡡��  : pGmDev            ͼ���豸
**           pGmVi             LW_GM_VARINFO ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT      __imx6qLcdGetVarInfo (PLW_GM_DEVICE  pGmDev, PLW_GM_VARINFO  pGmVi)
{
    __PIMX6Q_LCD_CONTROLER  pLcdControler = container_of(pGmDev, __IMX6Q_LCD_CONTROLER, LCDC_gmDev);

    if (pGmVi) {
        pGmVi->GMVI_ulXRes              = __LCD_XSIZE;
        pGmVi->GMVI_ulYRes              = __LCD_YSIZE;

        pGmVi->GMVI_ulXResVirtual       = __LCD_XSIZE;
        pGmVi->GMVI_ulYResVirtual       = __LCD_YSIZE;

        pGmVi->GMVI_ulXOffset           = 0;
        pGmVi->GMVI_ulYOffset           = 0;

        pGmVi->GMVI_ulBitsPerPixel      = __LCD_BITS_PER_PIXEL;
        pGmVi->GMVI_ulBytesPerPixel     = __LCD_BYTES_PER_PIXEL;

        switch (__LCD_BITS_PER_PIXEL) {
        case 16:
            pGmVi->GMVI_ulGrayscale     = (1 << __LCD_BITS_PER_PIXEL);
            pGmVi->GMVI_ulRedMask       = 0xF800;
            pGmVi->GMVI_ulGreenMask     = 0x07E0;
            pGmVi->GMVI_ulBlueMask      = 0x001F;
            pGmVi->GMVI_ulTransMask     = 0;
            break;

        case 24:
            pGmVi->GMVI_ulGrayscale     = (1 << __LCD_BITS_PER_PIXEL);
            pGmVi->GMVI_ulRedMask       = 0xFF0000;
            pGmVi->GMVI_ulGreenMask     = 0x00FF00;
            pGmVi->GMVI_ulBlueMask      = 0x0000FF;
            pGmVi->GMVI_ulTransMask     = 0;
            break;

        case 32:
            pGmVi->GMVI_ulGrayscale     = (1 << __LCD_BITS_PER_PIXEL);
            pGmVi->GMVI_ulRedMask       = 0x00FF0000;
            pGmVi->GMVI_ulGreenMask     = 0x0000FF00;
            pGmVi->GMVI_ulBlueMask      = 0x000000FF;
            pGmVi->GMVI_ulTransMask     = 0xFF000000;
            break;

        default:
            return  (PX_ERROR);
        }

        pGmVi->GMVI_bHardwareAccelerate = LW_FALSE;
        pGmVi->GMVI_ulMode              = LW_GM_SET_MODE;
        pGmVi->GMVI_ulStatus            = 0;
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __imx6qLcdGetScrInfo
** ��������: ��� LCD �豸 PLW_GM_SCRINFO ����
** �䡡��  : pGmDev            ͼ���豸
**           pGmSi             PLW_GM_SCRINFO ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT      __imx6qLcdGetScrInfo (PLW_GM_DEVICE  pGmDev, PLW_GM_SCRINFO  pGmSi)
{
    __PIMX6Q_LCD_CONTROLER  pLcdControler = container_of(pGmDev, __IMX6Q_LCD_CONTROLER, LCDC_gmDev);

    if (pGmSi) {
        pGmSi->GMSI_pcName              = (PCHAR)pLcdControler->LCDC_pcName;
        pGmSi->GMSI_ulId                = 0;
        pGmSi->GMSI_stMemSize           = __LCD_FRAMEBUFFER_SIZE;
        pGmSi->GMSI_stMemSizePerLine    = __LCD_XSIZE * __LCD_BYTES_PER_PIXEL;
        pGmSi->GMSI_pcMem               = (caddr_t)__LCD_FRAMEBUFFER_BASE;
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: lcdDevCreate
** ��������: ���� LCD �豸
** �䡡��  : cpcName           �豸��
**           pPinMux           �ܽŸ���
**           pData             LCD ��������
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  lcdDevCreate (CPCHAR  cpcName, PVOIDFUNCPTR  pinConfig, PIMX6Q_LCD_DATA  pData)
{
    __PIMX6Q_LCD_CONTROLER  pLcdControler = &_G_imx6qLcdControler;
    INT                     i;

    if (cpcName == LW_NULL || pData == LW_NULL) {
        return  (PX_ERROR);
    }

    for (i = 0; i < sizeof(__G_imx6qKnownLcdPanels)/sizeof(__G_imx6qKnownLcdPanels[0]); i++) {
        if (strcmp(__G_imx6qKnownLcdPanels[i].acName, pData->acType) == 0) {
            break;
        }
    }

    if (i == sizeof(__G_imx6qKnownLcdPanels)/sizeof(__G_imx6qKnownLcdPanels[0])) {
        _DebugFormat(__PRINTMESSAGE_LEVEL, "%s(): Panel not found!\r\n", __func__);
        return  (PX_ERROR);
    }

    if (pinConfig) {                                                    /*  config pin mux              */
        pinConfig();
    }

    /*
     *  ��֧�� framebuffer ģʽ.
     */
    _G_imx6qGmFileOper.GMFO_pfuncOpen        = __imx6qLcdOpen;
    _G_imx6qGmFileOper.GMFO_pfuncClose       = __imx6qLcdClose;
    _G_imx6qGmFileOper.GMFO_pfuncGetVarInfo  = (INT (*)(LONG, PLW_GM_VARINFO))__imx6qLcdGetVarInfo;
    _G_imx6qGmFileOper.GMFO_pfuncGetScrInfo  = (INT (*)(LONG, PLW_GM_SCRINFO))__imx6qLcdGetScrInfo;

    pLcdControler->LCDC_gmDev.GMDEV_gmfileop   = &_G_imx6qGmFileOper;
    pLcdControler->LCDC_gmDev.GMDEV_ulMapFlags = LW_VMM_FLAG_DMA;

    pLcdControler->LCDC_pcName      = cpcName;
    pLcdControler->LCDC_pData       = pData;
    pLcdControler->LCDC_pCtrlConfig = pData->pCtrlConfig;
    pLcdControler->LCDC_pPanel      = &__G_imx6qKnownLcdPanels[i];

    return  (gmemDevAdd(pLcdControler->LCDC_pcName, &pLcdControler->LCDC_gmDev));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
