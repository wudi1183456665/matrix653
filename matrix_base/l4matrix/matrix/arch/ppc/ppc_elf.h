/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: ppcElf.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2015 �� 12 �� 15 ��
**
** ��        ��: ʵ�� PowerPC ��ϵ�ṹ�� ELF �ļ��ض�λ.
*********************************************************************************************************/

#ifndef __PPC_ELF_H
#define __PPC_ELF_H

#ifdef MX_CFG_CPU_ARCH_PPC                                              /*  PowerPC ��ϵ�ṹ            */

#define ELF_CLASS       ELFCLASS32
#define ELF_ARCH        EM_PPC

#endif                                                                  /*  LW_CFG_CPU_ARCH_PPC         */
#endif                                                                  /*  __PPC_ELF_H 	            */
/*********************************************************************************************************
  END
*********************************************************************************************************/