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
** ��   ��   ��: rtc_isl1208.c
**
** ��   ��   ��: Zhang.Xu (����)
**
** �ļ���������: 2016 �� 06 �� 01 ��
**
** ��        ��: rtc_isl1208 RTC ����
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "time.h"
#include "rtc_isl1208/rtc_isl1208.h"
/*********************************************************************************************************
  ����
*********************************************************************************************************/
#define __RTC_CONTROLER_NR          (1)
#define __RTC_24_HOURS               1
/*********************************************************************************************************
  ISL1208 �Ĵ������壬�ο���Linux������
*********************************************************************************************************/
/*
 * rtc section
 */
#define ISL1208_REG_SC                   (0x00)
#define ISL1208_REG_MN                   (0x01)
#define ISL1208_REG_HR                   (0x02)
#define ISL1208_REG_HR_MIL               (1<<7)                         /* 24h/12h mode                 */
#define ISL1208_REG_HR_PM                (1<<5)                         /* PM/AM bit in 12h mode        */
#define ISL1208_REG_DT                   (0x03)
#define ISL1208_REG_MO                   (0x04)
#define ISL1208_REG_YR                   (0x05)
#define ISL1208_REG_DW                   (0x06)
#define ISL1208_RTC_SECTION_LEN          (7   )

/*
 * control/status section
 */
#define ISL1208_REG_SR                   (0x07)
#define ISL1208_REG_SR_ARST              (1<<7)                         /* auto reset                   */
#define ISL1208_REG_SR_XTOSCB            (1<<6)                         /* crystal oscillator           */
#define ISL1208_REG_SR_WRTC              (1<<4)                         /* write rtc                    */
#define ISL1208_REG_SR_ALM               (1<<2)                         /* alarm                        */
#define ISL1208_REG_SR_BAT               (1<<1)                         /* battery                      */
#define ISL1208_REG_SR_RTCF              (1<<0)                         /* rtc fail                     */
#define ISL1208_REG_INT                  (0x08)
#define ISL1208_REG_INT_ALME             (1<<6)                         /* alarm enable                 */
#define ISL1208_REG_INT_IM               (1<<7)                         /* interrupt/alarm mode         */
#define ISL1208_REG_09                   (0x09)                         /* reserved                     */
#define ISL1208_REG_ATR                  (0x0a)
#define ISL1208_REG_DTR                  (0x0b)

/*
 * alarm section
 */
#define ISL1208_REG_SCA                  (0x0c)
#define ISL1208_REG_MNA                  (0x0d)
#define ISL1208_REG_HRA                  (0x0e)
#define ISL1208_REG_DTA                  (0x0f)
#define ISL1208_REG_MOA                  (0x10)
#define ISL1208_REG_DWA                  (0x11)
#define ISL1208_ALARM_SECTION_LEN        (6   )

/*
 * user section
 */
#define ISL1208_REG_USR1                 (0x12)
#define ISL1208_REG_USR2                 (0x13)
#define ISL1208_USR_SECTION_LEN          (2   )
/*********************************************************************************************************
  RTC ���������Ͷ���
*********************************************************************************************************/
typedef struct {
    PLW_I2C_DEVICE      RTCC_pI2cDevice;
    CPCHAR              RTCC_pcI2cBusName;
} __ISL1208_RTC_CONTROLER, *__PISL1208_RTC_CONTROLER;
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
static __ISL1208_RTC_CONTROLER  _G_isl1208RtcControlers[__RTC_CONTROLER_NR];
/*********************************************************************************************************
** ��������: rtcBinToBcd
** ��������: BIN ��ת BCD ��
** �䡡��  : ucBin    BIN ��
** �䡡��  : return   BCD ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT8  rtcBinToBcd (UINT8  ucBin)
{
    UINT8  ucBcd;

    if (ucBin >= 100) {
        return  (0);
    }

    ucBcd  = ucBin % 10;
    ucBcd |= (ucBin / 10) << 4;

    return  (ucBcd);
}
/*********************************************************************************************************
** ��������: rtcBcdToBin
** ��������: BCD ��ת BIN ��
** �䡡��  : ucBcd    BCD ��
** �䡡��  : return   BIN ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT8  rtcBcdToBin (UINT8  ucBcd)
{
    UINT8  ucBin;

    ucBin = ((ucBcd & 0xF0) >> 4) * 10 + (ucBcd & 0x0F);

    return  (ucBin);
}
/*********************************************************************************************************
** ��������: isl1208SetRegs
** ��������: isl1208  �Ĵ���д����
** �䡡��  : pI2cDev  i2c�豸
**           reg      �Ĵ�����ַ
**           buf      ��Ҫд��Ĵ���������
**           len      д�����ݳ���
** �䡡��  : ���� 0 ��ʾ����ִ�гɹ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static int isl1208SetRegs(PLW_I2C_DEVICE  pI2cDev, UINT8 ucReg, UINT8 *ucBuf, UINT uiLen)
{
    INT             iError;

	UINT8  i2cBuf[ISL1208_REG_USR2 + 2];

    LW_I2C_MESSAGE  i2cMsgs[1] = {
        {
            .I2CMSG_usAddr    = pI2cDev->I2CDEV_usAddr,
            .I2CMSG_usFlag    = 0,
            .I2CMSG_usLen     = uiLen + 1,
            .I2CMSG_pucBuffer = i2cBuf,
        },
    };

    i2cBuf[0] = ucReg;
	memcpy(&i2cBuf[1], &ucBuf[0], uiLen);

    iError = API_I2cDeviceTransfer(pI2cDev, i2cMsgs, 1);
    if (iError < 0) {
        printk(KERN_ERR "isl1208SetRegs(): failed to i2c transfer!\n");
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: isl1208SetRegs
** ��������: isl1208  �Ĵ���������
** �䡡��  : pI2cDev  i2c�豸
**           reg      �Ĵ�����ַ
**           buf      ���ݽ��ջ�����
**           len      ��Ҫ��ȡ�����ݳ���
** �䡡��  : ���� 0 ��ʾ����ִ�гɹ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static int isl1208ReadReg(PLW_I2C_DEVICE  pI2cDev, UINT8 ucReg, UINT8 *ucBuf, UINT uiLen)
{
    INT      iError;

    LW_I2C_MESSAGE  i2cMsgs[2] = {
        {
            .I2CMSG_usAddr    = pI2cDev->I2CDEV_usAddr,
            .I2CMSG_usFlag    = 0,
            .I2CMSG_usLen     = 1,
            .I2CMSG_pucBuffer = &ucReg,
        }, {
            .I2CMSG_usAddr    = pI2cDev->I2CDEV_usAddr,
            .I2CMSG_usFlag    = LW_I2C_M_RD,
            .I2CMSG_usLen     = uiLen,
            .I2CMSG_pucBuffer = ucBuf,
        }
    };

    iError = API_I2cDeviceTransfer(pI2cDev, i2cMsgs, 2);
    if (iError < 0) {
        printk(KERN_ERR "isl1208ReadReg(): failed to i2c transfer!\n");
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: isl1208RtcSetTime
** ��������: ���� RTC ʱ��
** �䡡��  : pRtcFuncs         RTC ��������
**           pTimeNow          ��ǰʱ��
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  isl1208RtcSetTime (PLW_RTC_FUNCS  pRtcFuncs, time_t  *pTimeNow)
{
	__PISL1208_RTC_CONTROLER  pRtcControler = &_G_isl1208RtcControlers[0];
    struct tm                 tmNow;
    UINT8                     aucBuffer[ISL1208_RTC_SECTION_LEN] = { 0, };
    UINT8                     ucCtlDat[1], ucRegDat[1];

    gmtime_r(pTimeNow, &tmNow);                                         /*  ת���� tm ʱ���ʽ          */

    aucBuffer[ISL1208_REG_YR] = rtcBinToBcd(tmNow.tm_year % 100);
    aucBuffer[ISL1208_REG_MO] = rtcBinToBcd(tmNow.tm_mon + 1);
    aucBuffer[ISL1208_REG_DT] = rtcBinToBcd(tmNow.tm_mday);
    aucBuffer[ISL1208_REG_DW] = rtcBinToBcd(tmNow.tm_wday & 7);
    aucBuffer[ISL1208_REG_HR] = rtcBinToBcd(tmNow.tm_hour);
    aucBuffer[ISL1208_REG_MN] = rtcBinToBcd(tmNow.tm_min);
    aucBuffer[ISL1208_REG_SC] = rtcBinToBcd(tmNow.tm_sec);

    /*
     * ��ȡ RTC ��״ֵ̬ ���� WRTC
     */
    isl1208ReadReg(pRtcControler->RTCC_pI2cDevice, ISL1208_REG_SR, ucRegDat, 1);

    ucCtlDat[0] = ucRegDat[0] | ISL1208_REG_SR_WRTC;
    isl1208SetRegs(pRtcControler->RTCC_pI2cDevice, ISL1208_REG_SR, ucCtlDat, 1);

    /*
     * write RTC registers
     */
    isl1208SetRegs(pRtcControler->RTCC_pI2cDevice, 0, aucBuffer, ISL1208_RTC_SECTION_LEN);

    /*
     * ��λ WRTC
     */
    ucCtlDat[0] = ucRegDat[0] & ~ISL1208_REG_SR_WRTC;
    isl1208SetRegs(pRtcControler->RTCC_pI2cDevice, ISL1208_REG_SR, ucCtlDat, 1);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: isl1208RtcGetTime
** ��������: ��ȡ RTC ʱ��
** �䡡��  : pRtcFuncs         RTC ��������
**           pTimeNow          ��ǰʱ��
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  isl1208RtcGetTime (PLW_RTC_FUNCS  pRtcFuncs, time_t  *pTimeNow)
{
	__PISL1208_RTC_CONTROLER  pRtcControler = &_G_isl1208RtcControlers[0];
    struct tm                 tmNow;
    UINT8                     ucValue;
    UINT8                     aucBuffer[ISL1208_RTC_SECTION_LEN] = { 0, };

    isl1208ReadReg(pRtcControler->RTCC_pI2cDevice, 0, aucBuffer, ISL1208_RTC_SECTION_LEN);

    tmNow.tm_sec  = rtcBcdToBin(aucBuffer[ISL1208_REG_SC]);
    tmNow.tm_min  = rtcBcdToBin(aucBuffer[ISL1208_REG_MN]);

    ucValue = aucBuffer[ISL1208_REG_HR];

    if (ucValue & ISL1208_REG_HR_MIL) {                                 /*  24h format                  */
        tmNow.tm_hour = rtcBcdToBin(ucValue & 0x3F);
    } else {                                                            /*  12h format                  */
        if (ucValue & ISL1208_REG_HR_PM) {                              /*  PM flag set                 */
            tmNow.tm_hour = 12 + rtcBcdToBin(ucValue & 0x1F);
        } else {
            tmNow.tm_hour = rtcBcdToBin(ucValue & 0x1F);
        }
    }

    tmNow.tm_wday  = rtcBcdToBin(aucBuffer[ISL1208_REG_DW]);
    tmNow.tm_mday  = rtcBcdToBin(aucBuffer[ISL1208_REG_DT]);
    tmNow.tm_mon   = rtcBcdToBin(aucBuffer[ISL1208_REG_MO]) - 1;
    tmNow.tm_year  = rtcBcdToBin(aucBuffer[ISL1208_REG_YR]) + 100;
    tmNow.tm_yday  = 0;
    tmNow.tm_isdst = 0;

    if (pTimeNow) {
        *pTimeNow = timegm(&tmNow);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: isl1208RtcHwInit
** ��������: ��ȡ RTC ��ʱ��Ȼ����������, �����з���ISL1208 ���޵��, ��Ҫ����ʱ����ܹ���. ���е��
**           RTC ��һֱ����, ������￼�ǵ��˴������÷���, ���ַ�����Ӱ�������ٶ�.
**           �����ǵ�ʵ��ʹ��ʱ���޵��, ����RTCҲ������, �˲��ִ���ɾ����Ϊ����.
** �䡡��  :
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  isl1208RtcHwInit (VOID)
{
#if 0
	PLW_RTC_FUNCS  pRtcFuncs = NULL;
	time_t         pTimeInit;

	isl1208RtcGetTime(pRtcFuncs, &pTimeInit);
	isl1208RtcSetTime (pRtcFuncs, &pTimeInit);
#endif
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: isl1208RtcInit
** ��������: ��ʼ�� RTC �豸
** �䡡��  : pRtcControler     RTC ������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  isl1208RtcInit (VOID)
{
	__PISL1208_RTC_CONTROLER  pRtcControler = &_G_isl1208RtcControlers[0];

    if (!pRtcControler->RTCC_pI2cDevice) {
        pRtcControler->RTCC_pI2cDevice = API_I2cDeviceCreate(pRtcControler->RTCC_pcI2cBusName,
                                                             "/dev/rtc",
                                                             0x6F,      /*  ISL оƬ���豸��ַ          */
                                                             0);
        if (pRtcControler->RTCC_pI2cDevice) {
        	isl1208RtcHwInit();
        }
    }
}
/*********************************************************************************************************
  isl1208 RTC ��������
*********************************************************************************************************/
static LW_RTC_FUNCS     _G_isl1208RtcFuncs = {
        isl1208RtcInit,
        isl1208RtcSetTime,
        isl1208RtcGetTime,
        LW_NULL
};
/*********************************************************************************************************
** ��������: s35390aRtcGetFuncs
** ��������: ��ȡ RTC ��������
** �䡡��  : NONE
** �䡡��  : RTC ��������
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PLW_RTC_FUNCS  isl1208RtcGetFuncs (CPCHAR  pcI2cBusName)
{
	_G_isl1208RtcControlers[0].RTCC_pcI2cBusName = pcI2cBusName;

    return  (&_G_isl1208RtcFuncs);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
