/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: assist_arm.h
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 1 ��
**
** ��        ��: assist
*********************************************************************************************************/
#ifndef __T_ASSIST_ARM_H
#define __T_ASSIST_ARM_H

/*
 * ���� syscall time
 */
#define T_ARCH_TIME_SET(ullTimeout,pulArg1,pulArg2) do {          \
            *pulArg1 = (UINT32)ullTimeout;                        \
            *pulArg2 = (UINT32)(ullTimeout >> 32);                \
        } while(0)

/*
 * ��ȡ syscall time
 */
#define T_ARCH_TIME_GET(ulArg1,ulArg2)                            \
        (((0ULL | ulArg2) << 32) |                                \
          (0ULL | ulArg1))

#endif                                                                  /*  __T_ASSIST_ARM_H            */
/*********************************************************************************************************
  END
*********************************************************************************************************/