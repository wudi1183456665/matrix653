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
** ��   ��   ��: debug.h
**
*** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 02 �� 01 ��
**
** ��        ��: ϵͳ���Ե��Ի���.
*********************************************************************************************************/

#ifndef __DEBUG_H
#define __DEBUG_H

/*********************************************************************************************************
  �û�ģʽ��ӡ������Ϣ
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
  �߳�����
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
