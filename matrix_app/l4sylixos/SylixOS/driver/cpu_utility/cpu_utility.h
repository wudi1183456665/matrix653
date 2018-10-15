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
** 文   件   名: cpu_utility.h
**
** 创   建   人: Zhang.Xu (张旭)
**
** 文件创建日期: 2015 年 12 月 1 日
**
** 描        述: I.MX6Q 处理器 多核管理驱动文件
*********************************************************************************************************/
#ifndef __CPU_UTILITY_H__
#define __CPU_UTILITY_H__

/*********************************************************************************************************
  系统工作相关宏定义
*********************************************************************************************************/
/*
 * Number of cores available.
 */
enum _get_cores_results
{
    GET_CORES_ERROR    = 0,                                             /* Failed determine num of cores*/
    FOUR_CORES_ACTIVE  = 4,                                             /* Four available CPU cores.    */
    TWO_CORES_ACTIVE   = 2,                                             /* Two available CPU cores.     */
    ONE_CORE_ACTIVE    = 1                                              /* One available CPU core.      */
};
/*
 * List of all the available CPU work point
 */
typedef enum {
    CPU_WORKPOINT_1P2GHZ = 0,
    CPU_WORKPOINT_1GHZ   = 1,
    CPU_WORKPOINT_800MHZ = 2,
    CPU_WORKPOINT_400MHZ = 3,
    CPU_WORKPOINT_OUTOFRANGE
} cpu_wp_e;

typedef VOID (*cpu_entry_point_t)(VOID * arg);

/*********************************************************************************************************
  functions
*********************************************************************************************************/
VOID  imx6qCpuStart2nd(UINT8 ucCoreNum);
VOID  imx6qCpuDisable(UINT8 ucCoreNum);

#endif                                                                  /*  __CPU_UTILITY_H__           */
/*********************************************************************************************************
  END
*********************************************************************************************************/
