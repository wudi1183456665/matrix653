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
** 文   件   名: main.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 02 月 28 日
**
** 描        述: 应用进程.
*********************************************************************************************************/
#include <Matrix.h>
#include <api.h>
#include <libc.h>

INT  getCpsr(VOID);

#define UART1_BASE_ADDRESS  0x02020000                                    /*  UART1 base address     */
#define UART1_Rx_VAL        (*(volatile int *)(UART1_BASE_ADDRESS + 0x00))/*  Receiver reg           */
#define UART1_Tx_VAL        (*(volatile int *)(UART1_BASE_ADDRESS + 0x40))/*  Transmitter reg        */
#define UART1_Rx_TST        (*(volatile int *)(UART1_BASE_ADDRESS + 0x98))/*  Receiver status reg    */
#define UART1_Tx_TST        (*(volatile int *)(UART1_BASE_ADDRESS + 0xB4))/*  Transmitter status reg */

/*==============================================================================
 * - uart_putc()
 *
 * - use UART send a char
 */
int uart_putc (char c)
{
    if (c == '\n') {
        while (UART1_Tx_TST & (1 << 4)) ;
        UART1_Tx_VAL = '\r';
    }

    while (UART1_Tx_TST & (1 << 4)) ;
    UART1_Tx_VAL = c;

    return 1;
}

void uart_put_hex (int v)
{
    static char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    int i;

    for (i = 7; i >= 0; i--) {
        uart_putc(hex[(v >> (i*4)) & 0xF]);
    }
    uart_putc(';');
}
VOID  delayMs (ULONG  ulMs)
{
    volatile UINT  i;

    while (ulMs) {
        ulMs--;
        for (i = 0; i < 8065; i++) {
        }
    }
}
VOID  l_main (VOID)
{
    int i;
    //__asm__ __volatile__("swi  3\n\t");
    //__asm__ __volatile__("swi  3\n\t");
    while (1) {
        //__asm__ __volatile__("swi  3\n\t");
        //(*(volatile int *)(0x02020040)) = '2';
        //uart_putc('2');

#if 0
        int cpsr = getCpsr();
        uart_put_hex(cpsr);
#endif
        MXI_debugRead(&i);
        lib_printf("This is App2, %d\n", i);
        delayMs(2000);

        //int i = 10; while (i--);
    }
}


/*********************************************************************************************************
  END
*********************************************************************************************************/

