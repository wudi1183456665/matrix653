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
** ��   ��   ��: imx6q_sdi.c
**
** ��   ��   ��: Zeng.Bo(����)
**
** �ļ���������: 2015 �� 03 �� 02 ��
**
** ��        ��: I.MX6Q �������� SD ������������
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
  �궨��
*********************************************************************************************************/
#define SDI_CHAN_NUM                (4)                                 /*  ����ʹ�õ�ͨ������          */

#define SDI_CHAN0_BASE              (USDHC1_BASE_ADDR)                  /*  uSDHC ����������ַ          */
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
  SD ��չ���ƼĴ��� (SDI_CTL2)
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
  SD ��չ���ƼĴ���(SDI_CTL3)
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
  SD ��չ���ƼĴ���(SDI_CTL4)
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
  �����豸(eMMC)���, ͨ��ʹ�ñ�׼��������ʽ��bootloaderԤ���ռ�, ��ʱ�����豣������.
  ����������Ҫ�ɸ���ʵ���������
*********************************************************************************************************/
#define EMMC_RESERVE_SECTOR         0
/*********************************************************************************************************
  IMX.6 ϵͳ��Ϣ���
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
  SDHCI Ƶ�ʶ���
*********************************************************************************************************/
typedef enum {
    OPERATING_FREQ,
    IDENTIFICATION_FREQ,
    HS_FREQ
} SDHC_FREQ_T;
/*********************************************************************************************************
  ͨ�� ����
*********************************************************************************************************/
typedef struct {
    UINT32                  SDICH_uiChannel;                            /*  SD ��ͨ�����               */
    CPCHAR                  SDICH_cpcHostName;
    INT                     SDICH_iCardSta;                             /*  ��ǰ�豸����/�γ�״̬       */
    LW_SDHCI_HOST_ATTR      SDICH_sdhciattr;
    PVOID                   SDICH_pvSdhciHost;                          /*  ������ SDHCI ����           */
    PVOID                   SDICH_pvSdmHost;                            /*  ��Ӧ�� SDM HOST ����        */

    UINT32                  SDICH_uiPad;                                /*  SD ���Ĵ�������������;���� */
    UINT32                  SDICH_uiSdiCd;                              /*  SD �� CD �ܽ�               */
    UINT32                  SDICH_uiSdiWp;                              /*  SD �� WP �ܽ�               */
} __SDI_CHANNEL;
/*********************************************************************************************************
  ȫ�ֱ�������������
*********************************************************************************************************/
static __SDI_CHANNEL        _G_sdichannelTbl[SDI_CHAN_NUM];
static SDHCI_QUIRK_OP       _G_sdiquirkop = {LW_NULL, };
/*********************************************************************************************************
  ��������
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
** ��������: sdiClockInit
** ��������: SD ������ʱ�ӣ�����ʱ�ӵĽ�����67.4.5�½���˵������CCM Clock TreeͼƬ����ʾSD��
**           ʱ����PLL2���ʱ�Ӿ���2��Ƶ���ṩ��uSDHCn��������Table 8-4 ��ʾ��pll2���ĳ���������528M��
**           uSDHC����ʱ��Ϊ198M��ʱ��ԴΪPFD400��
**
** �䡡��  : iChannel
** �䡡��  : ����ʱ��Ƶ�� (0 ��ʾʧ��)
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT32 __sdiClockInit (INT  iChannel)
{
    /*
     * ����������ԣ�ʱ�����ò���Ҫ����ϵͳ��ʱ��Ƶ�ʣ������޸� SD ��Э�����á�
     */
    return  (200000000);                                                /*  uboot ���õ�����Ϊ200Mhz    */
}
/*********************************************************************************************************
** ��������: __sdiChannelDataInit
** ��������: SD ������������ʼ��
** ��    ��: iChannel      ͨ����
** ��    ��: NONE
** ȫ�ֱ���:
** ����ģ��:
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
     * ʹ���ڲ���ʱ�����÷���
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

    case 1:                                                             /*  ��Ӧ SD2                    */
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

    case 2:                                                             /*  ��Ӧ SD3                    */
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

    case 3:                                                             /*  ��ӦSD4                     */
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
** ��������: __sdiPortInit
** ��������: ͨ���˿ڳ�ʼ��, ��ʵ��ͨ����عܽŵķ����
** ��    ��: psdichannel      ͨ������
** ��    ��: ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
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
     * ��ʼ�� SD ���� CD �� WP �ܽ�
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
** ��������: __sdiExtRegInit
** ��������: ͨ����չ�Ĵ�����ʼ��,�˺���������ʱΪ��.
** ��    ��: psdichannel      ͨ������
** ��    ��: ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
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
     * �ÿ�������ʼ״̬Ϊ DDR ģʽ, ��Ҫ�Ƚ�ֹ
     */
    uiReg = read32(ulRegBase + SDHCI_MIX_CTRL);
    uiReg &= ~SDHCI_MIX_CTRL_DDREN;
    write32(uiReg, ulRegBase + SDHCI_MIX_CTRL);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __sdiCardStaGet
** ��������: �鿴�豸״̬��ͨ�� SD ���ļ��ܽţ���� SD ���Ƿ���ڡ�����״̬ʱ������ϵͳʼ�շ�����Ч
** ��    ��: psdichannel      ͨ������
** ��    ��: ���� 1: SD�������ۣ����� 0: SD���Ӳ�۰γ�
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __sdiCardStaGet
** ��������: �鿴�豸д״̬
** ��    ��: psdichannel      ͨ������
** ��    ��: 0: δд����  1: д����
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __sdiCdScan
** ��������: �豸״̬ɨ�裬ͨ�� SD �����ܽż��
** ��    ��: psdichannel      ͨ������
** ��    ��: NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID __sdiCdScan (__SDI_CHANNEL  *psdichannel)
{
    INT  iStaLast;
    INT  iStaCurr;

    iStaLast = psdichannel->SDICH_iCardSta;
    iStaCurr = __sdiCardStaGet(psdichannel);

    if (iStaLast ^ iStaCurr) {                                          /*  ����״̬�仯                */
        if (iStaCurr) {
            API_SdmEventNotify(psdichannel->SDICH_pvSdmHost, SDM_EVENT_DEV_INSERT);
        } else {
            API_SdhciDeviceCheckNotify(psdichannel->SDICH_pvSdhciHost, SDHOST_DEVSTA_UNEXIST);
            API_SdmEventNotify(psdichannel->SDICH_pvSdmHost, SDM_EVENT_DEV_REMOVE);
        }

        psdichannel->SDICH_iCardSta = iStaCurr;                         /*  ����״̬                    */
    }
}
/*********************************************************************************************************
** ��������: __sdiHotPlugInit
** ��������: �Ȳ��֧�ֳ�ʼ��
** ��    ��: psdichannel      ͨ������
** ��    ��: ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __sdiHotPlugInit (__SDI_CHANNEL  *psdichannel)
{
    hotplugPollAdd((VOIDFUNCPTR)__sdiCdScan, (PVOID)psdichannel);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6qSdiDrvInstall
** ��������: ��װ SDI ����������
** ��    ��: iChannel      ͨ����
**           uiSdCdPin     ��ͨ����Ӧ�� CD �ܽű��, ������eMMC, ������Ϊ0
**           uiSdWpPin     ��ͨ����Ӧ�� WP �ܽű��, ������eMMC, ������Ϊ0
**           bIsBootDev    �Ƿ��������豸(eMMC)
** ��    ��: ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
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
         * BOOT �豸���⴦��:
         *  �ڵ�ǰ�߳�(�����Ȳ���߳�)ֱ�Ӵ����豸, ��������ٶ�
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
** ��������: s5pv210sdiInit
** ��������: SD ������ʼ��
** ��    ��: NONE
** ��    ��: NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID imx6qSdLibInit (VOID)
{
    API_SdmLibInit();
    API_SdmSdioLibInit();                                               /*  SD ���ʼ��, �������ǰ��   */

    API_SdioBaseDrvInstall();
    API_SdMemDrvInstall();                                              /*  ��װӦ�ò�����              */
}
/*********************************************************************************************************
** ��������: __imx6SdHostCfgClk
** ��������: ���� SD �����������ź�ʱ��
** �䡡��  : uiInstance  SD ��������ͨ����
**           eFreq       ��Ҫ���õ��ź�ʱ��
** �䡡��  : NULL
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __imx6ClockSet
** ��������: ����ʵ��  SDHCIHOST_pquirkop ����� SDHCIQOP_pfuncClockSet ����
** ��    ��: psdhcihostattr      ͨ������
**           uiClock             SD_CLK ʱ�ӵȼ�
** ��    ��: NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __imx6QuirkClockSet (PLW_SDHCI_HOST_ATTR  psdhcihostattr, UINT32 uiClock)
{
    __SDI_CHANNEL  *psdichannel = (__SDI_CHANNEL *)psdhcihostattr->SDHCIHOST_pvUsrSpec;
    UINT32          uiChannel   = 0;

    uiChannel = psdichannel ->SDICH_uiChannel + 1;                      /*  ������� SD ����ͨ����1��ʼ */

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
          return  (PX_ERROR);                                           /*  ������Ҫ���                */
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __imx6QuirkIsCardWp
** ��������: �ж϶�Ӧ���Ƿ�д����
** ��    ��: psdhcihostattr      ͨ������
** ��    ��: LW_TRUE             д����
**           LW_FALSE            δд����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static BOOL  __imx6QuirkIsCardWp (PLW_SDHCI_HOST_ATTR     psdhcihostattr)
{
    __SDI_CHANNEL  *psdichannel = (__SDI_CHANNEL *)psdhcihostattr->SDHCIHOST_pvUsrSpec;
    BOOL            bIsWp;

    bIsWp = __sdiCardWpGet(psdichannel) ? LW_TRUE : LW_FALSE;

    return  (bIsWp);
}
/*********************************************************************************************************
** ��������: imx6ChipVersionGet
** ��������: ��ȡ��ǰ mx6 ϵ�д��������ͺźͰ汾��
** �䡡��  : NONE
** �䡡��  : ���ش������İ汾��
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6qChipVersionCheck
** ��������: ��鵱ǰ�Ĵ������ͺ��Ƿ��� I.MX6Q
** �䡡��  : NONE
** �䡡��  : ���ش������İ汾��
** ȫ�ֱ���:
** ����ģ��:
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
  I.MX6 �������� SD ����������д��������
*********************************************************************************************************/
static u32  esdhc_readl_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, int reg);
static u16  esdhc_readw_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, int reg);
static u8   esdhc_readb_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr,  int reg);

static void esdhc_writew_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, u16 val, int reg);
static void esdhc_writel_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, u32 val, int reg);
static void esdhc_writeb_le(PLW_SDHCI_HOST_ATTR  psdhcihostattr, u8 val, int reg);
/*********************************************************************************************************
** ��������: __imx6SdhciReadL
** ��������: �ڴ�ռ��ȡ32λ���ȵ�����
** ��    ��: ulAddr   �ڴ�ռ��ַ
** ��    ��: ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT32 __imx6SdhciReadL (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr)
{
    return  esdhc_readl_le(psdhcihostattr, ulAddr);
}
/*********************************************************************************************************
** ��������: __imx6SdhciReadW
** ��������: �ڴ�ռ��ȡ16λ���ȵ�����
** ��    ��: ulAddr   �ڴ�ռ��ַ
** ��    ��: ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT16 __imx6SdhciReadW (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr)
{
    return  esdhc_readw_le(psdhcihostattr, ulAddr);
}
/*********************************************************************************************************
** ��������: __imx6SdhciReadB
** ��������: �ڴ�ռ��ȡ8λ���ȵ�����
** ��    ��: ulAddr   �ڴ�ռ��ַ
** ��    ��: ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT8 __imx6SdhciReadB (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr)
{
    return esdhc_readb_le(psdhcihostattr, ulAddr);
}
/*********************************************************************************************************
** ��������: __imx6SdhciWriteL
** ��������: �ڴ�ռ�д��32λ���ȵ�����
** ��    ��: ulAddr     �ڴ�ռ��ַ
**           uiLword    д�������
** ��    ��: NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID __imx6SdhciWriteL (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr, UINT32 uiLword)
{
    esdhc_writel_le(psdhcihostattr, uiLword, ulAddr);
}
/*********************************************************************************************************
** ��������: __imx6SdhciWriteW
** ��������: �ڴ�ռ�д��16λ���ȵ�����
** ��    ��: ulAddr     �ڴ�ռ��ַ
**           usWord     д�������
** ��    ��: NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID __imx6SdhciWriteW (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr, UINT16 usWord)
{
    esdhc_writew_le(psdhcihostattr, usWord, ulAddr);
}
/*********************************************************************************************************
** ��������: __imx6SdhciWriteB
** ��������: �ڴ�ռ�д��8λ���ȵ�����
** ��    ��: ulAddr     �ڴ�ռ��ַ
**           ucByte     д�������
** ��    ��: NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID __imx6SdhciWriteB (PLW_SDHCI_HOST_ATTR   psdhcihostattr, ULONG ulAddr, UINT8 ucByte)
{
    esdhc_writeb_le(psdhcihostattr, ucByte, ulAddr);
}
/*********************************************************************************************************
** ��������: __imx6SdhciIoDrvInit
** ��������: IMX.6 SDHCI�������Ĵ�������������ʼ��
**           !!���ڸ�SDHCI�ļĴ������׼SDHCI���ܴ�, �Ѿ�������qurik��������, �������ʵ��
** ��    ��: psdhcihostattr   SD ��׼�����������Խṹ
** ��    ��: ERROR_NONE       ����ִ�гɹ�
** ȫ�ֱ���:
** ����ģ��:
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
   ���º������Ǵ� Linux ������ֲ����, Ϊ�˷������ ��������, ���ﲻ����ʽ���������
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
