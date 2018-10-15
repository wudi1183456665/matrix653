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
** 文   件   名: lib_vsprintf.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 03 月 02 日
**
** 描        述: vsprintf().
*********************************************************************************************************/
#include <Matrix.h>
#include "libc.h"

static INT  __vputc (char  *str, UINT  *puiIdx, UCHAR  ucChar)
{
    str[*puiIdx] = ucChar;
    *puiIdx     += 1;

    return  (1);
}
static INT  __vputs (char  *str, UINT  *puiIdx, PUCHAR  pucStr)
{
    INT  i = 0;;
    while (*pucStr != '\0') {
        __vputc(str, puiIdx, *pucStr);
        pucStr++;
        i++;
    }
    return  (i);
}
/*********************************************************************************************************
** 函数名称: lib_vsprintf
** 功能描述: 将可变参数格式化到字符串缓存区中
** 输　入  : str           输出结果缓冲区
**           pcFormat      格式化规则
**           ...           可变参数
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
int  lib_vsprintf (char  *str, const char  *pcFormat, va_list  arp)
{
    UCHAR                  ucCurr;                                      /*  当前处理的格式字符          */
    UINT8                  ucFlags;                                     /*  1:填充 2:%l 4:负数 8:%ll    */
    UINT8                  ucRadix;                                     /*  进制，支持 10, 16           */
    UINT32                 uiVal;                                       /*  从参数中读取到的整型值      */
    UCHAR                  ucVal[16];                                   /*  存放整型值转化成字符串      */
    INT                    i;
    INT                    iWidth;                                      /*  格式字符串中的宽度值        */
    INT                    iOnce;                                       /*  每次打印字符个数            */
    INT                    iReturn;                                     /*  函数返回值，总打印字符个数  */
    UINT                   uiIdx = 0;                                   /*  输出缓冲区索引              */

    for (iOnce = iReturn = 0; iOnce != -1; iReturn += iOnce) {
        ucCurr = *pcFormat++;
        if (ucCurr == 0) {
            break;                                                      /*  打印结束                    */
        }

        if (ucCurr != '%') {                                            /*  非格式化字符，直接打印      */
            iOnce = __vputc(str, &uiIdx, ucCurr);
            if (iOnce != -1) {
                iOnce = 1;
            }
            continue;
        }

        iWidth = ucFlags = 0;
        ucCurr = *pcFormat++;                                           /*  获取 % 后一个字符           */

        /*
         *  处理 %%
         */
        if (ucCurr == '%') {                                            /*  print '%'                   */
            iOnce = __vputc(str, &uiIdx, ucCurr);
            continue;
        }

        /*
         *  处理格式字符串中的填充格式 %08, % 12
         */
        if (ucCurr == '0') {                                            /*  Flag: '0' padding           */
            ucFlags = 1;
            ucCurr = *pcFormat++;
        }
        if (ucCurr == ' ') {                                            /*  Flag: ' ' padding           */
            ucCurr = *pcFormat++;
        }
        while (ucCurr >= '0' && ucCurr <= '9') {                        /*  print width                 */
            iWidth = iWidth * 10 + (ucCurr - '0');
            ucCurr = *pcFormat++;
        }

        /*
         *  处理格式字符串中的填充格式 %s, %c
         */
        if (ucCurr == 's') {                                            /*  Type is string              */
            iOnce = __vputs(str, &uiIdx, va_arg(arp, PUCHAR));
            continue;
        }
        if (ucCurr == 'c') {                                            /*  Type is character           */
            iOnce = __vputc(str, &uiIdx, va_arg(arp, INT));
            if (iOnce != -1) {
                iOnce = 1;
            }
            continue;
        }

        /*
         *  处理格式字符串中的整型长度 %l, %ll
         */
        if (ucCurr == 'l') {                                            /*  Prefix: Size is long int    */
            ucFlags |= 2;
            ucCurr   = *pcFormat++;
            if (ucCurr == 'l') {                                        /*  Prefix: Size is long long   */
                ucFlags |= 8;
                ucCurr   = *pcFormat++;
            }
        }

        /*
         *  处理格式字符串中的进制格式 %d, %u, %X, %x, %p
         */
        ucRadix = 0;
        if (ucCurr == 'd') {
            ucRadix = 10;                                               /*  Type is signed decimal      */
        }
        if (ucCurr == 'u') {
            ucRadix = 10;                                               /*  Type is unsigned decimal    */
        }
        if (ucCurr == 'X' || ucCurr == 'x' || ucCurr == 'p') {
            ucRadix = 16;                                               /*  Type is unsigned hexdecimal */
        }
        if (ucRadix == 0) {
            break;                                                      /*  Unknown type                */
        }

        /*
         *  获取参数中的整型值
         */
        if (ucFlags & 8) {                                              /*  Get the long long value     */
            uiVal = (UINT64)va_arg(arp, INT64);

        } else if (ucFlags & 2) {                                       /*  Get the long long value     */
            uiVal = (UINT32)va_arg(arp, INT32);

        } else {
            uiVal = (ucCurr == 'd') ? (UINT32)va_arg(arp, INT) : (UINT32)va_arg(arp, UINT);
        }

        /*
         *  对于负数值，首先取绝对值
         */
        if (ucCurr == 'd') {
            if (uiVal & 0x80000000) {                                   /*  负数                        */
                uiVal    = 0 - uiVal;
                ucFlags |= 4;
            }
        }

        /*
         *  将数组转化成字符串
         */
        i = sizeof(ucVal) - 1;
        ucVal[i] = 0;
        do {
            ucCurr = (UCHAR)(uiVal % ucRadix + '0');
            if (ucCurr > '9') {
                ucCurr += 7;
            }
            ucVal[--i] = ucCurr;
            uiVal /= ucRadix;
        } while (i && uiVal);
        if (i && (ucFlags & 4)) {
            ucVal[--i] = '-';
        }

        /*
         *  将字符串填充成指定宽度
         */
        iWidth = sizeof(ucVal) - 1 - iWidth;
        while (i && i > iWidth) {
            ucVal[--i] = (ucFlags & 1) ? '0' : ' ';
        }

        iOnce = __vputs(str, &uiIdx, &ucVal[i]);                        /*  打印数值字符串              */
    }
    __vputc(str, &uiIdx, '\0');

    return  (uiIdx);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

