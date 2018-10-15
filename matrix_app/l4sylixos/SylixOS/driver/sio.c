/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: poll_sio.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2014 年 12 月 23 日
**
** 描        述: POLL 串口 SIO 驱动.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#define  __SYLIXOS_STDIO
#include "SylixOS.h"
#include "uart.h"
/*********************************************************************************************************
  定义
*********************************************************************************************************/
#define __UART_DEFAULT_BAUD         (SIO_BAUD_115200)                   /*  缺省波特率                  */
#define __UART_DEFAULT_OPT          (CREAD | CS8)                       /*  缺省选项                    */

#define __UART_NR                   1                                   /*  仅支持串口 0                */
/*********************************************************************************************************
  UART SIO 通道控制块类型定义
*********************************************************************************************************/
typedef struct {
    SIO_CHAN            SIOCHAN_sioChan;

    INT               (*SIOCHAN_pcbGetTxChar)();                        /*  中断回调函数                */
    INT               (*SIOCHAN_pcbPutRcvChar)();

    PVOID               SIOCHAN_pvGetTxArg;                             /*  回调函数参数                */
    PVOID               SIOCHAN_pvPutRcvArg;

    INT                 SIOCHAN_iBaud;                                  /*  波特率                      */
    INT                 SIOCHAN_iHwOption;                              /*  硬件选项                    */

    LW_OBJECT_HANDLE    SIOCHAN_hThread;

} __POLL_UART_SIOCHAN;
typedef __POLL_UART_SIOCHAN        *__PPOLL_UART_SIOCHAN;               /*  指针类型                    */
/*********************************************************************************************************
  UART SIO 通道控制块数组定义
*********************************************************************************************************/
static __POLL_UART_SIOCHAN  _G_pollUartSioChannels[__UART_NR];
/*********************************************************************************************************
  定义
*********************************************************************************************************/
#define container_of(ptr, type, member) ({ const typeof( ((type *)0)->member ) *__mptr = (ptr); \
                                           (type *)( (char *)__mptr - offsetof(type,member) );})
/*********************************************************************************************************
** 函数名称: __pollSioCbInstall
** 功能描述: SIO 通道安装回调函数
** 输　入  : pSioChan                  SIO 通道
**           iCallbackType             回调类型
**           callbackRoute             回调函数
**           pvCallbackArg             回调参数
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __pollSioCbInstall (SIO_CHAN        *pSioChan,
                                INT              iCallbackType,
                                VX_SIO_CALLBACK  callbackRoute,
                                PVOID            pvCallbackArg)
{
    __PPOLL_UART_SIOCHAN  pUartSioChan = container_of(pSioChan, __POLL_UART_SIOCHAN, SIOCHAN_sioChan);
    
    switch (iCallbackType) {
    
    case SIO_CALLBACK_GET_TX_CHAR:                                      /*  发送回电函数                */
        pUartSioChan->SIOCHAN_pcbGetTxChar = callbackRoute;
        pUartSioChan->SIOCHAN_pvGetTxArg   = pvCallbackArg;
        return  (ERROR_NONE);
        
    case SIO_CALLBACK_PUT_RCV_CHAR:                                     /*  接收回电函数                */
        pUartSioChan->SIOCHAN_pcbPutRcvChar = callbackRoute;
        pUartSioChan->SIOCHAN_pvPutRcvArg   = pvCallbackArg;
        return  (ERROR_NONE);
        
    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** 函数名称: __pollSioPollRxChar
** 功能描述: SIO 通道轮询接收
** 输　入  : pSioChan                  SIO 通道
**           pcInChar                  接收的字节
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __pollSioPollRxChar (SIO_CHAN  *pSioChan, PCHAR  pcInChar)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __pollSioPollTxChar
** 功能描述: SIO 通道轮询发送
** 输　入  : pSioChan                  SIO 通道
**           cOutChar                  发送的字节
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __pollSioPollTxChar (SIO_CHAN  *pSioChan, CHAR  cOutChar)
{
    uart_putc(cOutChar);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __pollSioStartup
** 功能描述: SIO 通道发送(没有使用中断)
** 输　入  : pSioChan                  SIO 通道
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
**
** 注    意: 这个函数有可能在非发送中断的中断里调用!
*********************************************************************************************************/
static INT  __pollSioStartup (SIO_CHAN  *pSioChan)
{
    __PPOLL_UART_SIOCHAN  pUartSioChan = container_of(pSioChan, __POLL_UART_SIOCHAN, SIOCHAN_sioChan);
    CHAR                  cChar;

    while (pUartSioChan->SIOCHAN_pcbGetTxChar(pUartSioChan->SIOCHAN_pvGetTxArg, &cChar) == ERROR_NONE) {
        uart_putc(cChar);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __pollSioRecvThread
** 功能描述: 轮循接收线程
** 输　入  : pvArg              SIO 通道
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  *__pollSioRecvThread (VOID  *pvArg)
{
    SIO_CHAN             *pSioChan     = pvArg;
    __PPOLL_UART_SIOCHAN  pUartSioChan = container_of(pSioChan, __POLL_UART_SIOCHAN, SIOCHAN_sioChan);
    CHAR                  cChar;

    while (1) {
        while (uart_tstc() == 0){
            API_TimeSleep(1);
        }
        cChar = uart_getc();
        pUartSioChan->SIOCHAN_pcbPutRcvChar(pUartSioChan->SIOCHAN_pvPutRcvArg, cChar);
    }

    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: __pollSioIoctl
** 功能描述: SIO 通道控制
** 输　入  : pSioChan                  SIO 通道
**           iCmd                      命令
**           lArg                      参数
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __pollSioIoctl (SIO_CHAN  *pSioChan, INT  iCmd, PVOID  lArg)
{
    __PPOLL_UART_SIOCHAN  pUartSioChan = container_of(pSioChan, __POLL_UART_SIOCHAN, SIOCHAN_sioChan);
    LW_CLASS_THREADATTR   threadAtrr;
    switch (iCmd) {
    
    case SIO_OPEN:                                                      /*  打开串口                    */
        pUartSioChan->SIOCHAN_iBaud        = __UART_DEFAULT_BAUD;       /*  初始化波特率                */
        pUartSioChan->SIOCHAN_iHwOption    = __UART_DEFAULT_OPT;        /*  初始化硬件状态              */

        threadAtrr =  API_ThreadAttrGetDefault();
        threadAtrr.THREADATTR_pvArg      = pSioChan;
        threadAtrr.THREADATTR_ucPriority = 50;//LW_PRIO_LOWEST - 1;
        pUartSioChan->SIOCHAN_hThread = API_ThreadCreate("t_sio", __pollSioRecvThread, &threadAtrr, LW_NULL);
        break;
        
    case SIO_HUP:                                                       /*  关闭串口                    */
        break;

    case SIO_BAUD_SET:                                                  /*  设置波特率                  */
        pUartSioChan->SIOCHAN_iBaud = (INT)lArg;                        /*  记录波特率                  */
        break;

    case SIO_BAUD_GET:                                                  /*  获得波特率                  */
        *((LONG *)lArg) = pUartSioChan->SIOCHAN_iBaud;
        break;

    case SIO_HW_OPTS_SET:                                               /*  设置硬件参数                */
        pUartSioChan->SIOCHAN_iHwOption = (INT)lArg;                    /*  记录硬件参数                */
        break;

    case SIO_HW_OPTS_GET:                                               /*  获取硬件参数                */
        *((LONG *)lArg) = pUartSioChan->SIOCHAN_iHwOption;
        break;

    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  UART SIO 驱动程序
*********************************************************************************************************/
static SIO_DRV_FUNCS    _G_pollSioDrvFuncs = {
             __pollSioIoctl,
             __pollSioStartup,
             __pollSioCbInstall,
             __pollSioPollRxChar,
             __pollSioPollTxChar
};
/*********************************************************************************************************
** 函数名称: l4_sioChanCreate
** 功能描述: 创建一个 SIO 通道
** 输　入  : uiChannel                 UART 通道号
** 输　出  : SIO 通道
** 全局变量:
** 调用模块:
*********************************************************************************************************/
SIO_CHAN  *sioChanCreate (UINT  uiChannel)
{
    __PPOLL_UART_SIOCHAN        pUartSioChan;

    if (uiChannel >= __UART_NR) {
        return  (LW_NULL);                                              /*  通道号错误                  */
    }

    pUartSioChan = &_G_pollUartSioChannels[uiChannel];
    pUartSioChan->SIOCHAN_sioChan.pDrvFuncs = &_G_pollSioDrvFuncs;      /*  SIO FUNC                    */

    return  (&pUartSioChan->SIOCHAN_sioChan);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/



/**********************************************************************************************************
  serial_printf
**********************************************************************************************************/
static int df_putc (char c)
{
    uart_putc(c);
    return 1;
}
static int df_puts (char *s)
{
    int i = 0;
    if (s == 0) return 0;
    while (s[i]) {
        df_putc(s[i]);
        i++;
    }
    return i;
}
#define f_putc(c, f)  df_putc(c)
#define f_puts(s, f)  df_puts(s)
#define ULONG         unsigned long
#define UCHAR         unsigned char
#ifndef EOF
#define EOF           -1
#endif

#if 1
int serial_printf (const char* str, ... )
{
    va_list arp;
    UCHAR c, f, r;
    ULONG val;
    char s[16];
    int i, w, res, cc;


    va_start(arp, str);

    for (cc = res = 0; cc != EOF; res += cc) {
        c = *str++;
        if (c == 0) break;            /* End of string */
        if (c != '%') {               /* Non escape cahracter */
            cc = f_putc(c, fil);
            if (cc != EOF) cc = 1;
            continue;
        }
        w = f = 0;
        c = *str++;
        if (c == '%') {               /* print '%' _Dong */
            cc = f_putc(c, fil);
            continue;
        }
        if (c == '0') {               /* Flag: '0' padding */
            f = 1; c = *str++;
        }
        if (c == ' ') {               /* Flag: ' ' padding _Dong */
            c = *str++;
        }
        while (c >= '0' && c <= '9') {/* Precision */
            w = w * 10 + (c - '0');
            c = *str++;
        }
        if (c == 'l') {               /* Prefix: Size is long int */
            f |= 2; c = *str++;
            if (c == 'l') {
                f |= 8; c = *str++;
            }
        }
        if (c == 's') {               /* Type is string */
            cc = f_puts(va_arg(arp, char*), fil);
            continue;
        }
        if (c == 'c') {               /* Type is character */
            cc = f_putc(va_arg(arp, int), fil);
            if (cc != EOF) cc = 1;
            continue;
        }
        r = 0;
        if (c == 'd') r = 10;         /* Type is signed decimal */
        if (c == 'u') r = 10;         /* Type is unsigned decimal */
        if (c == 'X' || c == 'x' || c == 'p') r = 16;        /* Type is unsigned hexdecimal _Dong */
        if (r == 0) break;            /* Unknown type */
        if (f & 8) {                  /* Get the value */
            val = (ULONG long)va_arg(arp, long long);
        } else if (f & 2) {
            val = (ULONG)va_arg(arp, long);
        } else {
            val = (c == 'd') ? (ULONG)(long)va_arg(arp, int) : (ULONG)va_arg(arp, unsigned int);
        }
        /* Put numeral string */
        if (c == 'd') {
            if (val & 0x80000000) {
                val = 0 - val;
                f |= 4;
            }
        }
        i = sizeof(s) - 1; s[i] = 0;
        do {
            c = (UCHAR)(val % r + '0');
            if (c > '9') c += 7;
            s[--i] = c;
            val /= r;
        } while (i && val);
        if (i && (f & 4)) s[--i] = '-';
        w = sizeof(s) - 1 - w;
        while (i && i > w) s[--i] = (f & 1) ? '0' : ' ';
        cc = f_puts(&s[i], fil);
    }

    va_end(arp);
    return (cc == EOF) ? cc : res;
}
#else
/*
 * Actual printf innards.
 *
 * This code is large and complicated...
 */
#include "stdio.h"

#include "sys/types.h"

#include "limits.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

/*
 * Macros for converting digits to letters and vice versa
 */
#define        to_digit(c)        ((c) - '0')
#define        is_digit(c)        ((unsigned)to_digit(c) <= 9)
#define        to_char(n)         ((n) + '0')

/*
 * Convert an unsigned long to ASCII for printf purposes, returning
 * a pointer to the first character of the string representation.
 * Octal numbers can be forced to have a leading zero; hex numbers
 * use the given digits.
 */
static char * __ultoa(val, endp, base, octzero, xdigs)
        register u_long val;
        char *endp;
        int base, octzero;
        char *xdigs;
{
        register char *cp = endp;
        register long sval;

        /*
         * Handle the three cases separately, in the hope of getting
         * better/faster code.
         */
        switch (base) {
        case 10:
                if (val < 10) {        /* many numbers are 1 digit */
                        *--cp = to_char(val);
                        return (cp);
                }
                /*
                 * On many machines, unsigned arithmetic is harder than
                 * signed arithmetic, so we do at most one unsigned mod and
                 * divide; this is sufficient to reduce the range of
                 * the incoming value to where signed arithmetic works.
                 */
                if (val > LONG_MAX) {
                        *--cp = to_char(val % 10);
                        sval = val / 10;
                } else
                        sval = val;
                do {
                        *--cp = to_char(sval % 10);
                        sval /= 10;
                } while (sval != 0);
                break;

        case 8:
                do {
                        *--cp = to_char(val & 7);
                        val >>= 3;
                } while (val);
                if (octzero && *cp != '0')
                        *--cp = '0';
                break;

        case 16:
                do {
                        *--cp = xdigs[val & 15];
                        val >>= 4;
                } while (val);
                break;

        default:                        /* oops */
                abort();
        }
        return (cp);
}

/* Identical to __ultoa, but for quads. */
static char * __uqtoa(val, endp, base, octzero, xdigs)
        register u_quad_t val;
        char *endp;
        int base, octzero;
        char *xdigs;
{
        register char *cp = endp;
        register quad_t sval;

        /* quick test for small values; __ultoa is typically much faster */
        /* (perhaps instead we should run until small, then call __ultoa?) */
        if (val <= ULONG_MAX)
                return (__ultoa((u_long)val, endp, base, octzero, xdigs));
        switch (base) {
        case 10:
                if (val < 10) {
                        *--cp = to_char(val % 10);
                        return (cp);
                }
                if (val > QUAD_MAX) {
                        *--cp = to_char(val % 10);
                        sval = val / 10;
                } else
                        sval = val;
                do {
                        *--cp = to_char(sval % 10);
                        sval /= 10;
                } while (sval != 0);
                break;

        case 8:
                do {
                        *--cp = to_char(val & 7);
                        val >>= 3;
                } while (val);
                if (octzero && *cp != '0')
                        *--cp = '0';
                break;

        case 16:
                do {
                        *--cp = xdigs[val & 15];
                        val >>= 4;
                } while (val);
                break;

        default:
                abort();
        }
        return (cp);
}

#define        BUF              68
/*
 * Flags used during conversion.
 */
#define        ALT              0x001                /* alternate form */
#define        HEXPREFIX        0x002                /* add 0x or 0X prefix */
#define        LADJUST          0x004                /* left adjustment */
#define        LONGDBL          0x008                /* long double; unimplemented */
#define        LONGINT          0x010                /* long integer */
#define        QUADINT          0x020                /* quad integer */
#define        LLONGINT         QUADINT
#define        SHORTINT         0x040                /* short integer */
#define        ZEROPAD          0x080                /* zero (as opposed to blank) pad */
#define        FPT              0x100                /* Floating point number */
#define        GROUPING         0x200                /* use grouping ("'" flag) */
                                                     /* C99 additional size modifiers: */
#define        SIZET            0x400                /* size_t */
#define        PTRDIFFT         0x800                /* ptrdiff_t */
#define        INTMAXT          0x1000               /* intmax_t */
#define        CHARINT          0x2000               /* print char using int format */

int serial_printf (const char* fmt0, ... )
{
        register char *fmt;        /* format string */
        register int ch;        /* character from fmt */
        register int n;                /* handy integer (short term usage) */
        register char *cp;        /* handy char pointer (short term usage) */
        register int flags;        /* flags as above */
        int  ret;                /* return value accumulator */
        int  width;                /* width from format (%8d), or 0 */
        int  prec;                /* precision from format (%.3d), or -1 */
        char sign;                /* sign prefix (' ', '+', '-', or \0) */
        u_long   ulval;                /* integer arguments %[diouxX] */
        u_quad_t uqval;                /* %q integers */
        int base;                /* base for [diouxX] conversion */
        int dprec;                /* a copy of prec if [diouxX], 0 otherwise */
        int fieldsz;                /* field size expanded by sign, etc */
        int realsz;                /* field size expanded by dprec */
        int size;                /* size of converted field or string */
        char *xdigs;                /* digits for [xX] conversion */
        char buf[BUF];                /* space for %c, %[diouxX], %[eEfgG] */
        char ox[2];                /* space for 0x hex-prefix */

        va_list ap;
        va_start(ap, fmt0);
        /*
         * Choose PADSIZE to trade efficiency vs. size.  If larger printf
         * fields occur frequently, increase PADSIZE and make the initialisers
         * below longer.
         */
#define PADSIZE        16                /* pad chunk size */
        static char blanks[PADSIZE] =
         {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
        static char zeroes[PADSIZE] =
         {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};

#define PRINT(ptr, len) { \
    int i = 0; \
    for (i = 0; i < len; i++) { \
        df_putc((ptr)[i]); \
    } \
}
#define PAD(howmany, with) { \
        if ((n = (howmany)) > 0) { \
                while (n > PADSIZE) { \
                        PRINT(with, PADSIZE); \
                        n -= PADSIZE; \
                } \
                PRINT(with, n); \
        } \
}

        /*
         * To extend shorts properly, we need both signed and unsigned
         * argument extraction methods.
         */
#define SARG() \
        (flags&LONGINT ? va_arg(ap, long) : \
            flags&SHORTINT ? (long)(short)va_arg(ap, int) : \
            (long)va_arg(ap, int))
#define UARG() \
        (flags&LONGINT ? va_arg(ap, u_long) : \
            flags&SHORTINT ? (u_long)(u_short)va_arg(ap, int) : \
            (u_long)va_arg(ap, u_int))

        fmt = (char *)fmt0;
        ret = 0;

        /*
         * Scan the format for conversions (`%' character).
         */
        for (;;) {
                for (cp = fmt; (ch = *fmt) != '\0' && ch != '%'; fmt++)
                        /* void */;
                if ((n = fmt - cp) != 0) {
                        PRINT(cp, n);
                        ret += n;
                }
                if (ch == '\0')
                        goto done;
                fmt++;                /* skip over '%' */

                flags = 0;
                dprec = 0;
                width = 0;
                prec = -1;
                sign = '\0';

rflag:          ch = *fmt++;
reswitch:       switch (ch) {
                case ' ':
                        /*
                         * ``If the space and + flags both appear, the space
                         * flag will be ignored.''
                         *        -- ANSI X3J11
                         */
                        if (!sign)
                                sign = ' ';
                        goto rflag;
                case '#':
                        flags |= ALT;
                        goto rflag;
                case '*':
                        /*
                         * ``A negative field width argument is taken as a
                         * - flag followed by a positive field width.''
                         *        -- ANSI X3J11
                         * They don't exclude field widths read from args.
                         */
                        if ((width = va_arg(ap, int)) >= 0)
                                goto rflag;
                        width = -width;
                        /* FALLTHROUGH */
                case '-':
                        flags |= LADJUST;
                        goto rflag;
                case '+':
                        sign = '+';
                        goto rflag;
                case '.':
                        if ((ch = *fmt++) == '*') {
                                n = va_arg(ap, int);
                                prec = n < 0 ? -1 : n;
                                goto rflag;
                        }
                        n = 0;
                        while (is_digit(ch)) {
                                n = 10 * n + to_digit(ch);
                                ch = *fmt++;
                        }
                        prec = n < 0 ? -1 : n;
                        goto reswitch;
                case '0':
                        /*
                         * ``Note that 0 is taken as a flag, not as the
                         * beginning of a field width.''
                         *        -- ANSI X3J11
                         */
                        flags |= ZEROPAD;
                        goto rflag;
                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                        n = 0;
                        do {
                                n = 10 * n + to_digit(ch);
                                ch = *fmt++;
                        } while (is_digit(ch));
                        width = n;
                        goto reswitch;
                case 'h':
                        flags |= SHORTINT;
                        goto rflag;
                case 'l':
                        if (flags & LONGINT) {
                                flags &= ~LONGINT;
                                flags |= QUADINT;
                        } else
                                flags |= LONGINT;
                        goto rflag;
                case 'q':
                        flags |= QUADINT;
                        goto rflag;
                case 't':
                        flags |= PTRDIFFT;
                        goto rflag;
                case 'z':
                        flags |= SIZET;
                        goto rflag;
                case 'c':
                        *(cp = buf) = va_arg(ap, int);
                        size = 1;
                        sign = '\0';
                        break;
                case 'D':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'd':
                case 'i':
                        if (flags & QUADINT) {  /* long long int (max bits) */
                                uqval = va_arg(ap, quad_t);
                                if ((quad_t)uqval < 0) {
                                        uqval = -uqval;
                                        sign = '-';
                                }
                        } else if (flags & SIZET) { /* size_t type add by sylixos */
                            ulval = va_arg(ap, size_t);
                                if ((long)ulval < 0) {
                                        ulval = -ulval;
                                        sign = '-';
                                }
                        } else { /* long int (max bits) */
                                ulval = SARG();
                                if ((long)ulval < 0) {
                                        ulval = -ulval;
                                        sign = '-';
                                }
                        }
                        base = 10;
                        goto number;
                case 'n':
                        if (flags & QUADINT)
                                *va_arg(ap, quad_t *) = ret;
                        else if (flags & SIZET)
                                *va_arg(ap, ssize_t *) = (ssize_t)ret;
                        else if (flags & PTRDIFFT)
                                *va_arg(ap, ptrdiff_t *) = ret;
                        else if (flags & LONGINT)
                                *va_arg(ap, long *) = ret;
                        else if (flags & SHORTINT)
                                *va_arg(ap, short *) = ret;
                        else
                                *va_arg(ap, int *) = ret;
                        continue;        /* no output */
                case 'O':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'o':
                        if (flags & QUADINT)
                                uqval = va_arg(ap, u_quad_t);
                        else
                                ulval = UARG();
                        base = 8;
                        goto nosign;
                case 'p':
                        /*
                         * ``The argument shall be a pointer to void.  The
                         * value of the pointer is converted to a sequence
                         * of printable characters, in an implementation-
                         * defined manner.''
                         *        -- ANSI X3J11
                         */
                        ulval = (u_long)va_arg(ap, void *);
                        base = 16;
                        xdigs = "0123456789abcdef";
                        flags = (flags & ~QUADINT) | HEXPREFIX;
                        ch = 'x';
                        goto nosign;
                case 's':
                        if ((cp = va_arg(ap, char *)) == NULL)
                                cp = "(null)";
                        if (prec >= 0) {
                                /*
                                 * can't use strlen; can only look for the
                                 * NUL in the first `prec' characters, and
                                 * strlen() will go further.
                                 */
                                char *p = memchr(cp, 0, prec);

                                if (p != NULL) {
                                    size = p - cp;
                                    if (size > prec)
                                        size = prec;
                                } else
                                    size = prec;
                        } else
                                size = strlen(cp);
                        sign = '\0';
                        break;
                case 'U':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'u':
                        if (flags & QUADINT)
                            uqval = va_arg(ap, u_quad_t);
                        else if (flags & SIZET) /* size_t type add by sylixos */
                            ulval = va_arg(ap, size_t);
                        else
                            ulval = UARG();
                        base = 10;
                        goto nosign;
                case 'X':
                        xdigs = "0123456789ABCDEF";
                        goto hex;
                case 'x':
                        xdigs = "0123456789abcdef";
hex:                    if (flags & QUADINT)
                                uqval = va_arg(ap, u_quad_t);
                        else if (flags & SIZET) /* size_t type add by sylixos */
                                ulval = va_arg(ap, size_t);
                        else
                                ulval = UARG();
                        base = 16;
                        /* leading 0x/X only if non-zero */
                        if (flags & ALT &&
                            (flags & QUADINT ? uqval != 0 : ulval != 0))
                                flags |= HEXPREFIX;

                        /* unsigned conversions */
nosign:                 sign = '\0';
                        /*
                         * ``... diouXx conversions ... if a precision is
                         * specified, the 0 flag will be ignored.''
                         *        -- ANSI X3J11
                         */
number:                 if ((dprec = prec) >= 0)
                                flags &= ~ZEROPAD;

                        /*
                         * ``The result of converting a zero value with an
                         * explicit precision of zero is no characters.''
                         *        -- ANSI X3J11
                         */
                        cp = buf + BUF;
                        if (flags & QUADINT) {
                            if (uqval != 0 || prec != 0)
                                cp = __uqtoa(uqval, cp, base, flags & ALT, xdigs);
                        } else {
                            if (ulval != 0 || prec != 0)
                                cp = __ultoa(ulval, cp, base, flags & ALT, xdigs);
                        }
                        size = buf + BUF - cp;
                        break;
                default:        /* "%?" prints ?, unless ? is NUL */
                        if (ch == '\0')
                                goto done;
                        /* pretend it was %c with argument ch */
                        cp = buf;
                        *cp = ch;
                        size = 1;
                        sign = '\0';
                        break;
                }

                /*
                 * All reasonable formats wind up here.  At this point, `cp'
                 * points to a string which (if not flags&LADJUST) should be
                 * padded out to `width' places.  If flags&ZEROPAD, it should
                 * first be prefixed by any sign or other prefix; otherwise,
                 * it should be blank padded before the prefix is emitted.
                 * After any left-hand padding and prefixing, emit zeroes
                 * required by a decimal [diouxX] precision, then print the
                 * string proper, then emit zeroes required by any leftover
                 * floating precision; finally, if LADJUST, pad with blanks.
                 *
                 * Compute actual size, so we know how much to pad.
                 * fieldsz excludes decimal prec; realsz includes it.
                 */
                fieldsz = size;
                if (sign)
                        fieldsz++;
                else if (flags & HEXPREFIX)
                        fieldsz += 2;
                realsz = dprec > fieldsz ? dprec : fieldsz;

                /* right-adjusting blank padding */
                if ((flags & (LADJUST|ZEROPAD)) == 0)
                        PAD(width - realsz, blanks);

                /* prefix */
                if (sign) {
                        PRINT(&sign, 1);
                } else if (flags & HEXPREFIX) {
                        ox[0] = '0';
                        ox[1] = ch;
                        PRINT(ox, 2);
                }

                /* right-adjusting zero padding */
                if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
                        PAD(width - realsz, zeroes);

                /* leading zeroes from decimal precision */
                PAD(dprec - fieldsz, zeroes);

                /* the string or number proper */
                PRINT(cp, size);
                
                /* left-adjusting padding (always blank) */
                if (flags & LADJUST)
                        PAD(width - realsz, blanks);

                /* finally, adjust ret */
                ret += width > realsz ? width : realsz;
        }
done:
        va_end(ap);
        return (ret);
        /* NOTREACHED */
}
#endif

