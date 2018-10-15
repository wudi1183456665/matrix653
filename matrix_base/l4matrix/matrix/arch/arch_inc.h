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
** 文   件   名: arch_inc.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 10 月 31 日
**
** 描        述: 体系相关头文件.
*********************************************************************************************************/

#ifndef __ARCH_INC_H
#define __ARCH_INC_H

#include "config/cpu/cpu_cfg.h"

#if defined(MX_CFG_CPU_ARCH_ARM)
#include "arm/arm_compiler.h"
#include "arm/arm_limits.h"
#include "arm/arm_types.h"
#include "arm/arm_regs.h"
#include "arm/arm_elf.h"

#elif defined(MX_CFG_CPU_ARCH_X86)
#include "x86/x86_types.h"
#include "x86/x86_regs.h"
#include "x86/x86_elf.h"
#include "x86/x86_pc.h"

#elif defined(MX_CFG_CPU_ARCH_MIPS)
#include "mips/mips_types.h"
#include "mips/mips_elf.h"

#elif defined(MX_CFG_CPU_ARCH_PPC)

#include "ppc/ppc_compiler.h"
#include "ppc/ppc_limits.h"
#include "ppc/ppc_types.h"
#include "ppc/ppc_regs.h"
#include "ppc/ppc_elf.h"


#elif defined(MX_CFG_CPU_ARCH_C6X)
#include "c6x/c6x_types.h"
#include "c6x/c6x_elf.h"

#elif defined(MX_CFG_CPU_ARCH_SPARC)
#include "sparc/sparc_types.h"
#include "sparc/sparc_elf.h"
#endif                                                                  /*  MX_CFG_CPU_ARCH_ARM         */

#endif                                                                  /*  __ARCH_INC_H                */
/*********************************************************************************************************
  END
*********************************************************************************************************/
