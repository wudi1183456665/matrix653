/*==============================================================================
** uart.c -- L4 GuestOS uart driver.
**
** MODIFY HISTORY:
**
** 2016-10-27 wdf Create.
==============================================================================*/

#define UART1_BASE_ADDRESS  0x02020000                                    /*  UART1 base address     */
#define UART1_Rx_VAL        (*(volatile int *)(UART1_BASE_ADDRESS + 0x00))/*  Receiver reg           */
#define UART1_Tx_VAL        (*(volatile int *)(UART1_BASE_ADDRESS + 0x40))/*  Transmitter reg        */
#define UART1_Rx_TST        (*(volatile int *)(UART1_BASE_ADDRESS + 0x98))/*  Receiver status reg    */
#define UART1_Tx_TST        (*(volatile int *)(UART1_BASE_ADDRESS + 0xB4))/*  Transmitter status reg */

/*==============================================================================
 * - uart_init()
 *
 * - init UART hardware
 */
int uart_init ()
{
    return 0;
}

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

/*==============================================================================
 * - uart_tstc()
 *
 * - check whether have received char in FIFO
 */
int uart_tstc ()
{
    return (UART1_Rx_TST & (1 << 0));
}

/*==============================================================================
 * - uart_getc()
 *
 * - use UART receive a char
 */
int uart_getc ()
{
    return (UART1_Rx_VAL);
}

/*==============================================================================
** FILE END
==============================================================================*/

