/*
 * 16c550_sio.c
 *
 *  Created on: 2018��9��25��
 *      Author: LeoH
 */

//#define  __SYLIXOS_KERNEL
//#include "SylixOS.h"
#include "./drv_cfg.h"

//#define  __SYLIXOS_KERNEL
#include "../bsp/config.h"
//#include <SylixOS.h>
//#include <driver/sio/16c550.h>
/*********************************************************************************************************
  �ü�����
*********************************************************************************************************/
//#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_SIO_DEVICE_EN > 0) && (LW_CFG_DRV_SIO_16C550 > 0)
#include "16c550_sio.h"
/*********************************************************************************************************
  16c550 register operate
*********************************************************************************************************/
#define SET_REG(psiochan, reg, value)   psiochan->setreg(psiochan, reg, (UINT8)(value))
#define GET_REG(psiochan, reg)          psiochan->getreg(psiochan, reg)
/*********************************************************************************************************
  16c550 switch pin operate (RS-485 MODE)
*********************************************************************************************************/
#define IS_SWITCH_EN(psiochan)          (psiochan->switch_en)
#define IS_Tx_HOLD_REG_EMPTY(psiochan)  ((GET_REG(psiochan, LSR) & LSR_THRE) != 0x00)

#define SEND_START(psiochan) do {   \
            psiochan->send_start(psiochan); \
            if (psiochan->delay) {  \
               psiochan->delay(psiochan);  \
            }   \
        } while (0)

#define SEND_END(psiochan) do {   \
            psiochan->send_end(psiochan); \
            if (psiochan->delay) {  \
               psiochan->delay(psiochan);  \
            }   \
        } while (0)
/*********************************************************************************************************
  internal function declare
*********************************************************************************************************/
static INT sio16c550SetBaud(SIO16C550_CHAN *psiochan, ULONG baud);
static INT sio16c550SetHwOption(SIO16C550_CHAN *psiochan, ULONG hw_option);
static INT sio16c550Ioctl(SIO16C550_CHAN *psiochan, INT cmd, LONG arg);
static INT sio16c550TxStartup(SIO16C550_CHAN *psiochan);
static INT sio16c550CallbackInstall(SIO_CHAN *pchan,
                                    INT       callbackType,
                                    INT     (*callback)(),
                                    VOID     *callbackArg);
static INT sio16c550PollInput(SIO16C550_CHAN *psiochan, CHAR *pc);
static INT sio16c550PollOutput(SIO16C550_CHAN *psiochan, CHAR c);
/*********************************************************************************************************
  16c550 driver functions
*********************************************************************************************************/
static SIO_DRV_FUNCS sio16c550_drv_funcs = {
        (INT (*)())sio16c550Ioctl,
        (INT (*)())sio16c550TxStartup,
        (INT (*)())sio16c550CallbackInstall,
        (INT (*)())sio16c550PollInput,
        (INT (*)(SIO_CHAN *, CHAR))sio16c550PollOutput,
};
/*********************************************************************************************************
** ��������: sio16c550Init
** ��������: ��ʼ�� 16C550 ��������
** �䡡��  : psiochan      SIO CHAN
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  sio16c550Init (SIO16C550_CHAN *psiochan)
{
    /*
     * initialize the driver function pointers in the SIO_CHAN's
     */
    psiochan->pdrvFuncs = &sio16c550_drv_funcs;

    LW_SPIN_INIT(&psiochan->slock);

    psiochan->channel_mode = SIO_MODE_POLL;
    psiochan->switch_en    = 0;
    psiochan->hw_option    = (CLOCAL | CREAD | CS8);

    psiochan->mcr = MCR_OUT2;
    psiochan->lcr = 0;
    psiochan->ier = 0;

    psiochan->bdefer = LW_FALSE;

    psiochan->err_overrun = 0;
    psiochan->err_parity  = 0;
    psiochan->err_framing = 0;
    psiochan->err_break   = 0;

    psiochan->rx_trigger_level &= 0x3;

    /*
     * reset the chip
     */
    sio16c550SetBaud(psiochan, psiochan->baud);
    sio16c550SetHwOption(psiochan, psiochan->hw_option);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: sio16c550SetBaud
** ��������: ���ò�����
** �䡡��  : psiochan      SIO CHAN
**           baud          ������
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550SetBaud (SIO16C550_CHAN *psiochan, ULONG  baud)
{
    INTREG  intreg;
    INT     divisor = (INT)((psiochan->xtal + (8 * baud)) / (16 * baud));

    if ((divisor < 1) || (divisor > 0xffff)) {
        _ErrorHandle(EIO);
        return  (PX_ERROR);
    }

    /*
     * disable interrupts during chip access
     */
    LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

    /*
     * Enable access to the divisor latches by setting DLAB in LCR.
     */
    SET_REG(psiochan, LCR, (LCR_DLAB | psiochan->lcr));

    /*
     * Set divisor latches.
     */
    SET_REG(psiochan, DLL, divisor);
    SET_REG(psiochan, DLM, (divisor >> 8));

    /*
     * Restore line control register
     */
    SET_REG(psiochan, LCR, psiochan->lcr);

    psiochan->baud = baud;

    /*
     * some 16550 ip need reset hw here
     */
    SET_REG(psiochan, IER, 0);                                          /* disable interrupt            */

    SET_REG(psiochan, FCR,
            ((psiochan->rx_trigger_level << 6) |
             RxCLEAR | TxCLEAR | FIFO_ENABLE));

    if (psiochan->channel_mode == SIO_MODE_INT) {
        SET_REG(psiochan, IER, psiochan->ier);                          /* enable interrupt             */
    }

    LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: sio16c550SetHwOption
** ��������: ����Ӳ������ѡ��
** �䡡��  : psiochan      SIO CHAN
**           hw_option     Ӳ������ѡ��
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550SetHwOption (SIO16C550_CHAN *psiochan, ULONG  hw_option)
{
    INTREG  intreg;

    if (!psiochan) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    hw_option |= HUPCL;                                                 /* need HUPCL option            */

    LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

    psiochan->lcr = 0;
    psiochan->mcr &= (~(MCR_RTS | MCR_DTR));                            /* clear RTS and DTR bits       */

    switch (hw_option & CSIZE) {                                        /* data bit set                 */

    case CS5:
        psiochan->lcr = CHAR_LEN_5;
        break;

    case CS6:
        psiochan->lcr = CHAR_LEN_6;
        break;

    case CS7:
        psiochan->lcr = CHAR_LEN_7;
        break;

    case CS8:
        psiochan->lcr = CHAR_LEN_8;
        break;

    default:
        psiochan->lcr = CHAR_LEN_8;
        break;
    }

    if (hw_option & STOPB) {                                            /* stop bit set                 */
        psiochan->lcr |= LCR_STB;

    } else {
        psiochan->lcr |= ONE_STOP;
    }

    switch (hw_option & (PARENB | PARODD)) {

    case PARENB | PARODD:
        psiochan->lcr |= LCR_PEN;
        psiochan->ier |= IER_ELSI;                                      /* Enable receiver line status  */
        break;

    case PARENB:
        psiochan->lcr |= (LCR_PEN | LCR_EPS);
        psiochan->ier |= IER_ELSI;                                      /* Enable receiver line status  */
        break;

    default:
        psiochan->lcr |= PARITY_NONE;
        psiochan->ier &= (~IER_ELSI);
        break;
    }

    SET_REG(psiochan, IER, 0);

    if (!(hw_option & CLOCAL)) {
        /*
         * !clocal enables hardware flow control(DTR/DSR)
         */
        psiochan->mcr |= (MCR_DTR | MCR_RTS);
        psiochan->ier &= (~TxFIFO_BIT);
        psiochan->ier |= IER_EMSI;                                      /* en modem status interrupt    */

    } else {
        psiochan->ier &= (~IER_EMSI);                                   /* dis modem status interrupt   */
    }

    SET_REG(psiochan, LCR, psiochan->lcr);
    SET_REG(psiochan, MCR, psiochan->mcr);

    /*
     * now reset the channel mode registers
     */
    SET_REG(psiochan, FCR,
            ((psiochan->rx_trigger_level << 6) |
             RxCLEAR | TxCLEAR | FIFO_ENABLE));

    if (hw_option & CREAD) {
        psiochan->ier |= RxFIFO_BIT;
    }

    if (psiochan->channel_mode == SIO_MODE_INT) {
        SET_REG(psiochan, IER, psiochan->ier);                          /* enable interrupt             */
    }

    psiochan->hw_option = hw_option;

    LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: sio16c550Hup
** ��������: ���ڽӿڹ���
** �䡡��  : psiochan      SIO CHAN
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550Hup (SIO16C550_CHAN *psiochan)
{
    INTREG  intreg;

    LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

    psiochan->mcr &= (~(MCR_RTS | MCR_DTR));
    SET_REG(psiochan, MCR, psiochan->mcr);
    SET_REG(psiochan, FCR, (RxCLEAR | TxCLEAR));

    LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: sio16c550Open
** ��������: ���ڽӿڴ� (Set the modem control lines)
** �䡡��  : psiochan      SIO CHAN
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550Open (SIO16C550_CHAN *psiochan)
{
    INTREG  intreg;
    UINT8   mask;

    mask = (UINT8)(GET_REG(psiochan, MCR) & (MCR_RTS | MCR_DTR));

    if (mask != (MCR_RTS | MCR_DTR)) {
        /*
         * RTS and DTR not set yet
         */
        LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

        /*
         * set RTS and DTR TRUE
         */
        psiochan->mcr |= (MCR_DTR | MCR_RTS);
        SET_REG(psiochan, MCR, psiochan->mcr);

        /*
         * clear Tx and receive and enable FIFO
         */
        SET_REG(psiochan, FCR,
                ((psiochan->rx_trigger_level << 6) |
                 RxCLEAR | TxCLEAR | FIFO_ENABLE));

        LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: sio16550SetMode
** ��������: ���ڽӿ�����ģʽ
** �䡡��  : psiochan      SIO CHAN
**           newmode       �µ�ģʽ SIO_MODE_INT / SIO_MODE_POLL
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550SetMode (SIO16C550_CHAN *psiochan, INT newmode)
{
    INTREG  intreg;
    UINT8   mask;

    if ((newmode != SIO_MODE_POLL) && (newmode != SIO_MODE_INT)) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if (psiochan->channel_mode == newmode) {
        return  (ERROR_NONE);
    }

    LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

    if (newmode == SIO_MODE_INT) {
        /*
         * Enable appropriate interrupts
         */
        if (psiochan->hw_option & CLOCAL) {
            SET_REG(psiochan, IER, (psiochan->ier | RxFIFO_BIT | TxFIFO_BIT));

        } else {
            mask = (UINT8)(GET_REG(psiochan, MSR) & MSR_CTS);
            /*
             * if the CTS is asserted enable Tx interrupt
             */
            if (mask & MSR_CTS) {
                psiochan->ier |= TxFIFO_BIT;    /* enable Tx interrupt */

            } else {
                psiochan->ier &= (~TxFIFO_BIT); /* disable Tx interrupt */
            }

            SET_REG(psiochan, IER, psiochan->ier);
        }
    } else {
        /*
         * disable all ns16550 interrupts
         */
        SET_REG(psiochan, IER, 0);
    }

    psiochan->channel_mode = newmode;

    LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: sio16c550Ioctl
** ��������: ���ڽӿڿ���
** �䡡��  : psiochan      SIO CHAN
**           cmd           ����
**           arg           ����
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550Ioctl (SIO16C550_CHAN *psiochan, INT cmd, LONG arg)
{
    INT  error = ERROR_NONE;

    switch (cmd) {

    case SIO_BAUD_SET:
        if (arg < 50) {
            _ErrorHandle(EIO);
            error = PX_ERROR;
        } else {
            error = sio16c550SetBaud(psiochan, arg);
        }
        break;

    case SIO_BAUD_GET:
        *((LONG *)arg) = psiochan->baud;
        break;

    case SIO_MODE_SET:
        error = sio16c550SetMode(psiochan, (INT)arg);
        break;

    case SIO_MODE_GET:
        *((LONG *)arg) = psiochan->channel_mode;
        break;

    case SIO_HW_OPTS_SET:
        error = sio16c550SetHwOption(psiochan, arg);
        break;

    case SIO_HW_OPTS_GET:
        *(LONG *)arg = psiochan->hw_option;
        break;

    case SIO_HUP:
        if (psiochan->hw_option & HUPCL) {
            error = sio16c550Hup(psiochan);
        }
        break;

    case SIO_OPEN:
        if (psiochan->hw_option & HUPCL) {
            error = sio16c550Open(psiochan);
        }
        break;

    case SIO_SWITCH_PIN_EN_SET:
        if (*(INT *)arg) {
            if (!psiochan->send_start) {
                _ErrorHandle(ENOSYS);
                error = PX_ERROR;
                break;
            }
            if (psiochan->switch_en == 0) {
                SEND_END(psiochan);
            }
            psiochan->switch_en = 1;
        } else {
            psiochan->switch_en = 0;
        }
        break;

    case SIO_SWITCH_PIN_EN_GET:
        *(INT *)arg = psiochan->switch_en;
        break;

    default:
        _ErrorHandle(ENOSYS);
        error = PX_ERROR;
        break;
    }

    return  (error);
}
/*********************************************************************************************************
** ��������: sio16c550TxStartup
** ��������: ���ڽӿ���������
** �䡡��  : psiochan      SIO CHAN
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550TxStartup (SIO16C550_CHAN *psiochan)
{
    INTREG  intreg;
    UINT8   mask;
    CHAR    cTx;

    if (psiochan->switch_en && (psiochan->hw_option & CLOCAL)) {
        SEND_START(psiochan);                                           /* switch direct to send mode   */

        do {
            if (psiochan->pcbGetTxChar(psiochan->getTxArg, &cTx) != ERROR_NONE) {
                break;
            }
            while (!IS_Tx_HOLD_REG_EMPTY(psiochan));                    /* wait tx holding reg empty    */

            SET_REG(psiochan, THR, cTx);
        } while (1);

        while (!IS_Tx_HOLD_REG_EMPTY(psiochan));                        /* wait tx holding reg empty    */

        SEND_END(psiochan);                                             /* switch direct to rx mode     */

        return  (ERROR_NONE);
    }

    LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

    if (psiochan->channel_mode == SIO_MODE_INT) {
        if (psiochan->hw_option & CLOCAL) {                             /* No modem control             */
            if (psiochan->bdefer == LW_FALSE) {
                psiochan->ier |= TxFIFO_BIT;
            }

        } else {
            mask = (UINT8)(GET_REG(psiochan, MSR) & MSR_CTS);
            if (mask & MSR_CTS) {                                       /* if the CTS is enable Tx Int  */
                if (psiochan->bdefer == LW_FALSE) {
                    psiochan->ier |= TxFIFO_BIT;                        /* enable Tx interrupt          */
                }
            } else {
                psiochan->ier &= (~TxFIFO_BIT);                         /* disable Tx interrupt         */
            }
        }

        SET_REG(psiochan, IER, psiochan->ier);

        LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);

        return  (ERROR_NONE);

    } else {
        LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);

        _ErrorHandle(ENOSYS);
        return  (ENOSYS);
    }
}
/*********************************************************************************************************
** ��������: sio16c550CallbackInstall
** ��������: ���ڽӿڰ�װ�ص�����
** �䡡��  : pchan          SIO CHAN
**           callbackType   �ص�����
**           callback       �ص�����
**           callbackArg    �ص�����
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550CallbackInstall (SIO_CHAN *pchan,
                                     INT       callbackType,
                                     INT     (*callback)(),
                                     VOID     *callbackArg)
{
    SIO16C550_CHAN *psiochan = (SIO16C550_CHAN *)pchan;

    switch (callbackType) {

    case SIO_CALLBACK_GET_TX_CHAR:
        psiochan->pcbGetTxChar = callback;
        psiochan->getTxArg     = callbackArg;
        return  (ERROR_NONE);

    case SIO_CALLBACK_PUT_RCV_CHAR:
        psiochan->pcbPutRcvChar = callback;
        psiochan->putRcvArg     = callbackArg;
        return  (ERROR_NONE);

    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** ��������: sio16c550PollInput
** ��������: ���ڽӿڲ�ѯ��ʽ����
** �䡡��  : pchan          SIO CHAN
**           pc             ��ȡ��������
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550PollInput (SIO16C550_CHAN *psiochan, CHAR *pc)
{
    UINT8 poll_status = GET_REG(psiochan, LSR);

    if ((poll_status & LSR_DR) == 0x00) {
        _ErrorHandle(EAGAIN);
        return  (PX_ERROR);
    }

    *pc = GET_REG(psiochan, RBR);                                       /* got a character              */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: sio16c550PollInput
** ��������: ���ڽӿڲ�ѯ��ʽ���
** �䡡��  : pchan          SIO CHAN
**           c              ���������
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT sio16c550PollOutput (SIO16C550_CHAN *psiochan, char c)
{
    UINT8 msr = GET_REG(psiochan, MSR);

    while (!IS_Tx_HOLD_REG_EMPTY(psiochan));                            /* wait tx holding reg empty    */

    if (!(psiochan->hw_option & CLOCAL)) {                              /* modem flow control ?         */
        if (msr & MSR_CTS) {
            SET_REG(psiochan, THR, c);

        } else {
            _ErrorHandle(EAGAIN);
            return  (PX_ERROR);
        }

    } else {
        SET_REG(psiochan, THR, c);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: sio16c550TxIsr
** ��������: 16C550 ����жϷ�����
** �䡡��  : psiochan      SIO CHAN
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID sio16c550TxIsr (SIO16C550_CHAN *psiochan)
{
    INTREG  intreg;
    UINT8   ucTx;
    UINT8   lsr;
    INT     i;

    LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

    psiochan->bdefer = LW_FALSE;

    lsr = psiochan->lsr;                                                /*  first use save lsr value    */

    if (lsr & LSR_THRE) {
        for (i = 0; i < psiochan->fifo_len; i++) {
            LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);
            if (psiochan->pcbGetTxChar(psiochan->getTxArg, &ucTx)) {
                return;
            }
            LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

            SET_REG(psiochan, THR, ucTx);                               /* char to Transmit Holding Reg */
        }
    }

    psiochan->ier |= TxFIFO_BIT;                                        /*  enable Tx Int               */
    SET_REG(psiochan, IER, psiochan->ier);                              /*  update ier                  */

    LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);
}
/*********************************************************************************************************
** ��������: sio16c550RxIsr
** ��������: 16C550 �����жϷ�����
** �䡡��  : psiochan      SIO CHAN
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID sio16c550RxIsr (SIO16C550_CHAN *psiochan)
{
    INTREG  intreg;
    UINT8   ucRd;
    UINT8   lsr;

    LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

    lsr = psiochan->lsr;                                                /*  first use save lsr value    */

    do {
        if (lsr & (LSR_BI | LSR_FE | LSR_PE | LSR_OE)) {
            if (lsr & RxCHAR_AVAIL) {
                ucRd = GET_REG(psiochan, RBR);
            }
            if (lsr & LSR_BI) {
                psiochan->err_break++;
            }
            if (lsr & LSR_FE) {
                psiochan->err_framing++;
            }
            if (lsr & LSR_PE) {
                psiochan->err_parity++;
            }
            if (lsr & LSR_OE) {
                psiochan->err_overrun++;
            }

        } else if (lsr & RxCHAR_AVAIL) {
            ucRd = GET_REG(psiochan, RBR);

            LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);
            psiochan->pcbPutRcvChar(psiochan->putRcvArg, ucRd);
            LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);
        }

        lsr = GET_REG(psiochan, LSR);
    } while (lsr & RxCHAR_AVAIL);

    psiochan->ier |= RxFIFO_BIT;                                        /*  enable Rx Int               */
    SET_REG(psiochan, IER, psiochan->ier);                              /*  update ier                  */

    LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);
}
/*********************************************************************************************************
** ��������: sio16c550Isr
** ��������: 16C550 �жϷ�����
** �䡡��  : psiochan      SIO CHAN
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID sio16c550Isr (SIO16C550_CHAN *psiochan)
{
    INTREG  intreg;
    UINT8   iir;
    UINT8   lsr;
    UINT8   msr;

    LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);

    iir = (UINT8)(GET_REG(psiochan, IIR) & 0x0f);

    SET_REG(psiochan, IER, 0);                                          /* disable all interrupt        */

    lsr = GET_REG(psiochan, LSR);

    if (lsr & (RxCHAR_AVAIL | LSR_BI | LSR_FE | LSR_PE | LSR_OE)) {     /* Rx Int                       */
        psiochan->ier &= (~RxFIFO_BIT);
        SET_REG(psiochan, IER, psiochan->ier);                          /* disable Rx Int               */

        psiochan->lsr = lsr;                                            /* save lsr                     */
        LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);

#if LW_CFG_ISR_DEFER_EN > 0
        if (psiochan->pdeferq) {
            if (API_InterDeferJobAdd(psiochan->pdeferq,
                                     sio16c550RxIsr, psiochan)) {
                sio16c550RxIsr(psiochan);                               /* queue error                  */
            }
        } else
#endif                                                                  /* LW_CFG_ISR_DEFER_EN > 0      */
        {
            sio16c550RxIsr(psiochan);
        }

        LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);
    }

    if ((lsr & LSR_THRE) || ((iir & 0x06) == IIR_THRE)) {               /* Tx Int                       */
        psiochan->ier &= (~TxFIFO_BIT);                                 /* indicate to disable Tx Int   */
        SET_REG(psiochan, IER, psiochan->ier);                          /* disable Tx Int               */

        if (psiochan->bdefer == LW_FALSE) {                             /* not in queue                 */
            psiochan->bdefer =  LW_TRUE;

            psiochan->lsr = lsr;                                        /* save lsr                     */
            LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);

#if LW_CFG_ISR_DEFER_EN > 0
            if (psiochan->pdeferq) {
                if (API_InterDeferJobAdd(psiochan->pdeferq,
                                         sio16c550TxIsr, psiochan)) {
                    sio16c550TxIsr(psiochan);                           /* queue error                  */
                }
            } else
#endif                                                                  /* LW_CFG_ISR_DEFER_EN > 0      */
            {
                sio16c550TxIsr(psiochan);
            }

            LW_SPIN_LOCK_QUICK(&psiochan->slock, &intreg);
        }
    }

    switch (iir) {

    case IIR_RLS:                                                       /* overrun, parity error        */
        if (lsr & LSR_PE) {
            GET_REG(psiochan, RBR);
        }
        SET_REG(psiochan, IER, psiochan->ier);                          /*  update ier                  */
        LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);
        break;

    case IIR_MSTAT:                                                     /* modem status register        */
        msr = GET_REG(psiochan, MSR);
        if (msr & MSR_DCTS) {
            if (msr & MSR_CTS) {
                psiochan->ier |= TxFIFO_BIT;                            /* CTS was turned on            */

            } else {
                psiochan->ier &= (~TxFIFO_BIT);                         /* CTS was turned off           */
            }
        }
        SET_REG(psiochan, IER, psiochan->ier);                          /*  update ier                  */
        LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);
        break;

    default:
        SET_REG(psiochan, IER, psiochan->ier);                          /*  update ier                  */
        LW_SPIN_UNLOCK_QUICK(&psiochan->slock, intreg);
        break;
    }
}

//#endif                                                                  /*  LW_CFG_DEVICE_EN            */
                                                                        /*  LW_CFG_SIO_DEVICE_EN        */
                                                                        /*  LW_CFG_DRV_SIO_16C550       */
/*********************************************************************************************************
  16C550 SIO ���ö���
*********************************************************************************************************/
typedef struct {
       ULONG        CFG_ulBase;
       ULONG        CFG_ulXtal;
       ULONG        CFG_ulBaud;
       ULONG        CFG_ulVector;
       ULONG        CFG_ulRegShift;
} SIO16C550_CFG;
/*********************************************************************************************************
  Qemu SylixOS ƽ̨ 16C550 SIO ���ó�ʼ��
*********************************************************************************************************/
SIO16C550_CFG      _G_sio16C550Cfgs[BSP_CFG_16C550_SIO_NR] = {
#ifdef BSP_CFG_16C550_0_BASE
    {
        BSP_CFG_16C550_0_BASE,
        BSP_CFG_16C550_0_XTAL,
        BSP_CFG_16C550_0_BAUD,
        BSP_CFG_16C550_0_VECTOR,
        BSP_CFG_16C550_0_REG_SHIFT,
    },
#endif
#ifdef BSP_CFG_16C550_1_BASE
    {
        BSP_CFG_16C550_1_BASE,
        BSP_CFG_16C550_1_XTAL,
        BSP_CFG_16C550_1_BAUD,
        BSP_CFG_16C550_1_VECTOR,
        BSP_CFG_16C550_1_REG_SHIFT,
    }
#endif
};
/*********************************************************************************************************
  16C550 SIO ͨ��
*********************************************************************************************************/
static SIO16C550_CHAN   _G_sio16C550Chans[BSP_CFG_16C550_SIO_NR];
/*********************************************************************************************************
** ��������: __sio16C550SetReg
** ��������: ���� 16C550 �Ĵ���
** �䡡��  : pSio16C550Chan        16C550 SIO ͨ��
**           iReg                  �Ĵ���
**           ucValue               ֵ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  __sio16C550SetReg (SIO16C550_CHAN  *pSio16C550Chan, INT  iReg, UINT8  ucValue)
{
    SIO16C550_CFG   *pSio16C550Cfg = pSio16C550Chan->priv;

    write8(ucValue, pSio16C550Cfg->CFG_ulBase + (iReg << pSio16C550Cfg->CFG_ulRegShift));
}
/*********************************************************************************************************
** ��������: __sio16C550GetReg
** ��������: ��� 16C550 �Ĵ�����ֵ
** �䡡��  : pSio16C550Chan        16C550 SIO ͨ��
**           iReg                  �Ĵ���
** �䡡��  : �Ĵ�����ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT8  __sio16C550GetReg (SIO16C550_CHAN  *pSio16C550Chan, INT  iReg)
{
    SIO16C550_CFG   *pSio16C550Cfg = pSio16C550Chan->priv;

    return  (read8(pSio16C550Cfg->CFG_ulBase + (iReg << pSio16C550Cfg->CFG_ulRegShift)));
}
/*********************************************************************************************************
** ��������: __sio16C550Isr
** ��������: 16C550 �жϷ������
** �䡡��  : pSio16C550Chan        16C550 SIO ͨ��
**           ulVector              �ж�������
** �䡡��  : �жϷ���ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static irqreturn_t  __sio16C550Isr (SIO16C550_CHAN  *pSio16C550Chan, ULONG  ulVector)
{
    sio16c550Isr(pSio16C550Chan);

    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
** ��������: sioChan16C550Create
** ��������: ����һ�� SIO ͨ��
** �䡡��  : uiChannel                 Ӳ��ͨ����
** �䡡��  : SIO ͨ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
SIO_CHAN  *sioChan16C550Create (UINT  uiChannel)
{
    SIO16C550_CHAN          *pSio16C550Chan;
    SIO16C550_CFG           *pSio16C550Cfg;

    if (uiChannel < BSP_CFG_16C550_SIO_NR) {

        pSio16C550Chan = &_G_sio16C550Chans[uiChannel];
        pSio16C550Cfg  = &_G_sio16C550Cfgs[uiChannel];

        /*
         *  Receiver FIFO Trigger Level and Tirgger bytes table
         *  level  16 Bytes FIFO Trigger   32 Bytes FIFO Trigger  64 Bytes FIFO Trigger
         *    0              1                       8                    1
         *    1              4                      16                   16
         *    2              8                      24                   32
         *    3             14                      28                   56
         */
        pSio16C550Chan->fifo_len         = 16;
        pSio16C550Chan->rx_trigger_level = 3;

        pSio16C550Chan->baud   = pSio16C550Cfg->CFG_ulBaud;
        pSio16C550Chan->xtal   = pSio16C550Cfg->CFG_ulXtal;
        pSio16C550Chan->setreg = __sio16C550SetReg;
        pSio16C550Chan->getreg = __sio16C550GetReg;

        pSio16C550Chan->priv   = pSio16C550Cfg;

        API_InterVectorDisable(pSio16C550Cfg->CFG_ulVector);

        sio16c550Init(pSio16C550Chan);

        API_InterVectorConnect(pSio16C550Cfg->CFG_ulVector,
                               (PINT_SVR_ROUTINE)__sio16C550Isr,
                               (PVOID)pSio16C550Chan,
                               "16c550_isr");                           /*  ��װ����ϵͳ�ж�������      */

        API_InterVectorEnable(pSio16C550Cfg->CFG_ulVector);

        return  ((SIO_CHAN *)pSio16C550Chan);
    } else {
        return  (LW_NULL);
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/




