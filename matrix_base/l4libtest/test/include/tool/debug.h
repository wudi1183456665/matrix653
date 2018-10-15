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
** 文   件   名: debug.h
**
*** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 02 月 01 日
**
** 描        述: 系统测试调试环境.
*********************************************************************************************************/

#ifndef __DEBUG_H
#define __DEBUG_H

/*********************************************************************************************************
  用户模式打印调试信息
*********************************************************************************************************/
#define __T_PRINTF(col, prefix, format, args...)                      \
        do {                                                          \
            if (__CPU_MODE() == __CPU_KERN) {                         \
                if (MX_VCPU_GET_CUR() != MX_NULL) {                   \
                    dump("%s%s[%d][%x][%s]"#format"\n\033[0m",        \
                         col, prefix,                                 \
                         MX_CPU_GET_CUR_ID(),                         \
                         MX_VCPU_GET_CUR(),                           \
                         __func__, ##args);                           \
                } else {                                              \
                    bspDebugMsg("%s%s[%d][%x][%s]"#format"\n\033[0m", \
                                col, prefix,                          \
                                MX_CPU_GET_CUR_ID(),                  \
                                MX_VCPU_GET_CUR(),                    \
                                __func__, ##args);                    \
                }                                                     \
            } else {                                                  \
                T_printf("%s%s[%s]"#format"\n\033[0m",                \
                         col, prefix, __func__, ##args);              \
            }                                                         \
        } while(0)

#define T_ASSERT(format,args...)  __T_PRINTF(__COL_RED,    __MSG_ASS, format, ##args)
#define T_WARNING(format,args...) __T_PRINTF(__COL_PURPLE, __MSG_WAR, format, ##args)
#define T_ERROR(format,args...)   __T_PRINTF(__COL_RED,    __MSG_ERR, format, ##args)
#define T_TITLE(format,args...)   __T_PRINTF(__COL_GREEN,  __MSG_TIL, format, ##args)
#define T_NOTE(format,args...)    __T_PRINTF(__COL_YELLOW, __MSG_NOT, format, ##args)
#define T_DEBUG(format,args...)   __T_PRINTF(__COL_WHITE,  __MSG_DBG, format, ##args)


/*********************************************************************************************************
  线程休眠
*********************************************************************************************************/
#define K_SLEEP(ticks)                                                \
        do {                                                          \
            if (__CPU_MODE() == __CPU_KERN) {                         \
                vcpuSleep(ticks);                                     \
            } else if (__CPU_MODE() == __CPU_USER) {                  \
               /* __asm__ __volatile__("swi  9 \n\t"); */                 \
            }                                                         \
        } while(0)

VOID  T_printf(CPCHAR format, ...);

#endif                                                                  /*  DEBUG_H_                    */
/*********************************************************************************************************
  END
*********************************************************************************************************/
