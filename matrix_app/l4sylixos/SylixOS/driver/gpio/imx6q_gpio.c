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
** 文   件   名: imx6q_gpio.c
**
** 创   建   人: zhang.xu (张旭)
**
** 文件创建日期: 2015 年 12 月 13 日
**
** 描        述: Imx6q 的 GPIO 驱动
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include "gpio/imx6q_gpio.h"

#include "sdk.h"
#include "irq/irq_numbers.h"
#include "pinmux/iomux_config.h"
/*********************************************************************************************************
  定义
*********************************************************************************************************/
#define GPIO_CONTROLER_NR           (7)                                 /*  imx6q 有 7 组 gpio 控制器   */
#define GPIO_DR                     (0x00)                              /*  GPIO 电平设置寄存器         */
#define GPIO_GDIR                   (0x04)                              /*  GPIO 输入输出选择寄存器     */
#define GPIO_PSR                    (0x08)                              /*  GPIO 管脚状态寄存器         */
#define GPIO_ICR1                   (0x0C)                              /*  GPIO 中断模式设置寄存器 1   */
#define GPIO_ICR2                   (0x10)                              /*  GPIO 中断模式设置寄存器 2   */
#define GPIO_IMR                    (0x14)                              /*  GPIO 中断使能/屏蔽寄存器    */
#define GPIO_ISR                    (0x18)                              /*  GPIO 中断状态寄存器         */
#define GPIO_EDGE_SEL               (0x1C)                              /*  GPIO 双边沿触发中断使能     */

/*********************************************************************************************************
  GPIO 控制器类型定义
*********************************************************************************************************/
typedef struct {
    addr_t              GPIOC_ulPhyAddrBase;                            /*  物理地址基地址              */
    addr_t              GPIOC_stPhyAddrSize;                            /*  物理地址空间大小            */
    ULONG               CPIOC_ulIrqNum1;                                /*  0~15 引脚的中断号           */
    ULONG               CPIOC_ulIrqNum2;                                /*  16~32 引脚的中断号          */
    UINT                GPIOC_uiPinNumber;
    addr_t              GPIOC_ulVirtAddrBase;                           /*  虚拟地址基地址              */
} IMX6Q_GPIO_CONTROLER, *PIMX6Q_GPIO_CONTROLER;
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
static IMX6Q_GPIO_CONTROLER  _G_imx6qGpioControlers[] = {
        {
        		GPIO1_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO1_INT15_0,
				IMX_INT_GPIO1_INT31_16,
                32,
                GPIO1_BASE_ADDR,                                         /*  默认是物理地址             */
        }, {
        		GPIO2_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO2_INT15_0,
				IMX_INT_GPIO2_INT31_16,
                32,
                GPIO2_BASE_ADDR,
        }, {
        		GPIO3_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO3_INT15_0,
				IMX_INT_GPIO3_INT31_16,
                32,
                GPIO3_BASE_ADDR,
        }, {
        		GPIO4_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO4_INT15_0,
				IMX_INT_GPIO4_INT31_16,
                32,
                GPIO4_BASE_ADDR,
        }, {
        		GPIO5_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO5_INT15_0,
				IMX_INT_GPIO5_INT31_16,
                32,
                GPIO5_BASE_ADDR,
        }, {
        		GPIO6_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO6_INT15_0,
				IMX_INT_GPIO6_INT31_16,
                32,
                GPIO6_BASE_ADDR,
        }, {
        		GPIO7_BASE_ADDR,
                LW_CFG_VMM_PAGE_SIZE,
				IMX_INT_GPIO7_INT15_0,
				IMX_INT_GPIO7_INT31_16,
                14,
                GPIO7_BASE_ADDR,
        },
};
static LW_GPIO_CHIP  _G_imx6qGpioChip ;
/*********************************************************************************************************
** 函数名称: imx6qGpioRequest
** 功能描述: 实现 GPIO 管脚的 PINMUX 设置
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qGpioRequest (PLW_GPIO_CHIP  pGpioChip, UINT uiOffset)
{
    INT    iRet;

    iRet = boardGpioCheck(uiOffset);
    if (iRet == PX_ERROR) {
        return  (PX_ERROR);
    }

    if (uiOffset >= _G_imx6qGpioChip.GC_uiNGpios) {
        return (PX_ERROR);
    }

    iRet = GpioPinmuxSet(uiOffset / 32, uiOffset % 32, 0x10 | ALT5);    /*  强制设置为输入              */

    return (iRet);
}
/*********************************************************************************************************
** 函数名称: imx6qGpioFree
** 功能描述: 释放一个正在被使用的 GPIO, 如果当前是中断模式则, 放弃中断输入功能.
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  imx6qGpioFree (PLW_GPIO_CHIP  pGpioChip, UINT uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;

    uiRegVal = readl(atBase + GPIO_IMR) &(~(1 << (uiOffset % 32)));
    writel(uiRegVal, atBase + GPIO_IMR);                                /*  禁止该 GPIO 中断            */

    uiRegVal = readl(atBase + GPIO_GDIR) &(~(1 << (uiOffset % 32)));
    writel(uiRegVal, atBase + GPIO_GDIR);                               /*  设置该 GPIO 为输入          */
}
/*********************************************************************************************************
** 函数名称: imx6qGpioGetDirection
** 功能描述: 获得指定 GPIO 方向
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
** 输  出  : 0: 输入 1:输出
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qGpioGetDirection (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;

    uiRegVal = readl(atBase + GPIO_GDIR) & (1 << (uiOffset % 32));
    if(uiRegVal) {
    	return  (1);
    } else {
    	return  (0);
    }
}
/*********************************************************************************************************
** 函数名称: imx6qGpioDirectionInput
** 功能描述: 设置指定 GPIO 为输入模式
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
** 输  出  : 0: 正确 -1:错误
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qGpioDirectionInput (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;
    uiRegVal = readl(atBase + GPIO_GDIR) &(~(1 << (uiOffset % 32)));
    writel(uiRegVal, atBase + GPIO_GDIR);                               /*  设置该 GPIO 为输入          */

    return  (0);
}
/*********************************************************************************************************
** 函数名称: imx6qGpioGet
** 功能描述: 获得指定 GPIO 电平
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
** 输  出  : 0: 低电平 1:高电平
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qGpioGet (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;
    uiRegVal = readl(atBase + GPIO_PSR) & (1 << (uiOffset % 32));

    if(uiRegVal) {
    	return  (1);
    } else {
    	return  (0);
    }
}
/*********************************************************************************************************
** 函数名称: imx6qGpioDirectionOutput
** 功能描述: 设置指定 GPIO 为输出模式
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
**           iValue      输出电平
** 输  出  : 0: 正确 -1:错误
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qGpioDirectionOutput (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset, INT  iValue)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;

    uiRegVal = readl(atBase + GPIO_GDIR) | (1 << (uiOffset % 32));
    writel(uiRegVal, atBase + GPIO_GDIR);                               /*  设置该 GPIO 为输出          */

    if(iValue) {                                                        /*  设置该 GPIO 默认电平        */
        uiRegVal = readl(atBase + GPIO_DR) | (1 << (uiOffset % 32));
        writel(uiRegVal, atBase + GPIO_DR);                             /*  设置该 GPIO 默认电平为 1    */

    } else {
        uiRegVal = readl(atBase + GPIO_DR) &(~(1 << (uiOffset % 32)));
        writel(uiRegVal, atBase + GPIO_DR);                             /*  设置该 GPIO 默认电平为 0    */

    }

    return  (0);
}
/*********************************************************************************************************
** 函数名称: imx6qGpioSet
** 功能描述: 设置指定 GPIO 电平
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
**           iValue      输出电平
** 输  出  : 0: 正确 -1:错误
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  imx6qGpioSet (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset, INT  iValue)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;

    if(iValue) {                                                        /*  设置该 GPIO 电平            */
        uiRegVal = readl(atBase + GPIO_DR) | (1 << (uiOffset % 32));
        writel(uiRegVal, atBase + GPIO_DR);                             /*  设置该 GPIO 电平为 1        */

    } else {
        uiRegVal = readl(atBase + GPIO_DR) &(~(1 << (uiOffset % 32)));
        writel(uiRegVal, atBase + GPIO_DR);                             /*  设置该 GPIO 电平为 0        */

    }
}
/*********************************************************************************************************
** 函数名称: imx6qGpioSetDebounce
** 功能描述: 设置指定 GPIO 的去抖参数
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
**           uiDebounce  去抖参数
** 输  出  : 0: 正确 -1:错误
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qGpioSetDebounce (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset, UINT  uiDebounce)
{
	/*
	 * imx6q 的 gpio 控制器中好像没有消抖
	 */
    return  (0);
}
/*********************************************************************************************************
** 函数名称: imx6qGpioSetupIrq
** 功能描述: 设置指定 GPIO 为外部中断输入管脚
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
**           bIsLevel    是否为电平触发
**           uiType      如果为电平触发, 1 表示高电平触发, 0 表示低电平触发
**                       如果为边沿触发, 1 表示上升沿触发, 0 表示下降沿触发, 2 双边沿触发
** 输  出  : IRQ 向量号 -1:错误
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ULONG  imx6qGpioSetupIrq (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset, BOOL  bIsLevel, UINT  uiType)
{

    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase ;
    UINT32                 uiRegVal;
    UINT32                 uiPinOff;
    UINT32                 uiIrqNum;

    uiPinOff       = uiOffset % 32;
    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase         = pGpioControler->GPIOC_ulVirtAddrBase;

    if(uiPinOff > 15) {                                                 /*  计算需要设置的寄存器位      */
    	atBase = atBase + GPIO_ICR2;
    	uiIrqNum  = pGpioControler->CPIOC_ulIrqNum2;
        uiPinOff  = (uiPinOff - 15) << 1;

    } else {
    	atBase = atBase + GPIO_ICR1;
    	uiIrqNum  = pGpioControler->CPIOC_ulIrqNum1;
        uiPinOff  = uiPinOff << 1;
    }

    uiRegVal = readl(pGpioControler->GPIOC_ulVirtAddrBase + GPIO_EDGE_SEL);/*  清除双边沿中断触发设置   */
    writel(uiRegVal &(~(1 << (uiOffset % 32))), pGpioControler->GPIOC_ulVirtAddrBase + GPIO_EDGE_SEL);

    if (bIsLevel) {                                                     /*  如果是电平触发中断          */
    	if(uiType) {                                                    /*  高电平触发                  */
            uiRegVal = readl(atBase) & (~(0x3 << uiPinOff));
            uiRegVal = uiRegVal | 0x1 << uiPinOff;
            writel(uiRegVal, atBase);

    	} else {                                                        /*  低电平触发                  */
            uiRegVal = readl(atBase) & (~(0x3 << uiPinOff));
            uiRegVal = uiRegVal | 0x0 << uiPinOff;
            writel(uiRegVal, atBase);

    	}
    } else {
        if (uiType == 1) {                                              /*  上升沿触发                  */
            uiRegVal = readl(atBase) & (~(0x3 << uiPinOff));
            uiRegVal = uiRegVal | 0x2 << uiPinOff;
            writel(uiRegVal, atBase);

        } else if (uiType == 0) {                                       /*  下降沿触发                  */
            uiRegVal = readl(atBase) & (~(0x3 << uiPinOff));
            uiRegVal = uiRegVal | 0x3 << uiPinOff;
            writel(uiRegVal, atBase);

        } else {                                                        /*  双边沿触发                  */
            uiRegVal = readl(pGpioControler->GPIOC_ulVirtAddrBase + GPIO_EDGE_SEL) | (1<<(uiOffset%32));
            writel(uiRegVal, pGpioControler->GPIOC_ulVirtAddrBase + GPIO_EDGE_SEL);
        }
    }

    uiRegVal = readl(pGpioControler->GPIOC_ulVirtAddrBase + GPIO_IMR);  /*  使能该 GPIO 中断            */
    writel(uiRegVal | (1 << (uiOffset % 32)), pGpioControler->GPIOC_ulVirtAddrBase + GPIO_IMR);
    /*
     * 测试用代码
     */
    uiRegVal = readl(pGpioControler->GPIOC_ulVirtAddrBase + GPIO_IMR);

    return  (uiIrqNum);
}
/*********************************************************************************************************
** 函数名称: imx6qGpioClearIrq
** 功能描述: 清除指定 GPIO 中断标志
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  imx6qGpioClearIrq (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

    pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
    atBase   = pGpioControler->GPIOC_ulVirtAddrBase;
    uiRegVal = readl(atBase + GPIO_ISR) | (1 << (uiOffset % 32));
    writel(uiRegVal, atBase + GPIO_ISR);                                /*  清除该 GPIO 中断状态        */
}
/*********************************************************************************************************
** 函数名称: imx6qGpioSvrIrq
** 功能描述: 判断 GPIO 中断标志
** 输  入  : pGpioChip   GPIO 芯片
**           uiOffset    GPIO 针对 BASE 的偏移量
** 输  出  : 中断返回值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static irqreturn_t  imx6qGpioSvrIrq (PLW_GPIO_CHIP  pGpioChip, UINT  uiOffset)
{
    PIMX6Q_GPIO_CONTROLER  pGpioControler;
    addr_t                 atBase;
    UINT32                 uiRegVal;

	pGpioControler = &_G_imx6qGpioControlers[uiOffset / 32];
	atBase   = pGpioControler->GPIOC_ulVirtAddrBase;
    uiRegVal = readl(atBase + GPIO_ISR) & (1 << (uiOffset % 32));
    if (uiRegVal) {
        return  (LW_IRQ_HANDLED);
    } else {
        return  (LW_IRQ_NONE);
    }
}
/*********************************************************************************************************
  GPIO 驱动程序
*********************************************************************************************************/
static LW_GPIO_CHIP  _G_imx6qGpioChip = {
        .GC_pcLabel              = "IMX6Q  GPIO",
        .GC_ulVerMagic           = LW_GPIO_VER_MAGIC,
        .GC_pfuncRequest         = imx6qGpioRequest,
        .GC_pfuncFree            = imx6qGpioFree,

        .GC_pfuncGetDirection    = imx6qGpioGetDirection,
        .GC_pfuncDirectionInput  = imx6qGpioDirectionInput,
        .GC_pfuncGet             = imx6qGpioGet,
        .GC_pfuncDirectionOutput = imx6qGpioDirectionOutput,
        .GC_pfuncSetDebounce     = imx6qGpioSetDebounce,
        .GC_pfuncSetPull         = LW_NULL,
        .GC_pfuncSet             = imx6qGpioSet,
        .GC_pfuncSetupIrq        = imx6qGpioSetupIrq,
        .GC_pfuncClearIrq        = imx6qGpioClearIrq,
        .GC_pfuncSvrIrq          = imx6qGpioSvrIrq,
};
/*********************************************************************************************************
** 函数名称: am335xGpioDrv
** 功能描述: 安装 AM335X GPIO 驱动
** 输  入  : NONE
** 输  出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  imx6qGpioDrv (VOID)
{
    PIMX6Q_GPIO_CONTROLER    pGpioControler;
    INT                      i;

    _G_imx6qGpioChip.GC_uiBase   = 0;
    _G_imx6qGpioChip.GC_uiNGpios = 0;

    /*
     * GPIO 控制器时钟和电源使能，及地址映射，
     */
    for (i = 0; i < GPIO_CONTROLER_NR; i++) {
        pGpioControler = &_G_imx6qGpioControlers[i];
        _G_imx6qGpioChip.GC_uiNGpios += pGpioControler->GPIOC_uiPinNumber;
    }

    return  (API_GpioChipAdd(&_G_imx6qGpioChip));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
