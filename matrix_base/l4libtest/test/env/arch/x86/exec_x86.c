/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: exec_arm.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 3 �� 8 ��
**
** ��        ��: exec
*********************************************************************************************************/

#include "../test/include/t_sys.h"

#if defined(MX_CFG_CPU_ARCH_X86)

PVOID T_execGetCurSp ()
{
    PVOID sp;
    asm volatile ("mov %0, esp" : "=g" (sp));

    return  (sp);
}

VOID  T_execSetCurSp (PVOID pvSp)
{
    asm volatile ("mov esp, %0" :: "g" (pvSp));
}

#endif

/*********************************************************************************************************
  END
*********************************************************************************************************/