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
** ��   ��   ��: imx6q_rtc.c
**
** ��   ��   ��: Zhang.Xu (����)
**
** �ļ���������: 2015 �� 11 �� 28 ��
**
** ��        ��: I.MX6Q ������ RTC ����
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include <linux/compat.h>

#include "sdk.h"
#include "rtc/imx6q_rtc.h"
/*********************************************************************************************************
  ���壬��Linux�µ������ļ� rtc-snvs.c ��������
*********************************************************************************************************/
#define	SNVS_HPSR	            (0x14)	                                /* HP Status Register           */
#define	SNVS_LPCR	            (0x38)	                                /* LP Control Register          */
#define	SNVS_LPSR	            (0x4C)	                                /* LP Status Register           */
#define	SNVS_LPSRTCMR	        (0x50)	                                /* LP Secure RTC MSB            */
#define	SNVS_LPSRTCLR	        (0x54)	                                /* LP Secure RTC LSB            */
#define	SNVS_LPTAR	            (0x58)	                                /* LP Time Alarm Register       */
#define	SNVS_LPSMCMR	        (0x5C)	                                /* LP SMC MSB                   */
#define	SNVS_LPSMCLR	        (0x60)	                                /* LP SMC LSB                   */
#define	SNVS_LPPGDR	            (0x64)	                                /* LP Power Glitch Detector     */
#define	SNVS_LPGPR	            (0x68)	                                /* LP General Purpose           */

#define	SNVS_LPCR_SRTC_ENV	    (1 << 0)
#define	SNVS_LPPGDR_INIT	    (0x41736166)

/*********************************************************************************************************
  RTC ���������Ͷ���
*********************************************************************************************************/
typedef struct {
    addr_t     RTCC_ulPhyAddrBase;                                      /*  �����ַ����ַ              */
    BOOL       RTCC_bIsInit;                                            /*  �Ƿ��Ѿ���ʼ��              */
} __IMX6Q_RTC_CTRL, *__PIMX6Q_RTC_CTRL;

static __IMX6Q_RTC_CTRL  _G_imx6qRtcCtrl  =
	{
		.RTCC_ulPhyAddrBase = SNVS_BASE_ADDR,
		.RTCC_bIsInit       = 0,
	};
/*********************************************************************************************************
** ��������: imx6qRtcSetTime
** ��������: ���� RTC ʱ��
** �䡡��  : pRtcFuncs         RTC ��������
**           pTimeNow          ��ǰʱ��
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qRtcSetTime (PLW_RTC_FUNCS  pRtcFuncs, time_t  *pTimeNow)
{
	addr_t  atRtcAddr =  _G_imx6qRtcCtrl.RTCC_ulPhyAddrBase;
	UINT64  ulTimeBack;
	UINT32  uiRegVal;

	ulTimeBack = *pTimeNow;

	/*
	 * Disable RTC first, �� RTC ����״̬���޷����� RTC ���ã������Ҫ���Ƚ�ֹ RTC
	 */
	uiRegVal = readl(atRtcAddr + SNVS_LPCR) & ~SNVS_LPCR_SRTC_ENV;
	writel(uiRegVal, atRtcAddr + SNVS_LPCR);
	while (readl(atRtcAddr + SNVS_LPCR) & SNVS_LPCR_SRTC_ENV) {

	};

	/*
	 * Write 32-bit time to 47-bit timer, leaving 15 LSBs blank
	 */
	writel(ulTimeBack << 15, atRtcAddr + SNVS_LPSRTCLR);
	writel(ulTimeBack >> (32 - 15), atRtcAddr + SNVS_LPSRTCMR);

	/*
	 * Enable RTC again
	 */
	writel(uiRegVal | SNVS_LPCR_SRTC_ENV, atRtcAddr + SNVS_LPCR);
	while (!(readl(atRtcAddr + SNVS_LPCR) & SNVS_LPCR_SRTC_ENV)) {

	};

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6qRtcGetTime
** ��������: ��ȡ RTC ʱ��
** �䡡��  : pRtcFuncs         RTC ��������
**           pTimeNow          ��ǰʱ��
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qRtcGetTime (PLW_RTC_FUNCS  pRtcFuncs, time_t  *pTimeNow)
{
	/*
	 * linux �µ������ǽ�����ת���� tm �ṹ�壬
	 */
	addr_t  atRtcAddr = _G_imx6qRtcCtrl.RTCC_ulPhyAddrBase;
	UINT64  ulRegVal1, ulRegVal2;

	/*
	 * ���ζ�ȡ RTC ʱ�ӵĸ�λ���ݺ͵�λ����,Ϊ�����ڼ������仯�׶ζ�ȡ��Ч���ݣ�
	 * ������м򵥵�У�鴦��
	 */
	do {
		ulRegVal1 = readl(atRtcAddr + SNVS_LPSRTCMR);
		ulRegVal1 <<= 32;
		ulRegVal1 |= readl(atRtcAddr + SNVS_LPSRTCLR);

		ulRegVal2 = readl(atRtcAddr + SNVS_LPSRTCMR);
		ulRegVal2 <<= 32;
		ulRegVal2 |= readl(atRtcAddr + SNVS_LPSRTCLR);
	} while (ulRegVal1 != ulRegVal2);

	/*
	 * Convert 47-bit counter to 32-bit raw second count ������ת����������
	 * ����ת��������ݻ����� SNVS_LPSRTCMR �� 16bits ��Ϣ����������
	 */
	*pTimeNow = (ulRegVal1 >> 15) & 0x00000000FFFFFFFF;

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  imx6q ������ RTC ��������
*********************************************************************************************************/
static LW_RTC_FUNCS     _G_imx6qRtcFuncs = {
        LW_NULL,
        imx6qRtcSetTime,
        imx6qRtcGetTime,
        LW_NULL
};
/*********************************************************************************************************
** ��������: __imx6qRtcInit
** ��������: ��ʼ�� RTC �豸
** �䡡��  : pRtcControler     RTC ������
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��: �ο���Linux snvs_rtc_probe ����
*********************************************************************************************************/
static INT  imx6qRtcInit (__PIMX6Q_RTC_CTRL  _imx6qRtcCtrlr)
{
    addr_t  atRtcAddr = _imx6qRtcCtrlr->RTCC_ulPhyAddrBase;
    UINT32  uiRegVal;

    /*
     * initialize glitch detect
     */
    writel(SNVS_LPPGDR_INIT, atRtcAddr + SNVS_LPPGDR);
    udelay(100);

    /*
     * clear lp interrupt status
     */
    writel(0xFFFFFFFF, atRtcAddr + SNVS_LPSR);

    /*
     * Enable RTC
     */
    uiRegVal = readl(atRtcAddr + SNVS_LPCR);
    if ((uiRegVal & SNVS_LPCR_SRTC_ENV) == 0) {
    	writel(uiRegVal | SNVS_LPCR_SRTC_ENV, atRtcAddr + SNVS_LPCR);
    }
    udelay(100);

    writel(0xFFFFFFFF, atRtcAddr + SNVS_LPSR);
    udelay(100);

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: rtcGetFuncs
** ��������: ��ȡ RTC ��������
** �䡡��  : NONE
** �䡡��  : RTC ��������
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PLW_RTC_FUNCS  rtcGetFuncs (VOID)
{
	__PIMX6Q_RTC_CTRL  imx6qRtcCtl = &_G_imx6qRtcCtrl;

    if (!imx6qRtcCtl->RTCC_bIsInit) {

        if (imx6qRtcInit(imx6qRtcCtl) != ERROR_NONE) {
            printk(KERN_ERR "rtcGetFuncs(): failed to init!\n");
            goto  __error_handle;
        }
    }
    return  (&_G_imx6qRtcFuncs);

    __error_handle:

    return  (LW_NULL);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
