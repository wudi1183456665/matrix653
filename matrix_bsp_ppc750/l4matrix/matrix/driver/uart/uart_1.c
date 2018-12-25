/*
 * uart_1.c
 *
 *  Created on: 2018年9月17日
 *      Author: LeoH
 *
** 描        述: INTEL 8250 UART 驱动
*********************************************************************************************************/
#include <Matrix.h>
#include "../../config.h"
#include "uart_1.h"
/*********************************************************************************************************
  定义
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
** 函数名称: uart8250PutStr
** 功能描述: 8250 UART 输出字符串
** 输　入  : addrBase              8250 UART 基地址
**           pcMsg                 字符串
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  uart8250PutStr (addr_t  addrBase, CPCHAR  pcMsg)
{
    UINT32  uiLsr;

    if (!pcMsg) {                                                       /*  指针为空                    */
        return;
    }

    while (*pcMsg != '\0') {                                            /*  发送字符串                  */
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
** 函数名称: uart8250PutChar
** 功能描述: 8250 UART 输出字符
** 输　入  : addrBase              8250 UART 基地址
**           cChar                 字符
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
** 函数名称: uart8250GetChar
** 功能描述: 8250 UART 读取字符
** 输　入  : addrBase              8250 UART 基地址
** 输　出  : 字符
** 全局变量:
** 调用模块:
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




