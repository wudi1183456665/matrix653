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
** 文   件   名: k_debug.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 03 月 14 日
**
** 描        述: 微内核调试接口.
*********************************************************************************************************/

#ifndef __K_DEBUG_H
#define __K_DEBUG_H

/*********************************************************************************************************
  调试模块使用的系统调用号
*********************************************************************************************************/
#define DEBUG_SWI_ASS       3
#define DEBUG_SWI_WAR       4
#define DEBUG_SWI_ERR       5
#define DEBUG_SWI_TIL       6
#define DEBUG_SWI_NOT       7
#define DEBUG_SWI_DBG       8
/*********************************************************************************************************
  打印字体颜色
*********************************************************************************************************/
#define __COL_BLACK         "\033[1;30m"                                /*  黑   字                      */
#define __COL_RED           "\033[1;31m"                                /*  红   字                      */
#define __COL_GREEN         "\033[1;32m"                                /*  红   字                      */
#define __COL_YELLOW        "\033[1;33m"                                /*  黄   字                      */
#define __COL_BLUE          "\033[1;34m"                                /*  蓝   字                      */
#define __COL_PURPLE        "\033[1;35m"                                /*  紫   字                      */
#define __COL_BLUENESS      "\033[1;36m"                                /*  青   字                      */
#define __COL_WHITE         "\033[1;37m"                                /*  白   字                      */
/*********************************************************************************************************
  CPU 运行模式
*********************************************************************************************************/
#define __CPU_MODE()        (ppcGetMSR() & 0x1F)                      /*  获取当前运行模式            */
#define __CPU_USER          0x10                                        /*  用户模式                    */
#define __CPU_KERN          0x13                                        /*  特权模式                    */
/*********************************************************************************************************
  前缀标识 
*********************************************************************************************************/
#define __MSG_ASS           "[ASS]: "
#define __MSG_ERR           "[ERR]: "
#define __MSG_WAR           "[WAR]: "
#define __MSG_TIL           "[TIL]: "
#define __MSG_NOT           "[NOT]: "
#define __MSG_DBG           "[DBG]: "
/*********************************************************************************************************
  将宏内容转化为字符串
*********************************************************************************************************/
#define __STR(s)            #s
/*********************************************************************************************************
  初始化阶段和微内核代码打印调试信息宏
*********************************************************************************************************/
#define __K_PRINTF(col, prefix, swino, format, args...)               \
        do {                                                          \
            if (__CPU_MODE() == __CPU_KERN) {                         \
                if (MX_VCPU_GET_CUR() == MX_NULL) {                   \
                    bspDebugMsg("%s%s[%d][%X][%s]"#format"\n\033[0m", \
                                col, prefix,                          \
                                MX_CPU_GET_CUR_ID(),                  \
                                MX_VCPU_GET_CUR(),                    \
                                __func__, ##args);                    \
                } else {                                              \
                    dump("%s%s[%d][%X][%s]"#format"\n\033[0m",        \
                         col, prefix,                                 \
                         MX_CPU_GET_CUR_ID(),                         \
                         MX_VCPU_GET_CUR(),                           \
                         __func__, ##args);                           \
                }                                                     \
            } else if (__CPU_MODE() == __CPU_USER) {                  \
                /*__asm__ __volatile__("swi "__STR(swino)" \n\t");*/      \
            }                                                         \
        } while (0)

#define K_ASSERT(format, args...)   __K_PRINTF(__COL_RED,    __MSG_ASS, DEBUG_SWI_ASS, format, ##args)
#define K_WARNING(format, args...)  __K_PRINTF(__COL_PURPLE, __MSG_WAR, DEBUG_SWI_WAR, format, ##args)
#define K_ERROR(format, args...)    __K_PRINTF(__COL_RED,    __MSG_ERR, DEBUG_SWI_ERR, format, ##args)
#define K_TITLE(format, args...)    __K_PRINTF(__COL_GREEN,  __MSG_TIL, DEBUG_SWI_TIL, format, ##args)
#define K_NOTE(format, args...)     __K_PRINTF(__COL_YELLOW, __MSG_NOT, DEBUG_SWI_NOT, format, ##args)
#define K_DEBUG(format, args...)    __K_PRINTF(__COL_WHITE,  __MSG_DBG, DEBUG_SWI_DBG, format, ##args)

#endif                                                                  /*  DEBUG_H_                    */
/*********************************************************************************************************
  END
*********************************************************************************************************/


