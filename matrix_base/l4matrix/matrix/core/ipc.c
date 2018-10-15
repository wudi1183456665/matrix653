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
** ��   ��   ��: ipc.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2017 �� 12 �� 19 ��
**
** ��        ��: ���̼�ͨ��.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
  �ж� IPC_ES ״̬�Ƿ�����
  ���� IPC_ES_T ����
*********************************************************************************************************/
#define IPC_ES_TRUE(ipcEs)           (ipcEs == IPC_ES_OK)
#define IPC_ES_FALSE(ipcEs)          (ipcEs != IPC_ES_OK)
/*********************************************************************************************************
** ��������: __ipcArgTargetSet
** ��������: ���� IPC ��������
** �䡡��  : pIpc      IPC �ṹָ��
             pipcTag   ���ض���
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcArgTargetSet (PMX_IPC  pIpc, PMX_IPC  pipcTag)
{
    MX_SC_KOBJ_SET(MX_SCPARAM_GET(pIpc), IPC_VCPU_TO_OBJ(IPC_TO_VCPU(pipcTag)));
}
/*********************************************************************************************************
** ��������: __ipcErrorSet
** ��������: ���� IPC ������
** �䡡��  : pIpc      IPC �ṹָ��
             ucErrCode ������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcErrorSet (PMX_IPC  pIpc, MX_SC_RESULT_T  ucErrCode)
{
    MX_SC_RESULT_SET(MX_SCPARAM_GET(pIpc), ucErrCode);
}
/*********************************************************************************************************
** ��������: __ipcStateAdd
** ��������: ���� IPC ״̬
** �䡡��  : pIpc      IPC �ṹָ��
** ��  ��  : uiAddBits �����ӵ�״̬λ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcStateAdd (PMX_IPC  pIpc, UINT32  uiAddBits)
{
    pIpc->IPC_uiState |= uiAddBits;
}
/*********************************************************************************************************
** ��������: __ipcStateDel
** ��������: ɾ�� IPC ״̬
** �䡡��  : pIpc      IPC �ṹָ��
** ��  ��  : uiDelBits ��ɾ����״̬λ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcStateDel (PMX_IPC  pIpc, UINT32  uiDelBits)
{
    pIpc->IPC_uiState &= ~uiDelBits;
}
/*********************************************************************************************************
** ��������: __ipcStateSet
** ��������: ���� IPC ״̬
** �䡡��  : pIpc      IPC �ṹָ��
** ��  ��  : uiDelBits �������״̬λ
** ��  ��  : uiAddBits �����ӵ�״̬λ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcStateSet (PMX_IPC  pIpc, UINT32  uiDelBits, UINT32  uiAddBits)
{
    pIpc->IPC_uiState &= ~uiDelBits;
    pIpc->IPC_uiState |= uiAddBits;
}
/*********************************************************************************************************
** ��������: __ipcStateFind
** ��������: ���� IPC ״̬
** �䡡��  : pIpc      IPC �ṹָ��
** ��  ��  : uiBits    �����ҵ�״̬λ
** �䡡��  : ���ز��ҵ���λ; ����ֵ: =0  û��ָ��״̬��==uiBits ����ָ����״̬
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE UINT32  __ipcStateFind (PMX_IPC  pIpc, UINT32  uiBits)
{
    return  (pIpc->IPC_uiState & uiBits);
}
/*********************************************************************************************************
** ��������: __ipcRmStateChange
** ��������: remote state ���
** �䡡��  : pIpc      IPC �ṹָ��
** ��  ��  : uiDelBits �������״̬λ
** ��  ��  : uiAddBits �����ӵ�״̬λ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcRmStateChange (PMX_IPC  pIpc, UINT32  uiDelBits, UINT32  uiAddBits)
{
    pIpc->IPC_uiRmStateDel |= uiDelBits;
    pIpc->IPC_uiRmStateAdd &= ~uiDelBits;
    pIpc->IPC_uiRmStateAdd |= uiAddBits;
}
/*********************************************************************************************************
** ��������: __ipcRmStateClean
** ��������: remote state ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcRmStateClean (PMX_IPC  pIpc)
{
    pIpc->IPC_uiRmStateDel = 0;
    pIpc->IPC_uiRmStateAdd = 0;
}
/*********************************************************************************************************
** ��������: __ipcStateUpdate
** ��������: �� remote state ���µ�����״̬
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcStateUpdate (PMX_IPC  pIpc)
{
    __ipcStateSet(pIpc, pIpc->IPC_uiRmStateDel, pIpc->IPC_uiRmStateAdd);
    __ipcRmStateClean(pIpc);
}
/*********************************************************************************************************
** ��������: __ipcPartnerSet
** ��������: ���� IPC Ŀ�� IPC ����
** �䡡��  : pIpc      IPC �ṹָ��
**           pipcTag   Ŀ�� IPC ָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcTargetSet (PMX_IPC  pIpc, PMX_IPC  pipcTag)
{
    pIpc->IPC_pipcTag = pipcTag;
}
/*********************************************************************************************************
** ��������: __ipcPartnerGet
** ��������: ��ȡ IPC Ŀ�� IPC ����
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : Ŀ�� IPC ָ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE PMX_IPC  __ipcTargetGet (PMX_IPC  pIpc)
{
    return  (pIpc->IPC_pipcTag);
}
/*********************************************************************************************************
** ��������: __ipcTargetClean
** ��������: ��Ŀ�� VCPU ָ��
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcTargetClean (PMX_IPC  pIpc)
{
    pIpc->IPC_pipcTag = MX_NULL;
}
/*********************************************************************************************************
** ��������: __ipcReadySet
** ��������: ���� IPC �����е� VCPU ����
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcReadySet (PMX_IPC  pIpc)
{
    __ipcStateAdd(pIpc, IPC_TS_READY);
}
/*********************************************************************************************************
** ��������: __ipcTargetClean
** ��������: ���� IPC �����е� VCPU �Ǿ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcReadyClean (PMX_IPC  pIpc)
{
    __ipcStateDel(pIpc, IPC_TS_READY);
}
/*********************************************************************************************************
** ��������: ipcReadyTestSafe
** ��������: ��ȡ IPC_TS_READY ״̬�������̰߳�ȫģʽ
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : BOOL: TRUE - VCPU ������FALSE - VCPU δ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  ipcReadyTestSafe (PMX_IPC  pIpc)
{
    UINT            uiState;
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);
    uiState = __ipcStateFind(pIpc, IPC_TS_READY);
    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);

    return  (uiState == IPC_TS_READY);
}
/*********************************************************************************************************
** ��������: ipcReadyTryTestSetSafe
** ��������: ���Բ���λ IPC_TS_READY ״̬, �̰߳�ȫģʽ; ����������(�������жϹرյ�״̬�±�����)
** �䡡��  : pIpc        IPC �ṹָ��
**           pbIpcReady  �������������� IPC �Ƿ����̬��TRUE - ����
** �䡡��  : FALSE       ����,����ʧ��
**           TRUE        �����ɹ�
** ȫ�ֱ���:
** ����ģ��: ���� tick ����������
*********************************************************************************************************/
BOOL  ipcReadyTryTestSetSafe (PMX_IPC  pIpc, BOOL  *pbIpcReady)
{
    UINT uiState;                                                       /*  IPC ����״̬                */

    if (!spinTryLock(&pIpc->IPC_spStateLock)) {                         /*  ��������                    */
        return  (MX_FALSE);
    }

    uiState = __ipcStateFind(pIpc, IPC_TS_READY);
    if (uiState != IPC_TS_READY) {
        __ipcReadySet(pIpc);                                            /*  �þ���̬, �����ٴλ���      */
    }
    spinUnlock(&pIpc->IPC_spStateLock);

    *pbIpcReady = (uiState == IPC_TS_READY) ? MX_TRUE : MX_FALSE;
    return  (MX_TRUE);
}
/*********************************************************************************************************
** ��������: __ipcReadyTestSetSf
** ��������: ���Բ���λ IPC_TS_READY ״̬���̰߳�ȫģʽ; ��������
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : ״̬Ϊ�Ǿ���������Ϊ����̬������FALSE; �������� TRUE
** ȫ�ֱ���:
** ����ģ��: IPC �������ڲ�����
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcReadyTestSetSf (PMX_IPC  pIpc)
{
    UINT            uiState;                                            /*  IPC ����״̬                */
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);

    uiState = __ipcStateFind(pIpc, IPC_TS_READY);
    if (uiState != IPC_TS_READY) {
        __ipcReadySet(pIpc);                                            /*  �þ���̬, �����ٴλ���      */
    }

    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);
    return  (uiState == IPC_TS_READY);
}
/*********************************************************************************************************
** ��������: __ipcServiceTestSf
** ��������: ���� IPC �Ƿ��ڷ���״̬(�ɽ��� IPC ����)
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : BOOL: TRUE - ����״̬��FALSE - �Ƿ���״̬
** ȫ�ֱ���:
** ����ģ��: __ipcSendHandle
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcServiceTestSf (PMX_IPC  pIpc)
{
    UINT            uiState;
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);
    uiState = __ipcStateFind(pIpc, IPC_TS_SER);
    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);

    return  (uiState == IPC_TS_SER);
}
/*********************************************************************************************************
** ���÷���״̬
*********************************************************************************************************/
#define IPC_MSG_WRITEABLE_SET(pipcMsg, bWrite)  do {            \
            (pipcMsg->IMSG_uiAccess &= ~0x1);                   \
            (pipcMsg->IMSG_uiAccess |= (bWrite ? 0x1 : 0));     \
        } while (0)
/*********************************************************************************************************
** ��������: ipcMsgReadOpen
** ��������: ����ֻ��״̬
** �䡡��  : pipcMsg
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ipcMsgReadOpen (PMX_IPC_MSG  pipcMsg)
{
    pipcMsg->IMSG_uiOffset = 0;
    IPC_MSG_WRITEABLE_SET(pipcMsg, MX_FALSE);
}
/*********************************************************************************************************
** ��������: ipcMsgWriteOpen
** ��������: ����ֻд״̬
** �䡡��  : pipcMsg
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ipcMsgWriteOpen (PMX_IPC_MSG  pipcMsg)
{
    pipcMsg->IMSG_uiOffset = 0;
    pipcMsg->IMSG_uiCount  = 0;
    IPC_MSG_WRITEABLE_SET(pipcMsg, MX_TRUE);
}
/*********************************************************************************************************
** ��������: ipcMsgFlush
** ��������: �ύ����, д��������ô˲���
** �䡡��  : pipcMsg
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ipcMsgFlush (PMX_IPC_MSG  pipcMsg)
{
    IPC_MSG_WRITEABLE_SET(pipcMsg, MX_FALSE);                           /*  ����ֻ��״̬                */
    pipcMsg->IMSG_uiCount  = pipcMsg->IMSG_uiOffset;                    /*  ���α�ֵ���µ���Ϣ����      */
    pipcMsg->IMSG_uiOffset = 0;                                         /*  �α�� 0                    */
}
/*********************************************************************************************************
** ��������: ipcMsgUlWrite
** ��������: д������
** �䡡��  : pipcMsg
**           ulData       ��д����
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  ipcMsgUlWrite (PMX_IPC_MSG  pipcMsg, ULONG  ulData)
{
    if (IPC_MSG_WRITEABLE(pipcMsg)) {
        pipcMsg->IMSG_ulBuff[pipcMsg->IMSG_uiOffset++] = ulData;

        if (pipcMsg->IMSG_uiOffset >= IPC_MSG_ITEMS) {
            ipcMsgFlush(pipcMsg);
        }
        return  (MX_TRUE);
    }

    return  (MX_FALSE);
}
/*********************************************************************************************************
** ��������: ipcMsgUlRead
** ��������: ��������
** �䡡��  : pipcMsg
**           pulData       ��¼��������
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  ipcMsgUlRead (PMX_IPC_MSG  pipcMsg, ULONG  *pulData)
{
    if (!IPC_MSG_WRITEABLE(pipcMsg)) {

        if (pipcMsg->IMSG_uiOffset >= pipcMsg->IMSG_uiCount) {
            ipcMsgWriteOpen(pipcMsg);
            return  (MX_FALSE);
        }

        *pulData = pipcMsg->IMSG_ulBuff[pipcMsg->IMSG_uiOffset++];
        return  (MX_TRUE);
    }

    return  (MX_FALSE);
}
/*********************************************************************************************************
** ��������: __ipcMsgCopy
** ��������: IPC ��Ϣ����
** �䡡��  : pmsgSrc      Դ��Ϣ�ṹָ��
** �䡡��  : pmsgTag      Ŀ����Ϣ�ṹָ��
** �䡡��  : TRUE - �����ɹ�; FALSE - ����ʧ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcMsgCopy (PMX_IPC_MSG  pmsgSrc, PMX_IPC_MSG  pmsgTag)
{
    UINT32  index;

    if (!pmsgSrc || !pmsgTag || (pmsgSrc->IMSG_uiCount >= IPC_MSG_ITEMS)) {
        K_ERROR(" msg_count=%d, IPC_MSG_ITEMS=%d", pmsgSrc->IMSG_uiCount, IPC_MSG_ITEMS);
        return  (MX_FALSE);
    }

    for (index = 0; index < pmsgSrc->IMSG_uiCount; index++) {
        pmsgTag->IMSG_ulBuff[index] = pmsgSrc->IMSG_ulBuff[index];
    }

    pmsgTag->IMSG_uiCount = pmsgSrc->IMSG_uiCount;
    pmsgTag->IMSG_uiFlags = pmsgSrc->IMSG_uiFlags;

    return  (MX_TRUE);
}
/*********************************************************************************************************
** ��������: __ipcEvtqInit
** ��������: IPC �¼����г�ʼ��
** �䡡��  : pevtqQueue   �¼�����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_INLINE VOID  __ipcEvtqInit (PIPC_EVTQ  pevtqQueue)
{
    pevtqQueue->IEQ_pringHead = MX_NULL;
}
/*********************************************************************************************************
** ��������: __ipcEvtqInsert
** ��������: �� һ�� IPC ���������һ�� IPC �����¼�����
** �䡡��  : pipcTag       �ṩ�¼����е� IPC
**           pipcInsert    ��������е� IPC
**           uiIpcEvent    IPC �¼���
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcEvtqInsert (PMX_IPC  pipcTag,
                                 PMX_IPC  pipcInsert,
                                 UINT32   uiIpcEvent)
{
    if (_LIST_RING_IS_NOTLNK(&pipcInsert->IPC_ringEvtqNode)) {         /*  �жϽڵ��Ƿ��Ѿ��������     */
        _List_Ring_Add_Last(&pipcInsert->IPC_ringEvtqNode,             /*  ���¼������м���ڵ�         */
                            &(pipcTag->IPC_evtqQueue.IEQ_pringHead));
    }

    __ipcStateAdd(pipcInsert, uiIpcEvent);                             /*  �����¼�״̬                 */
}
/*********************************************************************************************************
** ��������: __ipcEvtqNext
** ��������: ���¼�������ȡ��һ���ڵ�
** �䡡��  : pIpc          �ṩ�¼����е� IPC
** �䡡��  : ���¼�������ȡ�õ����Ƚڵ������ IPC ����
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_LOCAL PMX_IPC  __ipcEvtqNext (PMX_IPC  pIpc)
{
    PMX_LIST_RING  pringNext = pIpc->IPC_evtqQueue.IEQ_pringHead;

    if (pringNext != MX_NULL) {                                         /*  ��ǰ�¼����зǿ�            */
        _List_Ring_Del(pringNext,
                       (&pIpc->IPC_evtqQueue.IEQ_pringHead));           /*  ɾ����ͷ�ڵ�                */
        //K_DEBUG("get a event");
        return  (_LIST_ENTRY(pringNext, MX_IPC, IPC_ringEvtqNode));
    }

    return  (MX_NULL);
}
/*********************************************************************************************************
** ��������: ipcTickTimeoutPro
** ��������: tick �Գ�ʱ�̵߳Ĵ��� (�������жϹرյ�״̬�±�����)
** �䡡��  : pIpc    ��ʱ�� IPC
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��: tick
*********************************************************************************************************/
VOID  ipcTickTimeoutPro (PMX_IPC  pIpc)
{
    PMX_IPC     pipcRecver = MX_NULL;

    __ipcRmStateChange(pIpc, 0, IPC_ES_TIMEOUT);                        /*  ����Ϊ��ʱ״̬              */

    IPC_ASSERT((pipcRecver = pIpc->IPC_pipcTag) != MX_NULL);
    spinLockIgnIrq(&pipcRecver->IPC_evtqQueue.IEQ_spLock);
    __ipcEvtqInsert(pipcRecver, pIpc, IPC_EVTS_TIMEOUT);                /*  ���볬ʱ�¼�                */
    spinUnlockIgnIrq(&pipcRecver->IPC_evtqQueue.IEQ_spLock);
}
/*********************************************************************************************************
** ��������: __ipcPlistInit
** ��������: IPC ���ȼ������б��ʼ��
** �䡡��  : piplList     ���ȼ������б�ָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcPlistInit (PIPC_PLIST  piplList)
{
    UINT32  index;

    for ( index = 0; index < MX_CFG_IPC_PRIO_MAX; index++ ) {
        piplList->IPL_pringHead[index] = MX_NULL;
    }

    piplList->IPL_uiPrioCur  = MX_CFG_IPC_PRIO_MAX - 1;
    piplList->IPL_uiSignal   = 0;
}
/*********************************************************************************************************
** ��������: __ipcPlistPrioFlush
** ��������: �������ȼ������б��е�ǰ������ȼ�
** �䡡��  : piplList     ���ȼ������б�ָ��
** �䡡��  : TRUE - �б�Ϊ��; FLASE - �б�Ϊ��
** ȫ�ֱ���:
** ����ģ��: PRIO_LIST �ڲ�����
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcPlistPrioFlush (PIPC_PLIST  piplList)
{
    UINT32      index;
    UINT32      uiPrioCur = piplList->IPL_uiPrioCur;                    /*  ��ǰ������ȼ�              */

    IPC_ASSERT(uiPrioCur < MX_CFG_IPC_PRIO_MAX);

    if (piplList->IPL_pringHead[uiPrioCur] != MX_NULL) {                /*  ��ǰ���ȼ�����ǿ�          */
        return  (MX_TRUE);
    }

    for (index = ++uiPrioCur; index < MX_CFG_IPC_PRIO_MAX; index++) {   /*  �����ȼ��ͷ�������          */
        if (piplList->IPL_pringHead[index] != MX_NULL) {
            piplList->IPL_uiPrioCur = index;
            return  (MX_TRUE);
        }
    }

    for (index = 0; index < uiPrioCur; index++) {                       /*  ������ȼ��ͷ���δ������    */
                                                                        /*  ��Ч�����ٴ�����ͷ��ʼ����*/
        if (piplList->IPL_pringHead[index] != MX_NULL) {
            piplList->IPL_uiPrioCur = index;
            return  (MX_TRUE);
        }
    }

    piplList->IPL_uiPrioCur = MX_CFG_IPC_PRIO_MAX - 1;                  /*  ��ǰ�б�Ϊ��                */
    return  (MX_FALSE);
}
/*********************************************************************************************************
** ��������: __ipcPlistInsert
** ��������: �� һ�� IPC ���������һ�� IPC �������ȼ������
** �䡡��  : pipcTag       �ṩ���ȼ���� IPC
** �䡡��  : uiPrio        pipcInsert �������ȼ�
** �䡡��  : pipcInsert    �������� IPC
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcPlistInsert (PMX_IPC pipcTag,  UINT32 uiPrio,
                                  PMX_IPC pipcInsert)
{
    IPC_ASSERT(uiPrio < MX_CFG_IPC_PRIO_MAX);

    _List_Ring_Add_Last(&pipcInsert->IPC_ringPlistNode,                 /*  ����Ӧ���ȼ��ı��м���ڵ�  */
        &(pipcTag->IPC_plistPrio.IPL_pringHead[uiPrio]));

    /*
     *  �¼���ڵ����ȼ����ڵ�ǰ������ȼ������µ�ǰ��¼
     */
    if (uiPrio < pipcTag->IPC_plistPrio.IPL_uiPrioCur) {
        pipcTag->IPC_plistPrio.IPL_uiPrioCur = uiPrio;
    }

    //K_DEBUG("uiPrioCur=%d", uiPrio);
}
/*********************************************************************************************************
** ��������: __ipcPlistInList
** ��������: �ж� IPC �Ƿ��ڱ���
** �䡡��  : pipc     ������ IPC
** �䡡��  : BOOL     TRUE - �ڱ���
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_INLINE BOOL  __ipcPlistInList (PMX_IPC  pIpc)
{
    return  (!_LIST_RING_IS_NOTLNK(&pIpc->IPC_ringPlistNode));
}
/*********************************************************************************************************
** ��������: __ipcPlistRemove
** ��������: �� һ�� IPC �������һ�� IPC �������ȼ���������Ƴ�
** �䡡��  : pipcTag       �ṩ���ȼ���� IPC
** �䡡��  : uiPrio        pipcRemove �������ȼ�
** �䡡��  : pipcInsert    ���Ƴ��� IPC
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcPlistRemove (PMX_IPC  pipcTag, UINT32 uiPrio, PMX_IPC  pipcRemove)
{
    IPC_ASSERT(uiPrio < MX_CFG_IPC_PRIO_MAX);

    _List_Ring_Del(&pipcRemove->IPC_ringPlistNode,                      /*  ����Ӧ���ȼ��ı���ɾ���ڵ�  */
                   &(pipcTag->IPC_plistPrio.IPL_pringHead[uiPrio]));

    __ipcPlistPrioFlush(&pipcTag->IPC_plistPrio);                       /*  �������ȼ���ǰ������ȼ�  */
}
/*********************************************************************************************************
** ��������: __ipcPlistNext
** ��������: Ѱ�����ȼ���ߵĽڵ㲢�ӱ����Ƴ�
** �䡡��  : pIpc          �ṩ���ȼ���� IPC
** �䡡��  : �����ȼ��б���ȡ�õ�������ȼ��ڵ������ IPC ����
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_LOCAL PMX_IPC  __ipcPlistNext (PMX_IPC  pIpc)
{
    PIPC_PLIST           piplList  = &pIpc->IPC_plistPrio;
    UINT32               uiPrioCur = piplList->IPL_uiPrioCur;           /*  ��ǰ������ȼ�              */

    PMX_LIST_RING        pringNext = MX_NULL;

    IPC_ASSERT(uiPrioCur < MX_CFG_IPC_PRIO_MAX);
    //K_DEBUG("uiPrioCur=%d", uiPrioCur);

    pringNext  = piplList->IPL_pringHead[uiPrioCur];                    /*  ��ȡ�����ͷ�ڵ�Ϊ��ѡ�ڵ�  */

    if (pringNext != MX_NULL) {                                         /*  ��ǰ���ȼ�����ǿ�          */
        _List_Ring_Del(pringNext,
                       (&piplList->IPL_pringHead[uiPrioCur]));          /*  ɾ����ͷ�ڵ�                */
        __ipcPlistPrioFlush(piplList);                                  /*  �������ȼ��б�ǰ���ȼ�    */
        return  (_LIST_ENTRY(pringNext, MX_IPC, IPC_ringPlistNode));
    }

    return  (MX_NULL);
}
/*********************************************************************************************************
** ��������: __ipcPlistSignalUpSf
** ��������: �źż����ۼ�
** �䡡��  : pIpc          �ṩ���ȼ���� IPC
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_INLINE VOID  __ipcPlistSignalAddSf (PMX_IPC  pIpc)
{
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);
    pIpc->IPC_plistPrio.IPL_uiSignal++;
    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);
}
/*********************************************************************************************************
** ��������: __ipcPlistSignalDelSf
** ��������: �źż����ۼ�
** �䡡��  : pIpc          �ṩ���ȼ���� IPC
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��: IPC ģ��ӿں���
*********************************************************************************************************/
IPC_INLINE VOID  __ipcPlistSignalDelSf (PMX_IPC  pIpc)
{
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);
    pIpc->IPC_plistPrio.IPL_uiSignal--;
    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);
}
/**********************************************************************************************************
** ��������: ipcInit
** ��������: IPC ��ʼ���ӿڣ�����VCPUʱ���г�ʼ��
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ipcInit (PMX_IPC  pIpc)
{
    pIpc->IPC_pMsg = kobjGet(kobjAlloc(_KOBJ_IPC_MSG));                 /*  ���� IPC_MSG                */
#if 0
    IPC_ASSERT(pIpc->IPC_pMsg);
#endif
    __ipcStateDel(pIpc, IPC_S_ALL_MASK);                                /*  ״̬���                    */
    __ipcRmStateClean(pIpc);                                            /*  �첽״̬���                */

    _LIST_RING_INIT_NODE_PTR(&pIpc->IPC_ringEvtqNode);                  /*  �¼����н���ʼ��          */
    __ipcEvtqInit(&pIpc->IPC_evtqQueue);                                /*  �¼����г�ʼ��              */

    _LIST_RING_INIT_NODE_PTR(&pIpc->IPC_ringPlistNode);                 /*  ���ȼ���������ʼ��      */
    __ipcPlistInit(&pIpc->IPC_plistPrio);                               /*  ���ȼ�������ʼ��          */

    spinInit(&pIpc->IPC_spStateLock);                                   /*  ��ʼ��������                */
    spinInit(&pIpc->IPC_spRmStateLock);
    spinInit(&pIpc->IPC_evtqQueue.IEQ_spLock);
    spinInit(&pIpc->IPC_plistPrio.IPL_spLock);
}
/*********************************************************************************************************
  IPC ����
              +---> SEND --!R--> SEND_WT ----> SEND_FI -----------------+
              |       |             |                                   |
      ENTRY --+       +-RECV[O]--+  +---------RECV[C]---------+         +----> RETURN
              |                  |                            |         |
              +---> RECV ------> RECV_WT ----> RECV_IN -----> RECV_FI --+

*********************************************************************************************************/
typedef   IPC_ES_T (*IPC_HANDLE) (PMX_IPC);

IPC_LOCAL IPC_ES_T __ipcSendHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcSendWaitHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcRecvHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcRecvWaitHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcRecvInHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcSendFiHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcRecvFiHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcCloseHandle (PMX_IPC pIpc);
/*********************************************************************************************************
** ��������: __ipcStateCheckPro
** ��������: ��� IPC ״̬�������쳣�������
** �䡡��  : pIpc          IPC �ṹָ��
**           uiIpcState    IPC ��ǰ״̬
** �䡡��  : IPC_ES_T      �쳣��; IPC_ES_OK - �����ɹ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_ES_T  __ipcStateCheckPro (PMX_IPC  pIpc, UINT32  uiIpcStage)
{
    IPC_ES_T    ucResult    = IPC_ES_OK;
    PMX_IPC     pipcTag     = MX_NULL;                                  /*  Ŀ�� IPC                    */
    UINT32      uiIpcState;                                             /*  IPC ״̬                    */

    /*
     *  ����쳣״̬
     */
    ucResult = __ipcStateFind(pIpc, IPC_ES_MASK);
    if (IPC_ES_FALSE(ucResult)) {
        K_ERROR(" ES=0x%x",
                (pIpc->IPC_uiState & IPC_ES_MASK));
        return  (ucResult);
    }

    /*
     *  ��� IPC ״̬
     */
    switch (uiIpcStage) {

    case IPC_S_SEND:
        if (!__ipcServiceTestSf(pIpc->IPC_pipcTag)) {                   /*  Ԥ���Ŀ�� IPC �Ƿ������� */
                                                                        /*  �ڼ��� IPC ����ʱ���ٴμ�� */
            K_ERROR("SEND, no service");
            __ipcStateAdd(pIpc, IPC_ES_ERROR);
            __ipcErrorSet(pIpc, MX_SC_RESULT_NOT_SER);
            ucResult = IPC_ES_ERROR;
        }
        break;

    case IPC_S_SEND | IPC_S_REPLY:
        break;

    case IPC_S_REPLY:
        pipcTag = __ipcTargetGet(pIpc);
        if (pipcTag == MX_NULL                      ||
            __ipcTargetGet(pipcTag) != pIpc         ||                  /*  target ���                 */
            __ipcStateFind(pipcTag, IPC_S_MASK) !=
             (IPC_S_SEND_WT | IPC_S_RECV) )           {                 /*  target IPC Ϊ�ȴ�����״̬   */
            K_ERROR("REPLY");
            ucResult = IPC_ES_ERROR;
        }
        break;

    case IPC_S_SEND_WT:
        uiIpcState  = __ipcStateFind(pIpc, IPC_S_NONE);
        //K_DEBUG(":SEND_WT, ipcState=0x%x", uiIpcState);
        if (uiIpcState != IPC_S_SEND_FI &&                              /*  IPC״̬: IPC_S_SEND_FI ��   */
            uiIpcState != IPC_S_RECV_FI)  {                             /*  IPC_S_RECV_FI ״̬          */
            __ipcStateAdd(pIpc, IPC_ES_ERROR);
            __ipcErrorSet(pIpc, MX_SC_RESULT_STATE);
            ucResult = IPC_ES_ERROR;
        }
        break;

    case IPC_S_RECV:
    case IPC_S_RECV_WT:
    case IPC_S_RECV_IN:
    case IPC_S_RECV_FI:
    default:
        break;
    }
    return  (ucResult);
}
/*********************************************************************************************************
** ��������: __ipcErrorPro
** ��������: IPC ������
** �䡡��  : pIpc          IPC �ṹָ��
**           uiIpcState    IPC ��ǰ״̬
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  __ipcErrorPro (PMX_IPC  pIpc, UINT32  uiIpcStage)
{
    K_ERROR("__ipcErrorPro");

    __ipcStateDel(pIpc, IPC_S_IPC_MASK);
    __ipcTargetClean(pIpc);
    return;
}
/*********************************************************************************************************
** ��������: __ipcSendPro
** ��������: IPC �����ӳ���, ������Ϣ
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendPro (PMX_IPC  pIpc)
{
    PMX_IPC     pipcTag      = pIpc->IPC_pipcTag;
    PMX_VCPU    pvcpuTag     = IPC_TO_VCPU(pipcTag);
    PIPC_EVTQ   pevtqTag     = &pipcTag->IPC_evtqQueue;
    ULONG       ulTicks      = 0;

    INTREG      iregIntLevel;

    //K_DEBUG(__ipcSendPro);
    /*
     *  NOTE: IPC_TS_SER ״̬ͬ��
     */
#if 1
    if (__ipcStateFind(pipcTag, IPC_TS_SER) != IPC_TS_SER) {            /*  Ŀ���̲߳��� IPC ����״̬   */
        K_WARNING(" server not in serivce");
        __ipcStateAdd(pIpc, IPC_ES_ERROR);
        __ipcErrorSet(pIpc, MX_SC_RESULT_NOT_SER);
        return  (IPC_ES_ERROR);
    }
#endif

    __ipcStateSet(pIpc, IPC_S_SEND, IPC_S_SEND_WT);                     /*  IPC ������ SEND_WT ״̬     */
    __ipcReadyClean(pIpc);                                              /*  IPC ������Ǿ���״̬        */
    scTimeToTicks(MX_SC_TIME_GET(MX_SCPARAM_GET(pIpc)), &ulTicks);

    spinLockAndDisIrq(&pevtqTag->IEQ_spLock, &iregIntLevel);            /*  ���¼�������                */
    __ipcEvtqInsert(pipcTag, pIpc, IPC_EVTS_REQUEST);                   /*  ���� IPC �����¼�           */
    spinUnlockAndEnIrq(&pevtqTag->IEQ_spLock, iregIntLevel);            /*  �ͷ��¼�������              */

    iregIntLevel = kernelEnterAndDisIrq();                              /*  ���ں���                    */

    __ipcPlistSignalAddSf(pipcTag);                                     /*  ��Ϣ�����ۼ�                */
                                                                        /*  ��service ֮������Ϣ��ͬ��  */

    if (__ipcStateFind(pipcTag, IPC_TS_SER_PEND) == IPC_TS_SER_PEND) {
         __ipcStateSet(pipcTag, IPC_TS_SER_PEND, 0);                    /*  �����������״̬            */
         K_NOTE(" wakeup vcpu=0x%x", pvcpuTag);
         vcpuWakeup(pvcpuTag);                                          /*  ֮ǰ�� IPC �������ѽ��շ� */
    }

    vcpuSleep(ulTicks);                                                 /*  ���ͷ�����, ���µ���ʱ����  */
    IPC_SET_IN_PROCESS(pIpc, MX_TRUE);                                  /*  IPC ����                    */
    K_WARNING(" SLEEP=0x%x, tick=%d", IPC_TO_VCPU(pIpc), ulTicks);

    kernelExitAndEnIrq(iregIntLevel);                                   /*  ������:�ͷ��ں���, ִ�е��� */

    return  (IPC_ES_OK);
}
/*********************************************************************************************************
** ��������: __ipcSendReplyPro
** ��������: IPC �����ӳ��򣬻ظ���Ϣ
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendReplyPro (PMX_IPC  pIpc)
{
    PMX_IPC     pipcTag   = __ipcTargetGet(pIpc);
    PMX_VCPU    pvcpuTag  = IPC_TO_VCPU(pipcTag);

    IPC_ASSERT(pvcpuTag != MX_NULL);
    //K_DEBUG("start, pvcpuTag=0x%x", pvcpuTag);
    /*
     *  �����̳߳�ʱ����ʱ�¼��������¼�����
     *  δ��ʱ�����������ظ� IPC ����
     */
    if (!__ipcReadyTestSetSf(pipcTag)) {                                /*  δ��ʱ����                  */

        if (IPC_ES_FALSE(__ipcStateCheckPro(pIpc, IPC_S_REPLY))) {      /*  ��� IPC ״̬�Ϸ���         */
            K_ERROR(" target ERROR");
            __ipcRmStateChange(pipcTag, IPC_S_NULL, IPC_ES_ERROR);      /*  ����Ŀ�� IPC ����           */

        } else if (!__ipcMsgCopy(pIpc->IPC_pMsg, pipcTag->IPC_pMsg)) {  /*  IPC ��Ϣ����                */
            /*
             *  NOTE: ��¼��־�����ϼ��������ڴ���������ǲ��ɻָ�
             */
            K_ERROR(" __ipcMsgCopy ERROR");
            __ipcRmStateChange(pipcTag, IPC_S_NULL, IPC_ES_ERROR);      /*  ����Ŀ�� IPC ����           */

        } else {
            __ipcRmStateChange(pipcTag, IPC_S_RECV_WT, IPC_S_RECV_FI);  /*  ����Ŀ�� IPC �������̬     */
        }

        K_NOTE("__ipcSendReplyPro: wakeup vcpu=0x%x", pvcpuTag);
        vcpuWakeup(pvcpuTag);                                           /*  ����Ŀ�� VCPU; ִ�е���     */
    }

    __ipcTargetClean(pIpc);                                             /*  ���Ŀ�� IPC                */
    __ipcStateDel(pIpc, IPC_S_SEND | IPC_S_REPLY);                      /*  IPC �������״̬            */

    if (__ipcStateFind(pIpc, IPC_S_RECV) == IPC_S_RECV) {               /*  �ж��Ƿ��н��ղ���        */
        __ipcStateSet(pIpc, IPC_S_RECV, IPC_S_RECV_WT);                 /*  IPC ���ý��յȴ�״̬        */

    } else {
        __ipcStateAdd(pIpc, IPC_S_SEND_FI);                             /*  IPC ���ý������״̬        */
    }
    return  (IPC_ES_OK);
}
/*********************************************************************************************************
** ��������: __ipcSendHandle
** ��������: IPC ���ͽ׶��ӳ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendHandle (PMX_IPC  pIpc)
{
    IPC_ES_T    uiResult    = IPC_ES_OK;
    UINT32      uiIpcStage;                                             /*  IPC �׶�                    */
    UINT32      uiIpcState;

    K_TITLE(__ipcSendHandle);

    if (__ipcStateFind(pIpc, IPC_S_REPLY) == IPC_S_REPLY) {             /*  IPC �ظ���Ϣ����            */
        uiIpcStage = IPC_S_SEND | IPC_S_REPLY;
        uiResult   = __ipcStateCheckPro(pIpc, uiIpcStage);

        if (IPC_ES_FALSE(uiResult)) {                                   /*  ��� IPC ״̬�Ϸ���         */
            K_ERROR("TAG1, result=%x", uiResult);
            __ipcErrorPro (pIpc, IPC_S_SEND);                           /*  ������                    */
            return  (uiResult);
        }
        /*
         *  IPC �����߳�����״̬��ȷ��, ����Ԥ�ȼ����״̬
         *  __ipcSendReplyPro ����Ҫ������� IPC ״̬
         */
        uiIpcStage = IPC_S_REPLY;
        uiResult = __ipcSendReplyPro(pIpc);

    } else {                                                            /*  IPC ������Ϣ����            */
        uiIpcStage = IPC_S_SEND;
        uiResult   = __ipcStateCheckPro(pIpc, uiIpcStage);

        if (IPC_ES_FALSE(uiResult)) {                                   /*  ��� IPC ״̬�Ϸ���         */
            K_ERROR("TAG2, result=%x", uiResult);
            __ipcErrorPro (pIpc, IPC_S_SEND);                           /*  ������                    */
            return  (uiResult);
        }

        uiResult = __ipcSendPro(pIpc);
        //K_DEBUG("__ipcSendPro RESULT=%x", uiResult);
    }

    if (IPC_ES_FALSE(uiResult)) {
        K_ERROR("TAG3, result=%x", uiResult);
         __ipcErrorPro (pIpc, uiIpcStage);                              /*  ������                    */
         return  (uiResult);
     }

    uiIpcState = __ipcStateFind(pIpc, IPC_S_NONE);
    switch (uiIpcState) {
    case IPC_S_SEND_WT:
    case IPC_S_SEND_WT | IPC_S_RECV:
        uiResult = __ipcSendWaitHandle(pIpc);                           /*  ���� IPC_SEND_WT �׶�       */
        break;

    case IPC_S_RECV_WT:
        uiResult = __ipcRecvWaitHandle(pIpc);                           /*  ���� IPC_RECV_WT �׶�       */
        break;

    case IPC_S_RECV_FI:
        uiResult = __ipcRecvFiHandle(pIpc);                             /*  ���� IPC_RECV_FI �׶�       */
        break;

    default:
        IPC_ASSERT(MX_FALSE);
        __ipcErrorSet(pIpc, MX_SC_RESULT_FAIL);
        K_ERROR("TAG4, result=%d", uiResult);
        __ipcErrorPro (pIpc, uiIpcStage);
        uiResult = IPC_ES_ERROR;
    }
    return  (uiResult);
}
/*********************************************************************************************************
** ��������: __ipcSendWaitHandle
** ��������: IPC ���͵ȴ��׶��ӳ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendWaitHandle (PMX_IPC  pIpc)
{
    IPC_ES_T uiResult    = IPC_ES_OK;

    K_TITLE(__ipcSendWaitHandle);

    /*
     *  δ����̬, �����ȴ�; ���ܵ����: sender sleep ��δ�����߳�
     */
    while (!ipcReadyTestSafe(pIpc)) {
        K_NOTE(" RE_SCHED");
        kernelSched();
    }
    K_WARNING(" WAKEUP=0x%x", IPC_TO_VCPU(pIpc));
    __ipcStateUpdate(pIpc);                                             /*  IPC ���ѣ����±���״̬      */
    /*
     *  �����ʱ�ӻ��ѣ����ѱ�ʶ TIMEOUT �쳣
     */
    uiResult = __ipcStateCheckPro(pIpc, IPC_S_SEND_WT);                 /*  ��� IPC ״̬�Ϸ���         */
    if (uiResult == IPC_ES_TIMEOUT) {
        K_WARNING(" TIMEOUT");

    } else if (IPC_ES_FALSE(uiResult)) {
        K_ERROR(" uiResult=%x", uiResult);
        __ipcErrorPro (pIpc, IPC_S_SEND_WT);                            /*  ͨ�ô�����                */
        return  (uiResult);
    }

    if (__ipcStateFind(pIpc, IPC_S_SEND_FI) == IPC_S_SEND_FI) {         /*  ���� IPC_SEND_FI �׶�       */
        uiResult = __ipcSendFiHandle(pIpc);

    } else {                                                            /*  ���� IPC_RECV_FI �׶�       */
        uiResult = __ipcRecvFiHandle(pIpc);
    }
    return  (uiResult);
}
/*********************************************************************************************************
** ��������: __ipcRecvHandle
** ��������: IPC ���ս׶��ӳ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcRecvHandle (PMX_IPC  pIpc)
{
    INTREG          iregIntLevel;

    K_TITLE(__ipcRecvHandle);
    if (IPC_ES_FALSE(__ipcStateCheckPro(pIpc, IPC_S_RECV))) {           /*  ��� IPC ״̬�Ϸ���         */
        K_ERROR(__ipcRecvHandle);
        __ipcErrorPro (pIpc, IPC_S_RECV);                               /*  �쳣����                    */
        return  (IPC_ES_ERROR);
    }

    spinLockAndDisIrq(&pIpc->IPC_plistPrio.IPL_spLock, &iregIntLevel);
    __ipcStateAdd(pIpc, IPC_TS_SER);                                    /*  ���� IPC ����ģʽ           */
    spinUnlockAndEnIrq(&pIpc->IPC_plistPrio.IPL_spLock, iregIntLevel);

    __ipcStateSet(pIpc, IPC_S_RECV, IPC_S_RECV_WT);                     /*  ����Ϊ IPC_RECV_WT ״̬     */
    return  (__ipcRecvWaitHandle(pIpc));                                /*  ���� IPC_RECV_WT �׶�       */
}
/*********************************************************************************************************
** ��������: __ipcRecvEventPro
** ��������: ���� IPC �첽�¼�
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : pIpcEvt   �����¼��� IPC
** ȫ�ֱ���:
** ����ģ��: __ipcRecvWaitHandle
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcRecvEventPro (PMX_IPC  pIpc, PMX_IPC  pIpcEvt)
{
    UINT32 uiRequest = __ipcStateFind(pIpcEvt,
                                     IPC_EVTS_REQUEST |
                                     IPC_EVTS_TIMEOUT);
    UINT32 uiEvtPrio = 0;

    //K_DEBUG("start");
    vcpuGetPriority(IPC_TO_VCPU(pIpcEvt), &uiEvtPrio);

    if (uiRequest == IPC_EVTS_REQUEST) {                                /*  ֻ�� IPC �����¼�           */
        __ipcPlistInsert(pIpc, uiEvtPrio, pIpcEvt);                     /*  IPC ������շ����ȼ������  */
        __ipcRmStateChange(pIpcEvt, IPC_EVTS_MASK, IPC_S_NULL);         /*  ����¼�                    */

    } else if (uiRequest) {                                             /*  IPC ��ʱ�¼�                */
        if (__ipcPlistInList(pIpcEvt)) {                                /*  �����ʱ IPC �Ѽ��������   */
            __ipcPlistRemove(pIpc, uiEvtPrio, pIpcEvt);                 /*  ����ʱ IPC ��������Ƴ�     */
            __ipcPlistSignalDelSf(pIpc);                                /*  �źż����ۼ�                */
        }

        __ipcRmStateChange(pIpcEvt, IPC_EVTS_MASK, IPC_S_NULL);         /*  ����¼�                    */

        K_NOTE(" wakeup timeout vcpu=0x%x", IPC_TO_VCPU(pIpcEvt));
        vcpuWakeup(IPC_TO_VCPU(pIpcEvt));                               /*  ���ѳ�ʱ�߳�;�䳬ʱ״̬     */
                                                                        /*  �ɴ����䳬ʱ���߳�����      */
    } else {                                                            /*  �����¼��ݲ�����            */
        IPC_ASSERT(FALSE);
    }
}
/*********************************************************************************************************
** ��������: __ipcRecvNextPro
** ��������: �� IPC ���ȼ��б���ȡ�����ȼ���ߵ� IPC
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : PMX_IPC   NULL - �б�Ϊ��
** ȫ�ֱ���:
** ����ģ��: __ipcRecvWaitHandle
*********************************************************************************************************/
IPC_LOCAL PMX_IPC  __ipcRecvNextPro (PMX_IPC  pIpc)
{
    PMX_IPC  pNext = __ipcPlistNext (pIpc);                             /*  ���б���ȡ��һ����������    */
#if 0
    pNext = pNext ? pNext : pIpc->IPC_pipcTag;                          /*  NOTE: ֻ�е���IPC����ʱ     */
#endif                                                                  /*  ֱ���� ipcTarget            */

    if (pNext) {
        __ipcPlistSignalDelSf(pIpc);                                    /*  �źż����ۼ�                */
    }
    //K_DEBUG("next_vcpu=0x%x", IPC_TO_VCPU(pNext));

    return  (pNext);
}
/*********************************************************************************************************
** ��������: __ipcRecvWaitHandle
** ��������: IPC ���յȴ��׶��ӳ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcRecvWaitHandle (PMX_IPC  pIpc)
{
    IPC_ES_T    uiResult   = IPC_ES_OK;
    PIPC_EVTQ   pevtqThis  = &pIpc->IPC_evtqQueue;
    PIPC_PLIST  piplThis   = &pIpc->IPC_plistPrio;
    PMX_IPC     pIpcNext   = MX_NULL;

    INTREG      iregIntLevel;

    while (1) {
        K_TITLE(__ipcRecvWaitHandle);
        /*
         *  �¼�����
         */
        do {                                                            /*  �ȴ����������첽�¼�        */
            spinLockAndDisIrq(&pevtqThis->IEQ_spLock, &iregIntLevel);
            pIpcNext = __ipcEvtqNext(pIpc);                             /*  ȡ��һ���¼�                */
            spinUnlockAndEnIrq(&pevtqThis->IEQ_spLock, iregIntLevel);

            if (pIpcNext) {
                __ipcRecvEventPro(pIpc, pIpcNext);                      /*  �����첽�¼�                */
            }
        } while (pIpcNext);
        /*
         *  ��ȡ IPC ����
         */
        iregIntLevel = kernelEnterAndDisIrq();                          /*  �ں���                      */
        if (piplThis->IPL_uiSignal == 0) {                              /*  �� IPC ������������ȴ�     */
                                                                        /*  �˴�����ֻ���ں���ͬ��      */
            __ipcStateSet(pIpc, 0, IPC_TS_SER_PEND);                    /*  ������״̬                  */
            vcpuSuspend();                                              /*  ֮ǰ�� IPC �������ѽ��շ� */

            K_WARNING(" PENDING=0x%x", IPC_TO_VCPU(pIpc));
            kernelExitAndEnIrq(iregIntLevel);                           /*  ������: ��������            */
            K_WARNING(" WAKEUP=0x%x", IPC_TO_VCPU(pIpc));
            continue;
        }
        kernelExitAndEnIrq(iregIntLevel);

        pIpcNext = __ipcRecvNextPro(pIpc);                              /*  �����ȼ��б��л�ȡ������IPC */
        if (pIpcNext == MX_NULL) {                                      /*  IPC �������¼�����, ��ѯ    */
            continue;
        }
        /*
         *  ���� IPC ����
         */
        __ipcStateSet(pIpc, IPC_S_RECV_WT, IPC_S_RECV_IN);              /*  ����Ϊ IPC_S_RECV_IN ״̬   */
        __ipcTargetSet(pIpc, pIpcNext);                                 /*  ����Ϊ IPC Ŀ�����         */
        uiResult = __ipcRecvInHandle(pIpc);                             /*  ���� IPC_RECV_IN �׶�       */

        if (IPC_ES_TRUE(uiResult)                          &&
            __ipcStateFind(pIpc, IPC_S_NONE) == IPC_S_RECV_WT) {        /*  IPC ����������һ����        */
            //K_DEBUG("handle next ipc");
            continue;
        }

        return  (uiResult);
    }
}
/*********************************************************************************************************
** ��������: __ipcRecvPro
** ��������: IPC ���մ����ӳ��򣻽���֮ǰ�Ѽ�����̬����������
** �䡡��  : pIpc      IPC �ṹָ��
**           pIpcNext  ������� IPC �������
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��: __ipcRecvInHandle
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcRecvPro (PMX_IPC  pIpc, PMX_IPC  pipcNext)
{
    INTREG          iregIntLevel;

    spinLockAndDisIrq(&pipcNext->IPC_spStateLock, &iregIntLevel);

    if (__ipcStateFind(pipcNext, IPC_TS_READY) == IPC_TS_READY) {       /*  �ѳ�ʱ����ʱ�¼�������      */
        spinUnlockAndEnIrq(&pipcNext->IPC_spStateLock, iregIntLevel);
        return  (IPC_ES_TIMEOUT);                                       /*  ���س�ʱ״̬, ��ע���쳣    */
    }

    if (__ipcStateFind(pipcNext, IPC_S_RECV) == 0) {
        /*
         *  Ŀ�� VCPU û�н��ս׶�, ������ SEND_FI ״̬
         */
        __ipcReadySet(pipcNext);                                        /*  Ŀ�� IPC ��Ϊ����           */
        spinUnlockAndEnIrq(&pipcNext->IPC_spStateLock, iregIntLevel);   /*  ��ʱ�ͷ���, ���� Copy ����  */

        IPC_ASSERT(__ipcMsgCopy(pipcNext->IPC_pMsg, pIpc->IPC_pMsg));   /*  IPC ��Ϣ����                */

        __ipcRmStateChange(pipcNext, IPC_S_SEND_WT, IPC_S_SEND_FI);     /*  Ŀ�� IPC ��Ϊ SEND_FI ̬    */

        K_NOTE(" wakeup vcpu=0x%x", IPC_TO_VCPU(pipcNext));
        vcpuWakeup(IPC_TO_VCPU(pipcNext));                          /*  ����Ŀ�� VCPU; ִ�е���     */

    } else {
        /*
         *  Ŀ�� VCPU �н��ս׶�, Ŀ�� VCPU �����ȴ�
         *  NOTE: ���Ż� - ��Ϣ����ռ���ж�ʱ�����;
         *        һ�ּ򵥷�����ֱ�ӽ����� IPC �ӻ��ѱ��Ƴ�
         */
        IPC_ASSERT(__ipcMsgCopy(pipcNext->IPC_pMsg, pIpc->IPC_pMsg));   /*  IPC ��Ϣ����                */
        __ipcStateAdd(pIpc, IPC_S_REPLY);                               /*  IPC �ظ�״̬,RECV_FI Ԥ��   */
        __ipcRmStateChange(pipcNext, IPC_S_SEND_WT | IPC_S_RECV,        /*  Ŀ�� IPC ��Ϊ RECV_WT ̬    */
                           IPC_S_RECV_WT);

        spinUnlockAndEnIrq(&pipcNext->IPC_spStateLock, iregIntLevel);
    }

    return  (IPC_ES_OK);
}
/*********************************************************************************************************
** ��������: __ipcRecvInHandle
** ��������: IPC ���մ���׶��ӳ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcRecvInHandle (PMX_IPC  pIpc)
{
    UINT32  uiResult = IPC_ES_OK;

    K_TITLE(__ipcRecvInHandle);

    IPC_ASSERT (pIpc->IPC_pipcTag != NULL);
    uiResult = __ipcRecvPro(pIpc, pIpc->IPC_pipcTag);

    if (IPC_ES_FALSE(uiResult)) {                                       /*  ״̬�쳣                    */
        if (IPC_ES_TRUE(__ipcStateFind(pIpc, IPC_ES_MASK))) {           /*  �ɻָ��쳣���ָ��� RECV_WT  */
            __ipcStateSet(pIpc, IPC_S_RECV_IN, IPC_S_RECV_WT);
            __ipcTargetClean(pIpc);

        } else {                                                        /*  ������ֹ IPC              */
            __ipcErrorPro(pIpc, IPC_S_RECV_IN);                         /*  ������                    */
        }

        return  (uiResult);
    }

    __ipcStateSet(pIpc, IPC_S_RECV_IN, IPC_S_RECV_FI);                  /*  ����Ϊ IPC_S_RECV_FI ״̬   */
    return  (__ipcRecvFiHandle(pIpc));                                  /*  ���� IPC_S_RECV_FI �׶�     */
}
/*********************************************************************************************************
** ��������: __ipcSendFiHandle
** ��������: IPC ������ɽ׶��ӳ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendFiHandle (PMX_IPC  pIpc)
{
    K_TITLE(__ipcSendFiHandle);

    __ipcTargetClean(pIpc);                                             /*  ���Ŀ�� IPC                */
    __ipcStateDel(pIpc, IPC_S_IPC_MASK);                                /*  ��� IPC ״̬               */

    __ipcErrorSet(pIpc, MX_SC_RESULT_OK);                               /*  ���� IPC ���               */
    return (IPC_ES_OK);
}
/*********************************************************************************************************
** ��������: __ipcRecvFiHandle
** ��������: IPC ������ɽ׶��ӳ���
** �䡡��  : pIpc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcRecvFiHandle (PMX_IPC  pIpc)
{
    PMX_IPC pipcTag = __ipcTargetGet(pIpc);
    IPC_ASSERT(pipcTag != MX_NULL);

    K_TITLE(__ipcRecvFiHandle);

    if (__ipcStateFind(pIpc, IPC_S_REPLY) == IPC_S_REPLY) {
        __ipcErrorSet(pIpc, MX_SC_RESULT_NEED_REPLY);                   /*  ��Ҫ�ظ� IPC ����           */
        __ipcArgTargetSet(pIpc, pipcTag);                               /*  ���� IPC ͨ�ŷ�             */

    } else {
        __ipcTargetClean(pIpc);                                         /*  ���Ŀ�� IPC                */
        __ipcErrorSet(pIpc, MX_SC_RESULT_OK);
    }
    __ipcStateDel(pIpc, IPC_S_IPC_MASK);                                /*  ��� IPC ״̬               */
    return  (IPC_ES_OK);
}
/*********************************************************************************************************
** ��������: __ipcCloseHandle
** ��������: �ر� IPC ����
** �䡡��  : pipc      IPC �ṹָ��
** �䡡��  : IPC_ES_T  �쳣���; IPC_ES_OK - ���쳣
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcCloseHandle (PMX_IPC  pIpc)
{
    PIPC_EVTQ   pevtqThis  = &pIpc->IPC_evtqQueue;                      /*  IPC �¼�����                */
    BOOL        bReCheck   =  MX_TRUE;                                  /*  ������, ��ʶ���¼������ؼ�  */
    PMX_IPC     pipcNext   =  MX_NULL;
    PMX_IPC     pipcTag    =  __ipcTargetGet(pIpc);                     /*  Ŀ�� IPC                    */
    IPC_ES_T    uiResult   =  IPC_ES_OK;

    INTREG      iregIntLevel;

    K_TITLE(__ipcCloseHandle);
    spinLockAndDisIrq(&pIpc->IPC_plistPrio.IPL_spLock, &iregIntLevel);
    __ipcStateDel(pIpc, IPC_TS_SER);                                    /*  �ر� IPC ����ģʽ           */
    spinUnlockAndEnIrq(&pIpc->IPC_plistPrio.IPL_spLock, iregIntLevel);
    /*
     *  �������ظ��̣߳�����ѳ�ʱ�򰴳�ʱ����
     *  CLOSE �ظ��Ķ���, һ����ͨ�� IPC ����񷢳� CLOSE ����,
     *  ����������������� CLOSE �������ö�����
     */
    if (__ipcReadyTestSetSf(pipcTag)) {
        __ipcTargetClean(pIpc);
        pipcTag = MX_NULL;
    }
    /*
     *  �����Ϣ�б�
     */
    while ((pipcNext = __ipcRecvNextPro(pIpc)) != MX_NULL) {            /*  �����ȼ��б��л�ȡ������IPC */
        if (!__ipcReadyTestSetSf(pipcNext)) {                           /*  δ��ʱ IPC                  */
            __ipcRmStateChange(pipcNext, 0, IPC_ES_ERROR);              /*  �����쳣״̬                */
            vcpuWakeup(IPC_TO_VCPU(pipcNext));                          /*  �����߳�                    */
        }
    }
    /*
     *  ����첽�¼�
     */
    do {                                                                /*  �����첽�¼�                */
        spinLockAndDisIrq(&pevtqThis->IEQ_spLock, &iregIntLevel);
        pipcNext = __ipcEvtqNext(pIpc);                                 /*  ȡ��һ���¼�                */
        spinUnlockAndEnIrq(&pevtqThis->IEQ_spLock, iregIntLevel);
        /*
         *  �¼����������, �ؼ��¼�����, ȷ��ʱ���ύ���һ����ʱ�¼�;
         *  �˴������ӳٷ���, ��û���� TICK ������ IPC_TS_SER ״̬����ͬ��,
         *  �ǿ��Ǽ��߼������� TICK ��ʹ�ù���ͬ�������ͷ��� IPC �ڲ�״̬
         */
        if (!pipcNext) {
            if (bReCheck) {
                vcpuSleep(1);                                           /*  sleep 1 TICK                */
                bReCheck = MX_FALSE;
                continue;                                               /*  �ؼ�һ���¼�����            */
            }
            break;
        }

        if (__ipcStateFind(pipcNext, IPC_EVTS_REQUEST) ==               /*  �������¼����޸��źż���    */
                                     IPC_EVTS_REQUEST)    {
            pIpc->IPC_plistPrio.IPL_uiSignal--;                         /*  �ź��ۼ�, ����ͬ��          */
        }
        __ipcRmStateChange(pipcNext, IPC_ES_MASK, IPC_ES_ERROR);        /*  �����쳣״̬                */
        vcpuWakeup(IPC_TO_VCPU(pipcNext));                              /*  �����߳�                    */

    } while (1);

    if (pIpc->IPC_plistPrio.IPL_uiSignal > 0) {                         /*  IPC �쳣����                */
                                                                        /*  IPC �����źż�������        */
        __ipcErrorSet(pIpc, MX_SC_RESULT_SER_SIGNAL);
        pIpc->IPC_plistPrio.IPL_uiSignal = 0;
        uiResult = IPC_ES_ERROR;
    }
    __ipcTargetClean(pIpc);                                             /*  ���Ŀ�� IPC                */
    __ipcStateDel(pIpc, IPC_S_IPC_MASK);                                /*  ��� IPC ״̬               */

    if (pipcTag) {                                                      /*  IPC REPLY ����              */
        vcpuWakeup(IPC_TO_VCPU(pipcNext));                              /*  �����߳�                    */
    }
    return  (uiResult);
}
/*********************************************************************************************************
** ��������: __ipcScTargetCheckPro
** ��������: ��鲢���� IPC Ŀ��
** �䡡��  : pvcpuIpc  IPC ϵͳ���� VCPU
**           pvcpuTag  IPC ͨ��Ŀ�� VCPU
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: SC_ipcEntry
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcScTargetCheckPro (PMX_VCPU  pvcpuIpc,
                                       PMX_VCPU  pvcpuTag)
 {
    PMX_IPC     pIpc = IPC_VCPU_TO_IPC(pvcpuIpc);

    if ((pvcpuTag == MX_NULL) || (pvcpuTag == pvcpuIpc)) {              /*  δ����Ŀ������Լ�������Ϣ  */
        __ipcErrorSet(pIpc, MX_SC_RESULT_INVALID_OBJ);
        return  (MX_FALSE);
    }
    __ipcTargetSet(pIpc, IPC_VCPU_TO_IPC(pvcpuTag));                    /*  ���� IPC Ŀ��               */
    return  (MX_TRUE);
}
/*********************************************************************************************************
** ��������: ipcDoSyscall
** ��������: IPC ϵͳ������ں���
** �䡡��  : pvcpu
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: SC_Entry, ipcKernSend
*********************************************************************************************************/
VOID  ipcDoSyscall (PMX_VCPU  pvcpu)
{
    PMX_IPC       pIpc      = IPC_VCPU_TO_IPC(pvcpu);
    PMX_SC_PARAM  pScParam  = MX_SC_PARAM_GET(pvcpu);
    UINT8         ucOpCode  = MX_SC_OPCODE_GET(pScParam);               /*  IPC ���ò�����              */
    MX_OBJECT_ID  ulTagId   = MX_SC_KOBJ_GET(pScParam);                 /*  IPC Ŀ�� VCPU �ں˶��� ID   */
    PMX_VCPU      pvcpuTag  = IPC_OBJ_TO_VCPU(ulTagId);                 /*  Ŀ�� VCPU                   */
    BOOL          bResult   = MX_TRUE;                                  /*  ϵͳ���ýӿڼ����        */

    UINT32        uiState   = 0;                                        /*  IPC ��ʼ״̬                */
    IPC_HANDLE    ipcHandle;                                            /*  IPC �ӳ���                  */

    K_TITLE(ipcDoSyscall);

    switch (ucOpCode) {

    case IPC_OP_CLOSE:
        ipcHandle = __ipcCloseHandle;
        break;

    case IPC_OP_SEND:
        if (!(bResult = __ipcScTargetCheckPro(pvcpu, pvcpuTag))) {
            K_ERROR(" ipc tag1_vcpu=0x%x is error", pvcpuTag);
            break;
        }
        uiState = IPC_S_SEND | IPC_TS_READY;
        ipcHandle = __ipcSendHandle;
        break;

    case (IPC_OP_SEND | IPC_OP_RECV):
        if (!(bResult = __ipcScTargetCheckPro(pvcpu, pvcpuTag))) {
            K_ERROR(" ipc tag2_vcpu=0x%x is error", pvcpuTag);
            break;
        }
        uiState = IPC_S_SEND | IPC_S_RECV | IPC_TS_READY;
        ipcHandle = __ipcSendHandle;
        break;

    case (IPC_OP_SEND | IPC_OP_RECV | IPC_OP_OPEN | IPC_OP_REPLY):
        uiState = IPC_S_SEND | IPC_S_RECV | IPC_S_OPEN | IPC_S_REPLY | IPC_TS_READY;
        /*
         *  �ظ� IPC ʱ, ��ͬ����� target, ��˲��ڵ�����ڼ��
         */
        ipcHandle = __ipcSendHandle;
        break;

    case (IPC_OP_RECV | IPC_OP_OPEN):
        uiState = IPC_S_RECV | IPC_S_OPEN | IPC_TS_READY;
        ipcHandle = __ipcRecvHandle;
        break;

    default:                                                           /*  IPC �������Ϸ�               */
        bResult = MX_FALSE;
        K_ERROR(" ipc opcode=0x%x is error", ucOpCode);
        __ipcErrorSet(pIpc, MX_SC_RESULT_ILLEGAL);
    }

    if (bResult) {
        __ipcStateAdd(pIpc, uiState);                                  /*  ���� IPC ״̬                */
        ipcHandle(pIpc);                                               /*  ���� IPC �ӳ���              */
        __ipcStateDel(pIpc, IPC_ES_MASK);                              /*  ��� IPC �쳣״̬            */
    }
}
/*********************************************************************************************************
** ��������: ipcKernSend
** ��������: �ں˷��� IPC ��Ϣ�ӿ�; ����֮ǰ�ڵ�ǰ IPC_MSG �������Ϣ
** �䡡��  : objTarget  IPC ���ն���
**           bSyn       ����ģʽ: TRUE - ͬ��ģʽ, ������Ϣ������
**                                FALSE- �첽ģʽ, ���ͺ�ȴ��ظ���Ϣ
** �䡡��  : BOOL       TRUE-���ͳɹ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  ipcKernSend (MX_OBJECT_ID  objTarget, BOOL  bSyn)
{
    UINT8       ucOpCode = bSyn ? IPC_OP_SEND :                         /*  ͬ��ģʽ                    */
                              (IPC_OP_SEND | IPC_OP_RECV);              /*  �첽ģʽ                    */
    PMX_VCPU    pvcpu    = MX_VCPU_GET_CUR();


    PMX_SC_PARAM   pscparam  = MX_SC_PARAM_GET(pvcpu);

    K_TITLE(ipcKernSend);
    MX_SC_KOBJ_SET(pscparam, objTarget);                                /*  ���� IPC Ŀ��               */
    MX_SC_OPCODE_SET(pscparam, ucOpCode);                               /*  ���� IPC ������             */
    MX_SC_TIME_SET(pscparam, SC_DTIME_DEF);                             /*  ���� IPC timeout ʱ��       */
    ipcDoSyscall(pvcpu);                                                /*  IPC ����                    */

    if (MX_SC_RESULT_GET(pscparam) != MX_SC_RESULT_OK) {
        return  (MX_FALSE);
    }
    return  (MX_TRUE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
