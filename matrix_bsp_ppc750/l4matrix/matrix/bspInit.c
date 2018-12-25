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
** 文   件   名: bspInit.c
**
** 创   建   人: Li.Jing (李靖)
**
** 文件创建日期: 2018 年 1 月 18 日
**
** 描        述: BSP 用户 C 程序入口
*********************************************************************************************************/
#include <matrix.h>
#include "config.h"                                                     /*  工程配置 & 处理器相关       */
#include "driver/int/i8259a.h"											/*	中断控制器					*/
#include "driver/timer/ppcTimer.h"										/*	定时器						*/
#include "driver/uart/uart_1.h"											/*	串口						*/

//#include "driver/gic/gic.h"                                             /*  i.MX6  中断控制器           */
//#include "driver/timer/timer.h"                                         /*  i.MX6  定时器               */
//#include "driver/ccm_pll/ccm_pll.h"                                     /*  i.MX6  PLL 时钟             */
//#include "driver/cpu_utility/cpu_utility.h"                             /*  i.MX6  多核控制             */
//#include "arch/arm/common/cp15/armCp15.h"

static PPC_TIME _G_i8254Data = {
    .iobase  = BSP_CFG_8254_IO_BASE,
    .qcofreq = 1193182,
};
static UINT32   _G_uiFullCnt;
static UINT64   _G_ui64NSecPerCnt7;                                     /*  提高 7bit 精度              */


static I8259A_CTL   _G_i8259aData = {
        .iobase_master  = BSP_CFG_8259A_IO_BASE_MASTER,
        .iobase_slave   = BSP_CFG_8259A_IO_BASE_SLAVE,
        .trigger        = 0,
        .vector_base    = 0,
};

/*********************************************************************************************************
** 函数名称: bspTickInit
** 功能描述: 初始化 tick 时钟
** 输  入  : NONE
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID	halTickInit	(VOID)
{
    ULONG                ulVector = BSP_CFG_8254_VECTOR;
    _G_uiFullCnt          = (_G_i8254Data.qcofreq / 100);
    _G_ui64NSecPerCnt7    = ((1000 * 1000 * 1000 / 100) << 7) / _G_uiFullCnt;

    i8254InitAsTick(&_G_i8254Data);

}


INT bspInit (VOID)
{
	bspDebugMsg("-----------------bspInit(%d)--------------------\n");

	i8259aInit(&_G_i8259aData);
    extern VOID  vector(VOID);
//    lib_memcpy((PVOID)BSP_CFG_RAM_BASE, (CPVOID)vector, 0x2000);


}


/*********************************************************************************************************
  END
*********************************************************************************************************/
