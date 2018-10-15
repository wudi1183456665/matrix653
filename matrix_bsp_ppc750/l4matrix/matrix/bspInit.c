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
#include "driver/gic/gic.h"                                             /*  i.MX6  中断控制器           */
#include "driver/timer/timer.h"                                         /*  i.MX6  定时器               */
#include "driver/ccm_pll/ccm_pll.h"                                     /*  i.MX6  PLL 时钟             */
#include "driver/cpu_utility/cpu_utility.h"                             /*  i.MX6  多核控制             */
#include "arch/arm/common/cp15/armCp15.h"
/*********************************************************************************************************
** 函数名称: targetInit
** 功能描述: 初始化 i.MX6DQ
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块: start.S
*********************************************************************************************************/
VOID  targetInit (VOID)
{
    imx6qCcmPLLInit();
}
/*********************************************************************************************************
** 函数名称: halTickInit
** 功能描述: 初始化 tick 时钟
** 输  入  : NONE
** 输  出  : NONE
** 全局变量:
** 调用模块: halPrimaryCpuMain()
*********************************************************************************************************/
static VOID  halTickInit (VOID)
{
    REGISTER UINT32      uiIncrementValue, uiPrescaler;

    /*
     *                         (PRESCALER_value+1) x (Load_value+1) x 2
     *  The timer interval = --------------------------------------------
     *                                         PERIPHCLK
     */
    uiIncrementValue = (imx6qMainClkGet(CPU_CLK) / 2 ) / BSP_CFG_TICK_HZ;
    uiPrescaler      = 0;

    /*
     *  初始化 Tick 定时器硬件
     */
    armGlobalTimerInit(MX_TRUE, uiIncrementValue, uiPrescaler, MX_TRUE);
    armGlobalTimerCounterSet(0);
    armGlobalTimerComparatorSet(uiIncrementValue);
    armGlobalTimerStart();

    /*
     *  使能 Tick 中断
     */
    armGicIntVecterEnable(GIC_TICK_INT_VEC, MX_FALSE, GIC_TICK_INT_PRIO, 1 << 0);
}
/*********************************************************************************************************
** 函数名称: halSmpCoreInit
** 功能描述: 初始化 SMP 核心
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: halPrimaryCpuMain(), halSecondaryCpuMain()
*********************************************************************************************************/
static VOID  halSmpCoreInit (VOID)
{
    ULONG  ulCpuId = archMpCur();

    irqIpiVecSet(ulCpuId, GIC_IPI_INT_VEC(ulCpuId));                    /*  安装 IPI 向量               */
}
/*********************************************************************************************************
** 函数名称: halPrimaryCpuMain
** 功能描述: Primary CPU C 入口
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块: startup.S
*********************************************************************************************************/
VOID  halPrimaryCpuMain (VOID)
{
    bspDebugMsg("-----------halPrimaryCpuMain(%d)------------------\n", archMpCur());

    armGicInit();
    armGicCpuInit(MX_FALSE, 255);
    armVectorBaseAddrSet(BSP_CFG_VECTOR_BASE);

    halTickInit();
    halSmpCoreInit();

    initMatrix("ncpus=4 tick=1000");                                    /*  进入 Matrix 微内核          */
}
/*********************************************************************************************************
** 函数名称: halSecondaryCpuMain
** 功能描述: Secondary CPU C 入口
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块: startup.S
*********************************************************************************************************/
VOID  halSecondaryCpuMain (VOID)
{
    //bspDebugMsg("-----------halSecondaryCpuMain(%d)----------------\n", archMpCur());

    armGicCpuInit(MX_FALSE, 255);
    armVectorBaseAddrSet(BSP_CFG_VECTOR_BASE);

    halSmpCoreInit();
    
    initSmpMatrix();                                                    /*  微内核启动多核              */
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
