/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: sio.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2013 �� 12 �� 23 ��
**
** ��        ��: i.MX6 Dual Quad ���������� SIO ����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "sio.h"

#include "sdk.h"
#include "pinmux/iomux_config.h"
/*********************************************************************************************************
  UART �������
*********************************************************************************************************/
#define BSP_CFG_UART_NR        (5)                                      /*  ��һ������ ʵ���ǲ��õ�     */
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
** ��������: imx6qUartRefFreqGet
** ��������: Obtain UART reference frequency
** �䡡��  : instance the UART instance number.
** �䡡��  : reference frequency in Hz
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6qPutChar
** ��������: Obtain UART reference frequency
** �䡡��  : instance the UART instance number.
**           ch pointer to the character for output
** �䡡��  : the character that has been sent
** ȫ�ֱ���:
** ����ģ��:
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
     * �� FIFO �������������ݣ�����ȴ�
     */
    uiRegVal = readl(atUartBase + UTS) & (1 << 4);

    while(uiRegVal) {
        uiRegVal = readl(atUartBase + UTS) & (1 << 4);
    }

    writel(*pChar, atUartBase + UTXD);
    return (*pChar);
}
/*********************************************************************************************************
** ��������: imx6qUartFifoCfg
** ��������: Configure the RX or TX FIFO level and trigger mode
** �䡡��  : instance the UART instance number.
**           fifo FIFO to configure: RX_FIFO or TX_FIFO.
**           ucTriggerLvl set the trigger level of the FIFO to generate
 *                          an IRQ or a DMA request: number of characters.
 *           ucServiceMode FIFO served with DMA or IRQ or polling (default).
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6qUartLoopCfg
** ��������: Enables UART loopback test mode
** �䡡��  : instance the UART instance number.
**           state enable/disable the loopback mode
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6qUartHwInit
** ��������: Enables UART loopback test mode
** �䡡��  : instance the UART instance number.
**           state enable/disable the loopback mode
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartChannelVaild
** ��������: �ж� UART ͨ�����Ƿ���Ч
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : LW_FALSE OR LW_TRUE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartInit
** ��������: ��ʼ�� UART
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LW_INLINE VOID  imx6UartInit (INT iChannelNum,INT uiBaud, UCHAR ucParity, UCHAR ucStopBits,
		                             UCHAR ucDataBits, UCHAR ucFlowCtrl, UCHAR ucTxFifo, UCHAR ucRxFifo)
{
	imx6qUartHwInit(iChannelNum,
                    uiBaud,
                    ucParity,
                    ucStopBits,
                    ucDataBits,
                    FLOWCTRL_OFF);                                      /*  ��ʼ������                  */

    imx6qUartLoopCfg(iChannelNum, LW_FALSE);

    imx6qUartFifoCfg(iChannelNum, TX_FIFO, ucTxFifo, 0);                /*  ���÷��� FIFO ��������      */
    imx6qUartFifoCfg(iChannelNum, RX_FIFO, ucRxFifo, 0);                /*  ���ý��� FIFO ��������      */
}
/*********************************************************************************************************
** ��������: imx6UartIntNum
** ��������: ��� UART �жϺ�
** �䡡��  : iChannelNum       Ӳ��ͨ����
** �䡡��  : UART �жϺ�
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartIsTxInt
** ��������: �ж��Ƿ����� UART �����ж�
** �䡡��  : iChannelNum         Ӳ��ͨ����
** �䡡��  : LW_FALSE OR LW_TRUE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartIntClear
** ��������: ��� UART �ж�
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartTxFifoFull
** ��������: �жϷ��� FIFO �Ƿ�Ϊ��
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : LW_FALSE OR LW_TRUE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartTxFifoPut
** ��������: ���һ�����ݵ����� FIFO
** �䡡��  : iChannelNum                    Ӳ��ͨ����
**           cOutChar                       ��Ҫ���������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartRxFifoEmpty
** ��������: �жϽ��� FIFO �Ƿ�Ϊ��
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : LW_FALSE OR LW_TRUE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartRxFifoGet
** ��������: �ӽ��� FIFO ���������
** �䡡��  : iChannelNum                    Ӳ��ͨ����
**           pcInChar                       ����������
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartTxIntDisable
** ��������: �رշ����ж�
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartTxIntEnable
** ��������: ʹ�ܷ����ж�
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartRxIntEnable
** ��������: ʹ�ܽ����ж�
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6UartHwOptionAnalyse
** ��������: ���� UART ͨ��Ӳ������ ��þ������
** �䡡��  : iHwOption                   Ӳ������
**           pucDataBits                 ����λ��
**           pucStopBits                 ����λ
**           pucParity                   У��λ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  imx6UartHwOptionAnalyse (INT iHwOption, PUCHAR  pucDataBits, PUCHAR  pucStopBits, PUCHAR  pucParity)
{
    if ((iHwOption & CS8) == CS8) {                                     /*  ȷ������λ��                */
        *pucDataBits = EIGHTBITS;
    } else {
        *pucDataBits = SEVENBITS;
    }

    if (iHwOption & STOPB) {                                            /*  ȷ������λ                  */
        *pucStopBits = STOPBITS_TWO;
    } else {
        *pucStopBits = STOPBITS_ONE;
    }

    if (iHwOption & PARENB) {                                           /*  ȷ��У��λ                  */
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

    SIO_DRV_FUNCS              *pdrvFuncs;                              /*  SIO ����������              */

    INT                       (*pcbGetTxChar)();                        /*  �жϻص�����                */
    INT                       (*pcbPutRcvChar)();

    PVOID                       pvGetTxArg;                             /*  �ص���������                */
    PVOID                       pvPutRcvArg;

    INT                         iChannelMode;                           /*  ͨ��ģʽ                    */

    UCHAR                     (*pfuncHwInByte)(INT);                    /*  ����Ӳ������һ���ֽ�        */
    VOID                      (*pfuncHwOutByte)(INT, CHAR);             /*  ����Ӳ������һ���ֽ�        */

    INT                         iChannelNum;                            /*  ͨ����                      */
    INT                         iBaud;                                  /*  ������                      */

    INT                         iHwOption;                              /*  Ӳ��ѡ��                    */

} __SIO_CHANNEL;
typedef __SIO_CHANNEL          *__PSIO_CHANNEL;                         /*  ָ������                    */
/*********************************************************************************************************
  SIO ͨ�����ƿ�
*********************************************************************************************************/
static  __SIO_CHANNEL           __G_SioChannels[BSP_CFG_UART_NR];
/*********************************************************************************************************
  SIO ��������
*********************************************************************************************************/
static INT   imx6SioIoctl(SIO_CHAN            *psiochanChan,
                          INT                  iCmd,
                          LONG                 lArg);                   /*  �˿ڿ���                    */
static INT   imx6SioStartup(SIO_CHAN          *psiochanChan);           /*  ����                        */
static INT   imx6SioCbInstall(SIO_CHAN        *psiochanChan,
                              INT              iCallbackType,
                              VX_SIO_CALLBACK  callbackRoute,
                              PVOID            pvCallbackArg);          /*  ��װ�ص�                    */
static INT   imx6SioPollRxChar(SIO_CHAN       *psiochanChan,
                               PCHAR           pcInChar);               /*  ��ѯ����                    */
static INT   imx6SioPollTxChar(SIO_CHAN       *psiochanChan,
                               CHAR            cOutChar);               /*  ��ѯ����                    */
static irqreturn_t  imx6SioIsr(SIO_CHAN       *psiochanChan,
                               ULONG           ulVector);               /*  �ж�                        */
/*********************************************************************************************************
  SIO ��������
*********************************************************************************************************/
static SIO_DRV_FUNCS    __G_SioDrvFuncs = {
             (INT (*)(SIO_CHAN *,INT, PVOID))imx6SioIoctl,
             imx6SioStartup,
             imx6SioCbInstall,
             imx6SioPollRxChar,
             imx6SioPollTxChar
};
/*********************************************************************************************************
** ��������: sioChanCreate
** ��������: ����һ�� SIO ͨ��
** �䡡��  : iChannelNum                    Ӳ��ͨ����
** �䡡��  : SIO ͨ��
** ȫ�ֱ���:
** ����ģ��:
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
        return  (LW_NULL);                                              /*  ͨ���Ŵ���                  */
    }

    psiochanUart               = &__G_SioChannels[iChannelNum - 1];
    psiochanUart->pdrvFuncs    = &__G_SioDrvFuncs;                      /*  SIO FUNC                    */
    psiochanUart->iChannelNum  = iChannelNum;
    psiochanUart->iChannelMode = SIO_MODE_INT;                          /*  ʹ���ж�ģʽ                */
    psiochanUart->iBaud        = CFG_DEFAULT_BAUD;                      /*  ��ʼ��������                */
    psiochanUart->iHwOption    = CFG_DEFAULT_OPT;                       /*  ��ʼ��Ӳ��״̬              */

    /*
     * ���д��ڵĹܽŷ��䣬����� iChannelNum �Ѿ��ڽ����ʼ��������ʱ������� + 1 ����
     * �� SabreLite �������У�����ֻ�д���1 �ʹ���2 ����ʹ�ã���˶�������ʼ��û������
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
     *  ��þ������
     */
    imx6UartHwOptionAnalyse(psiochanUart->iHwOption, &ucDataBits, &ucStopBits, &ucParity);
                          
    imx6UartInit(iChannelNum, psiochanUart->iBaud, ucParity, ucStopBits, ucDataBits,
                 FLOWCTRL_OFF,
                 CFG_TX_FIFO_LVL,
                 CFG_RX_FIFO_LVL);                                      /*  ��ʼ������                  */

    API_InterVectorConnect(imx6UartIntNum(iChannelNum), (PINT_SVR_ROUTINE)imx6SioIsr,
                           (PVOID)psiochanUart, pcIsrName);             /*  ��װ����ϵͳ�ж�������      */

    API_InterVectorDisable(imx6UartIntNum(iChannelNum));                /*  �رմ����ж�                */

    return  ((SIO_CHAN *)psiochanUart);
}
/*********************************************************************************************************
** ��������: imx6SioCbInstall
** ��������: SIO ͨ����װ�ص�����
** �䡡��  : psiochanChan                 SIO ͨ��
**           iCallbackType                �ص�����
**           callbackRoute                �ص�����
**           pvCallbackArg                �ص�����
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6SioCbInstall (SIO_CHAN          *psiochanChan,
                              INT                iCallbackType,
                              VX_SIO_CALLBACK    callbackRoute,
                              PVOID              pvCallbackArg)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;
    
    switch (iCallbackType) {
    
    case SIO_CALLBACK_GET_TX_CHAR:                                      /*  ���ͻص纯��                */
        psiochanUart->pcbGetTxChar = callbackRoute;
        psiochanUart->pvGetTxArg   = pvCallbackArg;
        return  (ERROR_NONE);
        
    case SIO_CALLBACK_PUT_RCV_CHAR:                                     /*  ���ջص纯��                */
        psiochanUart->pcbPutRcvChar = callbackRoute;
        psiochanUart->pvPutRcvArg   = pvCallbackArg;
        return  (ERROR_NONE);
        
    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** ��������: imx6SioPollRxChar
** ��������: SIO ͨ����ѯ����
** �䡡��  : psiochanChan                 SIO ͨ��
**           pcInChar                     ���յ��ֽ�
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: imx6SioPollTxChar
** ��������: SIO ͨ����ѯ����
** �䡡��  : psiochanChan                 SIO ͨ��
**           cOutChar                     ���͵��ֽ�
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6SioPollTxChar (SIO_CHAN  *psiochanChan, CHAR  cOutChar)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;

    while (imx6UartTxFifoFull(psiochanUart->iChannelNum));              /*  ���� FIFO ��                */
    
    imx6UartTxFifoPut(psiochanUart->iChannelNum, cOutChar);             /*  ��������                    */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6SioStartup
** ��������: SIO ͨ������(û��ʹ���ж�)
** �䡡��  : psiochanChan                 SIO ͨ��
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6SioStartup (SIO_CHAN  *psiochanChan)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;
    CHAR                    cChar;

    if (!imx6UartTxFifoFull(psiochanUart->iChannelNum)) {
    	psiochanUart->pcbGetTxChar(psiochanUart->pvGetTxArg, &cChar);
    	imx6UartTxFifoPut(psiochanUart->iChannelNum, cChar);
    }

    imx6UartTxIntEnable(psiochanUart->iChannelNum);                     /*  ʹ�ܷ����ж�                */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6SioIsr
** ��������: SIO ͨ���жϴ�����
** �䡡��  : psiochanChan                 SIO ͨ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static irqreturn_t  imx6SioIsr (SIO_CHAN  *psiochanChan, ULONG  ulVector)
{
    __PSIO_CHANNEL          psiochanUart = (__PSIO_CHANNEL)psiochanChan;
    CHAR                    cChar;

    if (imx6UartIsTxInt(psiochanUart->iChannelNum)) {                   /*  �����ж�                    */
    	while (!imx6UartTxFifoFull(psiochanUart->iChannelNum)) {
			if (psiochanUart->pcbGetTxChar(psiochanUart->pvGetTxArg, &cChar) != ERROR_NONE) {
				imx6UartTxIntDisable(psiochanUart->iChannelNum);        /*  �رշ����ж�                */
				break;
			} else {
				imx6UartTxFifoPut(psiochanUart->iChannelNum, cChar);    /*  ��������                    */
			}
		}
    }

    while (!imx6UartRxFifoEmpty(psiochanUart->iChannelNum)) {           /*  ��Ҫ��������                */
        if (imx6UartRxFifoGet(psiochanUart->iChannelNum, &cChar)        /*  ��������                    */
                == ERROR_NONE) {
            psiochanUart->pcbPutRcvChar(psiochanUart->pvPutRcvArg, cChar);
        } else {
            break;
        }
    }

    imx6UartIntClear(psiochanUart->iChannelNum);                        /*  ����ж�                    */

    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
** ��������: imx6SioIoctl
** ��������: SIO ͨ������
** �䡡��  : psiochanChan                 SIO ͨ��
**           iCmd                         ����
**           lArg                         ����
** �䡡��  : �����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6SioIoctl (SIO_CHAN  *psiochanChan, INT  iCmd, LONG  lArg)
{
    __PSIO_CHANNEL psiochanUart = (__PSIO_CHANNEL)psiochanChan;
    UCHAR          ucDataBits;
    UCHAR          ucStopBits;
    UCHAR          ucParity;
    
    switch (iCmd) {
    
    case SIO_BAUD_SET:                                                  /*  ���ò�����                  */
        imx6UartHwOptionAnalyse(psiochanUart->iHwOption, &ucDataBits, &ucStopBits, &ucParity);

        imx6UartInit(psiochanUart->iChannelNum, (INT)lArg, ucParity, ucStopBits, ucDataBits,
                     FLOWCTRL_OFF,
                     CFG_TX_FIFO_LVL,
                     CFG_RX_FIFO_LVL);                                  /*  ��ʼ������                  */

        imx6UartRxIntEnable(psiochanUart->iChannelNum);                 /*  �򿪽����ж�                */

        psiochanUart->iBaud = (INT)lArg;                                /*  ��¼������                  */
        break;
        
    case SIO_BAUD_GET:                                                  /*  ��ò�����                  */
        *((LONG *)lArg) = psiochanUart->iBaud;
        break;
    
    case SIO_HW_OPTS_SET:                                               /*  ����Ӳ������                */
        imx6UartHwOptionAnalyse((INT)lArg, &ucDataBits, &ucStopBits, &ucParity);

        imx6UartInit(psiochanUart->iChannelNum, psiochanUart->iBaud, ucParity, ucStopBits, ucDataBits,
                     FLOWCTRL_OFF,
                     CFG_TX_FIFO_LVL,
                     CFG_RX_FIFO_LVL);                                  /*  ��ʼ������                  */

        imx6UartRxIntEnable(psiochanUart->iChannelNum);                 /*  �򿪽����ж�                */

        psiochanUart->iHwOption = (INT)lArg;                            /*  ��¼Ӳ������                */
        break;
        
    case SIO_HW_OPTS_GET:                                               /*  ��ȡӲ������                */
        *((LONG *)lArg) = psiochanUart->iHwOption;
        break;
    
    case SIO_OPEN:                                                      /*  �򿪴���                    */
        imx6UartIntClear(psiochanUart->iChannelNum);                    /*  ����ж�                    */
        imx6UartRxIntEnable(psiochanUart->iChannelNum);                 /*  �򿪽����ж�                */
        API_InterVectorEnable(imx6UartIntNum(psiochanUart->iChannelNum));
        break;
        
    case SIO_HUP:                                                       /*  �رմ���                    */
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
