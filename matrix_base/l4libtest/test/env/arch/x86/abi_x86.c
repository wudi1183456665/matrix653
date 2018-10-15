/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: abi_arm.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 1 ��
**
** ��        ��: abi
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
