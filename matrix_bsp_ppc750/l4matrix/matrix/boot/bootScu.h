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
** 文   件   名: bootArmScu.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 02 月 07 日
**
** 描        述: ARM SNOOP CONTROL UNIT.
*********************************************************************************************************/

#ifndef __BOOT_SCU_H
#define __BOOT_SCU_H

/*********************************************************************************************************
  寄存器定义
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

