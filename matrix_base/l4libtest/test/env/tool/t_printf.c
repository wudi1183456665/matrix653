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
** 文   件   名: t_printf.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 6 日
**
** 描        述: 测试启动代码
*********************************************************************************************************/
#include "t_sys.h"
/*********************************************************************************************************
  类型定义
*********************************************************************************************************/
typedef IPC_STRING   T_STRING;
typedef T_STRING    *PT_STRING;

typedef __builtin_va_list va_list;

#define va_start(ap, last)         (__builtin_va_start(ap, last))
#define va_arg(ap, type)           (__builtin_va_arg(ap, type))
#define va_end(ap)

enum {
    MODE_FLAGS          = 0,
    MODE_WIDTH          = 1,
    MODE_PRECS          = 2,
    FLAG_SIGNED         = 1UL << 0,
    FLAG_ALT_FORM       = 1UL << 1,
    FLAG_ZERO_PAD       = 1UL << 2,
};
/*********************************************************************************************************
  color
*********************************************************************************************************/
static inline UINT64 __div64 (UINT64 n, UINT32 d, UINT32 *r)
{
    *r = (UINT32)(n % d);
    return  (n / d);
}

static inline void __vput (PT_STRING pStr, CHAR ch)
{
    pStr->ISTR_ucBuf[pStr->ISTR_ulSize++] = ch;
}

VOID __vprintNum (PT_STRING pStr, UINT64 val, UINT32 base, UINT32 width, UINT32 flags)
{
    BOOL neg = FALSE;

    if (flags & FLAG_SIGNED && (INT64)(val) < 0) {
        neg = TRUE;
        val = -val;
    }

    CHAR buffer[24], *ptr = buffer + sizeof(buffer);

    do {
        UINT32 r;
        val = __div64 (val, base, &r);
        *--ptr = r["0123456789abcdef"];
    } while (val);

    if (neg)
        *--ptr = '-';

    ULONG count = buffer + sizeof(buffer) - ptr;
    ULONG n     = count + (flags & FLAG_ALT_FORM ? 2 : 0);

    if (flags & FLAG_ZERO_PAD) {
        if (flags & FLAG_ALT_FORM) {
            __vput (pStr, '0');
            __vput (pStr, 'x');
        }
        while (n++ < width)
            __vput (pStr, '0');
    } else {
        while (n++ < width)
            __vput (pStr, ' ');
        if (flags & FLAG_ALT_FORM) {
            __vput (pStr, '0');
            __vput (pStr, 'x');
        }
    }

    while (count--)
        __vput (pStr, *ptr++);
}

VOID __vprintStr (PT_STRING pStr, CHAR const *s, UINT32 width, UINT32 precs)
{
    if (!s) {
        return;
    }
    UINT32 n;

    for (n = 0; *s && precs--; n++)
        __vput (pStr, *s++);

    while (n++ < width)
        __vput (pStr, ' ');
}

VOID __vprintf (PT_STRING pStr, CHAR const *format, va_list args)
{
    UINT64 u;

    while (*format) {

        if(*format != '%') {
            __vput(pStr, *format++);
            continue;
        }

        UINT32 flags = 0, width = 0, precs = 0, len = 0, mode = MODE_FLAGS;

        for (u=0;;) {

            switch (*++format) {

                case '0'...'9':
                    switch (mode) {
                        case MODE_FLAGS:
                            if (*format == '0') {
                                flags |= FLAG_ZERO_PAD;
                                break;
                            }
                            mode = MODE_WIDTH;
                        case MODE_WIDTH: width = width * 10 + *format - '0'; break;
                        case MODE_PRECS: precs = precs * 10 + *format - '0'; break;
                    }
                    continue;

                case '.':
                    mode = MODE_PRECS;
                    continue;

                case '#':
                    if (mode == MODE_FLAGS)
                        flags |= FLAG_ALT_FORM;
                    continue;

                case 'l':
                    len++;
                    continue;

                case 'c':
                    __vput (pStr, va_arg (args, INT));
                    break;

                case 's':
                    __vprintStr (pStr, va_arg (args, CHAR *), width, precs ? precs : ~0u);
                    break;

                case 'd':
                    switch (len) {
                        case 0:  u = va_arg (args, INT32);      break;
                        case 1:  u = va_arg (args, LONG);       break;
                        default: u = va_arg (args, INT64);      break;
                    }
                    __vprintNum (pStr, u, 10, width, flags | FLAG_SIGNED);
                    break;

                case 'u':
                case 'x':
                    switch (len) {
                        case 0:  u = va_arg (args, UINT32);  break;
                        case 1:  u = va_arg (args, ULONG);   break;
                        default: u = va_arg (args, UINT64);  break;
                    }
                    __vprintNum (pStr, u, *format == 'x' ? 16 : 10, width, flags);
                    break;

                case 'p':
                    __vprintNum (pStr, (LONG)(va_arg (args, VOID *)), 16, width, FLAG_ALT_FORM);
                    break;

                case 0:
                    format--;

                default:
                    __vput (pStr, *format);
                    break;
            }

            format++;

            break;
        }
    }
}

VOID T__sprintf(PT_STRING pStr, CPCHAR format, ...)
{
    va_list args;
    va_start (args, format);
    __vprintf (pStr, format, args);
    va_end (args);
}

/*
 * 解析格式化字符串
 */
VOID T_printf(CPCHAR format, ...)
{
    va_list args;

    PMX_IPC_MSG pMsg  = T_vcpuGetCurMsg();
    PT_STRING   pStr  = (PVOID)(pMsg->IMSG_chBuff + IPC_DEBUG_BUF_OFFSET);
    IPC_ASSERT(pMsg != MX_NULL);

    pStr->ISTR_ulSize = 0;

    va_start (args, format);
    __vprintf (pStr, format, args);
    va_end (args);
    __vput(pStr, '\0');

    API_debug(MX_SC_OPCODE_DEBUG_PRINT);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
