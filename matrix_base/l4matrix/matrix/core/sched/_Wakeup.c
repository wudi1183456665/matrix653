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
** ��   ��   ��: _Wakeup.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 01 �� 05 ��
**
** ��        ��: ����ϵͳ�ȴ���������������� (�������º���ʱ, һ��Ҫ���ں�״̬).

** BUG:
2018.02.08 _WakeupTick ���� IPC �̻߳���.
2018.02.26 ���� _WakeupIsIn.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** ��������: _WakeupAdd
** ��������: ��һ�� wakeup �ڵ���뻽������
** �䡡��  : pwu           ��������ͷ
**           pwnod         �ڵ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _WakeupAdd (PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod)
{
    PMX_LIST_LINE     plineLonger;
    PMX_WAKEUP_NODE   pwnodLonger;
    
    plineLonger = pwu->WU_plineHeader;
    while (plineLonger) {                                               /*  �ҳ���Ҫ����ڵ���ʱ������  */
        pwnodLonger = _LIST_ENTRY(plineLonger, MX_WAKEUP_NODE, WUN_lineManage);
        if (pwnod->WUN_ulCounter >= pwnodLonger->WUN_ulCounter) {       /*  ��Ҫ���������              */
            pwnod->WUN_ulCounter -= pwnodLonger->WUN_ulCounter;
            plineLonger = _list_line_get_next(plineLonger);
        
        } else {
            if (plineLonger == pwu->WU_plineHeader) {                   /*  ���������ͷ                */
                _List_Line_Add_Ahead(&pwnod->WUN_lineManage, &pwu->WU_plineHeader);
            } else {
                _List_Line_Add_Left(&pwnod->WUN_lineManage, plineLonger);
            }
            pwnodLonger->WUN_ulCounter -= pwnod->WUN_ulCounter;         /*  �Ҳ�ĵ���¼��������      */
            break;
        }
    }
    
    if (plineLonger == MX_NULL) {                                       /*  û����ʱ������              */
        if (pwu->WU_plineHeader == MX_NULL) {                           /*  ��ǰ��������Ϊ��            */
            _List_Line_Add_Ahead(&pwnod->WUN_lineManage, &pwu->WU_plineHeader);
        } else {
            _List_Line_Add_Right(&pwnod->WUN_lineManage, &pwnodLonger->WUN_lineManage);
                                                                        /*  ���뻽������β              */
        }
    }
    
    pwnod->WUN_bInQ = MX_TRUE;
}
/*********************************************************************************************************
** ��������: _WakeupDel
** ��������: �� wakeup ��������ɾ��ָ���ڵ�
** �䡡��  : pwu           ��������ͷ
**           pwnod         �ڵ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _WakeupDel (PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod)
{
    PMX_LIST_LINE     plineRight;
    PMX_WAKEUP_NODE   pwnodRight;

    plineRight = _list_line_get_next(&pwnod->WUN_lineManage);
    if (plineRight) {
        pwnodRight = _LIST_ENTRY(plineRight, MX_WAKEUP_NODE, WUN_lineManage);
        pwnodRight->WUN_ulCounter += pwnod->WUN_ulCounter;              /*  �Ҳ�ĵ���¼��������      */
    }
    
    _List_Line_Del(&pwnod->WUN_lineManage, &pwu->WU_plineHeader);
    pwnod->WUN_bInQ = MX_FALSE;
}
/*********************************************************************************************************
** ��������: __WakeupIsIn
** ��������: �ж��߳̽ڵ��Ƿ��ڻ��ѱ���
** �䡡��  : pwnod         �ڵ�
** �䡡��  : BOOL          TRUE - �ڵ��ڱ���
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  _WakeupIsIn (PMX_WAKEUP_NODE  pwnod)
{
    if (_K_wuDelay.WU_plineHeader == (&pwnod->WUN_lineManage)) {        /*  ��ͷָ��˽ڵ�              */
        return  (MX_TRUE);
    }

    return  (!_LIST_LINE_IS_NOTLNK(&pwnod->WUN_lineManage));
}
/*********************************************************************************************************
** ��������: _WakeupTime
** ��������: ���ָ���ڵ�ĵȴ�ʱ��(Tick ��)
** �䡡��  : pwu           ��������ͷ
**           pwnod         �ڵ�
**           pulLeft       ʣ��ʱ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _WakeupTime (PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod, ULONG  *pulLeft)
{
    PMX_LIST_LINE     plineTemp;
    PMX_WAKEUP_NODE   pwnodTemp;
    ULONG             ulCounter = 0;
    
    for (plineTemp  = pwu->WU_plineHeader;
         plineTemp != MX_NULL;
         plineTemp  = _list_line_get_next(plineTemp)) {
        
        pwnodTemp   = _LIST_ENTRY(plineTemp, MX_WAKEUP_NODE, WUN_lineManage);
        ulCounter  += pwnodTemp->WUN_ulCounter;
        if (pwnodTemp == pwnod) {
            break;
        }
    }
    
    if (plineTemp) {
        *pulLeft = ulCounter;
    } else {
        *pulLeft = 0ul;                                                 /*  û���ҵ��ڵ�                */
    }
}
/*********************************************************************************************************
** ��������: _WakeupTick
** ��������: ʱ��Ƭ���� (tick �жϷ�������б�����, �����ں��ҹر��ж�״̬)
** �䡡��  : ulCounter     �����ʱ�䳤��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _WakeupTick (ULONG  ulCounter)
{
    PMX_VCPU          pvcpu;
    PMX_LIST_LINE     plineOperate;
    PMX_WAKEUP_NODE   pwnodOperate;
    BOOL              bIpcReady;                                        /*  IPC �߳̾���״̬            */

    //bspDebugMsg("_K_wuDelay.WU_plineHeader = %p\n", _K_wuDelay.WU_plineHeader);

    plineOperate = _K_wuDelay.WU_plineHeader;
    while (plineOperate) {
        //bspDebugMsg("_WakeupTick() Never be here\n");
        pwnodOperate = _LIST_ENTRY(plineOperate, MX_WAKEUP_NODE, WUN_lineManage);
        plineOperate = _list_line_get_next(plineOperate);

        if (pwnodOperate->WUN_ulCounter  > ulCounter) {                 /*  ���ڵ�δ��ʱ, �˳�          */
            pwnodOperate->WUN_ulCounter -= ulCounter;
            break;
        }

        ulCounter                  -= pwnodOperate->WUN_ulCounter;
        pwnodOperate->WUN_ulCounter = 0;
        pvcpu                       = _LIST_ENTRY(pwnodOperate, MX_VCPU, VCPU_wunDelay);
        /*
         *  ��ʱ�߳���Ҫ������
         */
        if (IPC_GET_IN_PROCESS(&pvcpu->VCPU_ipc)) {
            /*
             *  IPC ����ģʽ
             */
            if (ipcReadyTryTestSetSafe(&pvcpu->VCPU_ipc, &bIpcReady)) { /*  ���� IPC �߳̾���״̬����λ */

                //K_NOTE("_WakeupTick: wk_ipc_del_vcpu=0x%x", pvcpu);
                _WakeupDel(&_K_wuDelay, &pvcpu->VCPU_wunDelay);         /*  �ӻ���������ɾ��            */

                if (!bIpcReady) {                                       /*  �̵߳�ǰδ�� IPC ����       */
                    ipcTickTimeoutPro(&pvcpu->VCPU_ipc);                /*  ���� IPC �¼���,�첽�����߳�*/
                }
            }                                                           /*  else (TryTest ��������):    */
                                                                        /*  ������ǰ�ڵ㣬�¸�ʱ���ٴ���*/
        } else {
            //bspDebugMsg("Wakeup : %p ", pvcpu);
            /*
             *  ʱ�ӻ���ģʽ
             */
            pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_DELAY;
            _WakeupDel(&_K_wuDelay, &pvcpu->VCPU_wunDelay);             /*  ��ʱ�̴߳ӻ��ѱ����Ƴ�      */

            if (pvcpu->VCPU_uiStatus == MX_VCPU_STATUS_READY) {
                if (!_CandTableTryAdd(pvcpu)) {                         /*  ���Լ����ѡ��              */
                    _ReadyTableAdd(pvcpu);                              /*  ���������                  */
                }
            }
        } 
    }
    //bspDebugMsg("_WakeupTick()\n");
} 
/*********************************************************************************************************
  END
*********************************************************************************************************/

