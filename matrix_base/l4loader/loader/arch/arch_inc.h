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
** ��   ��   ��: arch_inc.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 10 �� 31 ��
**
** ��        ��: ��ϵ���ͷ�ļ�.
*********************************************************************************************************/

#ifndef __ARCH_INC_H
#define __ARCH_INC_H

#include "config/cpu/cpu_cfg.h"

#if defined(LW_CFG_CPU_ARCH_ARM)
#include "arm/arm_types.h"
#include "arm/arm_elf.h"

#elif defined(LW_CFG_CPU_ARCH_X86)
#include "x86/x86_types.h"
#include "x86/x86_elf.h"

#elif defined(LW_CFG_CPU_ARCH_MIPS)
#include "mips/mips_types.h"
#include "mips/mips_elf.h"

#elif defined(LW_CFG_CPU_ARCH_PPC)
#include "ppc/ppc_types.h"
#include "ppc/ppc_elf.h"

#elif defined(LW_CFG_CPU_ARCH_C6X)
#include "c6x/c6x_types.h"
#include "c6x/c6x_elf.h"

#elif defined(LW_CFG_CPU_ARCH_SPARC)
#include "sparc/sparc_types.h"
#include "sparc/sparc_elf.h"
#endif                                                                  /*  LW_CFG_CPU_ARCH_ARM         */

#endif                                                                  /*  __ARCH_INC_H                */
/*********************************************************************************************************
  END
*********************************************************************************************************/
