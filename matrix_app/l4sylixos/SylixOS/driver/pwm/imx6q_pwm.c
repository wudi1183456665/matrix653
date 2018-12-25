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
** ��   ��   ��: imx6q_pwm.c
**
** ��   ��   ��: Zhang.Xu (����)
**
** �ļ���������: 2016 �� 2 �� 21 ��
**
** ��        ��: imx6q �������� PWM �豸��������Ҫ����ʵ�ֱ���ƿ���
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include <linux/compat.h>

#include "sdk.h"
/*********************************************************************************************************
  PWM �������Ĵ�������
*********************************************************************************************************/
#define  PWM_PWMCR                    (0x00)                            /*  Control Register            */
#define  PWM_PWMSR                    (0x04)                            /*  Status Register             */
#define  PWM_PWMIR                    (0x08)                            /*  Interrupt Register          */
#define  PWM_PWMSAR                   (0x0C)                            /*  Sample Register             */
#define  PWM_PWMPR                    (0x10)                            /*  Period Register             */
#define  PWM_PWMCNR                   (0x14)                            /*  Counter Register            */

#define  PWM_NUM                      (4)
/*
 *  PWM ioctl ����������
 */
#define  PWM_FREN                     (105)                             /*  0 ~ 4069, 260 = 1Khz        */
#define  PWM_DUTY                     (106)                             /*  0 ~ 255,  255 = 100%        */
/*********************************************************************************************************
  PWM ���������Ͷ���
*********************************************************************************************************/
typedef struct {
    LW_DEV_HDR                  PWMC_devHdr;                            /* �����ǵ�һ���ṹ���Ա       */
    LW_LIST_LINE_HEADER         PWMC_fdNodeHeader;
	addr_t                      PWMC_BaseAddr;
} __IMX6Q_PWM_CONTROLER, *__PIMX6Q_PWM_CONTROLER;

UINT32 _G_iPwmDrvNum  = 0;

static __IMX6Q_PWM_CONTROLER  _G_pwm[PWM_NUM];
/*********************************************************************************************************
** ��������: imx6qPwmHwInit
** ��������: ��ʼ�� pwm �豸������
** �䡡��  : atBaseAddr             PWM �豸����������ַ
**           uiPrescale             PWM ����ʱ��Ԥ��Ƶ, ����2600 = 100Hz
**           uiPeriod               ռ�ձ����������ֵ
**           uiSample               ռ�ձ�������ֵ, ������ uiPeriod
** �䡡��  : �ļ��ڵ�
** ȫ�ֱ���:
** ����ģ��: �ο�iMX6_Platform_SDK �������� pwm.c/pwm_init ������д
*********************************************************************************************************/
static INT  imx6qPwmHwInit (addr_t atBaseAddr, UINT32 uiPrescale, UINT32 uiPeriod, UINT32 uiSample)
{
	UINT32  uiRegVal;

	/*
	 * Disable PWM first
	 */
	uiRegVal = readl(atBaseAddr + PWM_PWMCR);
	writel((uiRegVal & 0xFFFFFFFE), (atBaseAddr + PWM_PWMCR));

	/*
	 * ѡ��PWM��������ʱ��ԴΪIPGCLK��
	 */
	uiRegVal = (uiRegVal & 0xFFFCFFFF) | (0x1 << 16);

    /*
     * Set FIFO watermark to 4 empty slots
     */
	uiRegVal = uiRegVal | (0x3 << 26);

	/*
	 * ���� Ԥ��Ƶ��, ��������ֵΪ 0 ���Զ����Ը�������
	 */
	if(uiPrescale > 0x0000FFF0) {
		printk("pwm prescale too big \n");
		return (PX_ERROR);
	}
	if (uiPrescale) {
	    uiRegVal = (uiRegVal & 0xFFFF000F) | ((uiPrescale-1) << 4);
	}
	/*
	 *  ���������ƽ��λ,Ĭ�϶���������λ���������
	 */
	uiRegVal = (uiRegVal & 0xFFF3FFFF) | (0 << 18);
	writel(uiRegVal, (atBaseAddr + PWM_PWMCR));

	/*
	 *  ���� period ��ֵ
	 */
	writel(uiPeriod, (atBaseAddr + PWM_PWMPR));

	/*
	 *  ���� sample ��ֵ����sdk�л�������repeat�ȣ���Linux�н��������� sar �Ĵ���
	 *  ������ֵΪ 0 ���Զ���������
	 */
	if (uiSample) {
	    writel(uiSample, (atBaseAddr + PWM_PWMSAR));
	}
	return (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6qPwmEnable
** ��������: ʹ�� pwm �豸������
** �䡡��  : atBaseAddr             PWM �豸����������ַ
** �䡡��  : �ļ��ڵ�
** ȫ�ֱ���:
** ����ģ��: �ο�iMX6_Platform_SDK �������� pwm.c/pwm_init ������д
*********************************************************************************************************/
static INT  imx6qPwmEnable (addr_t atBaseAddr)
{
	UINT32  uiRegVal;

	uiRegVal = readl(atBaseAddr + PWM_PWMCR);
	uiRegVal |= 1;
	writel(uiRegVal, atBaseAddr + PWM_PWMCR);

	return (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6qPwmHwInit
** ��������: ���� pwm �豸������
** �䡡��  : atBaseAddr             PWM �豸����������ַ
** �䡡��  : �ļ��ڵ�
** ȫ�ֱ���:
** ����ģ��: �ο�iMX6_Platform_SDK �������� pwm.c/pwm_init ������д
*********************************************************************************************************/
static INT  imx6qPwmDisable (addr_t atBaseAddr)
{
	UINT32  uiRegVal;

	uiRegVal = readl(atBaseAddr + PWM_PWMCR);
	uiRegVal &= 0xFFFFFFFE;
	writel(uiRegVal, atBaseAddr + PWM_PWMCR);

	return (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6qPwmOpen
** ��������: �� PWM �豸
** �䡡��  : pDev                  �豸
**           pcName                �豸����
**           iFlags                ��־
**           iMode                 ģʽ
** �䡡��  : �ļ��ڵ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LONG  imx6qPwmOpen (__PIMX6Q_PWM_CONTROLER pPwmDev, PCHAR pcName, INT iFlags, INT iMode)
{
    PLW_FD_NODE   pFdNode;
    BOOL          bIsNew;
	addr_t        atBaseAddr;

    if (pcName == LW_NULL) {
        _ErrorHandle(ERROR_IO_NO_DEVICE_NAME_IN_PATH);
        return  (PX_ERROR);

    } else {
        pFdNode = API_IosFdNodeAdd(&pPwmDev->PWMC_fdNodeHeader, (dev_t)pPwmDev, 0,
                                   iFlags, iMode, 0, 0, 0, LW_NULL, &bIsNew);
        if (pFdNode == LW_NULL) {
            printk(KERN_ERR "imx6qWdtOpen(): failed to add fd node!\n");
            return  (PX_ERROR);
        }

        if (LW_DEV_INC_USE_COUNT(&pPwmDev->PWMC_devHdr) == 1) {
            /*
             * ��ʼ�� PWM ��ʱ����Ĭ�����ò�������
             */
            atBaseAddr = pPwmDev->PWMC_BaseAddr;
            imx6qPwmHwInit(atBaseAddr, 260, 256, 200);
            imx6qPwmEnable(atBaseAddr);

            return  ((LONG)pFdNode);
        }
    }
    /*
     * ����豸��ʧ�ܽ������²���
     */
    if (pFdNode) {
        API_IosFdNodeDec(&pPwmDev->PWMC_fdNodeHeader, pFdNode, NULL);
        pFdNode = LW_NULL;
    }

    LW_DEV_DEC_USE_COUNT(&pPwmDev->PWMC_devHdr);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: imx6qPwmClose
** ��������: �ر� PWM �豸
** �䡡��  : pFdEntry              �ļ��ṹ
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT imx6qPwmClose (PLW_FD_ENTRY   pFdEntry)
{
	__PIMX6Q_PWM_CONTROLER  pPwmDev = (__PIMX6Q_PWM_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;
	addr_t                  atBaseAddr = pPwmDev->PWMC_BaseAddr;

	imx6qPwmDisable(atBaseAddr);

    LW_DEV_DEC_USE_COUNT(&pPwmDev->PWMC_devHdr);
    return (PX_ERROR);
}
/*********************************************************************************************************
** ��������: imx6qPwmIoctl
** ��������: ���� PWM �豸
** �䡡��  : pFdEntry              �ļ��ṹ
**           iCmd                  ����
**           lArg                  ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qPwmIoctl (PLW_FD_ENTRY  pFdEntry, INT  iCmd, LONG lArg)
{
	__PIMX6Q_PWM_CONTROLER  pPwmDev = (__PIMX6Q_PWM_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;
	addr_t                  atBaseAddr = pPwmDev->PWMC_BaseAddr;
    UINT32                  usRegVal = (UINT32)lArg;

    switch (iCmd) {
    case PWM_FREN:                                                       /*  PWM ��������                */
        imx6qPwmHwInit(atBaseAddr, usRegVal, 256, 0);
        imx6qPwmEnable(atBaseAddr);
        return  (ERROR_NONE);
    case PWM_DUTY:                                                       /*  ����ռ�ձ�                  */
        imx6qPwmHwInit(atBaseAddr, 0, 256, usRegVal);
        imx6qPwmEnable(atBaseAddr);
        return  (ERROR_NONE);
    default:
        return  (PX_ERROR);
    }

}
/*********************************************************************************************************
** ��������: imx6qPwmWrite
** ��������: ���� write ������ʵ�ֿ��Ź�ι�����ܣ�д���ַ�Ϊ0xAAAA5555
** �䡡��  : peeprom      eeprom �ṹ
**           pvBuf        д����
**           stLen        д���ݳ���
** �䡡��  : �ɹ���ȡ�����ֽ�����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ssize_t  imx6qPwmWrite (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
   /*
    * PWM �豸����Ҫ�κ�д�����
    */

    return  (stLen);
}
/*********************************************************************************************************
** ��������: imx6qPwmRead
** ��������: ���� write ����
** �䡡��  :
** �䡡��  : �ɹ���ȡ�����ֽ�����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ssize_t  imx6qPwmRead (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
	__PIMX6Q_PWM_CONTROLER  pPwmDev = (__PIMX6Q_PWM_CONTROLER)pFdentry->FDENTRY_pdevhdrHdr;
	addr_t                  atBaseAddr = pPwmDev->PWMC_BaseAddr;

    /*
     * ���ص�ǰ����ֵ
     */
    *(INT *)pvBuf = readl(atBaseAddr + PWM_PWMCNR);

    return  (4);
}
/*********************************************************************************************************
** ��������: imx6qPwmDrv
** ��������: ��װ PWM ����
** �䡡��  : NONE
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qPwmDrv (VOID)
{
    struct file_operations  fileop;

    if (_G_iPwmDrvNum) {
        return  (ERROR_NONE);
    }

    lib_memset(&fileop, 0, sizeof(struct file_operations));

    fileop.owner     = THIS_MODULE;
    fileop.fo_create = imx6qPwmOpen;
    fileop.fo_open   = imx6qPwmOpen;
    fileop.fo_close  = imx6qPwmClose;
    fileop.fo_ioctl  = imx6qPwmIoctl;
    fileop.fo_write  = imx6qPwmWrite;
    fileop.fo_read   = imx6qPwmRead;

    _G_iPwmDrvNum = iosDrvInstallEx2(&fileop, LW_DRV_TYPE_NEW_1);

    DRIVER_LICENSE(_G_iPwmDrvNum,     "Dual BSD/GPL->Ver 1.0");
    DRIVER_AUTHOR(_G_iPwmDrvNum,      "Zhang.Xu");
    DRIVER_DESCRIPTION(_G_iPwmDrvNum, "PWM driver.");

    return  (_G_iPwmDrvNum > 0) ? (ERROR_NONE) : (PX_ERROR);
}
/*********************************************************************************************************
** ��������: imx6qPwmDevAdd
** ��������: ���� PWM �豸
** �䡡��  : uiIndex           ID
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imx6qPwmDevAdd (UINT  uiIndex)
{
	__PIMX6Q_PWM_CONTROLER   pPwmDev;
	CPCHAR                   pcBuffer;

    if (uiIndex >= PWM_NUM) {
        printk(KERN_ERR "imx6qWdtDevAdd(): wdt index invalid!\n");
        return  (PX_ERROR);
    }
    pPwmDev = &_G_pwm[uiIndex];

    /*
     *  ԭ��ʹ�� snprintf(cBuffer, sizeof(cBuffer), "/dev/wdt%d", uiIndex);ʵ�֣�����Ϊ�˾������Ͷ�
     *  C ���ʹ�ã������������в����ñ�׼ C �ĺ���;
     *  �� PWM �����ã���Ҫ����ԭ��ͼ���д���Ĭ�ϲ�Ҫ���� Pinmux ������ɵ�·���쳣
     */
    switch (uiIndex) {
    case 0:
    	pcBuffer = "/dev/pwm0";
        pPwmDev ->PWMC_BaseAddr = PWM1_BASE_ADDR;
        pwm1_iomux_config();
        break;

    case 1:
    	pcBuffer = "/dev/pwm1";
        pPwmDev ->PWMC_BaseAddr = PWM2_BASE_ADDR;
        pwm2_iomux_config();
        break;

    case 2:
    	pcBuffer = "/dev/pwm2";
        pPwmDev ->PWMC_BaseAddr = PWM3_BASE_ADDR;
        pwm3_iomux_config();
        break;

    case 3:
    	pcBuffer = "/dev/pwm3";
        pPwmDev ->PWMC_BaseAddr = PWM4_BASE_ADDR;
        pwm4_iomux_config();
        break;
    }

    if (API_IosDevAddEx(&pPwmDev->PWMC_devHdr, pcBuffer, _G_iPwmDrvNum, DT_CHR) != ERROR_NONE) {
        printk(KERN_ERR "imx6qWdtDevAdd(): can not add device : %s.\n", strerror(errno));
        return  (PX_ERROR);
    }
    return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
