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
** 文   件   名: libc.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 03 月 02 日
**
** 描        述: .
*********************************************************************************************************/

#ifndef __LIBC_H
#define __LIBC_H

/*********************************************************************************************************
  编译器内部类型
*********************************************************************************************************/
typedef __builtin_va_list          va_list;
#define va_start(ap, fmt)          (__builtin_va_start(ap, fmt))
#define va_arg(ap, type)           (__builtin_va_arg(ap, type))
#define va_end(ap)                 (__builtin_va_end(ap))
/*********************************************************************************************************
  库函数声明
*********************************************************************************************************/
int  lib_vsprintf(char  *str, const char  *pcFormat, va_list  arp);
int  lib_printf(const char  *pcFormat, ...);

#endif                                                                  /*  __LIBC_H                    */
/*********************************************************************************************************
  END
*********************************************************************************************************/

