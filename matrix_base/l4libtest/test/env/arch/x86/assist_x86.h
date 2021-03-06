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
** 文   件   名: assist_x86.h
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: ASSIST
*********************************************************************************************************/
#ifndef __T_ASSIST_X86_H
#define __T_ASSIST_X86_H

/*
 * 设置 syscall time
 */
#define T_ARCH_TIME_SET(ullTimeout,pulArg1,pulArg2) do {          \
            *pulArg1 = (UINT32)ullTimeout;                        \
            *pulArg2 = (UINT32)(ullTimeout >> 32);                \
        } while(0)

/*
 * 获取 syscall time
 */
#define T_ARCH_TIME_GET(ulArg1,ulArg2)                            \
        (((0ULL | ulArg2) << 32) |                                \
          (0ULL | ulArg1))

#endif                                                                  /*  __T_ASSIST_X86_H            */
/*********************************************************************************************************
  END
*********************************************************************************************************/
