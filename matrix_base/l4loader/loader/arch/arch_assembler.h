/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: arch_assembler.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2013 �� 06 �� 14 ��
**
** ��        ��: ��ϵ�ṹ������.
*********************************************************************************************************/

#ifndef __ARCH_ASSEMBLER_H
#define __ARCH_ASSEMBLER_H

#include "config/cpu/cpu_cfg.h"

#if defined(LW_CFG_CPU_ARCH_ARM)
#include "arm/arm_assembler.h"

#elif defined(LW_CFG_CPU_ARCH_X86)
#include "x86/x86_assembler.h"

#elif defined(LW_CFG_CPU_ARCH_MIPS)
#include "mips/mips_assembler.h"

#elif defined(LW_CFG_CPU_ARCH_PPC)
#include "ppc/ppc_assembler.h"

#elif defined(LW_CFG_CPU_ARCH_C6X)
#include "c6x/c6x_assembler.h"

#elif defined(LW_CFG_CPU_ARCH_SPARC)
#include "sparc/sparc_assembler.h"
#endif                                                                  /*  LW_CFG_CPU_ARCH_ARM         */

#endif                                                                  /*  __ARCH_ASSEMBLER_H          */
/*********************************************************************************************************
  END
*********************************************************************************************************/

