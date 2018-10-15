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
** 文   件   名: imx6q_sylixos.c
**
** 创   建   人: acoinfo (翼辉)
**
** 文件创建日期: 2016年2月23日
**
** 描        述: 适配 SylixOS 官方开发板的 BSP 文件
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include <sys/gpiofd.h>
#include <mouse.h>
#include <linux/compat.h>
/*********************************************************************************************************
  驱动相关头文件
*********************************************************************************************************/
#include "sdk.h"
#include "ahci/ahciImx.h"
#include "uart/sio.h"
#include "wdt/imx6q_wdt.h"
#include "rtc/imx6q_rtc.h"
#include "i2c/imx6q_i2c.h"
#include "ecspi/ecspi.h"
#include "spi_nor/spi_nor.h"
#include "usb/imx6q_usb.h"
#include "netif/imx6q_netif.h"                                          /*  以太网的 BSP 头文件         */
#include "sdi/imx6q_sdi.h"                                              /*  SD 卡驱动的头文件           */
#include "hdmi/imx6q_hdmi.h"                                            /*  HDMI 驱动的头文件           */
#include "ipu_fb/imx6q_fb.h"                                            /*  LVDS/LCD 驱动的头文件       */
#include "usb/imx6q_usb.h"                                              /*  USB 接口代码                */
#include "gpio/imx6q_gpio.h"
#include "touch/touch.h"
#include "touch/chip/ft5x06/ft5x06.h"
#include "pwm/imx6q_pwm.h"
#include "pinmux/iomux_config.h"
#include "gpio/imx6q_gpio.h"
#include "tempmon/imx6q_tempmon.h"
#include "rtc_isl1208/rtc_isl1208.h"
#include "flexcan/flexcan.h"

//#include "sylixos_evm/sylixos_evm.h"
#include "../config.h"
/*********************************************************************************************************
  驱动相关头文件
*********************************************************************************************************/
#define __ALT0  0x0
#define __ALT1  0x1
#define __ALT2  0x2
#define __ALT3  0x3
#define __ALT4  0x4
#define __ALT5  0x5
#define __ALT6  0x6
#if 0 //l4
/*********************************************************************************************************
  FT5X06 电容屏数据
*********************************************************************************************************/
static TOUCH_DATA       _G_ft5x06Data = {
        .T_pcBusName    = "/bus/i2c/2",                                 /*  I2C 总线名称                */
        .T_uiIrq        = IMX6Q_GPIO_NUMR(6, 7),                        /*  IRQ 管脚号                  */
        .T_uiReset      = IMX6Q_GPIO_NUMR(2, 1),                        /*  Reset 管脚号                */
        .T_uiIrqCfg     = GPIO_FLAG_IN | GPIO_FLAG_TRIG_FALL,           /*  IRQ 中断模式                */
        .T_uiRstVal     = LW_GPIOF_INIT_LOW,                            /*  Reset 复位电平              */
        .T_usAddr       = 0x38,                                         /*  I2C 从机地址                */
        .T_iWidth       = 1024,                                         /*  分辨率 宽                   */
        .T_iHeight      = 600,                                          /*  分辨率 高                   */
        .T_iTouchNum    = 1,                                            /*  最大触摸点数                */
};

static TOUCH_DRV_FUNC   _G_Ft5x06DrvFunc = {
        .getevent       = ft5x06GetEvent,
        .init           = NULL,
        .deinit         = NULL,
        .reset          = NULL,
        .ioctl          = ft5x06Ioctl,
};

static INT              _G_touchDrvNum  = -1;
/*********************************************************************************************************
** 函数名称: boardTimeInit
** 功能描述: 初始化目标电路板时间系统,在bspInit.c文件的halTimeInit中调用
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID boardTimeInit (VOID)
{
    PLW_RTC_FUNCS   pRtcFuncs;
#if  RTC_IMX6Q_INSIDE
    pRtcFuncs = rtcGetFuncs();
#else
    pRtcFuncs = isl1208RtcGetFuncs("/bus/i2c/0");
#endif
    rtcDrv();
    rtcDevCreate(pRtcFuncs);                                            /*  创建硬件 RTC 设备           */
    rtcToSys();                                                         /*  将 RTC 时间同步到系统时间   */
}
#endif
/*********************************************************************************************************
** 函数名称: boardBusInit
** 功能描述: 初始化目标电路板总线系统,在bspInit.c文件的halBusInit中调用
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID boardBusInit (VOID)
{
#if 0
    INT              iErr;
    PLW_I2C_FUNCS    pI2cFuncs;
    PLW_SPI_FUNCS    pSpiFuncs;

    API_I2cLibInit();                                                   /*  初始化 i2c 组件库           */
    API_SpiLibInit();                                                   /*  初始化 spi 组件库           */

    pI2cFuncs = i2cBusFuns(0);                                          /*  创建 i2c总线适配器          */
    if (pI2cFuncs) {
        API_I2cAdapterCreate("/bus/i2c/0", pI2cFuncs, 10, 1);           /*  0 对应 I2C1                 */
    }
    pI2cFuncs = i2cBusFuns(1);                                          /*  创建 i2c总线适配器          */
    if (pI2cFuncs) {
        API_I2cAdapterCreate("/bus/i2c/1", pI2cFuncs, 10, 1);           /*  1 对应 I2C2                 */
    }
    pI2cFuncs = i2cBusFuns(2);                                          /*  创建 i2c总线适配器          */
    if (pI2cFuncs) {
        API_I2cAdapterCreate("/bus/i2c/2", pI2cFuncs, 10, 1);           /*  2 对应 I2C3                 */
    }

    pSpiFuncs = spiBusDrv(0, ecspi1_iomux_config);
    if (pSpiFuncs) {
        iErr = API_SpiAdapterCreate("/bus/spi/0", pSpiFuncs);           /*  创建 spi 总线适配器         */
        if (ERROR_NONE != iErr) {
            printf("[/bus/spi/0]create error!\n");
        }
    }

    pSpiFuncs= spiBusDrv(1, ecspi2_iomux_config);
    if (pSpiFuncs) {
        iErr = API_SpiAdapterCreate("/bus/spi/1", pSpiFuncs);           /*  创建 spi 总线适配器         */
        if (ERROR_NONE != iErr) {
            printf("[/bus/spi/1]create error!\n");
        }
    }
#endif
}
/*********************************************************************************************************
** 函数名称: boardDrvInit
** 功能描述: 初始化目标系统静态驱动程序。在bspInit.c文件的halDrvInit中调用
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID boardDrvInit (VOID)
{
    imx6qGpioDrv();
#if 0 //l4
    imx6qPwmDrv();
    imx6qWatchDogDrv();
    imx6qTempMonDrv();

    _G_touchDrvNum = touchDrv();
#endif
}
/*********************************************************************************************************
  lvds 显示参数设置结构体，将其中的lvds初始化函数删除了，需要注意
*********************************************************************************************************/
ips_dev_panel_t disp_lvds = {
     "HannStar XGA LVDS",                                               /* name                         */
     HannStar_XGA_LVDS,                                                 /* panel id flag                */
     DISP_DEV_LVDS,                                                     /* panel type                   */
     DCMAP_RGB666,                                                      /* data format for panel        */
     60,                                                                /* refresh rate                 */
     1024,                                                              /* panel width                  */
     600,                                                               /* panel height                 */
     65000000,                                                          /* pixel clock frequency        */
     220,                                                               /* hsync start width            */
     60,                                                                /* hsync width                  */
     40,                                                                /* hsyn back width              */
     21,                                                                /* vysnc start width            */
     10,                                                                /* vsync width                  */
     7,                                                                 /* vsync back width             */
     0,                                                                 /* delay from hsync to vsync    */
     0,                                                                 /* interlaced mode              */
     1,                                                                 /* clock selection, external    */
     1,                                                                 /* clock polarity               */
     1,                                                                 /* hsync polarity               */
     1,                                                                 /* vync polarity                */
     1,                                                                 /* drdy polarity                */
     0,                                                                 /* data polarity                */
     NULL,
     NULL,
 };
/*********************************************************************************************************
** 函数名称: boardDevInit
** 功能描述: 初始化目标系统静态驱动程序。在bspInit.c文件的halDevInit中调用
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID boardDevInit (VOID)
{
#if 0 //l4
    CAN_CHAN        *pcanchan;
	SIO_CHAN		*psio;

#define __TTY_RD_BUF_SIZE      256
#define __TTY_WR_BUF_SIZE      256

#if BSP_CFG_CLOSE_FB_EN
    imx6qPwmDevAdd(3);
#endif																	/*  #if BSP_CFG_CLOSE_FB_EN     */

    imx6qIpuDevCreate(IMX6Q_GPIO_NUMR(1, 21), "/dev/pwm3", &disp_lvds);
#endif

   /*
    * 在飞凌的OKMX6开发板中的CD、WP管脚有所不同
    * SD3: 不使用保护管脚,
    * SD4: 连接eMMC，无复位引脚，
    */
    imx6qSdLibInit();                                                   /*  初始化 SD 组件库            */

    imx6qSdiDrvInstall(3, GPIO_NONE, GPIO_NONE, LW_TRUE);
    //imx6qSdiDrvInstall(2, IMX6Q_GPIO_NUMR(4, 10), GPIO_NONE, LW_FALSE);

#if 0 //l4
#if BOARD_CFG_HDMI_EN
    imx6qHdmiCreate();
    imx6qHdmiDevCreate();
#endif																	/*  #if BOARD_CFG_HDMI_EN       */

    imx6qUsbPhyInit();                                                  /*  PHY 初始化，USB 驱动不开源  */

    imx6qTempMonDevAdd();

    /*
     * 添加CAN 设备
     */
#if BSP_CFG_CAN_EN
    pcanchan = flexcanChanCreate(0);                                    /*  对应 CAN 设备 1             */
    canDevCreate("/dev/can0", pcanchan, 64, 64);
    pcanchan = flexcanChanCreate(1);                                    /*  对应 CAN 设备 2             */
    canDevCreate("/dev/can1", pcanchan, 64, 64);
#endif																	/*  #if BSP_CFG_CAN_EN          */

#if BSP_CFG_YAFFS_EN
    yaffsDevCreate("/yaffs2");
#endif																	/*  #if BSP_CFG_YAFFS_EN        */
	
	/*
	 * 额外的串口设备
	 */
	psio = sioChanCreate(2);                                            /*  串口 2 通道 -- UART3        */
    if (psio) {
        ttyDevCreate("/dev/ttyS2", psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);
    }

    psio = sioChanCreate(3);                                            /*  串口 3 通道 -- UART4        */
    if (psio) {
        ttyDevCreate("/dev/ttyS3", psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);
    }

    psio = sioChanCreate(4);                                            /*  串口 4 通道 -- UART5        */
    if (psio) {
        ttyDevCreate("/dev/ttyS4", psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);
    }

#if BSP_CFG_YAFFS_EN
    norDevCreate("/bus/spi/0", IMX6Q_GPIO_NUMR(2, 30));
    spiNorDrvInstall("/flash", 1 * 1024 * 1024);
#endif                                                                  /*  #if BSP_CFG_YAFFS_EN        */

    if (_G_touchDrvNum > 0) {
        touchDevCreate("/dev/input/touch0", _G_touchDrvNum, &_G_ft5x06Data, &_G_Ft5x06DrvFunc);
    }

#if BSP_CFG_HDD_EN
    imxAhciDevInit();                                                   /*  SATA 控制器初始化           */
#endif                                                                  /*  #if BSP_CFG_HDD_EN          */
#endif
}

/*********************************************************************************************************
** 函数名称: boardNetifAttch
** 功能描述: 网络接口驱动程序 在bspInit.c文件的halNetifAttch中调用
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID boardNetifAttch (VOID)
{
    /*
     *  PHY 芯片的复位信号管脚为 ENET_CRS_DV(GPIO1_25), 在本驱动中借用了 uboot 的初始化, 因此不要对 PHY
     *  进行 硬复位
     */
#if 0
    API_GpioRequestOne(IMX6Q_GPIO_NUMR(1, 25), LW_GPIOF_OUT_INIT_LOW, "phy_rst");
    API_GpioSetValue(IMX6Q_GPIO_NUMR(1, 25), 0);
    udelay(100000);
    API_GpioSetValue(IMX6Q_GPIO_NUMR(1, 25), 1);
    udelay(100000);
#endif

	enetInit();
}
/*********************************************************************************************************
 针对 SylixOS 官方开发板电路板使能能够使用的 GPIO
*********************************************************************************************************/
static  UINT32  _G_uiGpioFilter[] = {
   IMX6Q_GPIO_NUMR(4, 10),                                              /*  SD3_CD                      */
   IMX6Q_GPIO_NUMR(7, 8),                                               /*  SD3_WP                      */
   IMX6Q_GPIO_NUMR(1, 25),                                              /*  以太网 PHY 芯片复位管脚     */
   IMX6Q_GPIO_NUMR(1, 21),                                              /*  LCD电源开关                 */
   IMX6Q_GPIO_NUMR(6, 7),                                               /*  触摸屏中断控制管脚          */
   IMX6Q_GPIO_NUMR(2, 1),                                               /*  触摸屏复位控制管脚          */
   IMX6Q_GPIO_NUMR(6, 9),                                               /*  电路板按键 Key1             */
   IMX6Q_GPIO_NUMR(2, 4),                                               /*  电路板按键 Key2             */
   IMX6Q_GPIO_NUMR(1, 4),                                               /*  电路板按键 Key3             */
   IMX6Q_GPIO_NUMR(2, 6),                                               /*  电路板按键 Key4             */
   IMX6Q_GPIO_NUMR(6, 15),                                              /*  电路板按键 Key5             */
   IMX6Q_GPIO_NUMR(2, 7),                                               /*  电路板按键 Key6             */
   IMX6Q_GPIO_NUMR(6, 10),                                              /*  电路板按键 Key7             */
   IMX6Q_GPIO_NUMR(2, 0),                                               /*  电路板按键 LED1             */
   IMX6Q_GPIO_NUMR(2, 5),                                               /*  电路板按键 LED2             */
   IMX6Q_GPIO_NUMR(1, 27),                                              /*  电路板按键 LED3             */
   IMX6Q_GPIO_NUMR(1, 26),                                              /*  电路板按键 LED4             */
   IMX6Q_GPIO_NUMR(2, 3),                                               /*  电路板按键 LED5             */
   IMX6Q_GPIO_NUMR(1, 29),                                              /*  电路板按键 LED6             */
   IMX6Q_GPIO_NUMR(1, 30),                                              /*  电路板按键 LED7             */
   IMX6Q_GPIO_NUMR(1, 24),                                              /*  电路板按键 LED8             */
   IMX6Q_GPIO_NUMR(7, 12),                                              /*  ZLG7290 芯片控制管脚 - INT  */
   IMX6Q_GPIO_NUMR(7, 13),                                              /*  ZLG7290 芯片控制管脚 - RST  */
   IMX6Q_GPIO_NUMR(1, 16),                                              /*  摄像头的电源使能管脚        */
   IMX6Q_GPIO_NUMR(2, 30),                                              /*  SPI1 CS0                    */
   IMX6Q_GPIO_NUMR(2, 26),                                              /*  SPI2 CS0                    */
};
/*********************************************************************************************************
** 函数名称: boardGpioCheck
** 功能描述: 检查要求打开的 GPIO 是否合法, 若编号不合法, 则返回错误, 避免打开不合适的 GPIO 管脚
**           (这里合法的GPIO指的是仅可用作输入/输出/中断功能的GPIO)
** 输  入  : uiOffset    GPIO 在系统中的编号
** 输  出  : 返回值      合法返回 ERROR_NONE, 不合法返回 PX_ERROR
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  boardGpioCheck (UINT32  uiOffset)
{
   INT     i;

   for (i = 0; i < (sizeof(_G_uiGpioFilter) / sizeof(_G_uiGpioFilter[0])); i++) {
       if (uiOffset == _G_uiGpioFilter[i]) {
           return  (ERROR_NONE);
       }
   }

   return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: boardSdQurikInfo
** 功能描述: 获得每个SD通道板级相关的信息(主要是总线位宽等, 如eMMC使用8位数据的情况)
** 输  入  : iChannel   通道编号
** 输  出  : 返回值     SD 通道信息
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  boardSdQurikInfo (INT iChannel)
{
    INT  iRet = 0;

    switch (iChannel) {

    case 0:
        iRet = SDHCI_QUIRK_FLG_CANNOT_SDIO_INT;                         /*  用于SD卡/SDIO卡             */
        break;
    case 1:
        iRet = SDHCI_QUIRK_FLG_CANNOT_SDIO_INT;
        break;
    case 2:
        iRet = SDHCI_QUIRK_FLG_CAN_DATA_8BIT;                           /*  用于8位eMMC                 */
        break;
    case 3:
        iRet = SDHCI_QUIRK_FLG_CAN_DATA_8BIT;
        break;

    default:
        break;
    }

    return  (iRet);
}
/*********************************************************************************************************
 以下是目标电路板的 pinmux 设置
*********************************************************************************************************/
VOID ccm_iomux_config (VOID)
{
    /*
     * Config ccm.CCM_CLKO1 to pad GPIO00(T5)
     * SGTL5000 - sys_mclk
     * HW_IOMUXC_SW_MUX_CTL_PAD_GPIO00_WR(0x00000000);
     * HW_IOMUXC_SW_PAD_CTL_PAD_GPIO00_WR(0x0001B0B0);
     */
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_GPIO00_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_GPIO00_ADDR);

    /*
     * Config ccm.CCM_CLKO2 to pad GPIO03(R7)
     * J5 - Camera mclk
     * HW_IOMUXC_SW_MUX_CTL_PAD_GPIO03_WR(0x00000004);
     * HW_IOMUXC_SW_PAD_CTL_PAD_GPIO03_WR(0x0001B0B0);
     */
    writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_GPIO03_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_GPIO03_ADDR);

}
VOID sjc_iomux_config (VOID)
{
	// Function to configure IOMUXC for JTAG module.
    // Config sjc.JTAG_MOD to pad JTAG_MOD(H6)
    // HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_MOD_WR(0x0000B060);
    writel(0x0000B060, HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_MOD_ADDR);

    // Config sjc.JTAG_TCK to pad JTAG_TCK(H5)
    // HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TCK_WR(0x00007060);
    writel(0x00007060, HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TCK_ADDR);

    // Config sjc.JTAG_TDI to pad JTAG_TDI(G5)
    // HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDI_WR(0x00007060);
    writel(0x00007060, HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDI_ADDR);

    // Config sjc.JTAG_TDO to pad JTAG_TDO(G6)
    // HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDO_WR(0x000090B1);
    writel(0x000090B1, HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDO_ADDR);

    // Config sjc.JTAG_TMS to pad JTAG_TMS(C3)
    // HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TMS_WR(0x00007060);
    writel(0x00007060, HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TMS_ADDR);

    // Config sjc.JTAG_TRSTB to pad JTAG_TRSTB(C2)
    // HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TRSTB_WR(0x00007060);
    writel(0x00007060, HW_IOMUXC_SW_PAD_CTL_PAD_JTAG_TRSTB_ADDR);
}
VOID usb_iomux_config (VOID)
{
	// Function to configure IOMUXC for usb module.
    /* Config usb.USB_H1_OC to pad EIM_DATA30(J20)
     * HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30_WR(0x00000006);
     * HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_WR(0x0001B0B0);  所有的io设置均相同，即为芯片复位默认设置
     * HW_IOMUXC_USB_H1_OC_SELECT_INPUT_WR(0x00000000);
     */
    writel(0x00000006, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA30_ADDR);
    writel(0x00000000, HW_IOMUXC_USB_H1_OC_SELECT_INPUT_ADDR);

    // Config usb.USB_OTG_OC to pad KEY_COL4(T6)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_WR(0x0001B0B0);
    // HW_IOMUXC_USB_OTG_OC_SELECT_INPUT_WR(0x00000000);
    writel(0x00000002, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_ADDR);
    writel(0x00000000, HW_IOMUXC_USB_OTG_OC_SELECT_INPUT_ADDR);

    // Config usb.USB_OTG_PWR to pad EIM_DATA22(E23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA22_WR(0x00000004);
    // HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA22_WR(0x0001B0B0);
    writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA22_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA22_ADDR);

}
/*!
 * HDMI pin mux and internal connection mux
 * be noted that the HDMI is drivern by the IPU1 di0 here
 */
VOID hdmi_pgm_iomux (VOID)
{
   /*
    * config instance hdmi_tx of Module HDMI_TX to Protocol CEC port including CEC_LINE
    */
   writel(0x00000006, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR25_ADDR);        /* EIM_A25 pad (alt6)           */
   writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_ADDR25_ADDR);        /* SLOW                         */
   writel(0x00000000, HW_IOMUXC_HDMI_ICECIN_SELECT_INPUT_ADDR);         /* EIM_ADDR25_ALT6              */

   /*!
    * config instance hdmi_tx of Module HDMI_TX to Protocol DDC ports including DDC_SCL, DDC_SDA.
    */
	/*
	 * I2C总线初始化时已经配置
	 */
//   writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_ADDR);          /*  KEY_COL3 (alt2)             */
//   writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_ADDR);          /*  SLOW                        */
//   writel(0x00000001, HW_IOMUXC_HDMI_II2C_CLKIN_SELECT_INPUT_ADDR);     /*  KEY_COL3_ALT2               */
//
//   writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_ADDR);          /*  KEY_ROW3 (alt2)             */
//   writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_ADDR);          /*  SLOW                        */
//   writel(0x00000001, HW_IOMUXC_HDMI_II2C_DATAIN_SELECT_INPUT_ADDR);    /*  KEY_ROW3_ALT2               */

}
/*********************************************************************************************************
** 函数名称: lcdif_iomux_config
** 功能描述: 参考Linux3.0.35的board-mx6q_sabresd.h实现对其管脚功能的分配
** 输　入  : NONE
** 输　出  : 错误代码.
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID lcdif_iomux_config(VOID)
{
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA00_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA00_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA01_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA01_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA02_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA02_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA03_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA03_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA04_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA04_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA05_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA05_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA06_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA06_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA07_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA07_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA08_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA08_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA09_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA09_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA10_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA10_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA11_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA11_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA12_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA12_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA13_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA13_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA14_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA14_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA15_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA15_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA16_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA16_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA17_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA17_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA18_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA18_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA19_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA19_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA20_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA20_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA21_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA22_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA22_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA23_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA23_ADDR);

    /*
     * 以下管脚为连接 TFT LCD 的多余信号 均为 ALT0 模式
     */
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_DISP_CLK_ADDR);     /*  设置DISP0_CLK   的功能选择  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_DISP_CLK_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN15_ADDR);        /*  设置DISP0_DE =IPU1_DI0_PIN15*/
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN15_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN02_ADDR);        /*  设置DISP0_HSYNCH 的功能选择 */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN02_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN03_ADDR);        /*  设置DISP0_VSYNCH 的功能选择 */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN03_ADDR);

    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN04_ADDR);        /*  设置contrast 的功能选择     */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN04_ADDR);

    API_GpioRequestOne(IMX6Q_GPIO_NUMR(1, 21), LW_GPIOF_OUT_INIT_LOW, "lcd_pwr");
}
/*
 * 分配串口的管脚 --电路板 SabreLite
 */
VOID uart1_iomux_config(VOID)
{
    // Config uart1.UART1_RX_DATA to pad SD3_DAT6(M3)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_WR(0x0001B0B0);
    // HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(0x00000001);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6_ADDR);        /*  设置管脚功能选择            */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA6_ADDR);        /*  设置管脚电气特性            */
    writel(0x00000003, HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_ADDR); /*  设置串口接收管脚            */

    // Config uart1.UART1_TX_DATA to pad SD3_DAT7(M1)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_WR(0x0001B0B0);
    // HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_WR(0x00000001);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7_ADDR);        /*  设置管脚功能选择            */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA7_ADDR);        /*  设置管脚电气特性            */
    writel(0x00000003, HW_IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT_ADDR); /*  设置串口接收管脚            */
}

// Function to configure IOMUXC for uart2 module.  --电路板 SabreLite
VOID uart2_iomux_config(VOID)
{
    // Config uart2.UART2_RX_DATA to pad EIM_DATA27(R5)
    // HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27_WR(0x00000004);
    // HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_WR(0x0001B0B0);
    // HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(0x00000003);
    writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27_ADDR);       /*  设置管脚功能选择            */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27_ADDR);       /*  设置管脚电气特性            */
    writel(0x00000001, HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_ADDR); /*  设置串口接收管脚            */
    // Config uart2.UART2_TX_DATA to pad EIM_DATA26(R3)
    // HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26_WR(0x00000004);
    // HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_WR(0x0001B0B0);
    // HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_WR(0x00000003);
    writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26_ADDR);       /*  设置管脚功能选择            */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA26_ADDR);       /*  设置管脚电气特性            */
    writel(0x00000001, HW_IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT_ADDR); /*  设置串口接收管脚            */
}

VOID uart3_iomux_config(VOID)
{
    // Config uart3.UART3_RX_DATA to pad SD4_CLK(E16)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CLK_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_WR(0x0001B0B0);
    // HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_WR(0x00000003);
    writel(0x00000002, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA25_ADDR);
    writel(0x00000001, HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_ADDR);
    // Config uart3.UART3_TX_DATA to pad SD4_CMD(B17)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CMD_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_WR(0x0001B0B0);
    // HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_WR(0x00000003);
    writel(0x00000002, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA24_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA24_ADDR);
    writel(0x00000001, HW_IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT_ADDR);
}
VOID uart4_iomux_config(VOID)
{
	/*
	 * 非常奇怪芯片的管脚功能设置为什么要如此处理
	 */
    /* Config uart4.UART4_RX_DATA to pad KEY_ROW0(V6)
     * HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_WR(0x00000004);
     * HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_WR(0x0001B0B0); 所有io设置均相同为默认设置，不进行此步骤设置
     * HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_WR(0x00000001);
    */
    writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_ADDR);
    writel(0x00000001, HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_ADDR);

    /* Config uart4.UART4_TX_DATA to pad KEY_COL0(W5)
     * HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_WR(0x00000004);
     * HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_WR(0x0001B0B0);
     * HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_WR(0x00000001);
     */
    writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_ADDR);
    writel(0x00000001, HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_ADDR);

}
VOID uart5_iomux_config(VOID)
{
    /* Config uart4.UART4_RX_DATA to pad KEY_ROW0(V6)
     * HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_WR(0x00000004);
     * HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW0_WR(0x0001B0B0); 所有的io设置均相同且为默认设置，不进行此步骤设置
     * HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_WR(0x00000001);
    */
    writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW1_ADDR);
    writel(0x00000001, HW_IOMUXC_UART5_UART_RX_DATA_SELECT_INPUT_ADDR);

    /* Config uart4.UART4_TX_DATA to pad KEY_COL0(W5)
     * HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_WR(0x00000004);
     * HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL0_WR(0x0001B0B0);
     * HW_IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT_WR(0x00000001);
     */
    writel(0x00000004, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL1_ADDR);
    writel(0x00000001, HW_IOMUXC_UART5_UART_RX_DATA_SELECT_INPUT_ADDR);

}
VOID enet_iomux_reconfig(VOID)
{
    // Config enet.RGMII_RD0 to pad RGMII_RD0(C24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD0_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD0_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_DATA0_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD0_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD0_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_DATA0_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RD1 to pad RGMII_RD1(B23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD1_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD1_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_DATA1_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD1_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD1_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_DATA1_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RD2 to pad RGMII_RD2(B24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD2_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD2_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_DATA2_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD2_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD2_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_DATA2_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RD3 to pad RGMII_RD3(D23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD3_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD3_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_DATA3_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD3_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD3_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_DATA3_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RXC to pad RGMII_RXC(B25)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RXC_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RXC_WR(0x00013030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_CLK_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RXC_ADDR);
    writel(0x00013030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RXC_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_CLK_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RX_CTL to pad RGMII_RX_CTL(D22)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RX_CTL_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RX_CTL_WR(0x00013030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_EN_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RX_CTL_ADDR);
    writel(0x00013030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RX_CTL_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_EN_SELECT_INPUT_ADDR);

}


// Function to configure IOMUXC for enet module.
VOID enet_iomux_config(VOID)
{
    // Config enet.ENET_MDC to pad ENET_MDC(V20)
    // HW_IOMUXC_SW_MUX_CTL_PAD_ENET_MDC_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_ENET_MDC_WR(0x0001B0B0);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_ENET_MDC_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_ENET_MDC_ADDR);

    // Config enet.ENET_MDIO to pad ENET_MDIO(V23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_ENET_MDIO_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_ENET_MDIO_WR(0x0001B0B0);
    // HW_IOMUXC_ENET_MAC0_MDIO_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_ENET_MDIO_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_ENET_MDIO_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_MDIO_SELECT_INPUT_ADDR);

#if 1 // configure following  pads as GPIOs

    // Config enet.RGMII_RD0 to pad RGMII_RD0(C24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD0_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD0_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_DATA0_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD0_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD0_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_DATA0_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RD1 to pad RGMII_RD1(B23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD1_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD1_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_DATA1_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD1_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD1_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_DATA1_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RD2 to pad RGMII_RD2(B24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD2_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD2_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_DATA2_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD2_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD2_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_DATA2_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RD3 to pad RGMII_RD3(D23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD3_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD3_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_DATA3_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD3_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RD3_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_DATA3_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RXC to pad RGMII_RXC(B25)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RXC_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RXC_WR(0x00013030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_CLK_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RXC_ADDR);
    writel(0x00013030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RXC_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_CLK_SELECT_INPUT_ADDR);

    // Config enet.RGMII_RX_CTL to pad RGMII_RX_CTL(D22)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RX_CTL_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RX_CTL_WR(0x00013030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    // HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_WR(0x00000000);
    // HW_IOMUXC_ENET_MAC0_RX_EN_SELECT_INPUT_WR(0x00000000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RX_CTL_ADDR);
    writel(0x00013030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_RX_CTL_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
    writel(0x00000000, HW_IOMUXC_SW_PAD_CTL_GRP_RGMII_TERM_ADDR);
    writel(0x00000000, HW_IOMUXC_ENET_MAC0_RX_EN_SELECT_INPUT_ADDR);


#endif
    // Config enet.RGMII_TD0 to pad RGMII_TD0(C22)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD0_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD0_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD0_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD0_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);

    // Config enet.RGMII_TD1 to pad RGMII_TD1(F20)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD1_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD1_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD1_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD1_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);

    // Config enet.RGMII_TD2 to pad RGMII_TD2(E21)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD2_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD2_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD2_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD2_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);

    // Config enet.RGMII_TD3 to pad RGMII_TD3(A24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD3_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD3_WR(0x0001B030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD3_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TD3_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);

    // Config enet.RGMII_TXC to pad RGMII_TXC(D21)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TXC_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TXC_WR(0x00013030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TXC_ADDR);
    writel(0x00013030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TXC_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);

    // Config enet.RGMII_TX_CTL to pad RGMII_TX_CTL(C23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TX_CTL_WR(0x00000001);
    // HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TX_CTL_WR(0x00013030);
    // HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_WR(0x00080000);
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TX_CTL_ADDR);
    writel(0x00013030, HW_IOMUXC_SW_PAD_CTL_PAD_RGMII_TX_CTL_ADDR);
    writel(0x00080000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);

    // configure ENET_REF_CLK pad as ENET_TX_CLK.
    // done by hand, so please find description in Reference Manual, rev.1; par.36.4.113; page 2049
    writel(0x00000001, HW_IOMUXC_SW_MUX_CTL_PAD_ENET_REF_CLK_ADDR);
    writel(0x0001B030, HW_IOMUXC_SW_PAD_CTL_PAD_ENET_REF_CLK_ADDR);
    writel(0x000c0000, HW_IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE_RGMII_ADDR);
}


/*
 * ! From obds ENET iomux config
 */
VOID imx_enet_iomux(VOID)
{
    enet_iomux_config();        // iomux tool output

    API_GpioRequestOne(IMX6Q_GPIO_NUMR(3, 23), LW_GPIOF_OUT_INIT_LOW, "netio1");
    API_GpioRequestOne(IMX6Q_GPIO_NUMR(6, 30), LW_GPIOF_OUT_INIT_HIGH, "netio2");
    API_GpioRequestOne(IMX6Q_GPIO_NUMR(6, 25), LW_GPIOF_OUT_INIT_HIGH, "netio3");
    API_GpioRequestOne(IMX6Q_GPIO_NUMR(6, 27), LW_GPIOF_OUT_INIT_HIGH, "netio4");
    API_GpioRequestOne(IMX6Q_GPIO_NUMR(6, 28), LW_GPIOF_OUT_INIT_HIGH, "netio5");
    API_GpioRequestOne(IMX6Q_GPIO_NUMR(6, 29), LW_GPIOF_OUT_INIT_HIGH, "netio6");
    API_GpioRequestOne(IMX6Q_GPIO_NUMR(6, 24), LW_GPIOF_OUT_INIT_HIGH, "netio7");

}

// Function to configure IOMUXC for flexcan1 module.
//VOID flexcan1_iomux_config(VOID)
//{
//    // Config flexcan1.FLEXCAN1_RX to pad KEY_ROW2(W4)
//    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_WR(0x00000002);
//    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_WR(0x0001B0B0);
//    // HW_IOMUXC_FLEXCAN1_RX_SELECT_INPUT_WR(0x00000000);
//    writel(0x00000002, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_ADDR);
//    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW2_ADDR);
//    writel(0x00000000, HW_IOMUXC_FLEXCAN1_RX_SELECT_INPUT_ADDR);
//
//    // Config flexcan1.FLEXCAN1_TX to pad KEY_COL2(W6)
//    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL2_WR(0x00000002);
//    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_WR(0x0001B0B0);
//    writel(0x00000002, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL2_ADDR);
//    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL2_ADDR);
//
//
//}

/*
 * 根据E9的开发板进行CAN的管脚分配
 */
VOID flexcan1_iomux_config(VOID)
{
    // Config flexcan1.FLEXCAN1_RX to pad GPIO08(W4)
    writel(0x00000003, HW_IOMUXC_SW_MUX_CTL_PAD_GPIO08_ADDR);    /* ALT3  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_GPIO08_ADDR);
    writel(0x00000001, HW_IOMUXC_FLEXCAN1_RX_SELECT_INPUT_ADDR);

    // Config flexcan1.FLEXCAN1_TX to pad GPIO07(W4)
    writel(0x00000003, HW_IOMUXC_SW_MUX_CTL_PAD_GPIO07_ADDR);    /* ALT3  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_GPIO07_ADDR);

}

// Function to configure IOMUXC for flexcan1 module.
VOID flexcan2_iomux_config(VOID)
{
    // Config flexcan1.FLEXCAN1_RX to pad KEY_ROW4(W4)
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW4_ADDR);  /*  ALT0  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW4_ADDR);
    writel(0x00000000, HW_IOMUXC_FLEXCAN2_RX_SELECT_INPUT_ADDR);

    // Config flexcan1.FLEXCAN1_TX to pad KEY_COL4(W6)
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_ADDR);  /*  ALT0  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL4_ADDR);

}
/*********************************************************************************************************
** 函数名称: ecspi1_iomux_config
** 功能描述: 初始化 spi1管脚分配
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID ecspi1_iomux_config (VOID)
{
    writel(__ALT1, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16_ADDR);           /*  CSPI1_CLK                   */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA16_ADDR);
    writel(0x0, HW_IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT_ADDR);

    writel(__ALT1, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17_ADDR);           /*  CSPI1_MISO                  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA17_ADDR);
    writel(0x0, HW_IOMUXC_ECSPI1_MISO_SELECT_INPUT_ADDR);

    writel(__ALT1, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18_ADDR);           /*  CSPI1_MOSI                  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_DATA18_ADDR);
    writel(0x0, HW_IOMUXC_ECSPI1_MOSI_SELECT_INPUT_ADDR);

    writel(__ALT5, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_ADDR);              /*  CSPI1_CS0  (GPIO 2_30)      */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_EB2_ADDR);

    API_GpioRequest(IMX6Q_GPIO_NUMR(2, 30), "spi1_cs0");
}
/*********************************************************************************************************
** 函数名称: ecspi2_iomux_config
** 功能描述: 初始化 spi2管脚分配
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID ecspi2_iomux_config (VOID)
{
    writel(__ALT2, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_CS0_ADDR);              /*  CSPI2_CLK                   */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_CS0_ADDR);
    writel(0x0, HW_IOMUXC_ECSPI2_CSPI_CLK_IN_SELECT_INPUT_ADDR);

    writel(__ALT2, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_OE_ADDR);               /*  CSPI2_MISO                  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_OE_ADDR);
    writel(0x0, HW_IOMUXC_ECSPI2_MISO_SELECT_INPUT_ADDR);

    writel(__ALT2, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_CS1_ADDR);              /*  CSPI2_MOSI                  */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_CS1_ADDR);
    writel(0x0, HW_IOMUXC_ECSPI2_MOSI_SELECT_INPUT_ADDR);

    writel(__ALT5, HW_IOMUXC_SW_MUX_CTL_PAD_EIM_RW_ADDR);               /*  CSPI2_CS0  (GPIO 2_26)      */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_EIM_RW_ADDR);

    API_GpioRequest(IMX6Q_GPIO_NUMR(2, 26), "spi2_cs0");

}

// Function to configure IOMUXC for i2c1 module.
VOID i2c1_iomux_config(VOID)
{
    // Config i2c1.I2C1_SCL to pad CSI0_DATA09_ALT4
    writel(0x00000014, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09_ADDR);
    writel(0x0001b8b1, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA09_ADDR);
    writel(0x00000001, HW_IOMUXC_I2C1_SCL_IN_SELECT_INPUT_ADDR);

    // Config i2c1.I2C1_SDA to pad CSI0_DATA08_ALT4
    writel(0x00000014, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08_ADDR);
    writel(0x0001b8b1, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA08_ADDR);
    writel(0x00000001, HW_IOMUXC_I2C1_SDA_IN_SELECT_INPUT_ADDR);
}

// Function to configure IOMUXC for i2c2 module.
VOID i2c2_iomux_config(VOID)
{
    // Config i2c2.I2C2_SCL to pad KEY_COL3(U5)
    // Camera, MIPI
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_WR(0x00000014);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_WR(0x0001B0B0);
    // HW_IOMUXC_I2C2_SCL_IN_SELECT_INPUT_WR(0x00000001);
    writel(0x00000014, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_COL3_ADDR);
    writel(0x00000001, HW_IOMUXC_I2C2_SCL_IN_SELECT_INPUT_ADDR);

    // Config i2c2.I2C2_SDA to pad KEY_ROW3(T7)
    // HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_WR(0x00000014);
    // HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_WR(0x0001B0B0);
    // HW_IOMUXC_I2C2_SDA_IN_SELECT_INPUT_WR(0x00000001);
    writel(0x00000014, HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3_ADDR);
    writel(0x00000001, HW_IOMUXC_I2C2_SDA_IN_SELECT_INPUT_ADDR);
}

// Function to configure IOMUXC for i2c3 module.
VOID i2c3_iomux_config(VOID)
{
    // Config i2c3.I2C3_SCL to pad GPIO05(R4)  OKMX6 is GPIO03
    // J7 - display card
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO05_WR(0x00000016);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO05_WR(0x0001B0B0);
    // HW_IOMUXC_I2C3_SCL_IN_SELECT_INPUT_WR(0x00000002);
    writel(0x00000012, HW_IOMUXC_SW_MUX_CTL_PAD_GPIO03_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_GPIO03_ADDR);
    writel(0x00000001, HW_IOMUXC_I2C3_SCL_IN_SELECT_INPUT_ADDR);

    // Config i2c3.I2C3_SDA to pad GPIO16(R2)  OKMX6 is GPIO06
    // J15 - RGB
    // HW_IOMUXC_SW_MUX_CTL_PAD_GPIO16_WR(0x00000016);
    // HW_IOMUXC_SW_PAD_CTL_PAD_GPIO16_WR(0x0001B0B0);
    // HW_IOMUXC_I2C3_SDA_IN_SELECT_INPUT_WR(0x00000002);
    writel(0x00000012, HW_IOMUXC_SW_MUX_CTL_PAD_GPIO06_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_GPIO06_ADDR);
    writel(0x00000001, HW_IOMUXC_I2C3_SDA_IN_SELECT_INPUT_ADDR);

}

VOID pwm1_iomux_config(VOID)
{
    // Config pwm1.PWM1_OUT to pad SD1_DATA3(F18)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA3_WR(0x0001B0B0);
    writel(0x00000003, HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA3_ADDR);
}

VOID pwm2_iomux_config(VOID)
{
    // Config pwm2.PWM2_OUT to pad SD1_DATA2(E19)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2_WR(0x00000003);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA2_WR(0x0001B0B0);
//    writel(0x00000003, HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2_ADDR);
//    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA2_ADDR);
}

VOID pwm3_iomux_config(VOID)
{
    // Config pwm3.PWM3_OUT to pad SD1_DATA1(C20)
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA1_WR(0x0001B0B0);
//    writel(0x00000002, HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1_ADDR);
//    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA1_ADDR);
}

VOID pwm4_iomux_config(VOID)
{
    // Config pwm4.PWM4_OUT to pad SD1_CMD(B21)
    // J6 - LVDS GP
    // HW_IOMUXC_SW_MUX_CTL_PAD_SD1_CMD_WR(0x00000002);
    // HW_IOMUXC_SW_PAD_CTL_PAD_SD1_CMD_WR(0x0001B0B0);
    writel(0x00000002, HW_IOMUXC_SW_MUX_CTL_PAD_SD1_CMD_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD1_CMD_ADDR);
}

/*
 * E9 卡片电脑使用 SD2 作为SD卡控制器，选项同样为ALT0
 *
 */
VOID usdhc2_iomux_config(VOID)
{
    /* -----------------管脚功能选择----------------------------
     *
     * 由于所有的引脚配置几乎都是一样的，因此这里可以使用固定数值0x0替代，
     */
    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD2_CLK_ADDR);

    /* -----------------管脚电气特性设置------------------------
     *
     * 由于所有的引脚配置几乎都是一样的，因此这里可以使用固定数值0x0001B0B0替代,
     * 这些配置就是芯片复位后的默认配置,即在实际使用中只需要进行功能选择即可,
     * 此语句设置可以不使用
     */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD2_CLK_ADDR);


    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD2_CMD_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD2_CMD_ADDR);


    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA0_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA0_ADDR);

    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA1_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA1_ADDR);

    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA2_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA2_ADDR);

    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA3_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA3_ADDR);
}


VOID usdhc3_iomux_config(VOID)
{
    /* -----------------管脚功能选择----------------------------
     *
     * 由于所有的引脚配置几乎都是一样的，因此这里可以使用固定数值0x0替代，
     */
    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CLK_ADDR);

    /* -----------------管脚电气特性设置------------------------
     *
     * 由于所有的引脚配置几乎都是一样的，因此这里可以使用固定数值0x0001B0B0替代,
     * 这些配置就是芯片复位后的默认配置,即在实际使用中只需要进行功能选择即可,
     * 此语句设置可以不使用
     */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD3_CLK_ADDR);


    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CMD_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD3_CMD_ADDR);

    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA0_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA0_ADDR);

    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA1_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA1_ADDR);

    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA2_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA2_ADDR);

    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA3_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD3_DATA3_ADDR);

}

VOID usdhc4_iomux_config(VOID)
{
    /* -----------------管脚功能选择----------------------------
     *
     * 由于所有的引脚配置几乎都是一样的，因此这里可以使用固定数值0x0替代，
     */
    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CLK_ADDR);

    /* -----------------管脚电气特性设置------------------------
     *
     * 由于所有的引脚配置几乎都是一样的，因此这里可以使用固定数值0x0001B0B0替代,
     * 这些配置就是芯片复位后的默认配置,即在实际使用中只需要进行功能选择即可,
     * 此语句设置可以不使用
     */
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CLK_ADDR);

    writel(0x0, HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CMD_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD4_CMD_ADDR);

    writel(0x1, HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA0_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA0_ADDR);

    writel(0x1, HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA1_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA1_ADDR);


    writel(0x1, HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA2_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA2_ADDR);

    writel(0x1, HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA3_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_SD4_DATA3_ADDR);

}
// Function to configure IOMUXC for ipu1 module.
VOID ipu1_iomux_config (VOID)
{
    // Config ipu1.IPU1_CSI0_DATA08 to pad CSI0_DATA08(N6)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA08_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA08_ADDR);

    // Config ipu1.IPU1_CSI0_DATA09 to pad CSI0_DATA09(N5)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA09_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA09_ADDR);

    // Config ipu1.IPU1_CSI0_DATA10 to pad CSI0_DATA10(M1)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA10_ADDR);

    // Config ipu1.IPU1_CSI0_DATA11 to pad CSI0_DATA11(M3)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA11_ADDR);

    // Config ipu1.IPU1_CSI0_DATA12 to pad CSI0_DATA12(M2)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA12_ADDR);

    // Config ipu1.IPU1_CSI0_DATA13 to pad CSI0_DATA13(L1)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA13_ADDR);

    // Config ipu1.IPU1_CSI0_DATA14 to pad CSI0_DATA14(M4)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA14_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA14_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA14_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA14_ADDR);

    // Config ipu1.IPU1_CSI0_DATA15 to pad CSI0_DATA15(M5)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA15_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA15_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA15_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA15_ADDR);

    // Config ipu1.IPU1_CSI0_DATA16 to pad CSI0_DATA16(L4)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA16_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA16_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA16_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA16_ADDR);

    // Config ipu1.IPU1_CSI0_DATA17 to pad CSI0_DATA17(L3)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA17_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA17_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA17_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA17_ADDR);

    // Config ipu1.IPU1_CSI0_DATA18 to pad CSI0_DATA18(M6)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA18_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA18_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA18_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA18_ADDR);

    // Config ipu1.IPU1_CSI0_DATA19 to pad CSI0_DATA19(L6)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA19_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA19_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA19_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA19_ADDR);

    // Config ipu1.IPU1_CSI0_DATA_EN to pad CSI0_DATA_EN(P3)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA_EN_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA_EN_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA_EN_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_DATA_EN_ADDR);

    // Config ipu1.IPU1_CSI0_HSYNC to pad CSI0_HSYNC(P4)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_HSYNC_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_HSYNC_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_HSYNC_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_HSYNC_ADDR);

    // Config ipu1.IPU1_CSI0_PIXCLK to pad CSI0_PIXCLK(P1)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_PIXCLK_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_PIXCLK_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_PIXCLK_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_PIXCLK_ADDR);

    // Config ipu1.IPU1_CSI0_VSYNC to pad CSI0_VSYNC(N2)
    // HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_VSYNC_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_VSYNC_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_VSYNC_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_CSI0_VSYNC_ADDR);

    // Config ipu1.IPU1_DI0_DISP_CLK to pad DI0_DISP_CLK(N19)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DI0_DISP_CLK_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DI0_DISP_CLK_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_DISP_CLK_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_DISP_CLK_ADDR);

    // Config ipu1.IPU1_DI0_PIN02 to pad DI0_PIN02(N25)
    // HSync
    // HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN02_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN02_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN02_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN02_ADDR);

    // Config ipu1.IPU1_DI0_PIN03 to pad DI0_PIN03(N20)
    // VSync
    // HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN03_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN03_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN03_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN03_ADDR);

    // Config ipu1.IPU1_DI0_PIN04 to pad DI0_PIN04(P25)
    // Contrast
    // HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN04_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN04_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN04_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN04_ADDR);

    // Config ipu1.IPU1_DI0_PIN15 to pad DI0_PIN15(N21)
    // DE
    // HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN15_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN15_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN15_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DI0_PIN15_ADDR);

    // Config ipu1.IPU1_DISP0_DATA00 to pad DISP0_DATA00(P24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA00_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA00_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA00_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA00_ADDR);

    // Config ipu1.IPU1_DISP0_DATA01 to pad DISP0_DATA01(P22)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA01_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA01_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA01_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA01_ADDR);

    // Config ipu1.IPU1_DISP0_DATA02 to pad DISP0_DATA02(P23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA02_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA02_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA02_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA02_ADDR);

    // Config ipu1.IPU1_DISP0_DATA03 to pad DISP0_DATA03(P21)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA03_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA03_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA03_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA03_ADDR);

    // Config ipu1.IPU1_DISP0_DATA04 to pad DISP0_DATA04(P20)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA04_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA04_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA04_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA04_ADDR);

    // Config ipu1.IPU1_DISP0_DATA05 to pad DISP0_DATA05(R25)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA05_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA05_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA05_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA05_ADDR);

    // Config ipu1.IPU1_DISP0_DATA06 to pad DISP0_DATA06(R23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA06_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA06_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA06_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA06_ADDR);

    // Config ipu1.IPU1_DISP0_DATA07 to pad DISP0_DATA07(R24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA07_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA07_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA07_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA07_ADDR);

    // Config ipu1.IPU1_DISP0_DATA08 to pad DISP0_DATA08(R22)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA08_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA08_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA08_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA08_ADDR);

    // Config ipu1.IPU1_DISP0_DATA09 to pad DISP0_DATA09(T25)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA09_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA09_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA09_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA09_ADDR);

    // Config ipu1.IPU1_DISP0_DATA10 to pad DISP0_DATA10(R21)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA10_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA10_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA10_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA10_ADDR);

    // Config ipu1.IPU1_DISP0_DATA11 to pad DISP0_DATA11(T23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA11_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA11_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA11_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA11_ADDR);

    // Config ipu1.IPU1_DISP0_DATA12 to pad DISP0_DATA12(T24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA12_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA12_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA12_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA12_ADDR);

    // Config ipu1.IPU1_DISP0_DATA13 to pad DISP0_DATA13(R20)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA13_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA13_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA13_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA13_ADDR);

    // Config ipu1.IPU1_DISP0_DATA14 to pad DISP0_DATA14(U25)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA14_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA14_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA14_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA14_ADDR);

    // Config ipu1.IPU1_DISP0_DATA15 to pad DISP0_DATA15(T22)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA15_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA15_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA15_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA15_ADDR);

    // Config ipu1.IPU1_DISP0_DATA16 to pad DISP0_DATA16(T21)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA16_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA16_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA16_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA16_ADDR);

    // Config ipu1.IPU1_DISP0_DATA17 to pad DISP0_DATA17(U24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA17_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA17_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA17_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA17_ADDR);

    // Config ipu1.IPU1_DISP0_DATA18 to pad DISP0_DATA18(V25)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA18_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA18_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA18_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA18_ADDR);

    // Config ipu1.IPU1_DISP0_DATA19 to pad DISP0_DATA19(U23)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA19_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA19_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA19_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA19_ADDR);

    // Config ipu1.IPU1_DISP0_DATA20 to pad DISP0_DATA20(U22)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA20_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA20_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA20_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA20_ADDR);

    // Config ipu1.IPU1_DISP0_DATA21 to pad DISP0_DATA21(T20)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA21_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA21_ADDR);

    // Config ipu1.IPU1_DISP0_DATA22 to pad DISP0_DATA22(V24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA22_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA22_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA22_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA22_ADDR);

    // Config ipu1.IPU1_DISP0_DATA23 to pad DISP0_DATA23(W24)
    // HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA23_WR(0x00000000);
    // HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA23_WR(0x0001B0B0);
    writel(0x00000000, HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA23_ADDR);
    writel(0x0001B0B0, HW_IOMUXC_SW_PAD_CTL_PAD_DISP0_DATA23_ADDR);

}
void ldb_iomux_config()
{
	/*
	 * 该函数不需要做任何工作
	 */
};

/*********************************************************************************************************
  END
*********************************************************************************************************/
