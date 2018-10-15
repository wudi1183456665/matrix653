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
** ��   ��   ��: _CandTable.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 19 ��
**
** ��        ��: ��ѡ�����.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** ��������: __candTableClear
** ��������: ����ѡ���еĺ�ѡ�̷߳��������
** �䡡��  : pcpu      CPU �ṹ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __candTableClear (PMX_PHYS_CPU  pcpu)
{
    REGISTER PMX_VCPU  pvcpu;
    
    pvcpu               = MX_CAND_VCPU(pcpu);
    pvcpu->VCPU_bIsCand = MX_FALSE;                                     /*  �屾�̺߳�ѡ��ʶ            */
    MX_CAND_VCPU(pcpu)  = MX_NULL;                                      /*  �˳���ѡ��                  */
    //bspDebugMsg(T_COL_YELLOW"       [%d]  CandDel          = %p\033[0m\n", archMpCur(), pvcpu);
    _ReadyTableAdd(pvcpu);                                              /*  ���¼��������              */
}
/*********************************************************************************************************
** ��������: __candTableFill
** ��������: �Ӿ�������ѡ��һ�����ִ�е��̷߳����ѡ��
** �䡡��  : pcpu          CPU �ṹ
**           ppcbbmap      ���ȼ�λͼ
**           uiPriority    ��Ҫ������������ȼ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __candTableFill (PMX_PHYS_CPU  pcpu, PMX_READY  pready, UINT32  uiPriority)
{
    REGISTER PMX_VCPU   pvcpu;
    
    pvcpu = _ReadyTableSeek(pready, uiPriority);                        /*  ȷ�����Ժ�ѡ���е��߳�      */
    
    //bspDebugMsg(T_COL_GREEN"       [%d]  CandNew          = %p\033[0m\n", archMpCur(), pvcpu);

    MX_CAND_VCPU(pcpu)     = pvcpu;                                     /*  �����ѡ��                  */
    pvcpu->VCPU_ulCpuIdRun = pcpu->CPU_ulCpuId;                         /*  ��¼ CPU ��                 */
    pvcpu->VCPU_bIsCand    = MX_TRUE;                                   /*  �ú�ѡ��ʶ                  */
    _ReadyTableDel(pvcpu);                                              /*  �Ӿ�������ɾ��              */
}
/*********************************************************************************************************
** ��������: _CandTableUpdate
** ��������: ���Խ�������ȼ���������װ���ѡ��
** �䡡��  : pcpu      CPU �ṹ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: schedActiveCpu(), _CandTableVcpuGet()
*********************************************************************************************************/
VOID  _CandTableUpdate (PMX_PHYS_CPU  pcpu)
{
             UINT32        uiPriority;
    REGISTER PMX_VCPU      pvcpuCand;
             PMX_READY     preadyHigh;                                  /*  ����������ȼ��̵߳ľ�����  */
             BOOL          bNeedFlush = MX_FALSE;

    preadyHigh = _ReadyTableGet(pcpu, &uiPriority);                     /*  ��ȡ��������ȼ��߳̾�����  */
    pvcpuCand  = MX_CAND_VCPU(pcpu);                                    /*  ��ȡ��ǰ��ѡ�߳�            */

    if (pvcpuCand == MX_NULL) {                                         /*  ��ǰû�к�ѡ�߳�            */
        __candTableFill(pcpu, preadyHigh, uiPriority);                  /*  �������߳� --> ��ѡ��       */
        goto  __update_done;
    }
    
    if (preadyHigh == MX_NULL) {                                        /*  һ�������߳�Ҳû��          */
        goto  __update_done;
    }
    
    if (pvcpuCand->VCPU_usSchedCounter == 0) {                          /*  �Ѿ�û��ʱ��Ƭ��            */
        if (MX_PRIO_IS_HIGH_OR_EQU(uiPriority, 
                                   pvcpuCand->VCPU_uiPriority)) {       /*  �Ƿ���Ҫ��ת                */
            bNeedFlush = MX_TRUE;
        }
    } else {
        if (MX_PRIO_IS_HIGH(uiPriority, 
                            pvcpuCand->VCPU_uiPriority)) {
            bNeedFlush = MX_TRUE;
        }
    }
    
    if (bNeedFlush) {                                                   /*  ���ڸ���Ҫ���е��߳�        */
        __candTableClear(pcpu);                                         /*  ��ѡ���߳� --> ������       */
        __candTableFill(pcpu, preadyHigh, uiPriority);                  /*  �������߳� --> ��ѡ��       */
    }
    
__update_done:
    MX_CAND_ROT(pcpu) = MX_FALSE;                                       /*  ������ȼ����Ʊ�־          */
}
/*********************************************************************************************************
** ��������: _CandTableTryAdd
** ��������: ��ͼ��һ�������߳�װ���ѡ�̱߳� (ֻ�к�ѡ���ǿ�ʱ�Ż�װ��, ����������ȼ����Ʊ�־)
** �䡡��  : pvcpu         �������߳�
** �䡡��  : �Ƿ�����˺�ѡ���б�
** ȫ�ֱ���: 
** ����ģ��: _WakeupTick(), vcpuWakeup()
** ע  ��  : ���̱߳�����ʱ, �ȳ��Խ���װ���ѡ��, ��������ٷŵ�������
*********************************************************************************************************/
BOOL  _CandTableTryAdd (PMX_VCPU  pvcpu)
{
    REGISTER PMX_PHYS_CPU   pcpu;
    REGISTER PMX_VCPU       pvcpuCand;
    REGISTER ULONG          i;

    if (pvcpu->VCPU_bCpuAffinity) {                                     /*  �׺����� CPU                */
        pcpu = MX_CPU_GET(pvcpu->VCPU_ulCpuIdAffinity);
        if (!MX_CPU_IS_ACTIVE(pcpu)) {
            goto    __can_not_cand;
        }
        
        pvcpuCand = MX_CAND_VCPU(pcpu);
        if (pvcpuCand == MX_NULL) {                                     /*  ��ѡ��Ϊ��                  */
            while (1) bspDebugMsg("_CandTableTryAdd(0)");;
            MX_CAND_VCPU(pcpu)     = pvcpu;
            pvcpu->VCPU_ulCpuIdRun = pvcpu->VCPU_ulCpuIdAffinity;       /*  ��¼ CPU ��                 */
            pvcpu->VCPU_bIsCand    = MX_TRUE;                           /*  �����ѡ���б�              */
            return  (MX_TRUE);
            
        } else if (MX_PRIO_IS_HIGH(pvcpu->VCPU_uiPriority, 
                                   pvcpuCand->VCPU_uiPriority)) {       /*  ���ȼ����ڵ�ǰ��ѡ�߳�      */
            MX_CAND_ROT(pcpu) = MX_TRUE;                                /*  �������ȼ�����              */
        }
    
    } else {                                                            /*  ���� CPU �������д�����     */
        //if (pvcpu == 0xC00C0000) bspDebugMsg("try :");
        MX_CPU_FOREACH_ACTIVE (i) {                                     /*  ��������״̬�� CPU          */
            pcpu      = MX_CPU_GET(i);
            pvcpuCand = MX_CAND_VCPU(pcpu);
            if (pvcpuCand == MX_NULL) {                                 /*  ��ѡ��Ϊ��                  */
                while (1) bspDebugMsg("_CandTableTryAdd()");;
                MX_CAND_VCPU(pcpu)     = pvcpu;
                pvcpu->VCPU_ulCpuIdRun = i;                             /*  ��¼ CPU ��                 */
                pvcpu->VCPU_bIsCand    = MX_TRUE;                       /*  �����ѡ���б�              */
                return  (MX_TRUE);
            
            } else if (MX_PRIO_IS_HIGH(pvcpu->VCPU_uiPriority, 
                                       pvcpuCand->VCPU_uiPriority)) {   /*  ���ȼ����ڵ�ǰ��ѡ�߳�      */
                MX_CAND_ROT(pcpu) = MX_TRUE;                            /*  �������ȼ�����              */
            }
            //if (pvcpu == 0xC00C0000) bspDebugMsg(" %d,%d,%p", MX_CAND_ROT(pcpu), pcpu->CPU_ulIpiPend, pvcpuCand);
        }
        //if (pvcpu == 0xC00C0000) bspDebugMsg("\n");
    }

__can_not_cand:
    return  (MX_FALSE);                                                 /*  �޷������ѡ���б�          */
}
/*********************************************************************************************************
** ��������: _CandTableTryDel
** ��������: ��ͼ��һ�����پ����̴߳Ӻ�ѡ�̱߳���ɾ��, ���ɾ���ɹ�, �Ӿ������������һ��
** �䡡��  : pvcpu         ���پ������߳�
** �䡡��  : �Ƿ�Ӻ�ѡ���б���ɾ��
** ȫ�ֱ���: 
** ����ģ��: vcpuSleep(), vcpuSuspend()
** ע  ��  : ���̲߳��پ���ʱ, ���ж��䵱ǰ�Ƿ��ں�ѡ����, �������, �ٴӾ�������ɾ��
*********************************************************************************************************/
BOOL  _CandTableTryDel (PMX_VCPU  pvcpu)
{
    REGISTER PMX_PHYS_CPU  pcpu = MX_CPU_GET(pvcpu->VCPU_ulCpuIdRun);
             PMX_READY     preadyHigh;
             UINT32        uiPriority;

    if (MX_CAND_VCPU(pcpu) == pvcpu) {
        //bspDebugMsg(T_COL_YELLOW"       [%d] *CandDel          = %p\033[0m\n", archMpCur(), pvcpu);
        pvcpu->VCPU_bIsCand = MX_FALSE;
        preadyHigh          = _ReadyTableGet(pcpu, &uiPriority);        /*  ��ȡ��������ȼ��߳̾�����  */
        __candTableFill(pcpu, preadyHigh, uiPriority);                  /*  ��ѡ�������һ�������߳�    */
        MX_CAND_ROT(pcpu)   = MX_FALSE;                                 /*  ������ȼ����Ʊ�־          */
        return  (MX_TRUE);
    } else {
        return  (MX_FALSE);
    }
}
/*********************************************************************************************************
** ��������: _CandTableVcpuGet
** ��������: ��ȡ��ǰ CPU ��Ҫ�����е��߳̿��ƿ�
** �䡡��  : pcpuCur   ��ǰ���� CPU
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: schedule()
*********************************************************************************************************/
PMX_VCPU  _CandTableVcpuGet (PMX_PHYS_CPU  pcpuCur)
{
    /*
     *  ������������²����������л�:
     *  1. �жϴ�����; (�˳��ж�ʱͳһ����)
     *  2. �������ں�״̬; (�˳��ں�ʱͳһ����)
     *  3. ��ǰ�̳߳���ռ���ں���֮��, ������������ spinlock; (���������ȳ�ȥ)
     */
    if (pcpuCur->CPU_ulInterNesting ||
        pcpuCur->CPU_iKernelCounter ||
        pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter > 1) {
        //bspDebugMsg("       [%d][%p]%d, %d, %d\n", archMpCur(), pcpuCur->CPU_pvcpuCur, pcpuCur->CPU_ulInterNesting, pcpuCur->CPU_iKernelCounter, pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter);
        return  (pcpuCur->CPU_pvcpuCur);                                /*  ���ص�ǰ�߳�                */

    } else {
        //bspDebugMsg("   [%d][%p]%d, %d, %d\n", archMpCur(), pcpuCur->CPU_pvcpuCur, pcpuCur->CPU_ulInterNesting, pcpuCur->CPU_iKernelCounter, pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter);
        if (MX_CAND_ROT(pcpuCur)) {                                     /*  �������ȼ�����              */
            //bspDebugMsg("%d.", archMpCur());
            //MX_CPU_CLR_SCHED_IPI_PEND(pcpuCur);
            _CandTableUpdate(pcpuCur);
        }
        return  (MX_CAND_VCPU(pcpuCur));                                /*  ����������ȼ��߳�          */
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

