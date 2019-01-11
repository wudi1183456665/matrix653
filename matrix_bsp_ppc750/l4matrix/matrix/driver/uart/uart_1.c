/*
 * uart_1.c
 *
 *  Created on: 2018��9��17��
 *      Author: LeoH
 *
** ��        ��: INTEL 8250 UART ����
*********************************************************************************************************/
#include <Matrix.h>
#include "../../config.h"
#include "uart_1.h"
/*********************************************************************************************************
  ����
*********************************************************************************************************/
#define UART_RBR            (0x0 << BSP_CFG_8250_REG_SHIFT)             /*  Receive Buffer Register     */
#define UART_THR            (0x0 << BSP_CFG_8250_REG_SHIFT)             /*  Transmit Holding Register   */
#define UART_DLL            (0x0 << BSP_CFG_8250_REG_SHIFT)             /*  Divisor Latch Low           */
#define UART_DLH            (0x1 << BSP_CFG_8250_REG_SHIFT)             /*  Divisor Latch High          */
#define UART_LCR            (0x3 << BSP_CFG_8250_REG_SHIFT)             /*  Line Control Register       */
#define UART_LSR            (0x5 << BSP_CFG_8250_REG_SHIFT)             /*  Line Status Register        */

#define UART_LCR_DLAB       (1 << 7)                                    /*  en divisor latch accessing  */
#define UART_LCR_DLS        (0x3)                                       /*  select 8bit data width      */
#define UART_LCR_VALUE1     (UART_LCR_DLAB | UART_LCR_DLS)
#define UART_LSR_THRE       (1 << 5)
#define UART_LSR_DR         (1)                                         /*  data ready                  */
/*********************************************************************************************************
** ��������: uart8250PutStr
** ��������: 8250 UART ����ַ���
** �䡡��  : addrBase              8250 UART ����ַ
**           pcMsg                 �ַ���
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  uart8250PutStr (addr_t  addrBase, CPCHAR  pcMsg)
{
    UINT32  uiLsr;

    if (!pcMsg) {                                                       /*  ָ��Ϊ��                    */
        return;
    }

    while (*pcMsg != '\0') {                                            /*  �����ַ���                  */
        while (1) {
            uiLsr = read8(addrBase + UART_LSR);
            if (uiLsr & UART_LSR_THRE) {
                break;
            }
        }

        write8(*pcMsg, addrBase + UART_THR);
        pcMsg++;
    }
}
/*********************************************************************************************************
** ��������: uart8250PutChar
** ��������: 8250 UART ����ַ�
** �䡡��  : addrBase              8250 UART ����ַ
**           cChar                 �ַ�
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  uartPutChar (CHAR  cChar)
{
    UINT32  uiLsr;

    while (1) {
        uiLsr = read8(BSP_CFG_8250_BASE + UART_LSR);
        if (uiLsr & UART_LSR_THRE) {
            break;
        }
    }

    write8(cChar, BSP_CFG_8250_BASE + UART_THR);
}
/*********************************************************************************************************
** ��������: uart8250GetChar
** ��������: 8250 UART ��ȡ�ַ�
** �䡡��  : addrBase              8250 UART ����ַ
** �䡡��  : �ַ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
CHAR  uart8250GetChar (addr_t  addrBase)
{
    UINT32  uiLsr;

    while (1) {
        uiLsr = read8(addrBase + UART_LSR);
        if (uiLsr & UART_LSR_DR) {
            break;
        }
    }

    return  (read8(addrBase + UART_RBR));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/




