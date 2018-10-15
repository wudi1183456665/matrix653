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
** 文   件   名: abi_arm.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: abi
*********************************************************************************************************/

#include "../test/include/t_sys.h"

#if defined(MX_CFG_CPU_ARCH_X86)

VOID MX_Syscall41 (ULONG ulArg0, ULONG ulArg1, ULONG ulArg2, ULONG ulArg3,
                   ULONG *pulOut1)
{
    ULONG dummy;
    asm volatile (
        "push %%ebp;"
        "mov %6, %%ebp;"
        "mov %%esp, %%ecx;"
        "mov $1f, %%edx;"
        "sysenter;"
        "1: ;"
        "pop %%ebp;"
        : "+a" (ulArg0), "=c" (dummy), "=d" (dummy), "=D" (ulArg1), "=S" (ulArg2), "=b" (ulArg3)
        :
        : "memory"
    );
    *pulOut1 = ulArg0;
}

VOID MX_Syscall42 (ULONG ulArg0, ULONG ulArg1, ULONG ulArg2, ULONG ulArg3,
                   ULONG *pulOut1, ULONG *pulOut2)
{
    ULONG dummy;
    asm volatile (
        "push %%ebp;"
        "mov %6, %%ebp;"
        "mov %%esp, %%ecx;"
        "mov $1f, %%edx;"
        "sysenter;"
        "1: ;"
        "pop %%ebp;"
        : "+a" (ulArg0), "=c" (dummy), "=d" (dummy), "+D" (ulArg1), "=S" (ulArg2), "=b" (ulArg3)
        :
        : "memory"
    );
    *pulOut1 = ulArg0;
    *pulOut2 = ulArg1;
}

#endif

/*********************************************************************************************************
  END
*********************************************************************************************************/
