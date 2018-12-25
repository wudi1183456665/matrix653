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
** ��   ��   ��: bootArmScu.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 02 �� 07 ��
**
** ��        ��: ARM SNOOP CONTROL UNIT.
*********************************************************************************************************/

#ifndef __BOOT_SCU_H
#define __BOOT_SCU_H

/*********************************************************************************************************
  �Ĵ�������
*********************************************************************************************************/

#define SCU_FEATURE_IC_STANDBY                     (1 << 6)
#define SCU_FEATURE_SCU_STANDBY                    (1 << 5)
#define SCU_FEATURE_ALL_DEV_TO_PORT0               (1 << 4)
#define SCU_FEATURE_SCU_SPECULATIVE_LINEFILL       (1 << 3)
#define SCU_FEATURE_SCU_RAMS_PARITY                (1 << 2)
#define SCU_FEATURE_ADDRESS_FILTERING              (1 << 1)
#define SCU_FEATURE_SCU                            (1 << 0)

VOID    bootArmScuFeatureEnable(UINT32  uiFeatures);
VOID    bootArmScuFeatureDisable(UINT32  uiFeatures);
VOID    bootArmScuSecureInvalidateAll(UINT32  uiCpuId,  UINT32  uiWays);
VOID    bootArmScuAccessCtrlSet(UINT32  uiCpuBits);

#endif                                                                  /*  __BOOT_SCU_H                */
/*********************************************************************************************************
  END
*********************************************************************************************************/

