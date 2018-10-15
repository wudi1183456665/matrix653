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
** ��   ��   ��: lib_vsprintf.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 03 �� 02 ��
**
** ��        ��: vsprintf().
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
** ��������: lib_vsprintf
** ��������: ���ɱ������ʽ�����ַ�����������
** �䡡��  : str           ������������
**           pcFormat      ��ʽ������
**           ...           �ɱ����
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
int  lib_vsprintf (char  *str, const char  *pcFormat, va_list  arp)
{
    UCHAR                  ucCurr;                                      /*  ��ǰ����ĸ�ʽ�ַ�          */
    UINT8                  ucFlags;                                     /*  1:��� 2:%l 4:���� 8:%ll    */
    UINT8                  ucRadix;                                     /*  ���ƣ�֧�� 10, 16           */
    UINT32                 uiVal;                                       /*  �Ӳ����ж�ȡ��������ֵ      */
    UCHAR                  ucVal[16];                                   /*  �������ֵת�����ַ���      */
    INT                    i;
    INT                    iWidth;                                      /*  ��ʽ�ַ����еĿ��ֵ        */
    INT                    iOnce;                                       /*  ÿ�δ�ӡ�ַ�����            */
    INT                    iReturn;                                     /*  ��������ֵ���ܴ�ӡ�ַ�����  */
    UINT                   uiIdx = 0;                                   /*  �������������              */

    for (iOnce = iReturn = 0; iOnce != -1; iReturn += iOnce) {
        ucCurr = *pcFormat++;
        if (ucCurr == 0) {
            break;                                                      /*  ��ӡ����                    */
        }

        if (ucCurr != '%') {                                            /*  �Ǹ�ʽ���ַ���ֱ�Ӵ�ӡ      */
            iOnce = __vputc(str, &uiIdx, ucCurr);
            if (iOnce != -1) {
                iOnce = 1;
            }
            continue;
        }

        iWidth = ucFlags = 0;
        ucCurr = *pcFormat++;                                           /*  ��ȡ % ��һ���ַ�           */

        /*
         *  ���� %%
         */
        if (ucCurr == '%') {                                            /*  print '%'                   */
            iOnce = __vputc(str, &uiIdx, ucCurr);
            continue;
        }

        /*
         *  �����ʽ�ַ����е�����ʽ %08, % 12
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
         *  �����ʽ�ַ����е�����ʽ %s, %c
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
         *  �����ʽ�ַ����е����ͳ��� %l, %ll
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
         *  �����ʽ�ַ����еĽ��Ƹ�ʽ %d, %u, %X, %x, %p
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
         *  ��ȡ�����е�����ֵ
         */
        if (ucFlags & 8) {                                              /*  Get the long long value     */
            uiVal = (UINT64)va_arg(arp, INT64);

        } else if (ucFlags & 2) {                                       /*  Get the long long value     */
            uiVal = (UINT32)va_arg(arp, INT32);

        } else {
            uiVal = (ucCurr == 'd') ? (UINT32)va_arg(arp, INT) : (UINT32)va_arg(arp, UINT);
        }

        /*
         *  ���ڸ���ֵ������ȡ����ֵ
         */
        if (ucCurr == 'd') {
            if (uiVal & 0x80000000) {                                   /*  ����                        */
                uiVal    = 0 - uiVal;
                ucFlags |= 4;
            }
        }

        /*
         *  ������ת�����ַ���
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
         *  ���ַ�������ָ�����
         */
        iWidth = sizeof(ucVal) - 1 - iWidth;
        while (i && i > iWidth) {
            ucVal[--i] = (ucFlags & 1) ? '0' : ' ';
        }

        iOnce = __vputs(str, &uiIdx, &ucVal[i]);                        /*  ��ӡ��ֵ�ַ���              */
    }
    __vputc(str, &uiIdx, '\0');

    return  (uiIdx);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

