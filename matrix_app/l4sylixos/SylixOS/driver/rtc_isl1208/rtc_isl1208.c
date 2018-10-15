/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: rtc_isl1208.c
**
** 创   建   人: Zhang.Xu (张旭)
**
** 文件创建日期: 2016 年 06 月 01 日
**
** 描        述: rtc_isl1208 RTC 驱动
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "time.h"
#include "rtc_isl1208/rtc_isl1208.h"
/*********************************************************************************************************
  定义
*********************************************************************************************************/
#define __RTC_CONTROLER_NR          (1)
#define __RTC_24_HOURS               1
/*********************************************************************************************************
  ISL1208 寄存器定义，参考其Linux驱动。
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
  RTC 控制器类型定义
*********************************************************************************************************/
typedef struct {
    PLW_I2C_DEVICE      RTCC_pI2cDevice;
    CPCHAR              RTCC_pcI2cBusName;
} __ISL1208_RTC_CONTROLER, *__PISL1208_RTC_CONTROLER;
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
static __ISL1208_RTC_CONTROLER  _G_isl1208RtcControlers[__RTC_CONTROLER_NR];
/*********************************************************************************************************
** 函数名称: rtcBinToBcd
** 功能描述: BIN 码转 BCD 码
** 输　入  : ucBin    BIN 码
** 输　出  : return   BCD 码
** 全局变量:
** 调用模块:
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
** 函数名称: rtcBcdToBin
** 功能描述: BCD 码转 BIN 码
** 输　入  : ucBcd    BCD 码
** 输　出  : return   BIN 码
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static UINT8  rtcBcdToBin (UINT8  ucBcd)
{
    UINT8  ucBin;

    ucBin = ((ucBcd & 0xF0) >> 4) * 10 + (ucBcd & 0x0F);

    return  (ucBin);
}
/*********************************************************************************************************
** 函数名称: isl1208SetRegs
** 功能描述: isl1208  寄存器写函数
** 输　入  : pI2cDev  i2c设备
**           reg      寄存器地址
**           buf      需要写入寄存器的数据
**           len      写入数据长度
** 输　出  : 返回 0 表示函数执行成功
** 全局变量:
** 调用模块:
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
** 函数名称: isl1208SetRegs
** 功能描述: isl1208  寄存器读函数
** 输　入  : pI2cDev  i2c设备
**           reg      寄存器地址
**           buf      数据接收缓冲区
**           len      需要读取的数据长度
** 输　出  : 返回 0 表示函数执行成功
** 全局变量:
** 调用模块:
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
** 函数名称: isl1208RtcSetTime
** 功能描述: 设置 RTC 时间
** 输　入  : pRtcFuncs         RTC 驱动程序
**           pTimeNow          当前时间
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  isl1208RtcSetTime (PLW_RTC_FUNCS  pRtcFuncs, time_t  *pTimeNow)
{
	__PISL1208_RTC_CONTROLER  pRtcControler = &_G_isl1208RtcControlers[0];
    struct tm                 tmNow;
    UINT8                     aucBuffer[ISL1208_RTC_SECTION_LEN] = { 0, };
    UINT8                     ucCtlDat[1], ucRegDat[1];

    gmtime_r(pTimeNow, &tmNow);                                         /*  转换成 tm 时间格式          */

    aucBuffer[ISL1208_REG_YR] = rtcBinToBcd(tmNow.tm_year % 100);
    aucBuffer[ISL1208_REG_MO] = rtcBinToBcd(tmNow.tm_mon + 1);
    aucBuffer[ISL1208_REG_DT] = rtcBinToBcd(tmNow.tm_mday);
    aucBuffer[ISL1208_REG_DW] = rtcBinToBcd(tmNow.tm_wday & 7);
    aucBuffer[ISL1208_REG_HR] = rtcBinToBcd(tmNow.tm_hour);
    aucBuffer[ISL1208_REG_MN] = rtcBinToBcd(tmNow.tm_min);
    aucBuffer[ISL1208_REG_SC] = rtcBinToBcd(tmNow.tm_sec);

    /*
     * 读取 RTC 的状态值 设置 WRTC
     */
    isl1208ReadReg(pRtcControler->RTCC_pI2cDevice, ISL1208_REG_SR, ucRegDat, 1);

    ucCtlDat[0] = ucRegDat[0] | ISL1208_REG_SR_WRTC;
    isl1208SetRegs(pRtcControler->RTCC_pI2cDevice, ISL1208_REG_SR, ucCtlDat, 1);

    /*
     * write RTC registers
     */
    isl1208SetRegs(pRtcControler->RTCC_pI2cDevice, 0, aucBuffer, ISL1208_RTC_SECTION_LEN);

    /*
     * 复位 WRTC
     */
    ucCtlDat[0] = ucRegDat[0] & ~ISL1208_REG_SR_WRTC;
    isl1208SetRegs(pRtcControler->RTCC_pI2cDevice, ISL1208_REG_SR, ucCtlDat, 1);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: isl1208RtcGetTime
** 功能描述: 获取 RTC 时间
** 输　入  : pRtcFuncs         RTC 驱动程序
**           pTimeNow          当前时间
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
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
** 函数名称: isl1208RtcHwInit
** 功能描述: 读取 RTC 的时间然后重新设置, 调试中发现ISL1208 若无电池, 需要设置时间后方能工作. 若有电池
**           RTC 会一直工作, 因此这里考虑到了此种设置方法, 这种方法会影响启动速度.
**           但考虑到实际使用时若无电池, 设置RTC也无意义, 此部分代码删除更为合理.
** 输　入  :
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
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
** 函数名称: isl1208RtcInit
** 功能描述: 初始化 RTC 设备
** 输　入  : pRtcControler     RTC 控制器
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  isl1208RtcInit (VOID)
{
	__PISL1208_RTC_CONTROLER  pRtcControler = &_G_isl1208RtcControlers[0];

    if (!pRtcControler->RTCC_pI2cDevice) {
        pRtcControler->RTCC_pI2cDevice = API_I2cDeviceCreate(pRtcControler->RTCC_pcI2cBusName,
                                                             "/dev/rtc",
                                                             0x6F,      /*  ISL 芯片的设备地址          */
                                                             0);
        if (pRtcControler->RTCC_pI2cDevice) {
        	isl1208RtcHwInit();
        }
    }
}
/*********************************************************************************************************
  isl1208 RTC 驱动程序
*********************************************************************************************************/
static LW_RTC_FUNCS     _G_isl1208RtcFuncs = {
        isl1208RtcInit,
        isl1208RtcSetTime,
        isl1208RtcGetTime,
        LW_NULL
};
/*********************************************************************************************************
** 函数名称: s35390aRtcGetFuncs
** 功能描述: 获取 RTC 驱动程序
** 输　入  : NONE
** 输　出  : RTC 驱动程序
** 全局变量:
** 调用模块:
*********************************************************************************************************/
PLW_RTC_FUNCS  isl1208RtcGetFuncs (CPCHAR  pcI2cBusName)
{
	_G_isl1208RtcControlers[0].RTCC_pcI2cBusName = pcI2cBusName;

    return  (&_G_isl1208RtcFuncs);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
