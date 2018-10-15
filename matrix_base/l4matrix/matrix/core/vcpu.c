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
** ��   ��   ��: vcpu.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 19 ��
**
** ��        ��: �̹߳���.

** BUG:
2018.02.07 ���� vcpuSuspend.
2018.02.07 ���� vcpuWakeup.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** ��������: __vcpuEntryShell
** ��������: �����̵߳���ں���
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __vcpuEntryShell (VOID)
{
    PMX_VCPU                pvcpuCur;
    PVCPU_START_ROUTINE     pfuncStart;
    PVOID                   pvArg;
    BOOL                    bIsUser;
    PVOID                   pvReturnVal;

    pvcpuCur    = MX_VCPU_GET_CUR();
    pfuncStart  = pvcpuCur->VCPU_pfuncStart;
    pvArg       = pvcpuCur->VCPU_pvArg;
    bIsUser     = pvcpuCur->VCPU_bIsUser;

#if 0
    PMX_STACK               pstkUser;
    static INT              iUserIndex = 0;

    pstkUser    = pvcpuCur->VCPU_pstkUStackStart;
    if (bIsUser) {                                                      /*  �û��߳�                    */
        //kernelEnter();
        if (pstkUser == MX_NULL) {
            KN_ASSERT(iUserIndex < MX_CFG_MAX_USTACKS);                 /*  ȷ�������п����û���ջ      */
            pstkUser = &_K_stkUser[iUserIndex + 1][0];                  /*  ����һ��Ĭ���û���ջ        */
            iUserIndex++;
        }
        //kernelExit();

        __asm__ __volatile__ ("MSR  CPSR_c , #(0x10 | 0x40) \n\t"
                              "MOV  SP , %0 \n\t"
                              :
                              :"r"(pstkUser)
                              :"cc", "memory"
                              );
    }
    pvReturnVal = pfuncStart(pvArg);                                    /*  ִ���߳�                    */
#else
    if (bIsUser) {                                                      /*  �û��߳�                    */
        archJumpToUser(pvArg, pfuncStart);
    } else {
        archJumpToKern(pvArg, pfuncStart);
    }
#endif

    (VOID)pvReturnVal;
}
/*********************************************************************************************************
** ��������: vcpuCreate
** ��������: ����һ���߳�
** �䡡��  : pfuncStart         �߳���ڵ�ַ
**           pvcpuattr          �߳����Լ���ָ��
** �䡡��  : �߳̿��ƿ�ָ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PMX_VCPU  vcpuCreate (PVCPU_START_ROUTINE  pfuncStart, PMX_VCPU_ATTR  pvcpuattr)
{
    MX_OBJECT_ID    ulVcpuId;
    PMX_VCPU        pvcpuNew;
    PMX_STACK       pstkTop;
    PMX_STACK       pstkNow;

    ulVcpuId = kobjAlloc(_KOBJ_VCPU);
    pvcpuNew = (PMX_VCPU)kobjGet(ulVcpuId);
    pstkTop  = (PMX_STACK)((UINT32)pvcpuNew + MX_CFG_KSTACK_SIZE);

    pstkNow  = archVcpuCtxCreate(__vcpuEntryShell, pstkTop, 0);

    pvcpuNew->VCPU_pstkKStackNow   = pstkNow;
    pvcpuNew->VCPU_pstkUStackStart = pvcpuattr->VCPUATTR_pstkLowAddr;   /*  NOTE: �����û�ջ(��)��ַ    */
    pvcpuNew->VCPU_pfuncStart      = pfuncStart;
    pvcpuNew->VCPU_uiPriority      = pvcpuattr->VCPUATTR_uiPriority;
    pvcpuNew->VCPU_pvArg           = pvcpuattr->VCPUATTR_pvArg;
    pvcpuNew->VCPU_bIsUser         = pvcpuattr->VCPUATTR_bIsUser;
    pvcpuNew->VCPU_uiStatus        = MX_VCPU_STATUS_INIT;

    pvcpuNew->VCPU_bCpuAffinity    = MX_FALSE;
    pvcpuNew->VCPU_ulCpuIdAffinity = 0;
    pvcpuNew->VCPU_bIsCand         = MX_FALSE;

    pvcpuNew->VCPU_ulStatus        = VCPU_STAT_DIS_INT;
    //pvcpuNew->VCPU_uiIrqPend     = 0;
    _vIrqPendIni(pvcpuNew);
    pvcpuNew->VCPU_pfuncIsr        = MX_NULL;
    pvcpuNew->VCPU_pfuncSwi        = MX_NULL;

    pvcpuNew->VCPU_usSchedSlice    = MX_VCPU_SCHED_SLICE;
    pvcpuNew->VCPU_usSchedCounter  = MX_VCPU_SCHED_SLICE;

    pvcpuNew->VCPU_ulVcpuId        = ulVcpuId;

    ipcInit(&pvcpuNew->VCPU_ipc);

    bspDebugMsg("vcpuCreate() id:%X, ptr:%X, prio:%d\n", ulVcpuId, pvcpuNew, pvcpuNew->VCPU_uiPriority);

    return  (pvcpuNew);
}
/*********************************************************************************************************
** ��������: vcpuStart
** ��������: ����һ���߳�
** �䡡��  : pvcpu              �߳̿��ƿ�ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_ERROR  vcpuStart (PMX_VCPU  pvcpu)
{
    INTREG     iregInterLevel;

    if (pvcpu == MX_NULL) {
        return  (ERROR_KOBJ_NULL);
    }

    iregInterLevel = archIntDisable();
    if (pvcpu->VCPU_uiStatus & MX_VCPU_STATUS_INIT) {
        pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_INIT;

        if (pvcpu->VCPU_uiStatus == MX_VCPU_STATUS_READY) {
            _ReadyTableAdd(pvcpu);                                      /*  ���뵽������                */
        }
    }
    archIntEnable(iregInterLevel);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: vcpuSetIrqEntry
** ��������: �����̵߳��ж����
** �䡡��  : pvcpu              �߳̿��ƿ�ָ��
**           pfuncIsr           �߳��ж���ں���
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_ERROR  vcpuSetIrqEntry (PMX_VCPU  pvcpu, VOID_FUNCPTR  pfuncIsr)
{
    MX_ERROR   eReturn = ERROR_KOBJ_NULL;

    if (pvcpu) {
        pvcpu->VCPU_pfuncIsr = pfuncIsr;
        eReturn              = ERROR_NONE;
    }

    return  (eReturn);
}
/*********************************************************************************************************
** ��������: vcpuSleep
** ��������: ��ǰ�߳�˯�ߺ���
** �䡡��  : ulTick             ˯�ߵ�ʱ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  vcpuSleep (ULONG  ulTick)
{
    PMX_VCPU        pvcpuCur;
    INTREG          iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();                            /*  �����ں�ͬʱ�ر��ж�        */

    pvcpuCur = MX_VCPU_GET_CUR();                                       /*  ��ǰ CPU ���ƿ�             */

    if (!_CandTableTryDel(pvcpuCur)) {                                  /*  ���ԴӺ�ѡ����ɾ��          */
        bspDebugMsg("vcpuSleep()");
        _ReadyTableDel(pvcpuCur);                                       /*  �Ӿ�������ɾ��              */
    }

    pvcpuCur->VCPU_ulDelay   = ulTick;
    pvcpuCur->VCPU_uiStatus |= MX_VCPU_STATUS_DELAY;
    _WakeupAdd(&_K_wuDelay, &pvcpuCur->VCPU_wunDelay);

    //bspDebugMsg("Sleep : %p\n", pvcpuCur);

    kernelExitAndEnIrq(iregInterLevel);                                 /*  �˳��ں�ͬʱ���ж�        */
}
/*********************************************************************************************************
** ��������: vcpuSuspend
** ��������: �߳���ͣ, ���û�������� vcpuWakeup() ����
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  vcpuSuspend (VOID)
{
    PMX_VCPU        pvcpuCur;
    INTREG          iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();                            /*  �����ں�ͬʱ�ر��ж�        */

    pvcpuCur = MX_VCPU_GET_CUR();                                       /*  ��ǰ CPU ���ƿ�             */
    IPC_SET_IN_PROCESS(&(pvcpuCur->VCPU_ipc), MX_FALSE);                /*  ��ͨ����                    */

    if (!_CandTableTryDel(pvcpuCur)) {                                  /*  ���ԴӺ�ѡ����ɾ��          */
        bspDebugMsg("vcpuSuspend()"); //while(1);
        _ReadyTableDel(pvcpuCur);                                       /*  �Ӿ�������ɾ��              */
    }

    pvcpuCur->VCPU_uiStatus |= MX_VCPU_STATUS_SUSPEND;

    kernelExitAndEnIrq(iregInterLevel);                                 /*  �˳��ں�ͬʱ���ж�        */
}
/*********************************************************************************************************
** ��������: vcpuWakeup
** ��������: �����߳�
** �䡡��  : pVcpu             �����ѵ��߳�
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  vcpuWakeup (PMX_VCPU  pvcpu)
{
    INTREG          iregInterLevel;

#if 1
    if (IPC_GET_IN_PROCESS(&pvcpu->VCPU_ipc)) {                         /*  IPC ����ģʽ: ���������    */
                                                                        /*  �߳��� tick �Ƴ����ѱ�      */
        if (!ipcReadyTestSafe(&pvcpu->VCPU_ipc)) {
            /*
             *  IPC��������: ��һ�������߳��������������� IPC ���ڵȴ�
             *  ״̬ʱ, ���߳���˷����� IPC ���󲢳��Ի�����, ���軽��
             *
             */
            K_NOTE("ipc not ready");
            return;
        }
    }

    iregInterLevel = kernelEnterAndDisIrq();                            /*  �����ں�ͬʱ�ر��ж�        */

    pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_SUSPEND;
    pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_DELAY;
    /*
     *  IPC ״̬���ڻ��ѱ��� ���� ��IPC ״̬�ҷ�PENDING ״̬
     */
    if (_WakeupIsIn(&pvcpu->VCPU_wunDelay)) {
        //K_DEBUG(" wk_del_vcpu=0x%x", pvcpu);
        _WakeupDel(&_K_wuDelay, &pvcpu->VCPU_wunDelay);                 /*  ��ʱ�̴߳ӻ��ѱ����Ƴ�      */
    }

    if (pvcpu->VCPU_uiStatus == MX_VCPU_STATUS_READY) {
        //K_DEBUG(" ready_vcpu=0x%x", pvcpu);
        if (!_CandTableTryAdd(pvcpu)) {                                 /*  ���Լ����ѡ��              */
            _ReadyTableAdd(pvcpu);                                      /*  ���������                  */
        }
    }

    kernelExitAndEnIrq(iregInterLevel);                                 /*  �˳��ں�ͬʱ���ж�        */
#else
    iregInterLevel = kernelEnterAndDisIrq();                            /*  �����ں�ͬʱ�ر��ж�        */

    pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_SUSPEND;

    if (pvcpu->VCPU_uiStatus == MX_VCPU_STATUS_READY) {
        if (!_CandTableTryAdd(pvcpu)) {                                 /*  ���Լ����ѡ��              */
            _ReadyTableAdd(pvcpu);                                      /*  ���������                  */
        }
    }

    kernelExitAndEnIrq(iregInterLevel);                                 /*  �˳��ں�ͬʱ���ж�        */
#endif
}
/*********************************************************************************************************
** ��������: vcpuGetPriority
** ��������: ����߳����ȼ�
** �䡡��  : pvcpu              �߳̿��ƿ�ָ��
**           puiPriority        ���ȼ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_ERROR  vcpuGetPriority (PMX_VCPU  pvcpu, UINT32  *puiPriority)
{
    MX_ERROR   eReturn = ERROR_KOBJ_NULL;

    if (pvcpu) {
        *puiPriority = pvcpu->VCPU_uiPriority;
        eReturn      = ERROR_NONE;
    }

    return  (eReturn);
}
/*********************************************************************************************************
** ��������: vcpuSetAffinity
** ��������: ���߳�������ָ���� CPU ����
** �䡡��  : pvcpu              �߳̿��ƿ�ָ��
**           ulCpuIdAffinity    Ҫ�󶨵� CPU (MX_CFG_MAX_PROCESSORS ʱ, �����)
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  vcpuSetAffinity (PMX_VCPU  pvcpu, ULONG  ulCpuIdAffinity)
{
    if (ulCpuIdAffinity == MX_CFG_MAX_PROCESSORS) {
        pvcpu->VCPU_bCpuAffinity    = MX_FALSE;
        pvcpu->VCPU_ulCpuIdAffinity = 0;
    } else {
        pvcpu->VCPU_bCpuAffinity    = MX_TRUE;
        pvcpu->VCPU_ulCpuIdAffinity = ulCpuIdAffinity;
    }
}
/*********************************************************************************************************
** ��������: vcpuSetUserStack
** ��������: �����̵߳��û���ջ��ʼ��ַ
** �䡡��  : pvcpu              �߳̿��ƿ�ָ��
**           pstkUser           �û���ջջ�׵�ַ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  vcpuSetUserStack (PMX_VCPU  pvcpu, PMX_STACK  pstkUser)
{
    pvcpu->VCPU_pstkUStackStart = pstkUser;
}
/*********************************************************************************************************
** ��������: vcpuBindPartition
** ��������: �����߳������Ľ���
** �䡡��  : pvcpu              �߳̿��ƿ�ָ��
**           pprtnBelong        �������̽ṹ��ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  vcpuBindPartition (PMX_VCPU  pvcpu, PMX_PRTN  pprtnBelong)
{
    pvcpu->VCPU_pprtnBelong = pprtnBelong;
}
/*********************************************************************************************************
** ��������: vcpuBindIpcMsg
** ��������: �̰߳��û���Ϣ, ���ں���Ϣ�����ڴ�ӳ��
** �䡡��  : pvcpu              �߳̿��ƿ�ָ��
**           pIpcMsg            IPC ��Ϣ��ָ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  vcpuBindIpcMsg (PMX_VCPU  pvcpu, PMX_IPC_MSG  pIpcMsg)
{
    MX_IPC_MSG_GET(pvcpu) = pIpcMsg;
}
/*********************************************************************************************************
** ��������: vcpuDisableInt
** ��������: ���ε�ǰ VCPU �ж�
** �䡡��  : NONE
** �䡡��  : ԭ VCPU ״̬
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
ULONG  vcpuDisableInt (VOID)
{
    PMX_VCPU         pvcpuCur = MX_VCPU_GET_CUR();
    ULONG            ulStatus;

    ulStatus = pvcpuCur->VCPU_ulStatus & VCPU_STAT_DIS_INT;

    pvcpuCur->VCPU_ulStatus |= VCPU_STAT_DIS_INT;
    //bspDebugMsg("(");

    return  (ulStatus);
}
/*********************************************************************************************************
** ��������: vcpuEnableInt
** ��������: ʹ�ܵ�ǰ VCPU �ж�
** �䡡��  : ulStatus   Ҫ�ָ��� VCPU ״̬
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  vcpuEnableInt (ULONG  ulStatus)
{
    PMX_VCPU         pvcpuCur = MX_VCPU_GET_CUR();

    if ((ulStatus & VCPU_STAT_DIS_INT) == 0) {
        pvcpuCur->VCPU_ulStatus &= ~VCPU_STAT_DIS_INT;
    }
}
/*********************************************************************************************************
** ��������: vcpuIsIntEnable
** ��������: VCPU �Ƿ��ж�
** �䡡��  : pvcpu         �߳̿��ƿ�ָ��
** �䡡��  : ��ǰ���ڿ��ж�״̬, ���� MX_TRUE; ���򷵻� MX_FALSE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
BOOL  vcpuIsIntEnable (PMX_VCPU  pvcpu)
{
    if (pvcpu->VCPU_ulStatus & VCPU_STAT_DIS_INT) {
        return  (MX_FALSE);
    } else {
        return  (MX_TRUE);
    }
}
/*********************************************************************************************************
** ��������: vcpuSaveStatus
** ��������: �� VCPU ״ֵ̬���� CPU ״̬�Ĵ���
** �䡡��  : puctx      �û�������ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: ���뱾����ʱ, UCTX �е�״̬һ���ǿ��жϵ�, ��Ϊ��ʱӲ���Ĵ���ֵ(�û�״̬Ӳ���ж�һֱ��)
*********************************************************************************************************/
VOID  vcpuSaveStatus (PARCH_USER_CTX  puctx)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();

    if (pvcpuCur->VCPU_ulStatus & VCPU_STAT_DIS_INT) {
        ARCH_UCTX_DIS_IRQ(puctx);
    }
}
/*********************************************************************************************************
** ��������: vcpuRestoreStatus
** ��������: �����û��������е� CPU ״̬�Ĵ���ֵ�޸� VCPU ״̬
** �䡡��  : puctx      �û�������ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: �˳���������, UCTX �е�״̬�����ǿ��жϵ�. ע���״̬�����ڽ�������ǰ���û��������滻
*********************************************************************************************************/
VOID  vcpuRestoreStatus (PARCH_USER_CTX  puctx)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();

    if (ARCH_UCTX_IS_DISIRQ(puctx)) {                                   /*  �жϹ�                      */
        pvcpuCur->VCPU_ulStatus |= VCPU_STAT_DIS_INT;
        ARCH_UCTX_EN_IRQ(puctx);
        //bspDebugMsg("[");
    } else {
        pvcpuCur->VCPU_ulStatus &= ~VCPU_STAT_DIS_INT;
        //bspDebugMsg("]");
    }
}
/*********************************************************************************************************
** ��������: vcpuOccurIrq
** ��������: �� VCPU ����ĳ���߼��ж�ʱ�Ĳ���
** �䡡��  : pvcpu         �߳̿��ƿ�ָ��
**           ulVec         �߼��жϺ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  vcpuOccurIrq (PMX_VCPU  pvcpu, ULONG  ulVec)
{
    _vIrqPendSet(pvcpu, ulVec);

    if (vcpuIsIntEnable(pvcpu)) {
        if (pvcpu == MX_VCPU_GET_CUR()) {
            vcpuDoPendIsr(pvcpu);

        } else if (MX_VCPU_IS_RUNNING(pvcpu)) {
            _SmpIpiSend(pvcpu->VCPU_ulCpuIdRun, MX_IPI_IRQ, 0, MX_TRUE);
        }
    } else {                                                            /*  VCPU �����жϹ�״̬         */
        irqDisable(ulVec);                                              /*  �ݹظ��ж�, ����һֱ���ж�  */
    }
}
/*********************************************************************************************************
** ��������: vcpuDoPendIsr
** ��������: ���� VCPU �Ѿ� Pend ���ж�
** �䡡��  : pvcpu         �߳̿��ƿ�ָ��(�����ǵ�ǰ VCPU)
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: �� VCPU ���ж�ʱ
*********************************************************************************************************/
VOID  vcpuDoPendIsr (PMX_VCPU  pvcpu)
{
    ULONG  ulVec;

    if (pvcpu->VCPU_pfuncIsr) {
        while (_vIrqPendGet(pvcpu, &ulVec)) {
            vcpuSaveStatus(pvcpu->VCPU_puctxSP);

            pvcpu->VCPU_pfuncIsr(ulVec, pvcpu->VCPU_puctxSP);
            _vIrqPendClr(pvcpu, ulVec);
            irqEnable(ulVec);                                           /*  ���¿����ж�                */

            vcpuRestoreStatus(pvcpu->VCPU_puctxSP);

            //bspDebugMsg("%d", ulVec);
        }
    }
}
/*********************************************************************************************************
** ��������: vcpuGetCur
** ��������: ��ȡ��ǰ���ж��߳� MX_VCPU �ṹ��ָ��
** �䡡��  : NONE
** �䡡��  : PMX_VCPU
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PMX_VCPU  vcpuGetCur (VOID)
{
    PMX_VCPU    pvcpuCur;

    pvcpuCur = MX_VCPU_GET_CUR();

    return  (pvcpuCur);
} 
/*********************************************************************************************************
  END
*********************************************************************************************************/

