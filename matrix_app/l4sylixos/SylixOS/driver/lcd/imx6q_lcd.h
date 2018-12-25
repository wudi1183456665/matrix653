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
** ��   ��   ��: imx6q_lcd.h
**
** ��   ��   ��: XuGuizhou(�����)
**
** �ļ���������: 2016 �� 11 �� 05 ��
**
** ��        ��: IMX6Q ������ LCD ����
*********************************************************************************************************/
#ifndef IMX6Q_LCD_H_
#define IMX6Q_LCD_H_
/*********************************************************************************************************
  LCD �������
*********************************************************************************************************/
typedef enum {
    QVGA = 0,
    WVGA,
} IMX6Q_LCD_PANEL_TYPE;
/*********************************************************************************************************
  LCD ��� shade
*********************************************************************************************************/
typedef enum  {
    MONOCHROME = 0,
    COLOR_ACTIVE,
    COLOR_PASSIVE,
} IMX6Q_LCD_PANEL_SHADE;
/*********************************************************************************************************
  ���ݼ���ģʽ
*********************************************************************************************************/
typedef enum {
    LOAD_DATA = 1,
    LOAD_PALETTE,
} IMX6Q_LCD_RASTER_LOAD_MODE;
/*********************************************************************************************************
  LCD ��嶨��
*********************************************************************************************************/
typedef struct {
    const CHAR                  acName[25];                             /*  Full name <vendor>_<model>  */
    INT                         iRefresh;
    INT                         iWidth;
    INT                         iHeight;
    INT                         iHfp;                                   /*  Horizontal front porch      */
    INT                         iHbp;                                   /*  Horizontal back porch       */
    INT                         iHsw;                                   /*  Horizontal Sync Pulse Width */
    INT                         iVfp;                                   /*  Vertical front porch        */
    INT                         iVbp;                                   /*  Vertical back porch         */
    INT                         iVsw;                                   /*  Vertical Sync Pulse Width   */
    INT                         iPixFmt;                                /*  Pixel format                */
    INT                         iBitsPerPix;                            /*  Bits per Pixel              */
    UINT                        uiPixelClk;                             /*  Pixel clock                 */
    BOOL                        bInterLace;                             /*  Inter Lace                  */
    BOOL                        bInvertPixelClk;                        /*  Invert Pixel clock          */
} IMX6Q_LCD_PANEL, *PIMX6Q_LCD_PANEL;
/*********************************************************************************************************
  ��ʾ���
*********************************************************************************************************/
typedef struct {
    IMX6Q_LCD_PANEL_TYPE            iPanelTtype;
    INT                             iMaxBpp;
    INT                             iMinBpp;
    IMX6Q_LCD_PANEL_SHADE           iPanelShade;
} IMX6Q_LCD_DISPLAY_PANEL, *PIMX6Q_LCD_DISPLAY_PANEL;
/*********************************************************************************************************
  LCD ��������
*********************************************************************************************************/
typedef struct {
    const IMX6Q_LCD_DISPLAY_PANEL *pDisplayPanel;

    /* AC Bias Pin Frequency */
    INT                             iAcBias;

    /* AC Bias Pin Transitions per Interrupt */
    INT                             iAcBiasInterrupt;

    /* DMA burst size */
    INT                             iDmaBurstSize;

    /* Bits per pixel */
    INT                             iBitsPerPixel;

    /* FIFO DMA Request Delay */
    INT                             iFifoDmaDelay;

    /* TFT Alternative Signal Mapping (Only for active) */
    BOOL                            bTftAltMode;

    /* 12 Bit Per Pixel (5-6-5) Mode (Only for passive) */
    BOOL                            bStn565Mode;

    /* Mono 8-bit Mode: 1=D0-D7 or 0=D0-D3 */
    BOOL                            bMono8bitMode;

    /* Invert line clock */
    BOOL                            bInvertLineClock;

    /* Invert frame clock  */
    BOOL                            bInvertFrameClock;

    /* Horizontal and Vertical Sync Edge: 0=rising 1=falling */
    BOOL                            bSyncEdge;

    /* Horizontal and Vertical Sync: Control: 0=ignore */
    BOOL                            bSyncCtrl;

    /* Raster Data Order Select: 1=Most-to-least 0=Least-to-most */
    BOOL                            bRasterOrder;

    /* DMA FIFO threshold */
    INT                             iFifoThreshold;
} IMX6Q_LCD_CTRL_CONFIG, *PIMX6Q_LCD_CTRL_CONFIG;
/*********************************************************************************************************
  LCD ��������
*********************************************************************************************************/
typedef struct {
    const CHAR                      acMenuName[10];
    PIMX6Q_LCD_CTRL_CONFIG          pCtrlConfig;
    const CHAR                      acType[25];
    UINT                            uiIpuChan;
    VOID                            (*pfuncPanelPowerCtrl)(INT);
} IMX6Q_LCD_DATA, *PIMX6Q_LCD_DATA;
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
INT  lcdDevCreate(CPCHAR  cpcName, PVOIDFUNCPTR  pPinMux, PIMX6Q_LCD_DATA  pData);

#endif                                                                  /* IMX6Q_LCD_H_                */
/*********************************************************************************************************
  END
*********************************************************************************************************/
