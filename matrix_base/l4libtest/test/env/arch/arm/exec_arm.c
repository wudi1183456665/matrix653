/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                嵌入式 L4 微内核操作系统
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: exec_arm.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 3 月 8 日
**
** 描        述: exec
*********************************************************************************************************/
#include "t_sys.h"

#if defined(MX_CFG_CPU_ARCH_ARM)

PVOID T_execGetCurSp ()
{
    PVOID sp = MX_NULL;
    __asm__ __volatile__(
        "mov    %0,  sp\n\t"
        : "+r"(sp)
        :
        :
    );

    return  (sp);
}

VOID  T_execSetCurSp (PVOID pvSp)
{
    __asm__ __volatile__(
        "mov    sp, %0\n\t"
        :
        : "r"(pvSp)
        :
    );
}

#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/
