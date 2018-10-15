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
** 文   件   名: imx6q_sdi.c
**
** 创   建   人: Zeng.Bo(曾波)
**
** 文件创建日期: 2015 年 03 月 02 日
**
** 描        述: I.MX6Q 处理器的 SD 卡控制器驱动
** BUG:
*********************************************************************************************************/
#define __SYLIXOS_IO
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"

#include "sdk.h"
#include "regsusdhc.h"
#include "regsccm.h"
#include "sdi/imx6q_sdi.h"
#include "gpio/imx6q_gpio.h"
/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#define SDI_CHAN_NUM                (4)                                 /*  配置使用的通道数量          */

#define SDI_CHAN0_BASE              (USDHC1_BASE_ADDR)                  /*  uSDHC 控制器基地址          */
#define SDI_CHAN1_BASE              (USDHC2_BASE_ADDR)
#define SDI_CHAN2_BASE              (USDHC3_BASE_ADDR)
#define SDI_CHAN3_BASE              (USDHC4_BASE_ADDR)

#define SDI_CHAN0_INT               (IMX_INT_USDHC1)
#define SDI_CHAN1_INT               (IMX_INT_USDHC2)
#define SDI_CHAN2_INT               (IMX_INT_USDHC3)
#define SDI_CHAN3_INT               (IMX_INT_USDHC4)

#define SDHCI_MIX_CTRL              (0x48)
#define SDHCI_MIX_CTRL_DDREN        (1 << 3)
#define SDHCI_PORT_CTRL             (0x28)
#define SDHCI_LITTLE_ENDIAN_MODE    (0x00000010)
#define SDHCI_DEFAULT_SETTING       (0x00000012)

#define SDI_CHAN0_QUIRK_FLG         boardSdQurikInfo(0)
#define SDI_CHAN1_QUIRK_FLG         boardSdQurikInfo(1)
#define SDI_CHAN2_QUIRK_FLG         boardSdQurikInfo(2)
#define SDI_CHAN3_QUIRK_FLG         boardSdQurikInfo(3)

#define SDI_CTL2                    (0x80)
#define SDI_CTL3                    (0x84)
#define SDI_CTL4                    (0x8c)
/*********************************************************************************************************
  SD 扩展控制寄存器 (SDI_CTL2)
*********************************************************************************************************/
#define SDI_CTL2_HWINITFIN          (1 << 0)                            /*  SHCI Hardware Init Finish   */
#define SDI_CTL2_ENCLKOUTMSKCON     (1 << 1)                            /*  SDCLK output clock masking  */
#define SDI_CTL2_PWRSYNC            (1 << 3)                            /*  SD OP Power Sync Support    */
#define SDI_CTL2_SELBASECLK         (2 << 4)                            /*  Base Clock Source Select    */
#define SDI_CTL2_DISBUFRD           (1 << 6)                            /*  Buffer Read Disable         */
#define SDI_CTL2_RWAITMODE          (1 << 7)                            /*  Read Wait Release Control   */
#define SDI_CTL2_ENCLKOUTHOLD       (1 << 8)                            /*  SDCLK Hold Enable           */
#define SDI_CTL2_DFCNT              (3 << 9)                            /*  Debounce Filter Count       */
#define SDI_CTL2_ENBUSYCHKTXSTART   (1 << 11)                           /*  CE-ATA I/F mode             */
#define SDI_CTL2_SDSIGPC            (1 << 12)                           /*  Signal Power Control Sup    */
#define SDI_CTL2_SDCDSEL            (1 << 13)                           /*  Detect Signal Selection     */
#define SDI_CTL2_ENFBCLKRX          (1 << 14)                           /*  Feedback Clock Enable RX    */
#define SDI_CTL2_ENFBCLKTX          (1 << 15)                           /*  Feedback Clock Enable TX    */
#define SDI_CTL2_SELCARDOUT         (1 << 28)                           /*  Card RM Condition Select    */
#define SDI_CTL2_ENCMDCNFMSK        (1 << 30)                           /*  Command Conflict Mask En    */
#define SDI_CTL2_ENSTAASYNCCLR      (1 << 31)                           /*  WRT StatClR Async Mode En   */
#define SDI_CTL2_CFG                (SDI_CTL2_SELBASECLK   |\
                                     SDI_CTL2_ENCLKOUTHOLD |\
                                     SDI_CTL2_ENFBCLKRX    |\
                                     SDI_CTL2_ENFBCLKTX    |\
                                     SDI_CTL2_ENCMDCNFMSK  |\
                                     SDI_CTL2_ENSTAASYNCCLR)
/*********************************************************************************************************
  SD 扩展控制寄存器(SDI_CTL3)
*********************************************************************************************************/
#define SDI_CTL3_FCSEL0             (1 << 7)                            /*  Feedback Clock Select [0]   */
#define SDI_CTL3_FCSEL1             (1 << 15)                           /*  Feedback Clock Select [1]   */
#define SDI_CTL3_FCSEL2             (1 << 23)                           /*  Feedback Clock Select [2]   */
#define SDI_CTL3_FCSEL3             (1 << 31)                           /*  Feedback Clock Select [3]   */
#define SDI_CTL3_CFG                (SDI_CTL3_FCSEL0 |\
                                     SDI_CTL3_FCSEL1 |\
                                     SDI_CTL3_FCSEL2 |\
                                     SDI_CTL3_FCSEL3)
/*********************************************************************************************************
  SD 扩展控制寄存器(SDI_CTL4)
*********************************************************************************************************/
#define SDI_CTL4_SELCLKPADDS        (3 << 16)                           /*  SD Clock Output PAD(9mA)    */
                                                                        /*  Drive Strength Select       */
#define SDI_CTL4_CFG                (SDI_CTL4_SELCLKPADDS)

#define ESDHC_IDENT_DVS             (0x8)
#define ESDHC_IDENT_SDCLKFS         (0x20)
#define ESDHC_OPERT_DVS             (0x3)
#define ESDHC_OPERT_SDCLKFS         (0x1)
#define ESDHC_HS_DVS                (0x1)
#define ESDHC_HS_SDCLKFS            (0x1)
#define ESDHC_SYSCTL_DTOCV_VAL      (0x0000000E)
#define ESDHC_SYSCTL_FREQ_MASK      (BM_USDHC_SYS_CTRL_DVS     |\
                                     BM_USDHC_SYS_CTRL_SDCLKFS |\
                                     BM_USDHC_SYS_CTRL_DTOCV)
/*********************************************************************************************************
  引导设备(eMMC)相关, 通常使用标准分区的形式给bootloader预留空间, 此时就无需保留分区.
  如有特殊需要可根据实际情况设置
*********************************************************************************************************/
#define EMMC_RESERVE_SECTOR         0
/*********************************************************************************************************
  IMX.6 系统信息相关
*********************************************************************************************************/
#define ANATOP_BASE_ADDR            (AIPS1_OFF_BASE_ADDR + 0x48000)
#define MX6_IO_ADDRESS(x)           ((VOID *)(x))

#define MX6_USB_ANALOG_DIGPROG      (0x260)
#define MX6SL_USB_ANALOG_DIGPROG    (0x280)

#define IMX_CHIP_REVISION_1_0       (0x10)
#define IMX_CHIP_REVISION_1_1       (0x11)
#define IMX_CHIP_REVISION_1_2       (0x12)
#define IMX_CHIP_REVISION_1_3       (0x13)
#define IMX_CHIP_REVISION_2_0       (0x20)
#define IMX_CHIP_REVISION_2_1       (0x21)
#define IMX_CHIP_REVISION_2_2       (0x22)
#define IMX_CHIP_REVISION_2_3       (0x23)
#define IMX_CHIP_REVISION_3_0       (0x30)
#define IMX_CHIP_REVISION_3_1       (0x31)
#define IMX_CHIP_REVISION_3_2       (0x32)
#define IMX_CHIP_REVISION_3_3       (0x33)
#define IMX_CHIP_REVISION_UNKNOWN   (0xff)

#define cpu_is_mx6()                LW_TRUE
#define cpu_is_mx6q()               LW_TRUE
#define cpu_is_mx6sl()              LW_FALSE
#define cpu_is_mx6dl()              LW_FALSE
/*********************************************************************************************************
  SDHCI 频率定义
*********************************************************************************************************/
typedef enum {
    OPERATING_FREQ,
    IDENTIFICATION_FREQ,
    HS_FREQ
} SDHC_FREQ_T;
/*********************************************************************************************************
  通道 描述
*********************************************************************************************************/
typedef struct {
    UINT32                  SDICH_uiChannel;                            /*  SD 卡通道编号               */
    CPCHAR                  SDICH_cpcHostName;
    INT                     SDICH_iCardSta;                             /*  当前设备插入/拔出状态       */
    LW_SDHCI_HOST_ATTR      SDICH_sdhciattr;
    PVOID                   SDICH_pvSdhciHost;                          /*  创建的 SDHCI 对象           */
    PVOID                   SDICH_pvSdmHost;                            /*  对应的 SDM HOST 对象        */

    UINT32                  SDICH_uiPad;                                /*  SD 卡寄存器访问特殊用途数据 */
    UINT32                  SDICH_uiSdiCd;                              /*  SD 的 CD 管脚               */
    UINT32                  SDICH_uiSdiWp;                              /*  SD 的 WP 管脚               */
} __SDI_CHANNEL;
/*********************************************************************************************************
  全局变量及函数声明
*********************************************************************************************************/
static __SDI_CHANNEL        _G_sdichannelTbl[SDI_CHAN_NUM];
static SDHCI_QUIRK_OP       _G_sdiquirkop = {LW_NULL, };
/*********************************************************************************************************
  函数声明
*********************************************************************************************************/
static INT    __imx6SdhciIoDrvInit(PLW_SDHCI_HOST_ATTR  psdhcihostattr);
static VOID   __imx6SdHostCfgClk(UINT32 uiInstance, SDHC_FREQ_T eFreq);
static INT    __imx6QuirkClockSet(PLW_SDHCI_HOST_ATTR  psdhcihostattr, UINT32 uiClock);
static BOOL   __imx6QuirkIsCardWp(PLW_SDHCI_HOST_ATTR     psdhcihostattr);
static INT    __imx6ChipVersionGet(VOID);
static INT    __imx6qChipVersionCheck(VOID);

static UINT32 __imx6SdhciReadL(PLW_SDHCI_HOST_ATTR  psdhcihostattr, ULONG ulAddr);
static UINT16 __imx6SdhciReadW(PLW_SDHCI_HOST_ATTR  psdhcihostattr, ULONG ulAddr);
static UINT8  __imx6SdhciReadB(PLW_SDHCI_HOST_ATTR  psdhcihostattr, ULONG ulAddr);
static VOID   __imx6SdhciWriteL(PLW_SDHCI_HOST_ATTR psdhcihostattr, ULONG ulAddr, UINT32 uiLword);
static VOID   __imx6SdhciWriteW(PLW_SDHCI_HOST_ATTR psdhcihostattr, ULONG ulAddr, UINT16 usWord);
static VOID   __imx6SdhciWriteB(PLW_SDHCI_HOST_ATTR psdhcihostattr, ULONG ulAddr, UINT8 ucByte);
/*********************************************************************************************************
** 函数名称: sdiClockInit
** 功能描述: SD 控制器时钟，对其时钟的介绍在67.4.5章节有说明，在CCM Clock Tree图片中显示SD的
**           时钟是PLL2输出时钟经过2分频。提供给uSDHCn控制器。Table 8-4 显示的pll2，的常用设置是528M，
**           uSDHC常用时钟为198M，时钟源为PFD400，
**
** 输　入  : iChannel
** 输　出  : 返回时钟频率 (0 表示失败)
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static UINT32 __sdiClockInit (INT  iChannel)
{
    /*
     * 根据裸机测试，时钟设置不需要更改系统的时钟频率，这里修改 SD 的协议设置。
     */
    return  (200000000);                                                /*  uboot 设置的内容为200Mhz    */
}
/*********************************************************************************************************
** 函数名称: __sdiChannelDataInit
** 功能描述: SD 控制器参数初始化
** 输    入: iChannel      通道号
** 输    出: NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __sdiChannelDataInit (INT  iChannel)
{
    __SDI_CHANNEL       *psdichannel;
    LW_SDHCI_HOST_ATTR  *psdhciattr;
    UINT32               uiClkRate;

    if ((iChannel < 0) || (iChannel >= SDI_CHAN_NUM)) {
        return  (PX_ERROR);
    }

    /*
     * 使用内部的时钟设置方法
     */
    if (!_G_sdiquirkop.SDHCIQOP_pfuncClockSet) {
        _G_sdiquirkop.SDHCIQOP_pfuncClockSet = __imx6QuirkClockSet;
        _G_sdiquirkop.SDHCIQOP_pfuncIsCardWp = __imx6QuirkIsCardWp;
    }

    uiClkRate = __sdiClockInit(iChannel);
    if (uiClkRate == 0) {
        return  (PX_ERROR);
    }

    psdichannel = &_G_sdichannelTbl[iChannel];
    psdichannel->SDICH_uiChannel = iChannel;
    psdhciattr  = &psdichannel->SDICH_sdhciattr;

    psdhciattr->SDHCIHOST_pvUsrSpec = (VOID *)psdichannel;

    switch (iChannel) {
    case 0:
        return  (PX_ERROR);

    case 1:                                                             /*  对应 SD2                    */
        psdichannel->SDICH_iCardSta          = 0;
        psdichannel->SDICH_cpcHostName       = "/bus/sd/1";
        psdhciattr->SDHCIHOST_pdrvfuncs      = LW_NULL;
        psdhciattr->SDHCIHOST_iRegAccessType = SDHCI_REGACCESS_TYPE_MEM;
        psdhciattr->SDHCIHOST_ulBasePoint    = SDI_CHAN1_BASE;
        psdhciattr->SDHCIHOST_ulIntVector    = SDI_CHAN1_INT;
        psdhciattr->SDHCIHOST_uiMaxClock     = uiClkRate;
        psdhciattr->SDHCIHOST_uiQuirkFlag    = SDI_CHAN1_QUIRK_FLG;
        psdhciattr->SDHCIHOST_pquirkop       = &_G_sdiquirkop;
        break;

    case 2:                                                             /*  对应 SD3                    */
        psdichannel->SDICH_iCardSta          = 0;
        psdichannel->SDICH_cpcHostName       = "/bus/sd/2";
        psdhciattr->SDHCIHOST_pdrvfuncs      = LW_NULL;
        psdhciattr->SDHCIHOST_iRegAccessType = SDHCI_REGACCESS_TYPE_MEM;
        psdhciattr->SDHCIHOST_ulBasePoint    = SDI_CHAN2_BASE;
        psdhciattr->SDHCIHOST_ulIntVector    = SDI_CHAN2_INT;
        psdhciattr->SDHCIHOST_uiMaxClock     = uiClkRate;
        psdhciattr->SDHCIHOST_uiQuirkFlag    = SDI_CHAN2_QUIRK_FLG;
        psdhciattr->SDHCIHOST_pquirkop       = &_G_sdiquirkop;
        break;

    case 3:                                                             /*  对应SD4                     */
        psdichannel->SDICH_iCardSta          = 0;
        psdichannel->SDICH_cpcHostName       = "/bus/sd/3";
        psdhciattr->SDHCIHOST_pdrvfuncs      = LW_NULL;
        psdhciattr->SDHCIHOST_iRegAccessType = SDHCI_REGACCESS_TYPE_MEM;
        psdhciattr->SDHCIHOST_ulBasePoint    = SDI_CHAN3_BASE;
        psdhciattr->SDHCIHOST_ulIntVector    = SDI_CHAN3_INT;
        psdhciattr->SDHCIHOST_uiMaxClock     = uiClkRate;
        psdhciattr->SDHCIHOST_uiQuirkFlag    = SDI_CHAN3_QUIRK_FLG;
        psdhciattr->SDHCIHOST_pquirkop       = &_G_sdiquirkop;
        break;

    default:
        return  (PX_ERROR);
    }

    __imx6SdhciIoDrvInit(psdhciattr);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __sdiPortInit
** 功能描述: 通道端口初始化, 即实现通道相关管脚的分配等
** 输    入: psdichannel      通道对象
** 输    出: ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __sdiPortInit (__SDI_CHANNEL  *psdichannel)
{
    UINT32 uiChannel = psdichannel->SDICH_uiChannel;

    switch (uiChannel + 1) {
    case HW_USDHC1:
        break;

    case HW_USDHC2:
        usdhc2_iomux_config();
        break;

    case HW_USDHC3:
        usdhc3_iomux_config();
        break;

    case HW_USDHC4:
        usdhc4_iomux_config();
        break;

    default:
        break;
    }

    /*
     * 初始化 SD 卡的 CD 和 WP 管脚
     */
    if (psdichannel->SDICH_uiSdiCd != GPIO_NONE) {
        API_GpioRequestOne(psdichannel->SDICH_uiSdiCd, LW_GPIOF_IN, "sd_cd");
    }

    if (psdichannel->SDICH_uiSdiWp != GPIO_NONE) {
        API_GpioRequestOne(psdichannel->SDICH_uiSdiWp, LW_GPIOF_IN, "sd_wp");
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __sdiExtRegInit
** 功能描述: 通道扩展寄存器初始化,此函数可以暂时为空.
** 输    入: psdichannel      通道对象
** 输    出: ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __sdiExtRegInit (__SDI_CHANNEL  *psdichannel)
{
    ULONG  ulRegBase;
    UINT32 uiReg;

    ulRegBase = psdichannel->SDICH_sdhciattr.SDHCIHOST_ulBasePoint;

    write32(SDI_CTL2_CFG, ulRegBase + SDI_CTL2);
    write32(SDI_CTL3_CFG, ulRegBase + SDI_CTL3);
    write32(SDI_CTL4_CFG, ulRegBase + SDI_CTL4);

    /*
     * 该控制器初始状态为 DDR 模式, 需要先禁止
     */
    uiReg = read32(ulRegBase + SDHCI_MIX_CTRL);
    uiReg &= ~SDHCI_MIX_CTRL_DDREN;
    write32(uiReg, ulRegBase + SDHCI_MIX_CTRL);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __sdiCardStaGet
** 功能描述: 查看设备状态，通过 SD 卡的检测管脚，检测 SD 卡是否存在。调试状态时可以让系统始终返回有效
** 输    入: psdichannel      通道对象
** 输    出: 返回 1: SD卡插入插槽，返回 0: SD卡从插槽拔出
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __sdiCardStaGet (__SDI_CHANNEL  *psdichannel)
{
    UINT32 uiValue = 0;

    if (psdichannel->SDICH_uiSdiCd != GPIO_NONE) {
        uiValue = API_GpioGetValue(psdichannel->SDICH_uiSdiCd);
    }

    return  (uiValue == 0);
}
/*********************************************************************************************************
** 函数名称: __sdiCardStaGet
** 功能描述: 查看设备写状态
** 输    入: psdichannel      通道对象
** 输    出: 0: 未写保护  1: 写保护
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __sdiCardWpGet (__SDI_CHANNEL  *psdichannel)
{
    UINT32  uiStatus = 0;

    if (psdichannel->SDICH_uiSdiWp != GPIO_NONE) {
        uiStatus = API_GpioGetValue(psdichannel->SDICH_uiSdiWp);
    }

    return  (uiStatus);
}
/*********************************************************************************************************
** 函数名称: __sdiCdScan
** 功能描述: 设备状态扫描，通过 SD 卡检查管脚检查
** 输    入: psdichannel      通道对象
** 输    出: NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID __sdiCdScan (__SDI_CHANNEL  *psdichannel)
{
    INT  iStaLast;
    INT  iStaCurr;

    iStaLast = psdichannel->SDICH_iCardSta;
    iStaCurr = __sdiCardStaGet(psdichannel);

    if (iStaLast ^ iStaCurr) {                                          /*  插入状态变化                */
        if (iStaCurr) {
            API_SdmEventNotify(psdichannel->SDICH_pvSdmHost, SDM_EVENT_DEV_INSERT);
        } else {
            API_SdhciDeviceCheckNotify(psdichannel->SDICH_pvSdhciHost, SDHOST_DEVSTA_UNEXIST);
            API_SdmEventNotify(psdichannel->SDICH_pvSdmHost, SDM_EVENT_DEV_REMOVE);
        }

        psdichannel->SDICH_iCardSta = iStaCurr;                         /*  保存状态                    */
    }
}
/*********************************************************************************************************
** 函数名称: __sdiHotPlugInit
** 功能描述: 热插拔支持初始化
** 输    入: psdichannel      通道对象
** 输    出: ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __sdiHotPlugInit (__SDI_CHANNEL  *psdichannel)
{
    hotplugPollAdd((VOIDFUNCPTR)__sdiCdScan, (PVOID)psdichannel);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imx6qSdiDrvInstall
** 功能描述: 安装 SDI 控制器驱动
** 输    入: iChannel      通道号
**           uiSdCdPin     该通道对应的 CD 管脚编号, 若连接eMMC, 则设置为0
**           uiSdWpPin     该通道对应的 WP 管脚编号, 若连接eMMC, 则设置为0
**           bIsBootDev    是否是引导设备(eMMC)
** 输    出: ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  imx6qSdiDrvInstall (INT  iChannel, UINT32 uiSdCdPin, UINT32 uiSdWpPin, BOOL bIsBootDev)
{
    __SDI_CHANNEL  *psdichannel;
    PVOID           pvSdhciHost;
    PVOID           pvSdmHost;
    INT             iRet;

    iRet = __sdiChannelDataInit(iChannel);
    if (iRet != ERROR_NONE) {
        printk("imx6qSdiDrvInstall() error: channel-%d data init error.\n", iChannel);
        return  (PX_ERROR);
    }

    psdichannel = &_G_sdichannelTbl[iChannel];
    psdichannel->SDICH_uiSdiCd = uiSdCdPin;
    psdichannel->SDICH_uiSdiWp = uiSdWpPin;

    iRet = __sdiPortInit(psdichannel);
    if (iRet != ERROR_NONE) {
        printk("imx6qSdiDrvInstall() error: channel-%d port error.\n", iChannel);
        return  (PX_ERROR);
    }

    iRet = __sdiExtRegInit(psdichannel);
    if (iRet != ERROR_NONE) {
        printk("imx6qSdiDrvInstall() error: channel-%d ext reg init error.\n", iChannel);
        return  (PX_ERROR);
    }

    pvSdhciHost = API_SdhciHostCreate(psdichannel->SDICH_cpcHostName, &psdichannel->SDICH_sdhciattr);
    if (!pvSdhciHost) {
        printk("imx6qSdiDrvInstall() error: channel-%d host create error.\n", iChannel);
        return  (PX_ERROR);
    }

    pvSdmHost = API_SdhciSdmHostGet(pvSdhciHost);

    API_SdmHostExtOptSet(pvSdmHost, SDHOST_EXTOPT_MAXBURST_SECTOR_SET, 128);

    if (bIsBootDev) {
        /*
         * BOOT 设备特殊处理:
         *  在当前线程(而非热插拔线程)直接创建设备, 提高启动速度
         */
        API_SdmHostExtOptSet(pvSdmHost, SDHOST_EXTOPT_RESERVE_SECTOR_SET, EMMC_RESERVE_SECTOR);
        API_SdmEventNotify(pvSdmHost, SDM_EVENT_BOOT_DEV_INSERT);
    }

    psdichannel->SDICH_pvSdhciHost = pvSdhciHost;
    psdichannel->SDICH_pvSdmHost   = pvSdmHost;

    __sdiHotPlugInit(psdichannel);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: s5pv210sdiInit
** 功能描述: SD 驱动初始化
** 输    入: NONE
** 输    出: NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID imx6qSdLibInit (VOID)
{
    API_SdmLibInit();
    API_SdmSdioLibInit();                                               /*  SD 库初始化, 必须放在前面   */

    API_SdioBaseDrvInstall();
    API_SdMemDrvInstall();                                              /*  安装应用层驱动              */
}
/*********************************************************************************************************
** 函数名称: __imx6SdHostCfgClk
** 功能描述: 设置 SD 卡控制器的信号时钟
** 输　入  : uiInstance  SD 卡控制器通道号
**           eFreq       需要设置的信号时钟
** 输　出  : NULL
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID __imx6SdHostCfgClk (UINT32 uiInstance, SDHC_FREQ_T eFreq)
{
    while (!(HW_USDHC_PRES_STATE(uiInstance).B.SDSTB)) {                /* Wait until clock stable      */
        ;
    }

    HW_USDHC_SYS_CTRL(uiInstance).U &= ~ESDHC_SYSCTL_FREQ_MASK;         /* Clear DTOCV SDCLKFS DVFS     */

    while (!(HW_USDHC_PRES_STATE(uiInstance).B.SDSTB)) {                /* Wait until clock stable      */
        ;
    }

    if (eFreq == IDENTIFICATION_FREQ) {                                 /* Set frequency dividers       */
        HW_USDHC_SYS_CTRL(uiInstance).B.DVS = ESDHC_IDENT_DVS;
        BW_USDHC_SYS_CTRL_SDCLKFS(uiInstance, ESDHC_IDENT_SDCLKFS);

    } else if (eFreq == OPERATING_FREQ) {
        HW_USDHC_SYS_CTRL(uiInstance).B.DVS = ESDHC_OPERT_DVS;
        BW_USDHC_SYS_CTRL_SDCLKFS(uiInstance, ESDHC_OPERT_SDCLKFS);

    } else {
        HW_USDHC_SYS_CTRL(uiInstance).B.DVS = ESDHC_HS_DVS;
        BW_USDHC_SYS_CTRL_SDCLKFS(uiInstance, ESDHC_HS_SDCLKFS);
    }

    while (!(HW_USDHC_PRES_STATE(uiInstance).B.SDSTB)) {                /* Wait until clock stable      */
        ;
    }

    BW_USDHC_SYS_CTRL_DTOCV(uiInstance, ESDHC_SYSCTL_DTOCV_VAL);        /* Set DTOCV bit                */

    KN_IO_MB();                                                         /* Must do this                 */
}
/*********************************************************************************************************
** 函数名称: __imx6ClockSet
** 功能描述: 重新实现  SDHCIHOST_pquirkop 里面的 SDHCIQOP_pfuncClockSet 函数
** 输    入: psdhcihostattr      通道属性
**           uiClock             SD_CLK 时钟等级
** 输    出: NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __imx6QuirkClockSet (PLW_SDHCI_HOST_ATTR  psdhcihostattr, UINT32 uiClock)
{
    __SDI_CHANNEL  *psdichannel = (__SDI_CHANNEL *)psdhcihostattr->SDHCIHOST_pvUsrSpec;
    UINT32          uiChannel   = 0;

    uiChannel = psdichannel ->SDICH_uiChannel + 1;                      /*  裸机代码 SD 总线通道从1开始 */

    switch (uiClock) {
       case SDARG_SETCLK_LOW:
          __imx6SdHostCfgClk(uiChannel, IDENTIFICATION_FREQ);
          break;

       case SDARG_SETCLK_NORMAL:
          __imx6SdHostCfgClk(uiChannel, OPERATING_FREQ);
          break;

       case SDARG_SETCLK_MAX:
          __imx6SdHostCfgClk(uiChannel, HS_FREQ);
          break;

       default:
          return  (PX_ERROR);                                           /*  根据需要添加                */
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __imx6QuirkIsCardWp
** 功能描述: 判断对应卡是否写保护
** 输    入: psdhcihostattr      通道属性
** 输    出: LW_TRUE             写保护
**           LW_FALSE            未写保护
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static BOOL  __imx6QuirkIsCardWp (PLW_SDHCI_HOST_ATTR     psdhcihostattr)
{
    __SDI_CHANNEL  *psdichannel = (__SDI_CHANNEL *)psdhcihostattr->SDHCIHOST_pvUsrSpec;
    BOOL            bIsWp;

    bIsWp = __sdiCardWpGet(psdichannel) ? LW_TRUE : LW_FALSE;

    return  (bIsWp);
}
/*********************************************************************************************************
** 函数名称: imx6ChipVersionGet
** 功能描述: 获取当前 mx6 系列处理器的型号和版本号
** 输　入  : NONE
** 输　出  : 返回处理器的版本号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6ChipVersionGet (VOID)
{
    VOID    *atAnaTop = MX6_IO_ADDRESS(ANATOP_BASE_ADDR);
    UINT32   uiChipVer;

    if (cpu_is_mx6sl()) {
        uiChipVer = readl(atAnaTop + MX6SL_USB_ANALOG_DIGPROG);
    } else {
        uiChipVer = readl(atAnaTop + MX6_USB_ANALOG_DIGPROG);
    }

    uiChipVer &= 0xFF;

    if (uiChipVer == 0) {
        return  (IMX_CHIP_REVISION_1_0);

    } else if (uiChipVer == 1) {
        return  (IMX_CHIP_REVISION_1_1);

    } else if (uiChipVer == 2) {
        return  (IMX_CHIP_REVISION_1_2);

    }
    return  (IMX_CHIP_REVISION_UNKNOWN);
}
/*********************************************************************************************************
** 函数名称: imx6qChipVersionCheck
** 功能描述: 检查当前的处理器型号是否是 I.MX6Q
** 输　入  : NONE
** 输　出  : 返回处理器的版本号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6qChipVersionCheck (VOID)
{
    INT  iCpuSiliconRev = -1;

    if (!cpu_is_mx6q()) {
        return  (-EINVAL);
    }

    if (iCpuSiliconRev == -1) {
        iCpuSiliconRev = __imx6ChipVersionGet();
    }

    return  (iCpuSiliconRev);
}
/*********************************************************************************************************
  I.MX6 处理器的 SD 卡控制器读写函数适配
*********************************************************************************************************/
static u32  esdhc_readl_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, int reg);
static u16  esdhc_readw_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, int reg);
static u8   esdhc_readb_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr,  int reg);

static void esdhc_writew_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, u16 val, int reg);
static void esdhc_writel_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, u32 val, int reg);
static void esdhc_writeb_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, u8 val, int reg);
/*********************************************************************************************************
** 函数名称: __imx6SdhciReadL
** 功能描述: 内存空间读取32位长度的数据
** 输    入: ulAddr   内存空间地址
** 输    出: 数据
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static UINT32 __imx6SdhciReadL (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr)
{
    return  esdhc_readl_le(psdhcihostattr, ulAddr);
}
/*********************************************************************************************************
** 函数名称: __imx6SdhciReadW
** 功能描述: 内存空间读取16位长度的数据
** 输    入: ulAddr   内存空间地址
** 输    出: 数据
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static UINT16 __imx6SdhciReadW (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr)
{
    return  esdhc_readw_le(psdhcihostattr, ulAddr);
}
/*********************************************************************************************************
** 函数名称: __imx6SdhciReadB
** 功能描述: 内存空间读取8位长度的数据
** 输    入: ulAddr   内存空间地址
** 输    出: 数据
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static UINT8 __imx6SdhciReadB (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr)
{
    return esdhc_readb_le(psdhcihostattr, ulAddr);
}
/*********************************************************************************************************
** 函数名称: __imx6SdhciWriteL
** 功能描述: 内存空间写入32位长度的数据
** 输    入: ulAddr     内存空间地址
**           uiLword    写入的数据
** 输    出: NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID __imx6SdhciWriteL (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr, UINT32 uiLword)
{
    esdhc_writel_le(psdhcihostattr, uiLword, ulAddr);
}
/*********************************************************************************************************
** 函数名称: __imx6SdhciWriteW
** 功能描述: 内存空间写入16位长度的数据
** 输    入: ulAddr     内存空间地址
**           usWord     写入的数据
** 输    出: NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID __imx6SdhciWriteW (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr, UINT16 usWord)
{
    esdhc_writew_le(psdhcihostattr, usWord, ulAddr);
}
/*********************************************************************************************************
** 函数名称: __imx6SdhciWriteB
** 功能描述: 内存空间写入8位长度的数据
** 输    入: ulAddr     内存空间地址
**           ucByte     写入的数据
** 输    出: NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID __imx6SdhciWriteB (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr, UINT8 ucByte)
{
    esdhc_writeb_le(psdhcihostattr, ucByte, ulAddr);
}
/*********************************************************************************************************
** 函数名称: __imx6SdhciIoDrvInit
** 功能描述: IMX.6 SDHCI控制器寄存器访问驱动初始化
**           !!由于该SDHCI的寄存器与标准SDHCI相差很大, 已经不能用qurik来描述了, 因此重新实现
** 输    入: psdhcihostattr   SD 标准主控制器属性结构
** 输    出: ERROR_NONE       函数执行成功
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6SdhciIoDrvInit (PLW_SDHCI_HOST_ATTR  psdhcihostattr)
{
    static SDHCI_DRV_FUNCS  sdhcidrvfunc;

    sdhcidrvfunc.sdhciReadB  = __imx6SdhciReadB;
    sdhcidrvfunc.sdhciReadW  = __imx6SdhciReadW;
    sdhcidrvfunc.sdhciReadL  = __imx6SdhciReadL;
    sdhcidrvfunc.sdhciWriteB = __imx6SdhciWriteB;
    sdhcidrvfunc.sdhciWriteW = __imx6SdhciWriteW;
    sdhcidrvfunc.sdhciWriteL = __imx6SdhciWriteL;

    psdhcihostattr->SDHCIHOST_pdrvfuncs = &sdhcidrvfunc;

    return  (ERROR_NONE);
}
/*********************************************************************************************************
   以下函数均是从 Linux 驱动移植而来, 为了方便后续 驱动升级, 这里不做格式方面的整理
*********************************************************************************************************/
static void esdhc_clrset_le(ULONG ioaddr, u32 mask, u32 val, int reg);

static void esdhc_writew_le(PLW_SDHCI_HOST_ATTR   psdhcihostattr, u16 val, int reg)
{
    __SDI_CHANNEL  *psdichannel = (__SDI_CHANNEL *)psdhcihostattr->SDHCIHOST_pvUsrSpec;

    u32   orig_reg;
    ULONG ioaddr;

    ioaddr = psdhcihostattr->SDHCIHOST_ulBasePoint;

    switch (reg) {
    case SDHCI_CLOCK_CONTROL:
        orig_reg = readl(ioaddr + SDHCI_VENDOR_SPEC);
        if (val & SDHCI_CLOCK_CARD_EN) {
            writel(orig_reg | SDHCI_VENDOR_SPEC_FRC_SDCLK_ON, \
                ioaddr + SDHCI_VENDOR_SPEC);
        } else {
            writel(orig_reg & ~SDHCI_VENDOR_SPEC_FRC_SDCLK_ON, \
                ioaddr + SDHCI_VENDOR_SPEC);
        }

        return;
    case SDHCI_HOST_CONTROL2:
        orig_reg = readl(ioaddr + SDHCI_VENDOR_SPEC);
        if (val & SDHCI_CTRL_VDD_180) {
            orig_reg |= SDHCI_VENDOR_SPEC_VSELECT;
            writel(orig_reg, ioaddr + SDHCI_VENDOR_SPEC);
        } else {
            orig_reg &= ~SDHCI_VENDOR_SPEC_VSELECT;
            writel(orig_reg, ioaddr + SDHCI_VENDOR_SPEC);
        }

        /*
         * FSL sdhc controls bus and signal voltage via one bit
         * VSELECT in VENDOR_SPEC, which has been set in
         * SDHCI_POWER_CONTROL. So we skip the SDHCI_CTRL_VDD_180
         * here.
         *
         * ignore exec_tuning flag written to SDHCI_HOST_CONTROL2,
         * tuning will be handled differently for FSL SDHC ip.
         */
        if (0) {
            orig_reg = readl(ioaddr + SDHCI_TUNE_CTRL);
            if (val & SDHCI_CTRL_EXEC_TUNING) {
                orig_reg |= SDHCI_TUNE_CTRL_STD_TUNING_EN;
                writel(orig_reg,
                    ioaddr + SDHCI_TUNE_CTRL);

                orig_reg =
                    readl(ioaddr + SDHCI_ACMD12_ERR);
                orig_reg |= SDHCI_MIX_CTRL_EXE_TUNE;
                writel(orig_reg,
                    ioaddr + SDHCI_ACMD12_ERR);
            }
        } else {
            orig_reg = readl(ioaddr + SDHCI_MIX_CTRL);
            orig_reg &= ~SDHCI_MIX_CTRL_SMPCLK_SEL;
            orig_reg |= (val & SDHCI_CTRL_TUNED_CLK)
                ? SDHCI_MIX_CTRL_SMPCLK_SEL : 0;
        }

        if (val & SDHCI_CTRL_UHS_DDR50) {
            orig_reg |= SDHCI_MIX_CTRL_DDREN;
            psdichannel->SDICH_uiPad |= SDHCI_MIX_CTRL_DDREN;
        } else {
            orig_reg &= ~SDHCI_MIX_CTRL_DDREN;
            psdichannel->SDICH_uiPad &= ~SDHCI_MIX_CTRL_DDREN;
        }
        writel(orig_reg, ioaddr + SDHCI_MIX_CTRL);

        return;
    case SDHCI_TRANSFER_MODE:
#if 0
        /*
         * Postpone this write, we must do it together with a
         * command write that is down below.
         */
        if (1//(imx_data->flags & ESDHC_FLAG_MULTIBLK_NO_INT)
                && (host->cmd->opcode == SD_IO_RW_EXTENDED)
                && (host->cmd->data->blocks > 1)
                && (host->cmd->data->flags & MMC_DATA_READ)) {
            u32 v;
            v = readl(ioaddr + SDHCI_VENDOR_SPEC);
            v |= SDHCI_VENDOR_SPEC_SDIO_QUIRK;
            writel(v, ioaddr + SDHCI_VENDOR_SPEC);
        }
#endif

        psdichannel->SDICH_uiPad = val;

        if (val & SDHCI_TRNS_AUTO_CMD23)
            psdichannel->SDICH_uiPad |= SDHCI_MIX_CTRL_AC23EN;

        return;
    case SDHCI_COMMAND:
        writel(0x08800880, ioaddr + SDHCI_CAPABILITIES_1);
        if (cpu_is_mx6()) {
            psdichannel->SDICH_uiPad |= \
            (readl(ioaddr + SDHCI_MIX_CTRL) & \
                (SDHCI_MIX_CTRL_EXE_TUNE | \
                SDHCI_MIX_CTRL_SMPCLK_SEL | \
                SDHCI_MIX_CTRL_AUTO_TUNE | \
                SDHCI_MIX_CTRL_FBCLK_SEL | \
                SDHCI_MIX_CTRL_DDREN));

            writel(psdichannel->SDICH_uiPad,
                ioaddr + SDHCI_MIX_CTRL);

            writel(val << 16,
                ioaddr + SDHCI_TRANSFER_MODE);
        } else {
            writel(val << 16 | psdichannel->SDICH_uiPad,
                ioaddr + SDHCI_TRANSFER_MODE);
        }
        return;

    case SDHCI_BLOCK_SIZE:
        val &= ~SDHCI_MAKE_BLKSZ(0x7, 0);
        break;
    }
    esdhc_clrset_le(ioaddr, 0xffff, val, reg);
}

/* pltfm-specific */
#define ESDHC_HOST_CONTROL_LE   0x20

static void esdhc_writeb_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, u8 val, int reg)
{
    u32 new_val;
    ULONG ioaddr;

    ioaddr = psdhcihostattr->SDHCIHOST_ulBasePoint;

    switch (reg) {
    case SDHCI_POWER_CONTROL:
        /*
         * FSL put some DMA bits here
         * If your board has a regulator, code should be here
         */
        if (val == (SDHCI_POWER_ON | SDHCI_POWER_180)) {
            u32 reg;

            /* stop sd clock */
            reg = readl(ioaddr + SDHCI_VENDOR_SPEC);
            writel(reg & ~SDHCI_VENDOR_SPEC_FRC_SDCLK_ON, \
                ioaddr + SDHCI_VENDOR_SPEC);

            /* switch to 1.8V */
            reg = readl(ioaddr + SDHCI_VENDOR_SPEC);
            reg |= SDHCI_VENDOR_SPEC_VSELECT;
            writel(reg, ioaddr + SDHCI_VENDOR_SPEC);

        } else {
            u32 reg;

            reg = readl(ioaddr + SDHCI_VENDOR_SPEC);
            reg &= ~SDHCI_VENDOR_SPEC_VSELECT;
            writel(reg, ioaddr + SDHCI_VENDOR_SPEC);
        }
        return;
    case SDHCI_HOST_CONTROL:
        /* FSL messed up here, so we can just keep those three */

        new_val = val & (SDHCI_CTRL_LED);
        if (val & SDHCI_CTRL_8BITBUS) {
            new_val |= SDHCI_PROT_CTRL_8BIT;
            new_val &= ~SDHCI_PROT_CTRL_4BIT;
        } else if (val & SDHCI_CTRL_4BITBUS) {
            new_val &= ~SDHCI_PROT_CTRL_8BIT;
            new_val |= SDHCI_PROT_CTRL_4BIT;
        }
        /* ensure the endianess */
        new_val |= ESDHC_HOST_CONTROL_LE;
        /* DMA mode bits are shifted */
        new_val |= (val & SDHCI_CTRL_DMA_MASK) << 5;

        esdhc_clrset_le(ioaddr, 0xffff, new_val, reg);
        return;
    }
    esdhc_clrset_le(ioaddr, 0xff, val, reg);
}

static void esdhc_writel_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, u32 val, int reg)
{
    ULONG ioaddr;

    ioaddr = psdhcihostattr->SDHCIHOST_ulBasePoint;


    if (unlikely((reg == SDHCI_INT_ENABLE || reg == SDHCI_SIGNAL_ENABLE))) {

    }

    if (unlikely(1 // imx_data->flags & ESDHC_FLAG_MULTIBLK_NO_INT TODO: modified by zengobo
                && (reg == SDHCI_INT_STATUS)
                && (val & SDHCI_INT_DATA_END))) {
            u32 v;
            v = readl(ioaddr + SDHCI_VENDOR_SPEC);
            v &= ~SDHCI_VENDOR_SPEC_SDIO_QUIRK;
            writel(v, ioaddr + SDHCI_VENDOR_SPEC);
    }

    if (unlikely(reg == SDHCI_INT_ENABLE || reg == SDHCI_SIGNAL_ENABLE)) {
        if (val & SDHCI_INT_ADMA_ERROR) {
            val &= ~SDHCI_INT_ADMA_ERROR;
            val |= SDHCI_INT_VENDOR_SPEC_DMA_ERR;
        }
    }

    writel(val, ioaddr + reg);
}

static u32 esdhc_readl_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, int reg)
{
#if 0
    struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
    struct pltfm_imx_data *imx_data = pltfm_host->priv;
    struct esdhc_platform_data *boarddata
                = host->mmc->parent->platform_data;
#endif

    ULONG ioaddr;

    ioaddr = psdhcihostattr->SDHCIHOST_ulBasePoint;

    /* fake CARD_PRESENT flag on mx25/35 */
    u32 val = readl(ioaddr + reg);

    /*
     * mx6q: SDHCI_PRESENT_STATE bit 16, CINST is not functional on SD3.
     * So move the section up, and check GPIO for card presence again in
     * the following block.
     */
    if (reg == SDHCI_PRESENT_STATE && cpu_is_mx6()) {
        u32 fsl_prss = readl(ioaddr + SDHCI_PRESENT_STATE);
        /* save the least 20 bits */
        val |= fsl_prss & 0x000FFFFF;
        /* move dat[0-3] line signal bits */
        val |= (fsl_prss & 0x0F000000) >> 4;
        /* move cmd line signal bits */
        val |= (fsl_prss & 0x00800000) << 1;
    }

#if 0
    if (unlikely(reg == SDHCI_PRESENT_STATE)) {
        if (boarddata && boarddata->always_present)
            val |= SDHCI_CARD_PRESENT;
        else if (imx_data->flags & ESDHC_FLAG_GPIO_FOR_CD_WP) {
            if (boarddata && gpio_is_valid(boarddata->cd_gpio)
                    && gpio_get_value(boarddata->cd_gpio))
                /* no card, if a valid gpio says so */
                val &= ~SDHCI_CARD_PRESENT;
            else
                /* in all other cases assume card is present */
                val |= SDHCI_CARD_PRESENT;
        }
    }
#endif

    if (reg == SDHCI_INT_STATUS && cpu_is_mx6()
        && __imx6qChipVersionCheck() == IMX_CHIP_REVISION_1_0) {
        /*
         * on mx6q TO1.0, there is low possibility that
         * DATA END interrupt comes ealier than DMA
         * END interrupt which is conflict with standard
         * host controller spec. In this case, read the
         * status register again will workaround this issue.
         */
        if ((val & SDHCI_INT_DATA_END) && \
            !(val & SDHCI_INT_DMA_END))
            val = readl(ioaddr + reg);
    } else if (reg == SDHCI_CAPABILITIES_1
            && (cpu_is_mx6q() || cpu_is_mx6dl())) {
        /*
         * on mx6q, no cap_1 available, fake one.
         */
        val = SDHCI_SUPPORT_DDR50 | SDHCI_SUPPORT_SDR104 | \
              SDHCI_SUPPORT_SDR50;
    } else if (reg == SDHCI_CAPABILITIES_1 && cpu_is_mx6sl()) {
        val = readl(ioaddr + SDHCI_CAPABILITIES) & 0xFFFF;
    } else if (reg == SDHCI_MAX_CURRENT && cpu_is_mx6()) {
        /*
         * on mx6q, no max current available, fake one.
         */
        val = 0;
        val |= 0xFF << SDHCI_MAX_CURRENT_330_SHIFT;
        val |= 0xFF << SDHCI_MAX_CURRENT_300_SHIFT;
        val |= 0xFF << SDHCI_MAX_CURRENT_180_SHIFT;
    }

    if (unlikely(reg == SDHCI_CAPABILITIES)) {
        /* In FSL esdhc IC module, only bit20 is used to indicate the
         * ADMA2 capability of esdhc, but this bit is messed up on
         * some SOCs (e.g. on MX25, MX35 this bit is set, but they
         * don't actually support ADMA2). So set the BROKEN_ADMA
         * uirk on MX25/35 platforms.
         */

        if (val & SDHCI_CAN_DO_ADMA1) {
            val &= ~SDHCI_CAN_DO_ADMA1;
            val |= SDHCI_CAN_DO_ADMA2;
        }
    }

    if (unlikely(reg == SDHCI_INT_STATUS)) {
        if (val & SDHCI_INT_VENDOR_SPEC_DMA_ERR) {
            val &= ~SDHCI_INT_VENDOR_SPEC_DMA_ERR;
            val |= SDHCI_INT_ADMA_ERROR;
        }
    }

    return val;
}


static u16 esdhc_readw_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, int reg)
{
    u16 ret;
    u32 val;

#if 0
    struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
    struct pltfm_imx_data *imx_data = pltfm_host->priv;
#else
    ULONG ioaddr;
    ioaddr = psdhcihostattr->SDHCIHOST_ulBasePoint;
#endif

    switch (reg) {
    case SDHCI_HOST_VERSION:
        reg ^= 2;
        val = readl(ioaddr + reg);
        if ((cpu_is_mx6q() || cpu_is_mx6dl()) &&
            ((val & SDHCI_SPEC_VER_MASK) >> SDHCI_SPEC_VER_SHIFT)
            == SDHCI_FSL_SVN_300) {
            val &= ~SDHCI_SPEC_VER_MASK;
            val |= SDHCI_SPEC_300;
        }
        ret = 0xFFFF & val;
        return ret;
    case SDHCI_HOST_CONTROL2:
        ret = 0;
        /* collect bit info from several regs */
        val = readl(ioaddr + SDHCI_VENDOR_SPEC);
        ret |= (val & SDHCI_VENDOR_SPEC_VSELECT)
            ? SDHCI_CTRL_VDD_180 : 0;

        if (cpu_is_mx6sl()) {
            val = readl(ioaddr + SDHCI_ACMD12_ERR);
            ret |= (val & SDHCI_MIX_CTRL_EXE_TUNE)
                ? SDHCI_CTRL_EXEC_TUNING : 0;
            ret |= (val & SDHCI_MIX_CTRL_SMPCLK_SEL)
                ? SDHCI_CTRL_TUNED_CLK : 0;
        } else {
            val = readl(ioaddr + SDHCI_MIX_CTRL);
            ret |= (val & SDHCI_MIX_CTRL_EXE_TUNE)
                ? SDHCI_CTRL_EXEC_TUNING : 0;
            ret |= (val & SDHCI_MIX_CTRL_SMPCLK_SEL)
                ? SDHCI_CTRL_TUNED_CLK : 0;
        }
        ret |= SDHCI_CTRL_UHS_MASK & 0;//imx_data->uhs_mode;
        /* no preset enable available  */
        ret &= ~SDHCI_CTRL_PRESET_VAL_ENABLE;

        return ret;
    }

    return readw(ioaddr + reg);
}

static u8 esdhc_readb_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr,  int reg)
{
    u8 ret;
    u32 reg_val;
    ULONG ioaddr;
    ioaddr = psdhcihostattr->SDHCIHOST_ulBasePoint;
    ret = 0;
    switch (reg) {
    case SDHCI_POWER_CONTROL:
        reg_val = readl(ioaddr + SDHCI_VENDOR_SPEC);
        ret |= reg_val & SDHCI_VENDOR_SPEC_VSELECT
                ? SDHCI_POWER_180 : SDHCI_POWER_300;
        /* could not power off */
        ret |= SDHCI_POWER_ON;
        return ret;
    case SDHCI_HOST_CONTROL:
        reg_val = readl(ioaddr + SDHCI_HOST_CONTROL);
        if (reg_val & SDHCI_PROT_CTRL_LCTL)
            ret |= SDHCI_CTRL_LED;
        else
            ret &= ~SDHCI_CTRL_LED;

        ret |= (reg_val & SDHCI_PROT_CTRL_DMASEL_MASK) >> 5;
        if (SDHCI_PROT_CTRL_8BIT == (reg_val & SDHCI_PROT_CTRL_DTW)) {
            ret &= ~SDHCI_CTRL_4BITBUS;
            ret |= SDHCI_CTRL_8BITBUS;
        } else if (SDHCI_PROT_CTRL_4BIT
                == (reg_val & SDHCI_PROT_CTRL_DTW)) {
            ret &= ~SDHCI_CTRL_8BITBUS;
            ret |= SDHCI_CTRL_4BITBUS;
        } else if (SDHCI_PROT_CTRL_1BIT
                == (reg_val & SDHCI_PROT_CTRL_DTW))
            ret &= ~(SDHCI_CTRL_8BITBUS | SDHCI_CTRL_4BITBUS);
        return ret;
    case SDHCI_SOFTWARE_RESET:
        reg_val = readl(ioaddr + SDHCI_CLOCK_CONTROL);
        ret = reg_val >> 24;
        return ret;
    case SDHCI_RESPONSE + 3:
        reg_val = readl(ioaddr + SDHCI_RESPONSE);
        ret = reg_val >> 24;
        return ret;
    case SDHCI_RESPONSE + 7:
        reg_val = readl(ioaddr + SDHCI_RESPONSE + 4);
        ret = reg_val >> 24;
        return ret;
    case SDHCI_RESPONSE + 11:
        reg_val = readl(ioaddr + SDHCI_RESPONSE + 8);
        ret = reg_val >> 24;
        return ret;
    default:
        ret = 0;
        break;
    }
    return ret;
}

static void esdhc_clrset_le(ULONG ioaddr, u32 mask, u32 val, int reg)
{
    addr_t base = ioaddr + (reg & ~0x3);
    u32 shift = (reg & 0x3) * 8;

    writel(((readl(base) & ~(mask << shift)) | (val << shift)), base);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
