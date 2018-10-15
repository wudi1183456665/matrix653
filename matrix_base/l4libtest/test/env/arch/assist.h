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
** 文   件   名: api.h
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: ABI 接口
*********************************************************************************************************/
#ifndef __T_ASSIST_H
#define __T_ASSIST_H

#include "config/cpu/cpu_cfg.h"

#if defined(MX_CFG_CPU_ARCH_ARM)
#include "./arm/assist_arm.h"

#elif defined(MX_CFG_CPU_ARCH_X86)
#include "./x86/assist_x86.h"

#elif defined(MX_CFG_CPU_ARCH_MIPS)


#elif defined(MX_CFG_CPU_ARCH_PPC)


#elif defined(MX_CFG_CPU_ARCH_C6X)


#elif defined(MX_CFG_CPU_ARCH_SPARC)

#endif

#endif                                                                  /*  __T_ASSIST_H                */
/*********************************************************************************************************
  END
*********************************************************************************************************/
