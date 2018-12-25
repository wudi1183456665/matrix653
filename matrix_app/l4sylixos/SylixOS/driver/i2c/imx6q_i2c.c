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
** ��   ��   ��: imx6q_i2c.c
**
** ��   ��   ��: Zhang.Xu (����)
**
** �ļ���������: 2015 �� 12 �� 13 ��
**
** ��        ��: Freescale ��˾�� IMX ϵͳ������ I2C ��������
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include <linux/compat.h>

#include "sdk.h"
#include "regsi2c.h"
#include "pinmux/iomux_config.h"
#include "i2c/imx6q_i2c.h"
/*********************************************************************************************************
  I2C ��������ض���
*********************************************************************************************************/
#define I2C_CHAN_NUM                (3)
#define I2C_BUS_FREQ_MAX            (400000)
#define I2C_BUS_FREQ_MIN            (100000)
#define IMX_I2C_BIT_RATE	        (100000)                            /*  Ĭ�� I2C CLK Ƶ�� 100kHz    */

#define IMX_I2C_IADR	            (0x00)
#define IMX_I2C_IFDR	            (0x04)
#define IMX_I2C_I2CR	            (0x08)
#define IMX_I2C_I2SR	            (0x0C)
#define IMX_I2C_I2DR	            (0x10)

#define I2SR_RXAK	                (0x01)
#define I2SR_IIF	                (0x02)
#define I2SR_SRW	                (0x04)
#define I2SR_IAL	                (0x10)
#define I2SR_IBB	                (0x20)
#define I2SR_IAAS	                (0x40)
#define I2SR_ICF	                (0x80)
#define I2CR_RSTA	                (0x04)
#define I2CR_TXAK	                (0x08)
#define I2CR_MTX	                (0x10)
#define I2CR_MSTA	                (0x20)
#define I2CR_IIEN	                (0x40)
#define I2CR_IEN	                (0x80)

#define I2C_WRITE                   (0)
#define I2C_READ                    (1)
#define ERR_TX                      (-1)
#define ERR_RX                      (-2)
#define ERR_ARB_LOST                (-3)
#define ERR_NO_ACK                  (-4)
#define ERR_XFER                    (-5)
#define ERR_RX_ACK                  (-6)
#define ERR_NO_ACK_ON_START         (-7)
#define ERR_INVALID_REQUEST         (-8)
#define IMX6_DEFAULT_SLAVE_ID       (0x60)
#define WAIT_RXAK_LOOPS             (1000000)
#define WAIT_BUSY_LOOPS             (100000)
/*********************************************************************************************************
  i2c ͨ�����Ͷ���
*********************************************************************************************************/
typedef struct {
    addr_t              I2C_ulPhyAddrBase;
    UINT                I2C_uiBusFreq;

    addr_t              I2C_ulVirtAddrBase;
    LW_HANDLE           I2C_hSignal;
    BOOL                I2C_bIsInit;
    PUCHAR              I2C_pucBuffer;
    UINT                I2C_uiBufferLen;
} IMX6Q_I2C_CHANNEL, *__IMX6Q_I2C_CHANNEL;
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
static IMX6Q_I2C_CHANNEL  _G_imx6qI2cChannels[I2C_CHAN_NUM] = {
        {
        		I2C1_BASE_ADDR,
                I2C_BUS_FREQ_MIN,
        }, {
        		I2C2_BASE_ADDR,
                I2C_BUS_FREQ_MIN,
        }, {
        		I2C3_BASE_ADDR,
                I2C_BUS_FREQ_MIN,
        }
};
/*********************************************************************************************************
  I2C ʱ��Ƶ�����ò�������
*********************************************************************************************************/
static UINT16 i2c_clk_div[50][2] = {
	{ 22,	0x20 }, { 24,	0x21 }, { 26,	0x22 }, { 28,	0x23 },
	{ 30,	0x00 },	{ 32,	0x24 }, { 36,	0x25 }, { 40,	0x26 },
	{ 42,	0x03 }, { 44,	0x27 },	{ 48,	0x28 }, { 52,	0x05 },
	{ 56,	0x29 }, { 60,	0x06 }, { 64,	0x2A },	{ 72,	0x2B },
	{ 80,	0x2C }, { 88,	0x09 }, { 96,	0x2D }, { 104,	0x0A },
	{ 112,	0x2E }, { 128,	0x2F }, { 144,	0x0C }, { 160,	0x30 },
	{ 192,	0x31 },	{ 224,	0x32 }, { 240,	0x0F }, { 256,	0x33 },
	{ 288,	0x10 }, { 320,	0x34 },	{ 384,	0x35 }, { 448,	0x36 },
	{ 480,	0x13 }, { 512,	0x37 }, { 576,	0x14 },	{ 640,	0x38 },
	{ 768,	0x39 }, { 896,	0x3A }, { 960,	0x17 }, { 1024,	0x3B },
	{ 1152,	0x18 }, { 1280,	0x3C }, { 1536,	0x3D }, { 1792,	0x3E },
	{ 1920,	0x1B },	{ 2048,	0x3F }, { 2304,	0x1C }, { 2560,	0x1D },
	{ 3072,	0x1E }, { 3840,	0x1F }
};
/*********************************************************************************************************
** ��������: imxSendStop
** ��������: Generates a STOP signal, called by rx and tx routines
** �䡡��  : uiInstance    Instance number of the I2C module
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  imxSendStop (UINT uiInstance)
{
    HW_I2C_I2CR(uiInstance).B.MSTA = 0;
}
/*********************************************************************************************************
** ��������: imxIsBusFree
** ��������: Loop status register for IBB to go 0.
** �䡡��  : uiInstance    Instance number of the I2C module
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imxIsBusFree (UINT uiInstance)
{
    INT i = WAIT_RXAK_LOOPS;

    while (HW_I2C_I2SR(uiInstance).B.IBB && (--i > 0));

    if (i <= 0) {
        printk("Error: I2C Bus not free!\n");
        return -1;
    }

    return 0;
}
/*********************************************************************************************************
** ��������: imxWaitOpDone
** ��������: Loop status register for IBB to go 0.
** �䡡��  : uiInstance    Instance number of the I2C module
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imxWaitOpDone (UINT32 uiInstance, INT iIsTx)
{
    hw_i2c_i2sr_t v;
    INT           i = WAIT_RXAK_LOOPS;

    /*
     * Loop until we get an interrupt
     */
    do {
        v.U = HW_I2C_I2SR_RD(uiInstance);
    } while (!v.B.IIF && (--i > 0));

    /*
     * If timeout error occurred return error
     */
    if (i <= 0) {
        printk("I2C Error: timeout unexpected\n");
        return (-1);
    }

    /*
     * Clear the interrupts
     */
    HW_I2C_I2SR_WR(uiInstance, 0);

    /*
     * Check for arbitration lost
     */
    if (v.B.IAL) {
        printk("Error %d: Arbitration lost\n", __LINE__);
        return (ERR_ARB_LOST);
    }

    /*
     * Check for ACK received in transmit mode
     */
    if (iIsTx) {
        if (v.B.RXAK) {
            /*
             * No ACK received, generate STOP by clearing MSTA bit
             */
            printk("Error %d: no ack received\n", __LINE__);

            imxSendStop(uiInstance);
            return (ERR_NO_ACK);
        }
    }

    return 0;
}
/*********************************************************************************************************
** ��������: imxRxBytes
** ��������: Loop status register for IBB to go 0.
** �䡡��  : uiInstance    Instance number of the I2C module
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imxRxBytes (UINT8 *pChar, UINT32 uiInstance, INT iSize)
{
    INT  i;

    for (i = 0; iSize > 0; iSize--, i++) {
        if (imxWaitOpDone(uiInstance, 0) != 0) {
            return (-1);
        }

        /*
         * the next two if-statements setup for the next read control register value
         */
        if (iSize == 1) {
            imxSendStop(uiInstance);
        }

        if (iSize == 2) {
            HW_I2C_I2CR(uiInstance).B.TXAK = 1;
        }

        /*
         * read the true data
         */
        pChar[i] = HW_I2C_I2DR_RD(uiInstance);
    }

    return (0);
}
/*********************************************************************************************************
** ��������: imxSetI2cClk
** ��������: Loop status register for IBB to go 0.
** �䡡��  : uiInstance    Instance number of the I2C module
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  imxSetI2cClk (UINT32 uiInstance, UINT32 uiBaud)
{
    UINT32 uiSrcClk = imx6qMainClkGet(IPG_PER_CLK);
    UINT32 uiDivider = uiSrcClk / uiBaud;
    UINT8 ucIndex = 0;

    if (uiDivider < i2c_clk_div[0][0]) {
        uiDivider = i2c_clk_div[0][0];
        ucIndex = 0;
        printk("Warning :can't find a smaller divider than %d.\n", uiDivider);
        printk("SCL frequency is set at %d - expected was %d.\n", uiSrcClk/uiDivider, uiBaud);

    } else if (uiDivider > i2c_clk_div[49][0]) {
        uiDivider = i2c_clk_div[49][0];
        ucIndex = 49;
        printk("Warning: can't find a bigger divider than %d.\n", uiDivider);
        printk("SCL frequency is set at %d - expected was %d.\n", uiSrcClk/uiDivider, uiBaud);

    } else {
        for (ucIndex = 0; i2c_clk_div[ucIndex][0] < uiDivider; ucIndex++);
        uiDivider = i2c_clk_div[ucIndex][0];
    }

    HW_I2C_IFDR_WR(uiInstance, BF_I2C_IFDR_IC(i2c_clk_div[ucIndex][1]));
}
/*********************************************************************************************************
** ��������: imxTxByte
** ��������: Loop status register for IBB to go 0.
** �䡡��  : uiInstance    Instance number of the I2C module
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imxTxByte (UINT8 *pChar, UINT32 uiInstance)
{
    /*
     * clear both IAL and IIF bits
     */
    HW_I2C_I2SR_WR(uiInstance, 0);

    /*
     * write to data register
     */
    HW_I2C_I2DR_WR(uiInstance, *pChar);

    /*
     * wait for transfer of byte to complete
     */
    return imxWaitOpDone(uiInstance, 1);
}
/*********************************************************************************************************
** ��������: imxWaitTillBusy
** ��������: Loop status register for IBB to go 0.
** �䡡��  : uiInstance    Instance number of the I2C module
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  imxWaitTillBusy (UINT32 uiInstance)
{
    INT i = WAIT_BUSY_LOOPS;

    while (!HW_I2C_I2SR(uiInstance).B.IBB && (--i > 0)) {

        if (HW_I2C_I2SR(uiInstance).B.IAL) {
            printk("Error: arbitration lost!\n");
            return -1;
        }
    }

    if (i <= 0) {
        printk("I2C Error: timeout in \n");
        return -1;
    }

    return 0;
}
/*********************************************************************************************************
** ��������: imx6qI2cClk
** ��������: ����I2C���ߵ�ͨ��ʱ�Ӽ������ʣ�ͨ��Ϊ400k��100k��
** �䡡��  : uiInstance     i2c ����������ַ  I2C1_BASE_ADDR  I2C2_BASE_ADDR  I2C3_BASE_ADDR
             uiBaud         i2c ����ʱ��   400000   100000 ��
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  imx6qI2cClk (UINT32 atBaseAddr, UINT32 uiBaud)
{
    UINT32 uiSrcClk   = imx6qMainClkGet(IPG_PER_CLK);
    UINT32 uiDivider  = uiSrcClk / uiBaud;
    UINT8  ucIndex    = 0;
    UINT32 uiInstance = REGS_I2C_INSTANCE(atBaseAddr);

    if (uiDivider < i2c_clk_div[0][0]) {
        uiDivider = i2c_clk_div[0][0];
        ucIndex = 0;
        printk("Warning :can't find a smaller divider than %d.\n", uiDivider);
        printk("SCL frequency is set at %d - expected was %d.\n", uiSrcClk/uiDivider, uiBaud);

    } else if (uiDivider > i2c_clk_div[49][0]) {
        uiDivider = i2c_clk_div[49][0];
        ucIndex = 49;
        printk("Warning: can't find a bigger divider than %d.\n", uiDivider);
        printk("SCL frequency is set at %d - expected was %d.\n", uiSrcClk/uiDivider, uiBaud);

    } else {
        for (ucIndex = 0; i2c_clk_div[ucIndex][0] < uiDivider; ucIndex++);
        uiDivider = i2c_clk_div[ucIndex][0];
    }

    HW_I2C_IFDR_WR(uiInstance, BF_I2C_IFDR_IC(i2c_clk_div[ucIndex][1]));
}

/*********************************************************************************************************
** ��������: imx6qI2cTransferMsg
** ��������: i2c ������Ϣ
** �䡡��  : pI2cChannel     i2c ͨ��
**           pI2cMsg         i2c ������Ϣ��
**           iNum            ��Ҫ�����  ��Ϣ����
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2cTransferMsg (__IMX6Q_I2C_CHANNEL  pI2cChannel, PLW_I2C_MESSAGE pI2cMsg, INT iNUM)
{
    INT     i, iRet;
    UINT8   *pucData, ucAddr;
    UINT16  usReg;
    UINT16  usDlen     = pI2cMsg->I2CMSG_usLen;
    addr_t  atI2cBase  = pI2cChannel->I2C_ulVirtAddrBase;
    INT     uiInstance = REGS_I2C_INSTANCE(atI2cBase);                  /*  ת���� i2c �ı�ʶ           */

    if (pI2cMsg->I2CMSG_usFlag & LW_I2C_M_RD) {                         /*  ��ȡ����                    */

        HW_I2C_I2CR(uiInstance).B.RSTA = 1;                             /*  do repeat-start             */

        /*
         * send slave address again, but indicate read operation
         */
        ucAddr  = pI2cMsg->I2CMSG_usAddr;
        pucData = &ucAddr;
        ucAddr  = (ucAddr << 1) | I2C_READ;

        if (imxTxByte(pucData, uiInstance) != 0) {
            return -1;
        }

        /*
         * change to receive mode
         */
        usReg = HW_I2C_I2CR_RD(uiInstance) & ~BM_I2C_I2CR_MTX;

        /*
         * if only one byte to read, make sure don't send ack
         */
        if (pI2cMsg->I2CMSG_usLen == 1) {
            usReg |= BM_I2C_I2CR_TXAK;
        }
        HW_I2C_I2CR_WR(uiInstance, usReg);

        /*
         * dummy read
         */
        *pucData = HW_I2C_I2DR_RD(uiInstance);

        /*
         * now reading ...
         */
        if (imxRxBytes(pI2cMsg->I2CMSG_pucBuffer, uiInstance, pI2cMsg->I2CMSG_usLen) != 0) {
            return (-1);
        }

    } else {                                                            /*  ���Ͳ���                    */

        /*
         * Step 2: send slave address + read/write at the LSB
         */
        ucAddr  = pI2cMsg->I2CMSG_usAddr;
        pucData = &ucAddr;
        ucAddr  = (ucAddr << 1) | I2C_WRITE;

        if ((iRet = imxTxByte(pucData, uiInstance)) != 0) {
            printk("START TX ERR %d\n", iRet);
            if (iRet == ERR_NO_ACK) {
                return ERR_NO_ACK_ON_START;
            } else {
                return iRet;
            }
        }

    	pucData = pI2cMsg->I2CMSG_pucBuffer;
    	for (i = 0; i < usDlen; i++) {
            /*
             * send device register value
             */
            if ((iRet = imxTxByte(pucData, uiInstance)) != 0) {
                break;
            }
            pucData++;
        }
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6qI2cTryTransfer
** ��������: i2c ���Դ��亯��
** �䡡��  : pI2cChannel     i2c ͨ��
**           pI2cAdapter     i2c ������
**           pI2cMsg         i2c ������Ϣ��
**           iNum            ��Ϣ����
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2cTryTransfer (__IMX6Q_I2C_CHANNEL  pI2cChannel,
		                         PLW_I2C_ADAPTER      pI2cAdapter,
                                 PLW_I2C_MESSAGE      pI2cMsg,
                                 INT                  iNum)
{
    int     i;
    addr_t  atI2cBase  = pI2cChannel->I2C_ulVirtAddrBase;
    INT     uiInstance = REGS_I2C_INSTANCE(atI2cBase);

    /*
     * If the request has device info attached and it has a non-zero bit rate, then
     * change the clock to the specified rate.
     */
    imxSetI2cClk(uiInstance, 100000);

    /*
     * clear the status register
     */
    HW_I2C_I2SR_WR(uiInstance, 0);

    /*
     * enable the I2C controller
     */
    HW_I2C_I2CR_WR(uiInstance, BM_I2C_I2CR_IEN);

    udelay(50);

    /*
     * Check if bus is free, if not return error
     */
    if (imxIsBusFree(uiInstance) != 0) {
        return (-1);
    }

    /*
     * Select master mode, assert START signal and also indicate TX mode
     */
    HW_I2C_I2CR_WR(uiInstance, BM_I2C_I2CR_IEN | BM_I2C_I2CR_MSTA | BM_I2C_I2CR_MTX);

    /*
     * make sure bus is busy after the START signal
     */
    if (imxWaitTillBusy(uiInstance) != 0) {
        return (-1);
    }

    /*
     * ����豸��ַ���ͺ󣬽����շ���Ϣ����
     */
    for (i = 0; i < iNum; i++, pI2cMsg++) {
        if (imx6qI2cTransferMsg(pI2cChannel, pI2cMsg, iNum) != ERROR_NONE) {
            break;
        }
    }

    /*
     * generate STOP by clearing MSTA bit
     */
    imxSendStop(uiInstance);

    /*
     * Check if bus is free, if not return error
     */
    if (imxIsBusFree(uiInstance) != 0) {
        printk("WARNING: bus is not free\n");
    }

    /*
     * disable the controller
     */
    HW_I2C_I2CR_WR(uiInstance, 0);

    return  (i);
}
/*********************************************************************************************************
** ��������: imx6qI2cTransfer
** ��������: i2c ���亯��
** �䡡��  : pI2cChannel     i2c ͨ��
**           pI2cAdapter     i2c ������
**           pI2cMsg         i2c ������Ϣ��
**           iNum            ��Ϣ����
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2cTransfer (__IMX6Q_I2C_CHANNEL  pI2cChannel,
		                      PLW_I2C_ADAPTER      pI2cAdapter,
                              PLW_I2C_MESSAGE      pI2cMsg,
                              INT                  iNum)
{
    INT  i;

    /*
     * ����ʹ���˴����ش��Ĺ��ܣ�������ʧ�����δ��䣬����ʵ��Ӧ���д���ʧ����С�����¼���
     * ����˹��ܷ����û���ʵ�֣��������������������ݴ���ͽ��ո����ʡ�
     */
    for (i = 0; i < pI2cAdapter->I2CADAPTER_iRetry; i++) {
        if (imx6qI2cTryTransfer(pI2cChannel, pI2cAdapter, pI2cMsg, iNum) == iNum) {
            return  (iNum);

        } else {
            API_TimeSleep(LW_OPTION_WAIT_A_TICK);                       /*  �ȴ�һ��������������        */

        }
    }

    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: imx6qI2c0Transfer
** ��������: i2c0 ���亯��
** �䡡��  : pI2cAdapter     i2c ������
**           pI2cMsg         i2c ������Ϣ��
**           iNum            ��Ϣ����
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2c0Transfer (PLW_I2C_ADAPTER     pI2cAdapter,
                               PLW_I2C_MESSAGE     pI2cMsg,
                               INT                 iNum)
{
    return  (imx6qI2cTransfer(&_G_imx6qI2cChannels[0], pI2cAdapter, pI2cMsg, iNum));
}
/*********************************************************************************************************
** ��������: imx6qI2c1Transfer
** ��������: i2c1 ���亯��
** �䡡��  : pI2cAdapter     i2c ������
**           pI2cMsg         i2c ������Ϣ��
**           iNum            ��Ϣ����
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2c1Transfer (PLW_I2C_ADAPTER     pI2cAdapter,
                               PLW_I2C_MESSAGE     pI2cMsg,
                               INT                 iNum)
{
    return  (imx6qI2cTransfer(&_G_imx6qI2cChannels[1], pI2cAdapter, pI2cMsg, iNum));
}
/*********************************************************************************************************
** ��������: imx6qI2c2Transfer
** ��������: i2c2 ���亯��
** �䡡��  : pI2cAdapter     i2c ������
**           pI2cMsg         i2c ������Ϣ��
**           iNum            ��Ϣ����
** �䡡��  : ��ɴ������Ϣ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2c2Transfer (PLW_I2C_ADAPTER     pI2cAdapter,
                               PLW_I2C_MESSAGE     pI2cMsg,
                               INT                 iNum)
{
    return  (imx6qI2cTransfer(&_G_imx6qI2cChannels[2], pI2cAdapter, pI2cMsg, iNum));
}
/*********************************************************************************************************
** ��������: imx6qI2cMasterCtl
** ��������: i2c ���ƺ���
** �䡡��  : pI2cChannel     i2c ͨ��
**           pI2cAdapter     i2c ������
**           iCmd            ����
**           lArg            ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2cMasterCtl (__IMX6Q_I2C_CHANNEL  pI2cChannel,
                               PLW_I2C_ADAPTER      pI2cAdapter,
                               INT                  iCmd,
                               LONG                 lArg)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6qI2c0MasterCtl
** ��������: i2c0 ���ƺ���
** �䡡��  : pI2cAdapter     i2c ������
**           iCmd            ����
**           lArg            ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2c0MasterCtl (PLW_I2C_ADAPTER   pI2cAdapter,
                                INT               iCmd,
                                LONG              lArg)
{
    return  (imx6qI2cMasterCtl(&_G_imx6qI2cChannels[0], pI2cAdapter, iCmd, lArg));
}
/*********************************************************************************************************
** ��������: imx6qI2c1MasterCtl
** ��������: i2c1 ���ƺ���
** �䡡��  : pI2cAdapter     i2c ������
**           iCmd            ����
**           lArg            ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2c1MasterCtl (PLW_I2C_ADAPTER   pI2cAdapter,
                                INT               iCmd,
                                LONG              lArg)
{
    return  (imx6qI2cMasterCtl(&_G_imx6qI2cChannels[1], pI2cAdapter, iCmd, lArg));
}
/*********************************************************************************************************
** ��������: imx6qI2c2MasterCtl
** ��������: i2c2 ���ƺ���
** �䡡��  : pI2cAdapter     i2c ������
**           iCmd            ����
**           lArg            ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2c2MasterCtl (PLW_I2C_ADAPTER   pI2cAdapter,
                                INT               iCmd,
                                LONG              lArg)
{
    return  (imx6qI2cMasterCtl(&_G_imx6qI2cChannels[2], pI2cAdapter, iCmd, lArg));
}
/*********************************************************************************************************
** ��������: imx6qI2cHwInit
** ��������: ��ʼ�� i2c Ӳ��ͨ��
** �䡡��  : pI2cChannel     i2c ͨ��
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2cHwInit (__IMX6Q_I2C_CHANNEL  pI2cChannel)
{
    addr_t  atI2cAddr  = pI2cChannel->I2C_ulVirtAddrBase;
    INT     uiInstance = REGS_I2C_INSTANCE(atI2cAddr);

    /*
     * enable the source clocks to the I2C port
     */
    imx6qClkGateSet(atI2cAddr, CLOCK_ON);

    HW_I2C_I2CR_WR(uiInstance, 0);

    imxSetI2cClk(uiInstance, I2C_BUS_FREQ_MAX);

    HW_I2C_IADR_WR(uiInstance, IMX6_DEFAULT_SLAVE_ID);

    HW_I2C_I2SR_WR(uiInstance, 0);

    HW_I2C_I2CR_WR(uiInstance, BM_I2C_I2CR_IEN);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: imx6qI2cInit
** ��������: ��ʼ�� i2c ͨ��
** �䡡��  : pI2cChannel     i2c ͨ��
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  imx6qI2cInit (__IMX6Q_I2C_CHANNEL  pI2cChannel)
{
    if (!pI2cChannel->I2C_bIsInit) {

    	/*
    	 * ��ʹ�� ioremap ����ʵ�������ַ�������ַת������ bspMap.h �н��о�̬ӳ��
    	 */
        pI2cChannel->I2C_ulVirtAddrBase = pI2cChannel->I2C_ulPhyAddrBase;

        /*
         * ��ʼ�� I2C ʹ�õ��ź���
         */
        pI2cChannel->I2C_hSignal = API_SemaphoreBCreate("i2c_signal", LW_FALSE,
                                                         LW_OPTION_OBJECT_GLOBAL, LW_NULL);
        if (pI2cChannel->I2C_hSignal == LW_OBJECT_HANDLE_INVALID) {
            printk(KERN_ERR "imx6qI2cInit(): failed to create i2c_signal!\n");
            goto  __error_handle;
        }

        /*
         * ��ʼ�� I2C ������
         */
        if (imx6qI2cHwInit(pI2cChannel) != ERROR_NONE) {
            printk(KERN_ERR "imx6qI2cInit(): failed to init!\n");
            goto  __error_handle;
        }

        pI2cChannel->I2C_bIsInit = LW_TRUE;
    }

    return  (ERROR_NONE);

    __error_handle:

    if (pI2cChannel->I2C_hSignal) {
        API_SemaphoreBDelete(&pI2cChannel->I2C_hSignal);
        pI2cChannel->I2C_hSignal = 0;
    }

    return  (PX_ERROR);
}
/*********************************************************************************************************
  i2c ��������
*********************************************************************************************************/
static LW_I2C_FUNCS  _G_am335xI2cFuncs[I2C_CHAN_NUM] = {
        {
                imx6qI2c0Transfer,
                imx6qI2c0MasterCtl,
        }, {
                imx6qI2c1Transfer,
                imx6qI2c1MasterCtl,
        }, {
                imx6qI2c2Transfer,
                imx6qI2c2MasterCtl,
        }
};
/*********************************************************************************************************
** ��������: i2cBusFuns
** ��������: ��ʼ�� i2c ���߲���ȡ��������
** �䡡��  : uiChannel         ͨ����
**           pPinMux           �ܽŸ���
** �䡡��  : i2c ������������
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PLW_I2C_FUNCS  i2cBusFuns (UINT  uiChannel)
{
    if (uiChannel >= I2C_CHAN_NUM) {
        printk(KERN_ERR "i2cBusFuns(): I2C channel invalid!\n");
        return  (LW_NULL);
    }

    /*
     * ����оƬ�ܽŷ��䣬SylixOS �������㿪ʼ���� IMX6Q �ֲ��Ǵ� 1 ��ʼ����Ҫע��
     */
    switch (uiChannel) {
    case 0:
        i2c1_iomux_config();
        break;

    case 1:
        i2c2_iomux_config();
        break;

    case 2:
        i2c3_iomux_config();
        break;
    }

    if (imx6qI2cInit(&_G_imx6qI2cChannels[uiChannel]) != ERROR_NONE) {
        return  (LW_NULL);
    }

    return  (&_G_am335xI2cFuncs[uiChannel]);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
