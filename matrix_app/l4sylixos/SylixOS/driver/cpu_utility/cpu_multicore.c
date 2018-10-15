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
** 文   件   名: cpu_multicore.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 01 月 21 日
**
** 描        述: imx6q 处理器多核设置处理
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "sdk.h"
#include <assert.h>
#include "cpu_utility/cpu_utility.h"
#include "regssrc.h"
/*********************************************************************************************************
  定义
*********************************************************************************************************/
#define MAX_CORE_COUNT (4)                                              /*  处理器 CPU 核的数目         */
/*********************************************************************************************************
** 函数名称: imx6qCpuStart2nd
** 功能描述: Start up a secondary CPU core.
** 输　入  : ucCoreNum CPU index from 1 through 3
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qCpuStart2nd (UINT8 ucCoreNum)
{
    /*
     * Exit if the requested core is not available.
     */
    if (ucCoreNum == 0 || ucCoreNum >= MAX_CORE_COUNT) {
        return;
    }
    /*
     * Prepare pointers for ROM code. The entry point is always _start, which does some
     * basic preparatory work and then calls the common_cpu_entry function, which itself
     * calls the entry point saved in s_core_info.
     */
    switch (ucCoreNum) {
    case 1:
        HW_SRC_GPR3_WR((UINT32) bspInfoRamBase());
        HW_SRC_GPR4_WR((UINT32) 0);

        if (HW_SRC_SCR.B.CORE1_ENABLE == 1) {
            HW_SRC_SCR.B.CORE1_RST = 1;
        } else {
            HW_SRC_SCR.B.CORE1_ENABLE = 1;
        }
        break;

    case 2:
        HW_SRC_GPR5_WR((UINT32) bspInfoRamBase());
        HW_SRC_GPR6_WR((UINT32) 0);

        if (HW_SRC_SCR.B.CORE2_ENABLE == 1) {
            HW_SRC_SCR.B.CORE2_RST = 1;
        } else {
            HW_SRC_SCR.B.CORE2_ENABLE = 1;
        }
        break;

    case 3:
        HW_SRC_GPR7_WR((UINT32) bspInfoRamBase());
        HW_SRC_GPR8_WR((UINT32) 0);

        if (HW_SRC_SCR.B.CORE3_ENABLE == 1) {
            HW_SRC_SCR.B.CORE3_RST = 1;
        } else {
            HW_SRC_SCR.B.CORE3_ENABLE = 1;
        }
        break;
    }
}
/*********************************************************************************************************
** 函数名称: imx6qCpuDisable
** 功能描述: Places a secondary CPU core in reset.
** 输　入  : ucCoreNum CPU index from 1 through 3
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qCpuDisable (UINT8 ucCoreNum)
{
    /*
     * Exit if the requested core is not available.
     */
    if (ucCoreNum == 0 || ucCoreNum >= MAX_CORE_COUNT) {
        return;
    }

    switch (ucCoreNum) {
    case 1:
        HW_SRC_SCR.B.CORE1_ENABLE = 0;
        break;

    case 2:
        HW_SRC_SCR.B.CORE2_ENABLE = 0;
        break;

    case 3:
        HW_SRC_SCR.B.CORE3_ENABLE = 0;
        break;
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
