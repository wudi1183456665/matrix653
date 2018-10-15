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
** 文   件   名: cpu_cfg.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 11 月 28 日
**
** 描        述: CPU 类型与功能配置.
*********************************************************************************************************/

#ifndef __CPU_CFG_H
#define __CPU_CFG_H

#ifdef __GNUC__
#if defined(__arm__)
#include "cpu_cfg_arm.h"

#elif defined(__mips__)
#include "cpu_cfg_mips.h"

#elif defined(__PPC__)
#include "cpu_cfg_ppc.h"

#elif defined(__i386__) || defined(__x86_64__)
#include "cpu_cfg_x86.h"

#elif defined(__TMS320C6X__)
#include "cpu_cfg_c6x.h"

#elif defined(__sparc__)
#include "cpu_cfg_sparc.h"
#endif

#else
//#include "cpu_cfg_arm.h"
#include "cpu_cfg_ppc.h"
#endif

#endif                                                                  /*  __CPU_CFG_H                 */
/*********************************************************************************************************
  END
*********************************************************************************************************/

