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
** 文   件   名: ppcLib.c
**
** 创   建   人: Li.Yanqi
**
** 文件创建日期: 2018 年 07 月 06 日
**
** 描        述: PowerPC 体系构架内部库.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include <Matrix.h>
/*********************************************************************************************************
** 函数名称: archPageCopy
** 功能描述: 拷贝一个页面
** 输　入  : pvTo      目标
**           pvFrom    源
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/

VOID  archPageCopy (PVOID  pvTo, PVOID  pvFrom)
{
    REGISTER INT      i;
    REGISTER UINT64  *pu64To   = (UINT64 *)pvTo;
    REGISTER UINT64  *pu64From = (UINT64 *)pvFrom;

    for (i = 0; i < (LW_CFG_VMM_PAGE_SIZE >> 3); i += 16) {             /*  4KB PAGE SIZE               */
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
        *pu64To++ = *pu64From++;
    }
}
