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
** ��   ��   ��: api.h
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 1 ��
**
** ��        ��: ABI �ӿ�
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
