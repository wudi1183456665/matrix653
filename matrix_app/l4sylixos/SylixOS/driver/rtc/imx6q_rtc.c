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
** 文   件   名: imx6q_rtc.c
**
** 创   建   人: Zhang.Xu (张旭)
**
** 文件创建日期: 2015 年 11 月 28 日
**
** 描        述: I.MX6Q 处理器 RTC 驱动
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include <linux/compat.h>

#include "sdk.h"
#include "rtc/imx6q_rtc.h"
/*********************************************************************************************************
  定义，从Linux下的驱动文件 rtc-snvs.c 拷贝而来
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
  RTC 控制器类型定义
*********************************************************************************************************/
typedef struct {
    addr_t     RTCC_ulPhyAddrBase;                                      /*  物理地址基地址              */
    BOOL       RTCC_bIsInit;                                            /*  是否已经初始化              */
} __IMX6Q_RTC_CTRL, *__PIMX6Q_RTC_CTRL;

static __IMX6Q_RTC_CTRL  _G_imx6qRtcCtrl  =
	{
		.RTCC_ulPhyAddrBase = SNVS_BASE_ADDR,
		.RTCC_bIsInit       = 0,
	};
/*********************************************************************************************************
** 函数名称: imx6qRtcSetTime
** 功能描述: 设置 RTC 时间
** 输　入  : pRtcFuncs         RTC 驱动程序
**           pTimeNow          当前时间
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qRtcSetTime (PLW_RTC_FUNCS  pRtcFuncs, time_t  *pTimeNow)
{
	addr_t  atRtcAddr =  _G_imx6qRtcCtrl.RTCC_ulPhyAddrBase;
	UINT64  ulTimeBack;
	UINT32  uiRegVal;

	ulTimeBack = *pTimeNow;

	/*
	 * Disable RTC first, 在 RTC 工作状态，无法进行 RTC 设置，因此需要首先禁止 RTC
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
** 函数名称: imx6qRtcGetTime
** 功能描述: 获取 RTC 时间
** 输　入  : pRtcFuncs         RTC 驱动程序
**           pTimeNow          当前时间
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qRtcGetTime (PLW_RTC_FUNCS  pRtcFuncs, time_t  *pTimeNow)
{
	/*
	 * linux 下的驱动是将数据转换成 tm 结构体，
	 */
	addr_t  atRtcAddr = _G_imx6qRtcCtrl.RTCC_ulPhyAddrBase;
	UINT64  ulRegVal1, ulRegVal2;

	/*
	 * 依次读取 RTC 时钟的高位数据和低位数据,为避免在计数器变化阶段读取无效数据，
	 * 这里进行简单的校验处理
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
	 * Convert 47-bit counter to 32-bit raw second count 将数据转换成秒数据
	 * 由于转换后的数据还包含 SNVS_LPSRTCMR 高 16bits 信息，将其舍弃
	 */
	*pTimeNow = (ulRegVal1 >> 15) & 0x00000000FFFFFFFF;

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  imx6q 处理器 RTC 驱动程序
*********************************************************************************************************/
static LW_RTC_FUNCS     _G_imx6qRtcFuncs = {
        LW_NULL,
        imx6qRtcSetTime,
        imx6qRtcGetTime,
        LW_NULL
};
/*********************************************************************************************************
** 函数名称: __imx6qRtcInit
** 功能描述: 初始化 RTC 设备
** 输　入  : pRtcControler     RTC 控制器
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块: 参考至Linux snvs_rtc_probe 函数
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
** 函数名称: rtcGetFuncs
** 功能描述: 获取 RTC 驱动程序
** 输　入  : NONE
** 输　出  : RTC 驱动程序
** 全局变量:
** 调用模块:
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
