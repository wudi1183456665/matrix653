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
** ��   ��   ��: flexcan.c
**
** ��   ��   ��: Hu.Hanzhang (����)
**
** �ļ���������: 2016 �� 06 �� 07 ��
**
** ��        ��: imx6q flexcan ����������
DEBUG:
2017.1.10:�޸���һ�����͹���ᵼ�¶�֡������
2017.1.12:���������ߴ����޷��ָ��������Լ���Ч�жϵ�����
2018.1.10:���� can ���ò�����
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include <linux/compat.h>

#include "irq/irq_numbers.h"
#include "flexcan/flexcanRegs.h"
#include "ccm_pll/ccm_pll.h"
/*********************************************************************************************************
  ͨ�ú궨��
*********************************************************************************************************/
#define FLEXCAN_DEBUG 0

#if     FLEXCAN_DEBUG > 0
#define can_debug(fmt, arg...)  		  _DebugFormat(__PRINTMESSAGE_LEVEL, fmt, ##arg)
#else
#define can_debug(fmt, arg...)
#endif

#define CAN_CHAN_NUMS                     2

#ifndef offsetof
#define offsetof(TYPE, MEMBER)            ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member)     ({\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);\
        (type *)( (char *)__mptr - offsetof(type, member) );})
#endif

#define CAN_READ(x)      readl((addr_t)(pChannel->CANCH_atVirtAddrBase + x))
#define CAN_WRITE(x, y)  writel(y, pChannel->CANCH_atVirtAddrBase + (addr_t)x)

#define FLEXCAN_DEF_MODE                  (3)
#define FLEXCAN_RETRY_MAX                 (1000)

#define __FLEXCAN1_BASE                   (0x2090000)
#define __FLEXCAN2_BASE                   (0x2094000)

#define FLEXCAN_LOCK_PREDO()               INTREG  intregLevel
#define FLEXCAN_LOCK(ch)                   LW_SPIN_LOCK_QUICK(&(ch)->CANCH_slLock, &intregLevel)
#define FLEXCAN_UNLOCK(ch)                 LW_SPIN_UNLOCK_QUICK(&(ch)->CANCH_slLock, intregLevel)
/*********************************************************************************************************
  Ĭ��ʱ���������
*********************************************************************************************************/
#define FLEFLEXCAN_DFT_BAUD                500000
#define FLEXCAN_DFT_propseg                0x6
#define FLEXCAN_DFT_tseg1                  0x6
#define FLEXCAN_DFT_tseg2                  0x2
#define FLEXCAN_DFT_sjw                    0x1
#define FLEXCAN_DFT_brp                    FLEXCAN_BRP(FLEFLEXCAN_DFT_BAUD)
#define FLEXCAN_DFT_TIME                   (FLEXCAN_DFT_propseg +FLEXCAN_DFT_sjw + FLEXCAN_DFT_tseg1 + \
                                            FLEXCAN_DFT_tseg2)
#define FLEXCAN_BRP(baud)                  __canBrpCalc(pChannel, baud)
/*********************************************************************************************************
  CAN ͨ�����ƿ����Ͷ���
*********************************************************************************************************/
typedef struct flexcan_can_chan {
    addr_t       CANCH_atPhyAddrBase;                                   /*  �����ַ����ַ              */
    ULONG        CANCH_ulVector;                                        /*  �ж�����                    */
    addr_t       CANCH_atVirtAddrBase;                                  /*  �����ַ����ַ              */
    CAN_CHAN     CANCH_canchan;

    UINT         CAHCH_uiChannel;

    INT        (*CANCH_pcbSetBusState)();                               /*  ����״̬�ص�                */
    INT        (*CANCH_pcbGetTx)();                                     /*  �жϻص�����                */
    INT        (*CANCH_pcbPutRcv)();

    PVOID        CANCH_pvGetTxArg;                                      /*  �ص���������                */
    PVOID        CANCH_pvPutRcvArg;
    PVOID        CANCH_pvSetBusStateArg;

    spinlock_t   CANCH_slLock;

    UINT         CANCH_uiInputClock;
    UINT8        CANCH_ucBrp;                                           /* �����ʷ�Ƶϵ��               */
    UINT8        CANCH_ucSjw;                                           /* ͬ���γ���                   */
    UINT8        CANCH_ucTseg1;                                         /* ��λ�����1                  */
    UINT8        CANCH_ucTseg2;                                         /* ��λ�����2                  */
    UINT8        CANCH_ucPropseg;                                       /* ����ʱ���                   */

    UINT32       CANCH_uiInts;

} FLEXCAN_CHAN;
/*********************************************************************************************************
  CAN ͨ�����ƿ����鶨��
*********************************************************************************************************/
static FLEXCAN_CHAN  _G_flexcanChannels[CAN_CHAN_NUMS] = {
    {
        __FLEXCAN1_BASE,
        IMX_INT_FLEXCAN1,
    },
    {
        __FLEXCAN2_BASE,
        IMX_INT_FLEXCAN2,
    },
};
/*********************************************************************************************************
  ǰ������
*********************************************************************************************************/
static INT __flexcanChanDataInit(UINT32 uiChannel);
static INT __flexcanChanHwInit(FLEXCAN_CHAN *pChannel);
static UINT __flexcanBittimingSet(FLEXCAN_CHAN  *pChannel,
                                  UINT32         uiPropSeg,
                                  UINT32         uiPhaseSeg1,
                                  UINT32         uiPhaseSeg2,
                                  UINT32         uiBrp,
                                  UINT32         uiSjw);
static INT __flexcanIoctl(CAN_CHAN          *pCanchan,
                          INT                iCmd,
                          LONG               lArg);
static INT __flexcanCbInstall(CAN_CHAN      *pCanchan,
                              INT            iCallbackType,
                              CAN_CALLBACK   callbackRoute,
                              PVOID          pvCallbackArg);
static INT __flexcanRecv(FLEXCAN_CHAN *pChannel, CAN_FRAME *pCanframe);
static INT __flexcanSend(FLEXCAN_CHAN *pChannel, CAN_FRAME *pCanFrame);
static UINT8 __canBrpCalc(FLEXCAN_CHAN *pChannel, UINT uiBaud);
static irqreturn_t __flexcanIrq(PVOID pvArg, ULONG ulVector);
static INT __flexcanStartup (CAN_CHAN *pCanchan);
extern VOID flexcan1_iomux_config(VOID);
extern VOID flexcan2_iomux_config(VOID);
/*********************************************************************************************************
  CAN ��������
*********************************************************************************************************/
static CAN_DRV_FUNCS    _G_flexcanDrvFuncs = {
             (INT (*)(CAN_CHAN *, INT, PVOID))__flexcanIoctl,
             __flexcanStartup,
             __flexcanCbInstall
};
/*********************************************************************************************************
** ��������: flexcanChanCreate
** ��������: ���� CAN ������ͨ��
** ��  ��  : uiChannel ͨ����
** ��  ��  : CAN_CHAN CAN�����ṹ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
CAN_CHAN  *flexcanChanCreate (UINT32 uiChannel)
{
    FLEXCAN_CHAN *pChannel;
    INT           iRet;
    FLEXCAN_LOCK_PREDO();

    iRet = __flexcanChanDataInit(uiChannel);
    if (iRet != ERROR_NONE) {
        return  ((CAN_CHAN *)LW_NULL);
    }

    pChannel = &_G_flexcanChannels[uiChannel];
    FLEXCAN_LOCK(pChannel);
    iRet = __flexcanChanHwInit(pChannel);
    FLEXCAN_UNLOCK(pChannel);
    if (iRet != ERROR_NONE) {
        return  ((CAN_CHAN *)LW_NULL);
    }

    return  (&pChannel->CANCH_canchan);
}
/*********************************************************************************************************
** ��������: __flexcanChanDataInit
** ��������: ��ʼ��ͨ������(��Դ)���
** ��  ��  : uiChannel ͨ����
** ��  ��  : error number
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __flexcanChanDataInit (UINT32 uiChannel)
{
    FLEXCAN_CHAN  *pChannel;
    addr_t         atVirtBase;
    ULONG          ulVector;
    UINT           uiInputClock;
    INT            iRet;

    static CPCHAR  pcCanIsrName[] = {"can_isr0", "can_isr1"};

    if (uiChannel >= CAN_CHAN_NUMS) {
        can_debug("channel invalid.\n");
        return  (PX_ERROR);
    }

    pChannel   = &_G_flexcanChannels[uiChannel];
    atVirtBase = (addr_t)API_VmmIoRemapNocache((PVOID)pChannel->CANCH_atPhyAddrBase,
                                                LW_CFG_VMM_PAGE_SIZE);
    if (!atVirtBase) {
        can_debug("remap can%d memory failed.\n", uiChannel);
        return  (PX_ERROR);
    }

    imx6qClkGateSet(__FLEXCAN1_BASE, CLOCK_ON);
    imx6qClkGateSet(__FLEXCAN2_BASE, CLOCK_ON);

    flexcan1_iomux_config();
    flexcan2_iomux_config();

    uiInputClock = imx6qPeriClkGet(CAN_CLK);

    pChannel->CANCH_atVirtAddrBase    = atVirtBase;
    pChannel->CANCH_uiInputClock      = uiInputClock;
    pChannel->CAHCH_uiChannel         = uiChannel;
    pChannel->CANCH_canchan.pDrvFuncs = &_G_flexcanDrvFuncs;
    pChannel->CANCH_ucSjw             = FLEXCAN_DFT_sjw;
    pChannel->CANCH_ucTseg1           = FLEXCAN_DFT_tseg1;
    pChannel->CANCH_ucTseg2           = FLEXCAN_DFT_tseg2;
    pChannel->CANCH_ucPropseg         = FLEXCAN_DFT_propseg;
    pChannel->CANCH_ucBrp             = FLEXCAN_DFT_brp;

    LW_SPIN_INIT(&pChannel->CANCH_slLock);

    ulVector = pChannel->CANCH_ulVector;
    iRet = (INT)API_InterVectorConnect(ulVector, __flexcanIrq, pChannel, pcCanIsrName[uiChannel]);
    if (iRet != ERROR_NONE) {
        API_VmmIoUnmap((PVOID)atVirtBase);
        can_debug("interrupt vector connect error.\n");
        return  (PX_ERROR);
    }

    can_debug("input clock = %d\n", uiInputClock);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __flexcanChanHwInit
** ��������: Ӳ����ʼ��
** ��  ��  : pChannel    ͨ������
** ��  ��  : error number
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __flexcanChanHwInit (FLEXCAN_CHAN *pChannel)
{
    UINT32 uiValue;

    uiValue = CAN_READ(FLEXCAN_MCR);
    uiValue &= ~FLEXCAN_MCR_MDIS;
    CAN_WRITE(FLEXCAN_MCR, uiValue);
    udelay(10);

    uiValue = CAN_READ(FLEXCAN_MCR);
    uiValue |= FLEXCAN_MCR_SOFT_RST;
    CAN_WRITE(FLEXCAN_MCR, uiValue);
    udelay(10);

    uiValue = CAN_READ(FLEXCAN_MCR);
    if (uiValue & FLEXCAN_MCR_SOFT_RST) {
        can_debug("[flexcanInit] Fail to reset FLEXCAN controller\n");
        return  (PX_ERROR);
    }

    if (__flexcanBittimingSet (pChannel, pChannel->CANCH_ucPropseg, pChannel->CANCH_ucTseg1,
            pChannel->CANCH_ucTseg2, pChannel->CANCH_ucBrp, pChannel->CANCH_ucSjw) != ERROR_NONE) {
        return  (PX_ERROR);
    }

   uiValue = CAN_READ(FLEXCAN_MCR);
   uiValue &= ~FLEXCAN_MCR_MAXMB_MSK;
   uiValue |= FLEXCAN_MCR_FRZ | FLEXCAN_MCR_RFEN | \
              FLEXCAN_MCR_HALT |  FLEXCAN_MCR_SUPV | \
              FLEXCAN_MCR_WRN_EN | FLEXCAN_MCR_IDAM_C | \
              FLEXCAN_MCR_WAK_MSK | FLEXCAN_MCR_SLF_WAK | \
              FLEXCAN_MCR_SRX_DIS | FLEXCAN_MCR_MAXMB(FLEXCAN_TX_BUF_ID);
   CAN_WRITE(FLEXCAN_MCR, uiValue);

   uiValue  = CAN_READ(FLEXCAN_CTRL1);
   uiValue &= ~FLEXCAN_CTRL1_TSYN;
   uiValue |= FLEXCAN_CTRL1_BOFF_REC | FLEXCAN_CTRL1_LBUF;
   CAN_WRITE(FLEXCAN_CTRL1, uiValue);

   CAN_WRITE(FLEXCAN_MB_CTRL(FLEXCAN_TX_BUF_ID), FLEXCAN_MB_CNT_CODE(0x4));

   CAN_WRITE(FLEXCAN_RXMGMASK, 0x0);
   CAN_WRITE(FLEXCAN_RX14MASK, 0x0);
   CAN_WRITE(FLEXCAN_RX15MASK, 0x0);

   CAN_WRITE(FLEXCAN_RXFGMASK, 0x0);

   uiValue = CAN_READ(FLEXCAN_MCR);
   uiValue &= ~FLEXCAN_MCR_HALT;
   CAN_WRITE(FLEXCAN_MCR, uiValue);

   CAN_WRITE(FLEXCAN_IMASK1, FLEXCAN_IFLAG_DEFAULT);

   return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __canBrpCalc
** ��������: ������Ҫ�Ĳ����ʼ���BRPֵ
** ��  ��  : pChannel         ͨ������
**           uiBaud           ������
** ��  ��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT8 __canBrpCalc (FLEXCAN_CHAN *pChannel, UINT uiBaud)
{
    UINT8 ucBrp;

    uiBaud *= FLEXCAN_DFT_TIME;
    ucBrp   = (pChannel->CANCH_uiInputClock + uiBaud - 1) / uiBaud;

    return  (ucBrp);
}
/*********************************************************************************************************
** ��������: __flexcanBittimingSet
** ��������: CAN          ����������
** ��  ��  : pChannel     ͨ������
**           uiPropSeg    ʱ�䴫������ʵֵ��δ��1��
**           uiPhaseSeg1  ��λ�����1��ʵֵ��δ��1��
**           uiPhaseSeg2  ��λ�����2��ʵֵ��δ��1��
**           uiBrp        ��Ƶϵ����ʵֵ��δ��1��
**           uiSjw        ͬ������ʵֵ��δ��1��
** ��  ��  : error number
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT __flexcanBittimingSet (FLEXCAN_CHAN *pChannel,
                                   UINT32        uiPropSeg,
                                   UINT32        uiPhaseSeg1,
                                   UINT32        uiPhaseSeg2,
                                   UINT32        uiBrp,
                                   UINT32        uiSjw)
{
    UINT32 uiValue = CAN_READ(FLEXCAN_CTRL1);

    uiValue &= ~(FLEXCAN_CTRL1_PRESDIV(0xff) | \
    		     FLEXCAN_CTRL1_RJW(0x3) | \
    		     FLEXCAN_CTRL1_PSEG1(0x7) | \
    		     FLEXCAN_CTRL1_PSEG2(0x7) | \
    		     FLEXCAN_CTRL1_PROP_SEG(0x7) | \
    		     FLEXCAN_CTRL1_LPB | \
    		     FLEXCAN_CTRL1_SMP | \
    		     FLEXCAN_CTRL1_LOM);

    uiValue |= FLEXCAN_CTRL1_PRESDIV(uiBrp - 1) | \
    		   FLEXCAN_CTRL1_PSEG1(uiPhaseSeg1 - 1) | \
    		   FLEXCAN_CTRL1_PSEG2(uiPhaseSeg2 - 1) | \
    		   FLEXCAN_CTRL1_RJW(uiSjw - 1) | \
    		   FLEXCAN_CTRL1_PROP_SEG(uiPropSeg - 1);

    CAN_WRITE(FLEXCAN_CTRL1, uiValue);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __flexcanIoctl
** ��������: CAN IO ����
** ��  ��  : pCanchan    ͨ������
**           iCmd        ��������
**           lArg        ���Ʋ���
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __flexcanIoctl (CAN_CHAN *pCanchan,INT iCmd, LONG lArg)
{
    INT32         i;
    UINT32        uiMcr;
	FLEXCAN_CHAN *pChannel = container_of(pCanchan, FLEXCAN_CHAN, CANCH_canchan);

    FLEXCAN_LOCK_PREDO();

    switch (iCmd) {
    case CAN_DEV_OPEN:                                                  /*  �� CAN �豸               */
        API_InterVectorEnable(pChannel->CANCH_ulVector);
        break;

    case CAN_DEV_CLOSE:
        API_InterVectorDisable(pChannel->CANCH_ulVector);
        break;

    case CAN_DEV_SET_BAUD:                                              /*  ���ò�����                  */
        switch (lArg) {
        case 1000000:
        case 900000:
        case 800000:
        case 700000:
        case 600000:
        case 666000:
        case 500000:
        case 400000:
        case 250000:
        case 200000:
        case 125000:
        case 100000:
        case 80000:
        case 50000:
        case 40000:
        case 30000:
        case 20000:
        case 10000:
        case 5000:
            uiMcr = CAN_READ(FLEXCAN_MCR);                              /*  ����HALTģʽ                */
            uiMcr |= FLEXCAN_MCR_HALT;
            CAN_WRITE(FLEXCAN_MCR, uiMcr);

            for (i = 10; i > 0 ; i--) {
                if (CAN_READ(FLEXCAN_MCR) & FLEXCAN_MCR_FRZ_ACK) {
                    pChannel->CANCH_ucBrp = FLEXCAN_BRP(lArg);
                    __flexcanBittimingSet(pChannel,
                                          pChannel->CANCH_ucPropseg,
                                          pChannel->CANCH_ucTseg1,
                                          pChannel->CANCH_ucTseg2,
                                          pChannel->CANCH_ucBrp,
                                          pChannel->CANCH_ucSjw);
                    break;
                }
                usleep(10);
            }

            uiMcr = CAN_READ(FLEXCAN_MCR);
            uiMcr &= ~FLEXCAN_MCR_HALT;
            CAN_WRITE(FLEXCAN_MCR, uiMcr);                              /*  �˳�HALTģʽ                */
        break;

        default:
            _ErrorHandle(EINVAL);
            return  (PX_ERROR);
        }
        break;

    case CAN_DEV_REST_CONTROLLER:
        FLEXCAN_LOCK(pChannel);
        pChannel->CANCH_pcbSetBusState(pChannel->CANCH_pvSetBusStateArg, CAN_DEV_BUS_ERROR_NONE);
        __flexcanChanHwInit(pChannel);
        FLEXCAN_UNLOCK(pChannel);
        break;

    case CAN_DEV_STARTUP:
    	__flexcanStartup(pCanchan);
        break;

    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __flexcanStartup
** ��������: �������ݷ���
** ��  ��  : pCanchan    ͨ������
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __flexcanStartup (CAN_CHAN  *pCanchan)
{
    FLEXCAN_CHAN   *pChannel = container_of(pCanchan, FLEXCAN_CHAN, CANCH_canchan);
    CAN_FRAME       canFrame;
    INT             iCount;

    if (!pChannel->CANCH_pcbGetTx) {
        return  (PX_ERROR);
    }

    while (pChannel->CANCH_pcbGetTx(pChannel->CANCH_pvGetTxArg,         /* �ӷ��ͻ������ж�ȡ���ݷ���   */
                                   &canFrame) == ERROR_NONE) {
        __flexcanSend(pChannel, &canFrame);
        iCount = FLEXCAN_RETRY_MAX;
        while (((CAN_READ(FLEXCAN_MB_CTRL(FLEXCAN_TX_BUF_ID)) & FLEXCAN_MB_CNT_CODE(0xF)) !=
                 FLEXCAN_MB_CNT_CODE(0x8)) &&
                 iCount) {
            iCount--;
            usleep(100);
        }

        if (iCount == 0) {
            can_debug("[CAN]%s:%d send message time out\r\n", __func__, __LINE__);
            pChannel->CANCH_pcbSetBusState(pChannel->CANCH_pvSetBusStateArg, CAN_DEV_BUS_OVERRUN);
            return  (PX_ERROR);
        }
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __flexcanCbInstall
** ��������: ��ʼ��ͨ���������
** ��  ��  : pCanchan         ͨ������
**           iCallbackType    ��Ҫ��װ�Ļص���������
**           callbackRoute    �ص�����ָ��
**           pvCallbackArg    �ص���������
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __flexcanCbInstall (CAN_CHAN     *pCanchan,
                               INT           iCallbackType,
                               CAN_CALLBACK  callbackRoute,
                               PVOID         pvCallbackArg)
{
    FLEXCAN_CHAN *pChannel = container_of(pCanchan, FLEXCAN_CHAN, CANCH_canchan);

    switch (iCallbackType) {
    case CAN_CALLBACK_GET_TX_DATA:                                      /*  ���ͻص�����                */
        pChannel->CANCH_pcbGetTx   = callbackRoute;
        pChannel->CANCH_pvGetTxArg = pvCallbackArg;
        return  (ERROR_NONE);

    case CAN_CALLBACK_PUT_RCV_DATA:                                     /*  ���ջص�����                */
        pChannel->CANCH_pcbPutRcv   = callbackRoute;
        pChannel->CANCH_pvPutRcvArg = pvCallbackArg;
        return  (ERROR_NONE);                                           /*  ����״̬����                */

    case CAN_CALLBACK_PUT_BUS_STATE:
        pChannel->CANCH_pcbSetBusState   = callbackRoute;
        pChannel->CANCH_pvSetBusStateArg = pvCallbackArg;
        return  (ERROR_NONE);

    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __flexCanRecv
** ��������: can ���ݽ���
** ��  ��  : pChannel         ͨ������
**           pCanframe        CAN����֡��ַ
** ��  ��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __flexcanRecv (FLEXCAN_CHAN *pChannel, CAN_FRAME *pCanframe)
{
    UINT32 uiRegCtrl, uiRegId, uiValue;

    uiRegCtrl = CAN_READ(FLEXCAN_MB_CTRL(0));
    uiRegId   = CAN_READ(FLEXCAN_MB_ID(0));

    if (uiRegCtrl & FLEXCAN_MB_CNT_IDE) {
        pCanframe->CAN_bExtId = LW_TRUE;
        pCanframe->CAN_uiId = uiRegId & CAN_EFF_MASK;
    } else {
        pCanframe->CAN_uiId = (uiRegId >> 18) & CAN_SFF_MASK;
    }

    if (uiRegCtrl & FLEXCAN_MB_CNT_RTR) {
        pCanframe->CAN_bRtr = LW_TRUE;
    }

    pCanframe->CAN_ucLen = (uiRegCtrl >> 16) & 0xf;

    uiValue = CAN_READ(FLEXCAN_MB_DATA0(0));
    pCanframe->CAN_ucData[0] = uiValue >> 24;
    pCanframe->CAN_ucData[1] = uiValue >> 16;
    pCanframe->CAN_ucData[2] = uiValue >> 8;
    pCanframe->CAN_ucData[3] = uiValue;

    uiValue = CAN_READ(FLEXCAN_MB_DATA1(0));
    pCanframe->CAN_ucData[4] = uiValue >> 24;
    pCanframe->CAN_ucData[5] = uiValue >> 16;
    pCanframe->CAN_ucData[6] = uiValue >> 8;
    pCanframe->CAN_ucData[7] = uiValue;

    CAN_WRITE(FLEXCAN_IFLAG1, FLEXCAN_IFLAG_RX_FIFO_AVAILABLE);

    can_debug("[flexcanRecv] id 0x%x, extended %s, rtr %s, lengh 0x%x\n",
               pCanframe->CAN_uiId,
              (pCanframe->CAN_bExtId) ? "ture":"false",
              (pCanframe->CAN_bRtr) ? "ture":"false",
               pCanframe->CAN_ucLen);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __flexCanSend
** ��������: can ���ݷ���
** ��  ��  : pChannel         ͨ������
**           pCanframe        Ҫ���͵�����֡��ַ
** ��  ��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __flexcanSend (FLEXCAN_CHAN *pChannel, CAN_FRAME *pCanFrame)
{
    UINT32  uiCanId;

    can_debug("start sending msg!\n");
    UINT uiCtrl = FLEXCAN_MB_CNT_CODE(0xc) | (pCanFrame->CAN_ucLen << 16);

    if (pCanFrame->CAN_bExtId) {                                        /* �������չ֡                 */
        uiCanId = pCanFrame->CAN_uiId & CAN_EFF_MASK;
        uiCtrl |= FLEXCAN_MB_CNT_IDE | FLEXCAN_MB_CNT_SRR;
    } else {
        uiCanId = (pCanFrame->CAN_uiId & CAN_SFF_MASK) << 18;
    }

    if (pCanFrame->CAN_bRtr) {                                          /* �����Զ��֡                 */
        uiCtrl |= FLEXCAN_MB_CNT_RTR;
    };

    if (pCanFrame->CAN_ucLen > 0) {
        UINT uiData;
        uiData  = pCanFrame->CAN_ucData[0] << 24;
        uiData += pCanFrame->CAN_ucData[1] << 16;
        uiData += pCanFrame->CAN_ucData[2] << 8;
        uiData += pCanFrame->CAN_ucData[3] << 0;
        CAN_WRITE(FLEXCAN_MB_DATA0(FLEXCAN_TX_BUF_ID), uiData);
    }
    if (pCanFrame->CAN_ucLen > 3) {
        UINT uiData;
        uiData  = pCanFrame->CAN_ucData[4] << 24;
        uiData += pCanFrame->CAN_ucData[5] << 16;
        uiData += pCanFrame->CAN_ucData[6] << 8;
        uiData += pCanFrame->CAN_ucData[7] << 0;
        CAN_WRITE(FLEXCAN_MB_DATA1(FLEXCAN_TX_BUF_ID), uiData);
    }

    CAN_WRITE(FLEXCAN_MB_ID(FLEXCAN_TX_BUF_ID), uiCanId);
    CAN_WRITE(FLEXCAN_MB_CTRL(FLEXCAN_TX_BUF_ID), uiCtrl);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __flexcanIrq
** ��������: can �жϷ������
** ��  ��  : pChannel         ͨ������
**           ulVector         �ж�������
** ��  ��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static irqreturn_t __flexcanIrq (PVOID pvArg, ULONG ulVector)
{
    FLEXCAN_CHAN  *pChannel;
    UINT32         uiIflag1, uiEsr, uiValue;
    CAN_FRAME      canframe;

    can_debug("[CAN]irq\r\n");

    pChannel = (FLEXCAN_CHAN *)pvArg;
    uiIflag1 = CAN_READ(FLEXCAN_IFLAG1);
    uiEsr    = CAN_READ(FLEXCAN_ESR1);

    uiValue = CAN_READ(FLEXCAN_CTRL1);
    uiValue &= ~(FLEXCAN_CTRL1_ERR_ALL);
    CAN_WRITE(FLEXCAN_CTRL1, uiValue);
    CAN_WRITE(FLEXCAN_IMASK1, 0);

    if (uiEsr & FLEXCAN_ESR1_ERR_ALL) {
        can_debug("There is something wrong!\n");
        CAN_READ(FLEXCAN_ESR1);
        CAN_WRITE(FLEXCAN_ESR1, FLEXCAN_ESR1_ERR_ALL);
    }

    if (uiEsr & FLEXCAN_ESR1_RX) {
        if (uiIflag1 & FLEXCAN_IFLAG_RX_FIFO_AVAILABLE) {
            CAN_WRITE(FLEXCAN_IMASK1, FLEXCAN_IFLAG_DEFAULT & ~FLEXCAN_IFLAG_RX_FIFO_AVAILABLE);
            memset(&canframe, 0, sizeof(CAN_FRAME));
            __flexcanRecv(pChannel, &canframe);
            if (pChannel->CANCH_pcbPutRcv(pChannel->CANCH_pvPutRcvArg, &canframe) != ERROR_NONE) {
                pChannel->CANCH_pcbSetBusState(pChannel->CANCH_pvSetBusStateArg,
                                               CAN_DEV_BUS_RXBUFF_OVERRUN);
            }
        }
    }

    if (uiIflag1 & FLEXCAN_IFLAG_RX_FIFO_OVERFLOW) {
        can_debug("FIFO overflow\n");
        CAN_WRITE(FLEXCAN_IFLAG1, FLEXCAN_IFLAG_RX_FIFO_OVERFLOW);
    }

    CAN_WRITE(FLEXCAN_IMASK1, FLEXCAN_IFLAG_DEFAULT);
    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
