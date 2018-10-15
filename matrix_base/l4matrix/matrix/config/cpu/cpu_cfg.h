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
** ��   ��   ��: cpu_cfg.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 11 �� 28 ��
**
** ��        ��: CPU �����빦������.
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

