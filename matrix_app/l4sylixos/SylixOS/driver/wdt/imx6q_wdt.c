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
** ��   ��   ��: imx6q_wdt.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2015 �� 12 �� 13 ��
**
** ��        ��: ���Ź���ʱ�����������Ź���ʹ������Ϊ,���Ź�1��2����;��ͬ��ʹ��ʱ��Ҫʹ�ÿ��Ź�1
**               int  fd;
**               unsigned int  weigou = 0xAAAA5555;
**               fd = open("/dev/wdt0", O_RDWR, 0666);
**               write(fd, &weigou, 4);
**               sleep(2);
**               write(fd, &weigou, 4);
**               sleep(2);
**               close(fd);
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"

#include "wdt/imx6q_wdt.h"
#include "sdk.h"
/*********************************************************************************************************
  WATCH_DOG �Ĵ�������,�Ĵ���Ϊ 16bits�������� 32bits��
*********************************************************************************************************/
#define WCR             (0x0)                                           /* control                      */
#define WSR             (0x2)                                           /* service                      */
#define WRSR            (0x4)                                           /* reset status                 */
#define WICR            (0x6)                                           /* interrupt control            */
#define WMCR            (0x8)                                           /* misc control                 */

#define WCR_WDE         (0x0004)                                        /* write once                   */
#define WCR_WDT         (0x0008)                                        /* write once                   */
#define WCR_SRS         (0x0010)                                        /* software reset               */
#define WCR_WDA         (0x0020)                                        /* assert output                */
#define WCR_WDBG        (0x0002)                                        /* suspend time while in debug  */
#define WCR_WT          (0x0500)                                        /* 3 seconds                    */

#define WDT_SEQ1        (0x5555)		                                /* -> service sequence 1        */
#define WDT_SEQ2        (0xAAAA)		                                /* -> service sequence 2        */

#define WDT_NUM         (2)
#define WDTTIME         (103)                                           /*  ioctl ����������            */
#define WDTSTOP         (104)
/*********************************************************************************************************
  WATCH_DOG ���������Ͷ���
*********************************************************************************************************/
typedef struct {
    LW_DEV_HDR                  WDTC_devHdr;                            /* �����ǵ�һ���ṹ���Ա       */
    LW_LIST_LINE_HEADER         WDTC_fdNodeHeader;
	addr_t                      WDTC_BaseAddr;
} __IMX6Q_WDT_CONTROLER, *__PIMX6Q_WDT_CONTROLER;

UINT32                        _G_iWdtDrvNum = 0;
static __IMX6Q_WDT_CONTROLER  _G_wdt[WDT_NUM];
/*********************************************************************************************************
** ��������: __imx6qWatchDogOpen
** ��������: �� WATCH_DOG �豸
** �䡡��  : pDev                  �豸
**           pcName                �豸����
**           iFlags                ��־
**           iMode                 ģʽ
** �䡡��  : �ļ��ڵ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LONG  __imx6qWatchDogOpen (__PIMX6Q_WDT_CONTROLER   pWdtDev, PCHAR pcName, INT iFlags, INT iMode)
{
    PLW_FD_NODE   pFdNode;
    BOOL          bIsNew;
	addr_t        atBaseAddr;

    if (pcName == LW_NULL) {
        _ErrorHandle(ERROR_IO_NO_DEVICE_NAME_IN_PATH);
        return  (PX_ERROR);

    } else {
        pFdNode = API_IosFdNodeAdd(&pWdtDev->WDTC_fdNodeHeader, (dev_t)pWdtDev, 0,
                                   iFlags, iMode, 0, 0, 0, LW_NULL, &bIsNew);
        if (pFdNode == LW_NULL) {
            printk(KERN_ERR "imx6qWdtOpen(): failed to add fd node!\n");
            return  (PX_ERROR);
        }

        if (LW_DEV_INC_USE_COUNT(&pWdtDev->WDTC_devHdr) == 1) {
            /*
             * ����豸��ʼ����ʹ�ܲ���,���Ź�Ĭ�ϸ�λʱ��Ϊ3s
             */
            atBaseAddr = pWdtDev->WDTC_BaseAddr;
            writew(WCR_WDA | WCR_SRS | WCR_WT | WCR_WDBG | WCR_WDE, atBaseAddr + WCR);
            return  ((LONG)pFdNode);
        }
    }
    /*
     * ����豸��ʧ�ܽ������²���
     */
    if (pFdNode) {
        API_IosFdNodeDec(&pWdtDev->WDTC_fdNodeHeader, pFdNode, NULL);
        pFdNode = LW_NULL;
    }
    LW_DEV_DEC_USE_COUNT(&pWdtDev->WDTC_devHdr);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: __imx6qWatchDogClose
** ��������: �ر� WATCH_DOG �豸
** �䡡��  : pFdEntry              �ļ��ṹ
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __imx6qWatchDogClose (PLW_FD_ENTRY   pFdEntry)
{
	/*
	 * ���Ź�һ����ʹ�ܣ��Ͳ��ܹ���ֹ��
	 */
    return (PX_ERROR);
}
/*********************************************************************************************************
** ��������: __imx6qWatchDogIoctl
** ��������: ���� WATCH_DOG �豸
** �䡡��  : pFdEntry              �ļ��ṹ
**           iCmd                  ����
**           lArg                  ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __imx6qWatchDogIoctl (PLW_FD_ENTRY  pFdEntry, INT  iCmd, LONG lArg)
{
	__PIMX6Q_WDT_CONTROLER   pWdtDev    = (__PIMX6Q_WDT_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;
	addr_t                   atBaseAddr = pWdtDev->WDTC_BaseAddr;
    UINT16                   usRegVal;

    switch (iCmd) {
    case WDTTIME:                                                       /*  ���ÿ��Ź���λ����,         */
    	   usRegVal = readw(atBaseAddr + WCR) & 0x00FF;                 /*  ������ҪС��0xFF            */
           writew(usRegVal | (lArg << 8), atBaseAddr + WCR);
        return  (ERROR_NONE);
    case WDTSTOP:                                                       /*  ʹ���Ź�������ͣ״̬        */
    	   usRegVal = readw(atBaseAddr + WCR) | (1 << 7);
           writew(usRegVal, atBaseAddr + WCR);
        return  (ERROR_NONE);
    default:
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** ��������: __imx6qWatchDogWrite
** ��������: ���� write ������ʵ�ֿ��Ź�ι�����ܣ�д���ַ�Ϊ0xAAAA5555
** �䡡��  : peeprom      eeprom �ṹ
**           pvBuf        д����
**           stLen        д���ݳ���
** �䡡��  : �ɹ���ȡ�����ֽ�����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ssize_t  __imx6qWatchDogWrite (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
	__PIMX6Q_WDT_CONTROLER   pWdtDev    = (__PIMX6Q_WDT_CONTROLER)pFdentry->FDENTRY_pdevhdrHdr;
	addr_t                   atBaseAddr = pWdtDev->WDTC_BaseAddr;
    UINT32                   usRegVal;

    usRegVal = *(UINT32 *)pvBuf;
    writew(usRegVal, atBaseAddr + WSR);

    usRegVal = usRegVal >> 16;
    writew(usRegVal, atBaseAddr + WSR);

   /*
    * ----------ι����˳��------------
    * writew(0x5555, atBaseAddr + WSR);
    * writew(0xAAAA, atBaseAddr + WSR);
    */
    return  (stLen);
}
/*********************************************************************************************************
** ��������: __imx6qWatchDogRead
** ��������: ���� write ����
** �䡡��  :
** �䡡��  : �ɹ���ȡ�����ֽ�����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ssize_t  __imx6qWatchDogRead (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
	__PIMX6Q_WDT_CONTROLER   pWdtDev    = (__PIMX6Q_WDT_CONTROLER)pFdentry->FDENTRY_pdevhdrHdr;
	addr_t                   atBaseAddr = pWdtDev->WDTC_BaseAddr;

    /*
     * ���ؿ��Ź�״̬��4:�ϵ縴λ��1:���Ź���λ��0:�����λ
     */
    *(INT *)pvBuf = readw(atBaseAddr + WSR);

    return  (2);
}
/*********************************************************************************************************
** ��������: imx6qWatchDogDrv
** ��������: ��װ WATCH_DOG ����
** �䡡��  : NONE
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qWatchDogDrv (VOID)
{
    struct file_operations  fileop;

    if (_G_iWdtDrvNum) {
        return  (ERROR_NONE);
    }

    lib_memset(&fileop, 0, sizeof(struct file_operations));

    fileop.owner     = THIS_MODULE;
    fileop.fo_create = __imx6qWatchDogOpen;
    fileop.fo_open   = __imx6qWatchDogOpen;
    fileop.fo_close  = __imx6qWatchDogClose;
    fileop.fo_ioctl  = __imx6qWatchDogIoctl;
    fileop.fo_write  = __imx6qWatchDogWrite;
    fileop.fo_read   = __imx6qWatchDogRead;

    _G_iWdtDrvNum = iosDrvInstallEx2(&fileop, LW_DRV_TYPE_NEW_1);

    DRIVER_LICENSE(_G_iWdtDrvNum,     "Dual BSD/GPL->Ver 1.0");
    DRIVER_AUTHOR(_G_iWdtDrvNum,      "Zhang.Xu");
    DRIVER_DESCRIPTION(_G_iWdtDrvNum, "WDT driver.");

    return  (_G_iWdtDrvNum > 0) ? (ERROR_NONE) : (PX_ERROR);
}
/*********************************************************************************************************
** ��������: imx6qWatchDogDevAdd
** ��������: ���� WATCH_DOG �豸
** �䡡��  : uiIndex           ID
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qWatchDogDevAdd (UINT  uiIndex)
{
	__PIMX6Q_WDT_CONTROLER   pWdtDev;
    CHAR                     cBuffer[64];

    if (uiIndex >= WDT_NUM) {
        printk(KERN_ERR "imx6qWdtDevAdd(): wdt index invalid!\n");
        return  (PX_ERROR);
    }
    pWdtDev = &_G_wdt[uiIndex];

    if(uiIndex == 0) {
        pWdtDev ->WDTC_BaseAddr = WDOG1_BASE_ADDR;
    } else {
        pWdtDev ->WDTC_BaseAddr = WDOG2_BASE_ADDR;
    }

    snprintf(cBuffer, sizeof(cBuffer), "/dev/wdt%d", uiIndex);

    if (API_IosDevAddEx(&pWdtDev->WDTC_devHdr, cBuffer, _G_iWdtDrvNum, DT_CHR) != ERROR_NONE) {
        printk(KERN_ERR "imx6qWdtDevAdd(): can not add device : %s.\n", strerror(errno));
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
