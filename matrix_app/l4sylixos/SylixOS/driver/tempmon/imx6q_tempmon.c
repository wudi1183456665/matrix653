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
** ��   ��   ��: imx6q_tempmon.c
**
** ��   ��   ��: Zhang.Xu
**
** �ļ���������: 2016 �� 03 �� 05 ��
**
** ��        ��: imx6q ���������¶Ȳ��Թ��ܣ��ܹ�ʵ�ֵ��ζ�ȡ�򱨾�����
**               int  fd, ret, read_temp;
**               fd = open("/dev/temp0", O_RDWR, 0666);
**               ret = read(fd, &read_temp, 4);      ���ζ�ȡ�¶�����
**               ioctl(fd, TEMP_ALARMEN_CMD, 75);    ���ñ����¶���75��
**               ioctl(fd, TEMP_ALARMDIS_CMD, NULL); ��ֹ�¶ȱ���
**               close(fd);
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"

#include "tempmon/imx6q_tempmon.h"
#include "sdk.h"
/*********************************************************************************************************
  WATCH_DOG �Ĵ�������,�Ĵ���Ϊ 16bits�������� 32bits��
*********************************************************************************************************/
#define TEMPMON_BASE           (0x020C8000)
#define PMU_MISC1_BASE         (0x020C8160)

#define TEMPMONCTL0            (0x180)
#define TEMPMONCTL0_SET        (0x184)
#define TEMPMONCTL0_CLR        (0x188)
#define TEMPMONCTL0_TOG        (0x18C)
#define TEMPMONCTL1            (0x190)
#define TEMPMONCTL1_SET        (0x194)
#define TEMPMONCTL1_CLR        (0x198)
#define TEMPMONCTL1_TOG        (0x19C)
#define PMU_MISC1              (0x160)
#define PMU_MISC1_SET          (0x164)
#define PMU_MISC1_CLR          (0x168)
#define PMU_MISC1_TOG          (0x16C)

#define BM_ROOM_COUNT          (0xfff00000)
#define BP_ROOM_COUNT          (20)
#define BM_HOT_COUNT           (0x000fff00)
#define BP_HOT_COUNT           (8)
#define BM_HOT_TEMP            (0x000000ff)
#define BP_HOT_TEMP            (0)
#define ROOM_TEMP              (25.0f)
#define DEFAULT_TEMP_CAL_DATA  (0x57d4df7d)
/*********************************************************************************************************
  WATCH_DOG ���������Ͷ���
*********************************************************************************************************/
typedef struct {
    LW_DEV_HDR                  TEMP_devHdr;                            /* �����ǵ�һ���ṹ���Ա       */
    LW_LIST_LINE_HEADER         TEMP_fdNodeHeader;
	addr_t                      TEMP_BaseAddr;
	INT                         TEMP_iIrqNum;
} __IMX6Q_TEMP_CONTROLER, *__PIMX6Q_TEMP_CONTROLER;

static UINT32                  _G_iTempDrvNum = 0;
static __IMX6Q_TEMP_CONTROLER  _G_tempctller;
/*********************************************************************************************************
** ��������: imx6qComputeTemp
** ��������: ����ȡ�� TEMPMON ��ֵת�����¶�ֵ
** �䡡��  : fTempCnt   ��ȡ���¶ȴ�����ԭʼ��ֵ
** �䡡��  : ���ص�ǰ CPU ���¶�ֵ����λ C
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static float imx6qComputeTemp (float fTempCnt)
{
	float fRoomCount = (DEFAULT_TEMP_CAL_DATA & BM_ROOM_COUNT) >> BP_ROOM_COUNT;
	float fHotCount  = (DEFAULT_TEMP_CAL_DATA & BM_HOT_COUNT) >> BP_HOT_COUNT;
	float fHotTemp   = (DEFAULT_TEMP_CAL_DATA & BM_HOT_TEMP) >> BP_HOT_TEMP;

    float fA = (fHotTemp - ROOM_TEMP);
    float fB = (fRoomCount - fHotCount);
    float fC = fA / fB;
    float fD = (fTempCnt - fHotCount);
    float fE = fD * fC;
    float fF = fHotTemp - fE;
    return fF;
}
/*********************************************************************************************************
** ��������: imx6qComputeAlarm
** ��������: �����õĵı����¶�ֵת���ɼĴ���������ֵ
** �䡡��  : fAlarmTemp   ���õı���ֵ
** �䡡��  : ������Ҫ���õļĴ�������ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT32  imx6qComputeAlarm (float fAlarmTemp)
{
	float  fRoomCount = (DEFAULT_TEMP_CAL_DATA & BM_ROOM_COUNT) >> BP_ROOM_COUNT;
	float  fHotCount  = (DEFAULT_TEMP_CAL_DATA & BM_HOT_COUNT) >> BP_HOT_COUNT;
	float  fHotTemp   = (DEFAULT_TEMP_CAL_DATA & BM_HOT_TEMP) >> BP_HOT_TEMP;

    float  fA = (fAlarmTemp - fHotTemp);
    float  fB = (fHotTemp - ROOM_TEMP);
    float  fC = (fRoomCount - fHotCount);
    float  fD = (fB / fC);

    return (fHotCount + fA / fD);
}
/*********************************************************************************************************
** ��������: imx6qTempMonIsr
** ��������: �¶ȼ�ر����жϺ���
** �䡡��  : pvArg   :  �жϲ���
**           uiVector:  �ж�������
** �䡡��  : �жϷ���ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static irqreturn_t  imx6qTempMonIsr (PVOID  pvArg, UINT32  uiVector)
{
	__PIMX6Q_TEMP_CONTROLER  pTempDev   = (__PIMX6Q_TEMP_CONTROLER)pvArg;
    addr_t                   atBaseAddr = pTempDev->TEMP_BaseAddr;

    writel((1 << 29), (atBaseAddr + PMU_MISC1));                        /*  ���tempsense���ж�        */

    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
** ��������: imx6qTempMonAlarmInit
** ��������: �¶ȼ�ر����жϳ�ʼ������
** �䡡��  : pTempDev   :  ������˽�����ݽṹ��
**           uiAlarmLvl :  ��Ҫ���õı����¶�
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  imx6qTempMonAlarmInit (__PIMX6Q_TEMP_CONTROLER  pTempDev, UINT32 uiAlarmLvl)
{
	addr_t  atBaseAddr = pTempDev->TEMP_BaseAddr;
	UINT32  uiRegVal;
	INT     iRet;

    /*
     * Wake up the temp monitor
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0x000FFFFC;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);

    /*
     * set the measure frequency 0.5Hz,because temp change slowly
     */
    writel(0xFFFF, atBaseAddr + TEMPMONCTL1);

    /*
     * Calculate and fill in the alarm value.
     */
    uiRegVal = imx6qComputeAlarm((float)uiAlarmLvl);
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) | uiRegVal;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);

    /*
     * clear irq and init it
     */
    writel((1 << 29), atBaseAddr + PMU_MISC1);
    iRet = API_InterVectorConnect((ULONG)pTempDev->TEMP_iIrqNum,
                                  (PINT_SVR_ROUTINE)imx6qTempMonIsr,
                                  (PVOID)pTempDev,
                                  "temp_isr");
    if (iRet != ERROR_NONE) {
        printk(KERN_ERR "failed to connect temp_isr!\n");
    }
    API_InterVectorEnable(pTempDev->TEMP_iIrqNum);

    /*
     * Start  measurement
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) | 0x2;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);
}
/*********************************************************************************************************
** ��������: imx6qTempMonAlarmInit
** ��������: �¶ȼ�ر����жϳ�ʼ������
** �䡡��  : pTempDev   :  ������˽�����ݽṹ��
**           uiAlarmLvl :  ��Ҫ���õı����¶�
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  imx6qTempMonAlarmDisable (__PIMX6Q_TEMP_CONTROLER  pTempDev)
{
	addr_t  atBaseAddr = pTempDev->TEMP_BaseAddr;
	UINT32  uiRegVal;

	/*
	 * �����жϣ���ȡ���жϺ���ע��
	 */
    API_InterVectorDisable(pTempDev->TEMP_iIrqNum);
    API_InterVectorDisconnect((ULONG)pTempDev->TEMP_iIrqNum,
    		                  (PINT_SVR_ROUTINE)imx6qTempMonIsr,
    		                  (PVOID)pTempDev);

	/*
	 * ����¶Ȳ���Ƶ�����ã��������¶Ȳ���ģ��
	 */
    writel(0x0, atBaseAddr + TEMPMONCTL1);
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0x000FFFFD;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);
}
/*********************************************************************************************************
** ��������: imx6qTempMonOpen
** ��������: �� WATCH_DOG �豸
** �䡡��  : pDev                  �豸
**           pcName                �豸����
**           iFlags                ��־
**           iMode                 ģʽ
** �䡡��  : �ļ��ڵ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LONG  imx6qTempMonOpen (__PIMX6Q_TEMP_CONTROLER pTempDev, PCHAR pcName, INT iFlags, INT iMode)
{
    PLW_FD_NODE   pFdNode;
    BOOL          bIsNew;

    if (pcName == LW_NULL) {
        _ErrorHandle(ERROR_IO_NO_DEVICE_NAME_IN_PATH);
        return  (PX_ERROR);

    } else {
        pFdNode = API_IosFdNodeAdd(&pTempDev->TEMP_fdNodeHeader, (dev_t)pTempDev, 0,
                                   iFlags, iMode, 0, 0, 0, LW_NULL, &bIsNew);
        if (pFdNode == LW_NULL) {
            printk(KERN_ERR "imx6qWdtOpen(): failed to add fd node!\n");
            return  (PX_ERROR);
        }

        if (LW_DEV_INC_USE_COUNT(&pTempDev->TEMP_devHdr) == 1) {
            /*
             * ����豸��ʼ����ʹ�ܲ���,temp �ƺ�����Ҫʲô��ʼ��
             */

            return  ((LONG)pFdNode);
        }
    }
    /*
     * ����豸��ʧ�ܽ������²���
     */
    if (pFdNode) {
        API_IosFdNodeDec(&pTempDev->TEMP_fdNodeHeader, pFdNode, NULL);
        pFdNode = LW_NULL;
    }
    LW_DEV_DEC_USE_COUNT(&pTempDev->TEMP_devHdr);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: imx6qTempMonClose
** ��������: �ر� WATCH_DOG �豸
** �䡡��  : pFdEntry              �ļ��ṹ
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT imx6qTempMonClose (PLW_FD_ENTRY   pFdEntry)
{
	__PIMX6Q_TEMP_CONTROLER   pTempDev  = (__PIMX6Q_TEMP_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;
    PLW_FD_NODE               pFdNode = (PLW_FD_NODE)pFdEntry->FDENTRY_pfdnode;

    API_IosFdNodeDec(&pTempDev->TEMP_fdNodeHeader, pFdNode, NULL);
    LW_DEV_DEC_USE_COUNT(&pTempDev->TEMP_devHdr);

    return (PX_ERROR);
}
/*********************************************************************************************************
** ��������: imx6qTempMonIoctl
** ��������: ���� WATCH_DOG �豸
** �䡡��  : pFdEntry              �ļ��ṹ
**           iCmd                  ����
**           lArg                  ����  �������¶ȱ�����ֵ
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qTempMonIoctl (PLW_FD_ENTRY  pFdEntry, INT  iCmd, LONG lArg)
{
	__PIMX6Q_TEMP_CONTROLER pTempDev = (__PIMX6Q_TEMP_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;

    switch (iCmd) {
    case TEMP_ALARMEN_CMD:                                              /*  ʹ���¶Ȳ���Alarm�ж�       */
    	imx6qTempMonAlarmInit(pTempDev, (UINT32)lArg);
        return  (ERROR_NONE);

    case TEMP_ALARMDIS_CMD:                                             /*  ��ֹ�¶Ȳ���Alarm�ж�       */
    	imx6qTempMonAlarmDisable(pTempDev);
        return  (ERROR_NONE);

    default:
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** ��������: imx6qTempMonWrite
** ��������: ���� write ������ʵ�ֿ��Ź�ι�����ܣ�д���ַ�Ϊ0xAAAA5555
** �䡡��  : peeprom      eeprom �ṹ
**           pvBuf        д����
**           stLen        д���ݳ���
** �䡡��  : �ɹ���ȡ�����ֽ�����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ssize_t  imx6qTempMonWrite (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
   /*
    *  ���¶ȵ�д��û���κ�����
    */
    return  (stLen);
}
/*********************************************************************************************************
** ��������: imx6qTempMonRead
** ��������: ���� write ����
** �䡡��  :
** �䡡��  : �ɹ���ȡ�����ֽ�����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ssize_t  imx6qTempMonRead (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
	__PIMX6Q_TEMP_CONTROLER   pTempDev   = (__PIMX6Q_TEMP_CONTROLER)pFdentry->FDENTRY_pdevhdrHdr;
	addr_t                    atBaseAddr = pTempDev->TEMP_BaseAddr;

	UINT32 uiRegVal;

    /*
     * Wake up the temp monitor.
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0xFFFFFFFC;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);

    /*
     * Clear the measure frequency so we only get single measurements.
     */
    writel(0x0, atBaseAddr + TEMPMONCTL1);

    /*
     * Start a measurement cycle.
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) | 0x2;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);

    /*
     * Wait until the measurement is ready.
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0x4;
    while (uiRegVal == 0) {
        udelay(1);
        uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0x4;
    }

    /*
     *  disable tempmon and back the val to user
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0);
    writel(uiRegVal | 0x1, atBaseAddr + TEMPMONCTL0);

    float fTempVal = (uiRegVal & 0x000FFF00) >> 8;
    INT iTempVal = (INT)imx6qComputeTemp(fTempVal);

    *(INT *)pvBuf = iTempVal;

    return  (4);
}
/*********************************************************************************************************
** ��������: imx6qTempMonDrv
** ��������: ��װ WATCH_DOG ����
** �䡡��  : NONE
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qTempMonDrv (VOID)
{
    struct file_operations  fileop;

    if (_G_iTempDrvNum) {
        return  (ERROR_NONE);
    }

    lib_memset(&fileop, 0, sizeof(struct file_operations));

    fileop.owner     = THIS_MODULE;
    fileop.fo_create = imx6qTempMonOpen;
    fileop.fo_open   = imx6qTempMonOpen;
    fileop.fo_close  = imx6qTempMonClose;
    fileop.fo_ioctl  = imx6qTempMonIoctl;
    fileop.fo_write  = imx6qTempMonWrite;
    fileop.fo_read   = imx6qTempMonRead;

    _G_iTempDrvNum = iosDrvInstallEx2(&fileop, LW_DRV_TYPE_NEW_1);

    DRIVER_LICENSE(_G_iTempDrvNum,     "Dual BSD/GPL->Ver 1.0");
    DRIVER_AUTHOR(_G_iTempDrvNum,      "Zhang.Xu");
    DRIVER_DESCRIPTION(_G_iTempDrvNum, "TempMon driver.");

    return  (_G_iTempDrvNum > 0) ? (ERROR_NONE) : (PX_ERROR);
}
/*********************************************************************************************************
** ��������: imx6qTempMonDevAdd
** ��������: ���� WATCH_DOG �豸
** �䡡��  : uiIndex           ID
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qTempMonDevAdd (VOID)
{
	__PIMX6Q_TEMP_CONTROLER   pTempDev;

    pTempDev = &_G_tempctller;
    pTempDev ->TEMP_BaseAddr = TEMPMON_BASE;
    pTempDev ->TEMP_iIrqNum  = IMX_INT_TEMPERATURE;

    if (API_IosDevAddEx(&pTempDev->TEMP_devHdr, "/dev/temp0", _G_iTempDrvNum, DT_CHR) != ERROR_NONE) {
        printk(KERN_ERR "imx6qTempDevAdd(): can not add device : %s.\n", strerror(errno));
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
