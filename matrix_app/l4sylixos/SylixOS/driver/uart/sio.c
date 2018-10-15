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
** 文   件   名: sio.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2013 年 12 月 23 日
**
** 描        述: i.MX6 Dual Quad 处理器串口 SIO 驱动.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "sio.h"

#include "sdk.h"
#include "pinmux/iomux_config.h"
/*********************************************************************************************************
  UART 相关配置
*********************************************************************************************************/
#define BSP_CFG_UART_NR        (5)                                      /*  第一个变量 实际是不用的     */
#define CFG_TX_FIFO_LVL        (2)
#define CFG_RX_FIFO_LVL        (1)
#define CFG_DEFAULT_BAUD       SIO_BAUD_115200
#define CFG_DEFAULT_OPT        (CREAD | CS8)

#define URXD                   (0x00)                                   /*  UART receiver reg           */
#define UTXD                   (0x40)                                   /*  UART transmitter reg        */
#define UCR1                   (0x80)                                   /*  UART control 1 reg          */
#define UCR2                   (0x84)                                   /*  UART control 2 reg          */
#define UCR3                   (0x88)                                   /*  UART control 3 reg          */
#define UCR4                   (0x8C)                                   /*  UART control 4 reg          */
#define UFCR                   (0x90)                                   /*  UART fifo control reg       */
#define USR1                   (0x94)                                   /*  UART status 1 reg           */
#define USR2                   (0x98)                                   /*  UART status 2 reg           */
#define UESC                   (0x9C)                                   /*  UART escape char reg        */
#define UTIM                   (0xA0)                                   /*  UART escape timer reg       */
#define UBIR                   (0xA4)                                   /*  UART BRM incremental reg    */
#define UBMR                   (0xA8)                                   /*  UART BRM modulator reg      */
#define UBRC                   (0xAC)                                   /*  UART baud rate count reg    */
#define ONEMS                  (0xB0)                                   /*  UART one ms reg             */
#define UTS                    (0xB4)                                   /*  UART test reg               */

#define __REG_VALUE_TYPE(v, t) ((t)(v))
#define BP_UART_UFCR_RFDIV     (7)                                      /*Bit position UART_UFCR_RFDIV. */
#define BM_UART_UFCR_RFDIV     (0x00000380)                             /*Bit mask for UART_UFCR_RFDIV. */
#define BF_UART_UFCR_RFDIV(v) ((__REG_VALUE_TYPE((v), reg32_t)<< BP_UART_UFCR_RFDIV) & BM_UART_UFCR_RFDIV)
#define UART_UFCR_RFDIV        BF_UART_UFCR_RFDIV(4)

#define BM_UART_UCR1_TRDYEN    (0x00002000)                             /* Bit mask  UART_UCR1_TRDYEN.  */
#define BM_UART_UCR1_TXDMAEN   (0x00000008)                             /* Bit mask  UART_UCR1_TXDMAEN. */
#define BM_UART_UCR1_RRDYEN    (0x00000200)                             /* Bit mask  UART_UCR1_RRDYEN.  */
#define BM_UART_UCR1_RXDMAEN   (0x00000100)                             /* Bit mask  UART_UCR1_RXDMAEN. */
#define BM_UART_UCR2_STPB      (0x00000040)                             /* Bit mask  UART_UCR2_STPB.    */
#define BP_UART_UFCR_TXTL      (10)                                     /* Bit position  UART_UFCR_TXTL */
#define BM_UART_UFCR_TXTL      (0x0000fc00)                             /* Bit mask  UART_UFCR_TXTL.    */
#define BP_UART_UFCR_RXTL      (0)                                      /* Bit position  UART_UFCR_RXTL */
#define BM_UART_UFCR_RXTL      (0x0000003f)                             /* Bit mask  UART_UFCR_RXTL.    */
#define BF_UART_UFCR_RXTL(v)   ((__REG_VALUE_TYPE((v), reg32_t) << BP_UART_UFCR_RXTL) & BM_UART_UFCR_RXTL)
#define BF_UART_UFCR_TXTL(v)   ((__REG_VALUE_TYPE((v), reg32_t) << BP_UART_UFCR_TXTL) & BM_UART_UFCR_TXTL)
#define BM_UART_UTS_LOOP       (0x00001000)                             /* Bit mask UART_UTS_LOOP.      */
#define BM_UART_UTS_DBGEN      (0x00000800)                             /* Bit mask UART_UTS_DBGEN.     */
#define BM_UART_UCR2_TXEN      (0x00000004)                             /* Bit mask UART_UCR2_TXEN.     */
#define BM_UART_UCR2_RXEN      (0x00000002)                             /* Bit mask UART_UCR2_RXEN.     */
#define BM_UART_UCR2_SRST      (0x00000001)                             /* Bit mask UART_UCR2_SRST.     */
#define BM_UART_UCR1_UARTEN    (0x00000001)                             /* Bit mask UART_UCR1_UARTEN.   */
#define BM_UART_UCR3_RXDMUXSEL (0x00000004)                             /* Bit mask UART_UCR3_RXDMUXSEL.*/
#define BM_UART_UCR2_IRTS      (0x00004000)                             /* Bit mask UART_UCR2_IRTS.     */
#define BM_UART_UCR2_CTSC      (0x00002000)                             /* Bit mask UART_UCR2_CTSC.     */
#define BM_UART_UCR2_WS        (0x00000020)                             /* Bit mask UART_UCR2_WS.       */
#define BM_UART_UCR2_PREN      (0x00000100)                             /* Bit mask UART_UCR2_PREN      */
#define BM_UART_UCR2_PROE      (0x00000080)                             /* Bit mask UART_UCR2_PROE.     */
#define BF_UART_UFCR_TXTL(v)   ((__REG_VALUE_TYPE((v), reg32_t) << BP_UART_UFCR_TXTL) & BM_UART_UFCR_TXTL)

/*********************************************************************************************************
** 函数名称: imx6qUartRefFreqGet
** 功能描述: Obtain UART reference frequency
** 输　入  : instance the UART instance number.
** 输　出  : reference frequency in Hz
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT32  imx6qUartRefFreqGet (UINT32 uiChann)
{
    UINT32 uiDiv  = UART_UFCR_RFDIV;
    UINT32 uiRet  = 0;
    UINT32 uiFreq = imx6qPeriClkGet(UART1_MODULE_CLK);                    /* UART src clk fixed PLL3 / 6  */

    if (uiDiv == BF_UART_UFCR_RFDIV(4)) {
        uiRet = uiFreq / 2;
    } else if (uiDiv == BF_UART_UFCR_RFDIV(2)) {
        uiRet = uiFreq / 4;
    } else if (uiDiv == BF_UART_UFCR_RFDIV(6)) {
        uiRet = uiFreq / 7;
    }
    return uiRet;
}
/*********************************************************************************************************
** 函数名称: imx6qPutChar
** 功能描述: Obtain UART reference frequency
** 输　入  : instance the UART instance number.
**           ch pointer to the character for output
** 输　出  : the character that has been sent
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT8  imx6qPutChar (UINT32 uiChann, UINT8 *pChar)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(uiChann) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
        break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
        break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
        break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
        break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
        break;
    }

    /*
     * 若 FIFO 不满就填入数据，否则等待
     */
    uiRegVal = readl(atUartBase + UTS) & (1 << 4);

    while(uiRegVal) {
        uiRegVal = readl(atUartBase + UTS) & (1 << 4);
    }

    writel(*pChar, atUartBase + UTXD);
    return (*pChar);
}
/*********************************************************************************************************
** 函数名称: imx6qUartFifoCfg
** 功能描述: Configure the RX or TX FIFO level and trigger mode
** 输　入  : instance the UART instance number.
**           fifo FIFO to configure: RX_FIFO or TX_FIFO.
**           ucTriggerLvl set the trigger level of the FIFO to generate
 *                          an IRQ or a DMA request: number of characters.
 *           ucServiceMode FIFO served with DMA or IRQ or polling (default).
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qUartFifoCfg (UINT32 uiChann, UINT8 ucFifo, UINT8 ucTriggerLvl, UINT8 ucServiceMode)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(uiChann) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }

    if (ucFifo == TX_FIFO) {
        uiRegVal = readl(atUartBase + UFCR) & (~BM_UART_UFCR_TXTL);     /* Cfg TX_FIFO trigger level    */
        writel(uiRegVal | BF_UART_UFCR_TXTL(ucTriggerLvl), atUartBase + UFCR);

        /*
         * Configure the TX_FIFO service mode
         */
        uiRegVal = readl(atUartBase + UCR1) & (~(BM_UART_UCR1_TRDYEN | BM_UART_UCR1_TXDMAEN));

        if (ucServiceMode == DMA_MODE)
             writel(uiRegVal | BM_UART_UCR1_TXDMAEN, atUartBase + UCR1);
        else if (ucServiceMode == IRQ_MODE)
             writel(uiRegVal | BM_UART_UCR1_TRDYEN, atUartBase + UCR1);
    } else {                                                            /* fifo = RX_FIFO               */
        uiRegVal = readl(atUartBase + UFCR) & (~BM_UART_UFCR_RXTL);     /* cfg RX_FIFO trigger level    */
        writel(uiRegVal | BF_UART_UFCR_RXTL(ucTriggerLvl), atUartBase + UFCR);

        /*
         * Configure the RX_FIFO service mode
         */
        uiRegVal = readl(atUartBase + UCR1) & (~(BM_UART_UCR1_RRDYEN | BM_UART_UCR1_RXDMAEN));

        if (ucServiceMode == DMA_MODE) {
            writel(uiRegVal | BM_UART_UCR1_RXDMAEN, atUartBase + UCR1);
        } else if (ucServiceMode == IRQ_MODE) {
            writel(uiRegVal | BM_UART_UCR1_RRDYEN, atUartBase + UCR1);
        }
    }
}
/*********************************************************************************************************
** 函数名称: imx6qUartLoopCfg
** 功能描述: Enables UART loopback test mode
** 输　入  : instance the UART instance number.
**           state enable/disable the loopback mode
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qUartLoopCfg (UINT32 uiChann, UINT8 ucState)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(uiChann) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;
    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;
    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;
    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;
    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }

    uiRegVal = readl(atUartBase + UTS) & (~BM_UART_UTS_LOOP);

	if (ucState == TRUE) {
	    writel((uiRegVal | BM_UART_UTS_LOOP), atUartBase + UTS);
	} else {
	    writel(uiRegVal , atUartBase + UTS);
	}
}
/*********************************************************************************************************
** 函数名称: imx6qUartHwInit
** 功能描述: Enables UART loopback test mode
** 输　入  : instance the UART instance number.
**           state enable/disable the loopback mode
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6qUartHwInit (UINT32 uiChann, UINT32 uiBaudrate,
		               UINT8 ucParity,UINT8 ucStop, UINT8 ucData, UINT8 ucFlowCtl)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(uiChann) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }

    imx6qClkGateSet(atUartBase, CLOCK_ON);                              /* enable source clocks of UART */

    /*
     * Wait for UART to finish transmitting before changing the configuration
     */
    while (!(readl(atUartBase + UTS) & (1 << 6))) ;

    /*
     * Disable UART Set to default POR state
     */
    writel(0x00000004, atUartBase + UCR1);
    writel(0x00000000, atUartBase + UCR2);
    writel(0x00000000, atUartBase + UCR3);
    writel(0x00008040, atUartBase + UCR4);

    writel(0x0000002B, atUartBase + UESC);
    writel(0x00000000, atUartBase + UTIM);
    writel(0x00000000, atUartBase + UBIR);
    writel(0x00000000, atUartBase + UBMR);
    writel(0x00000000, atUartBase + UTS);

    /*
     * Configure FIFOs trigger level to half-full and half-empty
     */
    writel(BF_UART_UFCR_RXTL(16) | UART_UFCR_RFDIV | BF_UART_UFCR_TXTL(16), atUartBase + UFCR);


    uiRegVal = imx6qUartRefFreqGet(uiChann) / 1000;                     /* Setup One Millisecond timer  */
    writel(uiRegVal, atUartBase + ONEMS);

    /*
     * Set parity
     */
    if (ucParity == PARITY_NONE) {
        uiRegVal = readl(atUartBase + UCR2) & (~(BM_UART_UCR2_PREN| BM_UART_UCR2_PROE));
        writel(uiRegVal, atUartBase + UCR2);
    } else if (ucParity == PARITY_ODD) {
        uiRegVal = readl(atUartBase + UCR2) | BM_UART_UCR2_PREN| BM_UART_UCR2_PROE;
        writel(uiRegVal, atUartBase + UCR2);
    } else {                                                            /* parity == PARITY_EVEN        */
        uiRegVal = readl(atUartBase + UCR2) & (~(BM_UART_UCR2_PREN| BM_UART_UCR2_PROE));
        writel(uiRegVal | BM_UART_UCR2_PREN, atUartBase + UCR2);
    }

    /*
     * Set stop bit
     */
    if (ucStop == STOPBITS_ONE) {
        uiRegVal = readl(atUartBase + UCR2) & (~BM_UART_UCR2_STPB);
        writel(uiRegVal, atUartBase + UCR2);
    } else {                                                            /* stopbits == STOPBITS_TWO     */
        uiRegVal = readl(atUartBase + UCR2) | BM_UART_UCR2_STPB;
        writel(uiRegVal, atUartBase + UCR2);
    }
    /*
     * Set data size
     */
    if (ucData == EIGHTBITS) {
        uiRegVal = readl(atUartBase + UCR2) | BM_UART_UCR2_WS;
        writel(uiRegVal, atUartBase + UCR2);
    } else {                                                            /* stopbits == STOPBITS_TWO     */
        uiRegVal = readl(atUartBase + UCR2) & (~BM_UART_UCR2_WS);
        writel(uiRegVal, atUartBase + UCR2);
    }
    /*
     * Configure the flow control
     */
    if (ucFlowCtl == FLOWCTRL_ON) {
        uiRegVal = readl(atUartBase + UCR2) & (~(BM_UART_UCR2_IRTS| BM_UART_UCR2_CTSC));
        writel(uiRegVal | BM_UART_UCR2_CTSC, atUartBase + UCR2);
    } else {                                                            /* flowcontrol == FLOWCTRL_OFF  */
        uiRegVal = readl(atUartBase + UCR2) & (~(BM_UART_UCR2_IRTS| BM_UART_UCR2_CTSC));
        writel(uiRegVal | BM_UART_UCR2_IRTS, atUartBase + UCR2);
    }


    uiRegVal = readl(atUartBase + UCR3);                                /* manual says this bit must set*/
    writel(uiRegVal | BM_UART_UCR3_RXDMUXSEL, atUartBase + UCR3);

    uiRegVal = readl(atUartBase + UCR1);                                /* Enable UART                  */
    writel(uiRegVal | BM_UART_UCR1_UARTEN, atUartBase + UCR1);

    /*
     * Enable FIFOs and software reset to clear status, reset RX & TX state machine, and reset the FIFOs
     */
    uiRegVal = readl(atUartBase + UCR2);
    writel(uiRegVal | BM_UART_UCR2_TXEN | BM_UART_UCR2_RXEN | BM_UART_UCR2_SRST, atUartBase + UCR2);
    /*
     * Set the numerator value minus one of the BRM ratio
     */
    uiRegVal = (uiBaudrate / 100) - 1;
    writel(uiRegVal , atUartBase + UBIR);

    /*
     * Set the denominator value minus one of the BRM ratio
     */
    uiRegVal = (imx6qUartRefFreqGet(uiChann) / 1600) - 1;
    writel(uiRegVal , atUartBase + UBMR);

    /*
     * prevent UART to enter debug state. Useful when debugging the code with a JTAG and without IRQ
     */
    uiRegVal = readl(atUartBase + UTS);
    writel(uiRegVal | BM_UART_UTS_DBGEN, atUartBase + UTS);

}
/*********************************************************************************************************
** 函数名称: imx6UartChannelVaild
** 功能描述: 判断 UART 通道号是否有效
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : LW_FALSE OR LW_TRUE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE BOOL  imx6UartChannelVaild (INT  iChannelNum)
{
    switch (iChannelNum) {
    case HW_UART1:
    case HW_UART2:
    case HW_UART3:
    case HW_UART4:
    case HW_UART5:
        return (LW_TRUE);

    default:
        return (LW_FALSE);
    }
}
/*********************************************************************************************************
** 函数名称: imx6UartInit
** 功能描述: 初始化 UART
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE VOID  imx6UartInit (INT iChannelNum,INT uiBaud, UCHAR ucParity, UCHAR ucStopBits,
		                             UCHAR ucDataBits, UCHAR ucFlowCtrl, UCHAR ucTxFifo, UCHAR ucRxFifo)
{
	imx6qUartHwInit(iChannelNum,
                    uiBaud,
                    ucParity,
                    ucStopBits,
                    ucDataBits,
                    FLOWCTRL_OFF);                                      /*  初始化串口                  */

    imx6qUartLoopCfg(iChannelNum, LW_FALSE);

    imx6qUartFifoCfg(iChannelNum, TX_FIFO, ucTxFifo, 0);                /*  设置发送 FIFO 触发级别      */
    imx6qUartFifoCfg(iChannelNum, RX_FIFO, ucRxFifo, 0);                /*  设置接收 FIFO 触发级别      */
}
/*********************************************************************************************************
** 函数名称: imx6UartIntNum
** 功能描述: 获得 UART 中断号
** 输　入  : iChannelNum       硬件通道号
** 输　出  : UART 中断号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE ULONG  imx6UartIntNum (INT  iChannelNum)
{
    switch(iChannelNum) {
    case HW_UART1:
    	return IMX_INT_UART1;

    case HW_UART2:
    	return IMX_INT_UART2;

    case HW_UART3:
    	return IMX_INT_UART3;

    case HW_UART4:
    	return IMX_INT_UART4;

    case HW_UART5:
    	return IMX_INT_UART5;

    }
    return PX_ERROR;
}
/*********************************************************************************************************
** 函数名称: imx6UartIsTxInt
** 功能描述: 判断是否发生了 UART 接收中断
** 输　入  : iChannelNum         硬件通道号
** 输　出  : LW_FALSE OR LW_TRUE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE BOOL  imx6UartIsTxInt (INT  iChannelNum)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }
    uiRegVal = readl(atUartBase + USR1) & (1 << 13);

    if (uiRegVal) {
        return  (LW_TRUE);
    } else {
        return  (LW_FALSE);
    }
}
/*********************************************************************************************************
** 函数名称: imx6UartIntClear
** 功能描述: 清除 UART 中断
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE VOID  imx6UartIntClear (INT  iChannelNum)
{
	addr_t atUartBase = UART1_BASE_ADDR;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }
    writel(0x0000FFFF, atUartBase + USR1);
}
/*********************************************************************************************************
** 函数名称: imx6UartTxFifoFull
** 功能描述: 判断发送 FIFO 是否为满
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : LW_FALSE OR LW_TRUE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE BOOL  imx6UartTxFifoFull (INT  iChannelNum)
{
	addr_t  atUartBase = UART1_BASE_ADDR;
	UINT32  uiRegVal;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }
    uiRegVal = readl(atUartBase + UTS) & (1 << 4);

    if (uiRegVal) {
        return  (LW_TRUE);
    } else {
        return  (LW_FALSE);
    }
}
/*********************************************************************************************************
** 函数名称: imx6UartTxFifoPut
** 功能描述: 输出一个数据到发送 FIFO
** 输　入  : iChannelNum                    硬件通道号
**           cOutChar                       需要输出的数据
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE VOID  imx6UartTxFifoPut (INT  iChannelNum, CHAR  cOutChar)
{
	addr_t atUartBase = UART1_BASE_ADDR;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }
    writel(cOutChar, atUartBase + UTXD);

}
/*********************************************************************************************************
** 函数名称: imx6UartRxFifoEmpty
** 功能描述: 判断接收 FIFO 是否为空
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : LW_FALSE OR LW_TRUE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE BOOL  imx6UartRxFifoEmpty (INT  iChannelNum)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }
    uiRegVal = readl(atUartBase + USR2) & (1 << 0);

    if (uiRegVal) {
        return  (LW_FALSE);
    } else {
        return  (LW_TRUE);
    }
}
/*********************************************************************************************************
** 函数名称: imx6UartRxFifoGet
** 功能描述: 从接收 FIFO 里读出数据
** 输　入  : iChannelNum                    硬件通道号
**           pcInChar                       读出的数据
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE INT  imx6UartRxFifoGet (INT  iChannelNum, PCHAR  pcInChar)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }
    uiRegVal = readl(atUartBase + URXD);

    if (uiRegVal & 0x7C00) {
        return  (PX_ERROR);
    }
    *pcInChar = uiRegVal;
    return  (ERROR_NONE);

}
/*********************************************************************************************************
** 函数名称: imx6UartTxIntDisable
** 功能描述: 关闭发送中断
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE VOID  imx6UartTxIntDisable (INT  iChannelNum)
{
	addr_t  atUartBase = UART1_BASE_ADDR;
	UINT32  uiRegVal;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }

    uiRegVal = readl(atUartBase + UCR1) & (~(BM_UART_UCR1_TRDYEN | BM_UART_UCR1_TXDMAEN));
    writel(uiRegVal, atUartBase + UCR1);

}
/*********************************************************************************************************
** 函数名称: imx6UartTxIntEnable
** 功能描述: 使能发送中断
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE VOID  imx6UartTxIntEnable (INT  iChannelNum)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }

    uiRegVal = readl(atUartBase + UCR1) & (~(BM_UART_UCR1_TRDYEN | BM_UART_UCR1_TXDMAEN));
    writel((uiRegVal | BM_UART_UCR1_TRDYEN), atUartBase + UCR1);
}
/*********************************************************************************************************
** 函数名称: imx6UartRxIntEnable
** 功能描述: 使能接收中断
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LW_INLINE VOID  imx6UartRxIntEnable (INT  iChannelNum)
{
	addr_t atUartBase = UART1_BASE_ADDR;
	UINT32 uiRegVal;

    switch(iChannelNum) {
    case HW_UART1:
    	atUartBase = UART1_BASE_ADDR;
    break;

    case HW_UART2:
    	atUartBase = UART2_BASE_ADDR;
    break;

    case HW_UART3:
    	atUartBase = UART3_BASE_ADDR;
    break;

    case HW_UART4:
    	atUartBase = UART4_BASE_ADDR;
    break;

    case HW_UART5:
    	atUartBase = UART5_BASE_ADDR;
    break;
    }

    uiRegVal = readl(atUartBase + UCR1) & (~(BM_UART_UCR1_RRDYEN | BM_UART_UCR1_RXDMAEN));
    writel((uiRegVal | BM_UART_UCR1_RRDYEN), atUartBase + UCR1);
}
/*********************************************************************************************************
** 函数名称: imx6UartHwOptionAnalyse
** 功能描述: 分析 UART 通道硬件参数 获得具体参数
** 输　入  : iHwOption                   硬件参数
**           pucDataBits                 数据位数
**           pucStopBits                 结束位
**           pucParity                   校验位
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  imx6UartHwOptionAnalyse (INT iHwOption, PUCHAR  pucDataBits, PUCHAR  pucStopBits, PUCHAR  pucParity)
{
    if ((iHwOption & CS8) == CS8) {                                     /*  确定数据位数                */
        *pucDataBits = EIGHTBITS;
    } else {
        *pucDataBits = SEVENBITS;
    }

    if (iHwOption & STOPB) {                                            /*  确定结束位                  */
        *pucStopBits = STOPBITS_TWO;
    } else {
        *pucStopBits = STOPBITS_ONE;
    }

    if (iHwOption & PARENB) {                                           /*  确定校验位                  */
        if (iHwOption & PARODD) {
            *pucParity = PARITY_ODD;
        } else {
            *pucParity = PARITY_EVEN;
        }
    } else {
        *pucParity = PARITY_NONE;
    }
}
/*********************************************************************************************************
  STREAM UART CHANNEL (SIO CHANNEL)
*********************************************************************************************************/
typedef struct {

    SIO_DRV_FUNCS              *pdrvFuncs;                              /*  SIO 驱动程序组              */

    INT                       (*pcbGetTxChar)();                        /*  中断回调函数                */
    INT                       (*pcbPutRcvChar)();

    PVOID                       pvGetTxArg;                             /*  回调函数参数                */
    PVOID                       pvPutRcvArg;

    INT                         iChannelMode;                           /*  通道模式                    */

    UCHAR                     (*pfuncHwInByte)(INT);                    /*  物理硬件接收一个字节        */
    VOID                      (*pfuncHwOutByte)(INT, CHAR);             /*  物理硬件发送一个字节        */

    INT                         iChannelNum;                            /*  通道号                      */
    INT                         iBaud;                                  /*  波特率                      */

    INT                         iHwOption;                              /*  硬件选项                    */

} __SIO_CHANNEL;
typedef __SIO_CHANNEL          *__PSIO_CHANNEL;                         /*  指针类型                    */
/*********************************************************************************************************
  SIO 通道控制块
*********************************************************************************************************/
static  __SIO_CHANNEL           __G_SioChannels[BSP_CFG_UART_NR];
/*********************************************************************************************************
  SIO 驱动程序
*********************************************************************************************************/
static INT   imx6SioIoctl(SIO_CHAN            *psiochanChan,
                          INT                  iCmd,
                          LONG                 lArg);                   /*  端口控制                    */
static INT   imx6SioStartup(SIO_CHAN          *psiochanChan);           /*  发送                        */
static INT   imx6SioCbInstall(SIO_CHAN        *psiochanChan,
                              INT              iCallbackType,
                              VX_SIO_CALLBACK  callbackRoute,
                              PVOID            pvCallbackArg);          /*  安装回调                    */
static INT   imx6SioPollRxChar(SIO_CHAN       *psiochanChan,
                               PCHAR           pcInChar);               /*  轮询接收                    */
static INT   imx6SioPollTxChar(SIO_CHAN       *psiochanChan,
                               CHAR            cOutChar);               /*  轮询发送                    */
static irqreturn_t  imx6SioIsr(SIO_CHAN       *psiochanChan,
                               ULONG           ulVector);               /*  中断                        */
/*********************************************************************************************************
  SIO 驱动程序
*********************************************************************************************************/
static SIO_DRV_FUNCS    __G_SioDrvFuncs = {
             (INT (*)(SIO_CHAN *,INT, PVOID))imx6SioIoctl,
             imx6SioStartup,
             imx6SioCbInstall,
             imx6SioPollRxChar,
             imx6SioPollTxChar
};
/*********************************************************************************************************
** 函数名称: sioChanCreate
** 功能描述: 创建一个 SIO 通道
** 输　入  : iChannelNum                    硬件通道号
** 输　出  : SIO 通道
** 全局变量:
** 调用模块:
*********************************************************************************************************/
SIO_CHAN  *sioChanCreate (INT  iChannelNum)
{
    __PSIO_CHANNEL          psiochanUart;
    
    UCHAR                   ucDataBits;
    UCHAR                   ucStopBits;
    UCHAR                   ucParity;
    CPCHAR                  pcIsrName;

    iChannelNum += 1;

    if (!imx6UartChannelVaild(iChannelNum)) {
        return  (LW_NULL);                                              /*  通道号错误                  */
    }

    psiochanUart               = &__G_SioChannels[iChannelNum - 1];
    psiochanUart->pdrvFuncs    = &__G_SioDrvFuncs;                      /*  SIO FUNC                    */
    psiochanUart->iChannelNum  = iChannelNum;
    psiochanUart->iChannelMode = SIO_MODE_INT;                          /*  使用中断模式                */
    psiochanUart->iBaud        = CFG_DEFAULT_BAUD;                      /*  初始化波特率                */
    psiochanUart->iHwOption    = CFG_DEFAULT_OPT;                       /*  初始化硬件状态              */

    /*
     * 进行串口的管脚分配，这里的 iChannelNum 已经在进入初始化函数的时候进行了 + 1 操作
     * 在 SabreLite 开发板中，对外只有串口1 和串口2 可以使用，因此对其他初始化没有意义
     */
    switch (iChannelNum) {
    case HW_UART1:
    	pcIsrName = "uart0_isr";
        break;

    case HW_UART2:
    	uart2_iomux_config();
    	pcIsrName = "uart1_isr";
        break;

    case HW_UART3:
    	uart3_iomux_config();
    	pcIsrName = "uart2_isr";
        break;

    case HW_UART4:
    	uart4_iomux_config();
    	pcIsrName = "uart3_isr";
        break;

    case HW_UART5:
    	uart5_iomux_config();
    	pcIsrName = "uart4_isr";
        break;

    default:
    break;
    }
    /*
     *  获得具体参数
     */
    imx6UartHwOptionAnalyse(psiochanUart->iHwOption, &ucDataBits, &ucStopBits, &ucParity);
                          
    imx6UartInit(iChannelNum, psiochanUart->iBaud, ucParity, ucStopBits, ucDataBits,
                 FLOWCTRL_OFF,
                 CFG_TX_FIFO_LVL,
                 CFG_RX_FIFO_LVL);                                      /*  初始化串口                  */

    API_InterVectorConnect(imx6UartIntNum(iChannelNum), (PINT_SVR_ROUTINE)imx6SioIsr,
                           (PVOID)psiochanUart, pcIsrName);             /*  安装操作系统中断向量表      */

    API_InterVectorDisable(imx6UartIntNum(iChannelNum));                /*  关闭串口中断                */

    return  ((SIO_CHAN *)psiochanUart);
}
/*********************************************************************************************************
** 函数名称: imx6SioCbInstall
** 功能描述: SIO 通道安装回调函数
** 输　入  : psiochanChan                 SIO 通道
**           iCallbackType                回调类型
**           callbackRoute                回调函数
**           pvCallbackArg                回调参数
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6SioCbInstall (SIO_CHAN          *psiochanChan,
                              INT                iCallbackType,
                              VX_SIO_CALLBACK    callbackRoute,
                              PVOID              pvCallbackArg)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;
    
    switch (iCallbackType) {
    
    case SIO_CALLBACK_GET_TX_CHAR:                                      /*  发送回电函数                */
        psiochanUart->pcbGetTxChar = callbackRoute;
        psiochanUart->pvGetTxArg   = pvCallbackArg;
        return  (ERROR_NONE);
        
    case SIO_CALLBACK_PUT_RCV_CHAR:                                     /*  接收回电函数                */
        psiochanUart->pcbPutRcvChar = callbackRoute;
        psiochanUart->pvPutRcvArg   = pvCallbackArg;
        return  (ERROR_NONE);
        
    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** 函数名称: imx6SioPollRxChar
** 功能描述: SIO 通道轮询接收
** 输　入  : psiochanChan                 SIO 通道
**           pcInChar                     接收的字节
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6SioPollRxChar (SIO_CHAN  *psiochanChan, PCHAR  pcInChar)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;

    if (!imx6UartRxFifoEmpty(psiochanUart->iChannelNum)) {
        if (imx6UartRxFifoGet(psiochanUart->iChannelNum, pcInChar) == ERROR_NONE) {
            return  (ERROR_NONE);
        }
    }

    _ErrorHandle(EAGAIN);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: imx6SioPollTxChar
** 功能描述: SIO 通道轮询发送
** 输　入  : psiochanChan                 SIO 通道
**           cOutChar                     发送的字节
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6SioPollTxChar (SIO_CHAN  *psiochanChan, CHAR  cOutChar)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;

    while (imx6UartTxFifoFull(psiochanUart->iChannelNum));              /*  发送 FIFO 满                */
    
    imx6UartTxFifoPut(psiochanUart->iChannelNum, cOutChar);             /*  发送数据                    */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imx6SioStartup
** 功能描述: SIO 通道发送(没有使用中断)
** 输　入  : psiochanChan                 SIO 通道
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6SioStartup (SIO_CHAN  *psiochanChan)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;
    CHAR                    cChar;

    if (!imx6UartTxFifoFull(psiochanUart->iChannelNum)) {
    	psiochanUart->pcbGetTxChar(psiochanUart->pvGetTxArg, &cChar);
    	imx6UartTxFifoPut(psiochanUart->iChannelNum, cChar);
    }

    imx6UartTxIntEnable(psiochanUart->iChannelNum);                     /*  使能发送中断                */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imx6SioIsr
** 功能描述: SIO 通道中断处理函数
** 输　入  : psiochanChan                 SIO 通道
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static irqreturn_t  imx6SioIsr (SIO_CHAN  *psiochanChan, ULONG  ulVector)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;
    CHAR                    cChar;

    if (imx6UartIsTxInt(psiochanUart->iChannelNum)) {                   /*  发送中断                    */
    	while (!imx6UartTxFifoFull(psiochanUart->iChannelNum)) {
			if (psiochanUart->pcbGetTxChar(psiochanUart->pvGetTxArg, &cChar) != ERROR_NONE) {
				imx6UartTxIntDisable(psiochanUart->iChannelNum);        /*  关闭发送中断                */
				break;
			} else {
				imx6UartTxFifoPut(psiochanUart->iChannelNum, cChar);    /*  发送数据                    */
			}
		}
    }

    while (!imx6UartRxFifoEmpty(psiochanUart->iChannelNum)) {           /*  需要接收数据                */
        if (imx6UartRxFifoGet(psiochanUart->iChannelNum, &cChar)        /*  接收数据                    */
                == ERROR_NONE) {
            psiochanUart->pcbPutRcvChar(psiochanUart->pvPutRcvArg, cChar);
        } else {
            break;
        }
    }

    imx6UartIntClear(psiochanUart->iChannelNum);                        /*  清除中断                    */

    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
** 函数名称: imx6SioIoctl
** 功能描述: SIO 通道控制
** 输　入  : psiochanChan                 SIO 通道
**           iCmd                         命令
**           lArg                         参数
** 输　出  : 错误号
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6SioIoctl (SIO_CHAN  *psiochanChan, INT  iCmd, LONG  lArg)
{
    __PSIO_CHANNEL psiochanUart = (__PSIO_CHANNEL)psiochanChan;
    UCHAR          ucDataBits;
    UCHAR          ucStopBits;
    UCHAR          ucParity;
    
    switch (iCmd) {
    
    case SIO_BAUD_SET:                                                  /*  设置波特率                  */
        imx6UartHwOptionAnalyse(psiochanUart->iHwOption, &ucDataBits, &ucStopBits, &ucParity);

        imx6UartInit(psiochanUart->iChannelNum, (INT)lArg, ucParity, ucStopBits, ucDataBits,
                     FLOWCTRL_OFF,
                     CFG_TX_FIFO_LVL,
                     CFG_RX_FIFO_LVL);                                  /*  初始化串口                  */

        imx6UartRxIntEnable(psiochanUart->iChannelNum);                 /*  打开接收中断                */

        psiochanUart->iBaud = (INT)lArg;                                /*  记录波特率                  */
        break;
        
    case SIO_BAUD_GET:                                                  /*  获得波特率                  */
        *((LONG *)lArg) = psiochanUart->iBaud;
        break;
    
    case SIO_HW_OPTS_SET:                                               /*  设置硬件参数                */
        imx6UartHwOptionAnalyse((INT)lArg, &ucDataBits, &ucStopBits, &ucParity);

        imx6UartInit(psiochanUart->iChannelNum, psiochanUart->iBaud, ucParity, ucStopBits, ucDataBits,
                     FLOWCTRL_OFF,
                     CFG_TX_FIFO_LVL,
                     CFG_RX_FIFO_LVL);                                  /*  初始化串口                  */

        imx6UartRxIntEnable(psiochanUart->iChannelNum);                 /*  打开接收中断                */

        psiochanUart->iHwOption = (INT)lArg;                            /*  记录硬件参数                */
        break;
        
    case SIO_HW_OPTS_GET:                                               /*  获取硬件参数                */
        *((LONG *)lArg) = psiochanUart->iHwOption;
        break;
    
    case SIO_OPEN:                                                      /*  打开串口                    */
        imx6UartIntClear(psiochanUart->iChannelNum);                    /*  清除中断                    */
        imx6UartRxIntEnable(psiochanUart->iChannelNum);                 /*  打开接收中断                */
        API_InterVectorEnable(imx6UartIntNum(psiochanUart->iChannelNum));
        break;
        
    case SIO_HUP:                                                       /*  关闭串口                    */
        API_InterVectorDisable(imx6UartIntNum(psiochanUart->iChannelNum));
        break;

    default:
        _ErrorHandle(ENOSYS);
	    return  (ENOSYS);
	}
	
	return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
