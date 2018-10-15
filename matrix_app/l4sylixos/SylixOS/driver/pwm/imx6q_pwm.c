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
** 文   件   名: imx6q_pwm.c
**
** 创   建   人: Zhang.Xu (张旭)
**
** 文件创建日期: 2016 年 2 月 21 日
**
** 描        述: imx6q 处理器的 PWM 设备驱动，主要用来实现背光灯控制
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include <linux/compat.h>

#include "sdk.h"
/*********************************************************************************************************
  PWM 控制器寄存器定义
*********************************************************************************************************/
#define  PWM_PWMCR                    (0x00)                            /*  Control Register            */
#define  PWM_PWMSR                    (0x04)                            /*  Status Register             */
#define  PWM_PWMIR                    (0x08)                            /*  Interrupt Register          */
#define  PWM_PWMSAR                   (0x0C)                            /*  Sample Register             */
#define  PWM_PWMPR                    (0x10)                            /*  Period Register             */
#define  PWM_PWMCNR                   (0x14)                            /*  Counter Register            */

#define  PWM_NUM                      (4)
/*
 *  PWM ioctl 函数命令字
 */
#define  PWM_FREN                     (105)                             /*  0 ~ 4069, 260 = 1Khz        */
#define  PWM_DUTY                     (106)                             /*  0 ~ 255,  255 = 100%        */
/*********************************************************************************************************
  PWM 控制器类型定义
*********************************************************************************************************/
typedef struct {
    LW_DEV_HDR                  PWMC_devHdr;                            /* 必须是第一个结构体成员       */
    LW_LIST_LINE_HEADER         PWMC_fdNodeHeader;
	addr_t                      PWMC_BaseAddr;
} __IMX6Q_PWM_CONTROLER, *__PIMX6Q_PWM_CONTROLER;

UINT32 _G_iPwmDrvNum  = 0;

static __IMX6Q_PWM_CONTROLER  _G_pwm[PWM_NUM];
/*********************************************************************************************************
** 函数名称: imx6qPwmHwInit
** 功能描述: 初始化 pwm 设备控制器
** 输　入  : atBaseAddr             PWM 设备控制器基地址
**           uiPrescale             PWM 输入时钟预分频, 设置2600 = 100Hz
**           uiPeriod               占空比最大设置数值
**           uiSample               占空比设置数值, 不大于 uiPeriod
** 输　出  : 文件节点
** 全局变量:
** 调用模块: 参考iMX6_Platform_SDK 裸机程序的 pwm.c/pwm_init 函数编写
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
	 * 选择PWM控制器的时钟源为IPGCLK，
	 */
	uiRegVal = (uiRegVal & 0xFFFCFFFF) | (0x1 << 16);

    /*
     * Set FIFO watermark to 4 empty slots
     */
	uiRegVal = uiRegVal | (0x3 << 26);

	/*
	 * 设置 预分频数, 若设置数值为 0 则自动忽略该配置项
	 */
	if(uiPrescale > 0x0000FFF0) {
		printk("pwm prescale too big \n");
		return (PX_ERROR);
	}
	if (uiPrescale) {
	    uiRegVal = (uiRegVal & 0xFFFF000F) | ((uiPrescale-1) << 4);
	}
	/*
	 *  设置输出电平相位,默认都是用正相位，方便理解
	 */
	uiRegVal = (uiRegVal & 0xFFF3FFFF) | (0 << 18);
	writel(uiRegVal, (atBaseAddr + PWM_PWMCR));

	/*
	 *  设置 period 数值
	 */
	writel(uiPeriod, (atBaseAddr + PWM_PWMPR));

	/*
	 *  设置 sample 数值，在sdk中还设置了repeat等，在Linux中仅仅设置了 sar 寄存器
	 *  若设置值为 0 则自动忽略设置
	 */
	if (uiSample) {
	    writel(uiSample, (atBaseAddr + PWM_PWMSAR));
	}
	return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imx6qPwmEnable
** 功能描述: 使能 pwm 设备控制器
** 输　入  : atBaseAddr             PWM 设备控制器基地址
** 输　出  : 文件节点
** 全局变量:
** 调用模块: 参考iMX6_Platform_SDK 裸机程序的 pwm.c/pwm_init 函数编写
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
** 函数名称: imx6qPwmHwInit
** 功能描述: 禁用 pwm 设备控制器
** 输　入  : atBaseAddr             PWM 设备控制器基地址
** 输　出  : 文件节点
** 全局变量:
** 调用模块: 参考iMX6_Platform_SDK 裸机程序的 pwm.c/pwm_init 函数编写
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
** 函数名称: imx6qPwmOpen
** 功能描述: 打开 PWM 设备
** 输　入  : pDev                  设备
**           pcName                设备名字
**           iFlags                标志
**           iMode                 模式
** 输　出  : 文件节点
** 全局变量:
** 调用模块:
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
             * 初始化 PWM 定时器，默认设置参数如下
             */
            atBaseAddr = pPwmDev->PWMC_BaseAddr;
            imx6qPwmHwInit(atBaseAddr, 260, 256, 200);
            imx6qPwmEnable(atBaseAddr);

            return  ((LONG)pFdNode);
        }
    }
    /*
     * 如果设备打开失败进行如下操作
     */
    if (pFdNode) {
        API_IosFdNodeDec(&pPwmDev->PWMC_fdNodeHeader, pFdNode, NULL);
        pFdNode = LW_NULL;
    }

    LW_DEV_DEC_USE_COUNT(&pPwmDev->PWMC_devHdr);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: imx6qPwmClose
** 功能描述: 关闭 PWM 设备
** 输　入  : pFdEntry              文件结构
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
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
** 函数名称: imx6qPwmIoctl
** 功能描述: 控制 PWM 设备
** 输　入  : pFdEntry              文件结构
**           iCmd                  命令
**           lArg                  参数
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qPwmIoctl (PLW_FD_ENTRY  pFdEntry, INT  iCmd, LONG lArg)
{
	__PIMX6Q_PWM_CONTROLER  pPwmDev = (__PIMX6Q_PWM_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;
	addr_t                  atBaseAddr = pPwmDev->PWMC_BaseAddr;
    UINT32                  usRegVal = (UINT32)lArg;

    switch (iCmd) {
    case PWM_FREN:                                                       /*  PWM 工作周期                */
        imx6qPwmHwInit(atBaseAddr, usRegVal, 256, 0);
        imx6qPwmEnable(atBaseAddr);
        return  (ERROR_NONE);
    case PWM_DUTY:                                                       /*  设置占空比                  */
        imx6qPwmHwInit(atBaseAddr, 0, 256, usRegVal);
        imx6qPwmEnable(atBaseAddr);
        return  (ERROR_NONE);
    default:
        return  (PX_ERROR);
    }

}
/*********************************************************************************************************
** 函数名称: imx6qPwmWrite
** 功能描述: 驱动 write 函数，实现看门狗喂狗功能，写入字符为0xAAAA5555
** 输　入  : peeprom      eeprom 结构
**           pvBuf        写缓冲
**           stLen        写数据长度
** 输　出  : 成功读取数据字节数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  imx6qPwmWrite (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
   /*
    * PWM 设备不需要任何写入操作
    */

    return  (stLen);
}
/*********************************************************************************************************
** 函数名称: imx6qPwmRead
** 功能描述: 驱动 write 函数
** 输　入  :
** 输　出  : 成功读取数据字节数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  imx6qPwmRead (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
	__PIMX6Q_PWM_CONTROLER  pPwmDev = (__PIMX6Q_PWM_CONTROLER)pFdentry->FDENTRY_pdevhdrHdr;
	addr_t                  atBaseAddr = pPwmDev->PWMC_BaseAddr;

    /*
     * 返回当前计数值
     */
    *(INT *)pvBuf = readl(atBaseAddr + PWM_PWMCNR);

    return  (4);
}
/*********************************************************************************************************
** 函数名称: imx6qPwmDrv
** 功能描述: 安装 PWM 驱动
** 输　入  : NONE
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
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
** 函数名称: imx6qPwmDevAdd
** 功能描述: 创建 PWM 设备
** 输　入  : uiIndex           ID
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
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
     *  原来使用 snprintf(cBuffer, sizeof(cBuffer), "/dev/wdt%d", uiIndex);实现，这里为了尽量降低对
     *  C 库的使用，在驱动程序中不调用标准 C 的函数;
     *  对 PWM 的设置，需要根据原理图进行处理，默认不要进行 Pinmux 以免造成电路板异常
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
