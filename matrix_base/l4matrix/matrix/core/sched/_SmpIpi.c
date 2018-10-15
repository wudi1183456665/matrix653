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
** ��   ��   ��: _SmpIpi.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 20 ��
**
** ��        ��: CPU �˼��ж�.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
  IPI LOCK
*********************************************************************************************************/
#define __SMP_IPI_LOCK(pcpu, bIntLock)                                  \
        if (bIntLock) {                                                 \
            spinLockIgnIrq(&pcpu->CPU_slIpi);                           \
        } else {                                                        \
            spinLockAndDisIrq(&pcpu->CPU_slIpi, &iregInterLevel);       \
        }
#define __SMP_IPI_UNLOCK(pcpu, bIntLock)                                \
        if (bIntLock) {                                                 \
            spinUnlockIgnIrq(&pcpu->CPU_slIpi);                         \
        } else {                                                        \
            spinUnlockAndEnIrq(&pcpu->CPU_slIpi, iregInterLevel);       \
        }
#define __SMP_IPI_IRQ_LOCK(bIntLock)                                    \
        if (bIntLock == MX_FALSE) {                                     \
            iregInterLevel = archIntDisable();                          \
        }
#define __SMP_IPI_IRQ_UNLOCK(bIntLock)                                  \
        if (bIntLock == MX_FALSE) {                                     \
            archIntEnable(iregInterLevel);                              \
        }
/*********************************************************************************************************
** ��������: _SmpIpiSend
** ��������: ����һ�����Զ�������ĺ˼��жϸ�ָ���� CPU. (������ȴ����ⲿ�ɲ�������ǰ CPU)
** �䡡��  : ulCpuId       CPU ID
**           ulIpiType     �˼��ж����� (���Զ��������ж�����)
**           iWait         �Ƿ�ȴ�������� (MX_IPI_SCHED ��������ȴ�, ���������)
**           bIntLock      �ⲿ�Ƿ���ж���.
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
** ע  ��  : ֪ͨ���� (MX_IPI_SCHED) ��֪ͨ CPU ֹͣ (MX_IPI_DOWN) ����Ҫ�ȴ�����.
*********************************************************************************************************/
VOID  _SmpIpiSend (ULONG  ulCpuId, ULONG  ulIpiType, INT  iWait, BOOL  bIntLock)
{
    INTREG         iregInterLevel;
    PMX_PHYS_CPU   pcpuDst = MX_CPU_GET(ulCpuId);
    PMX_PHYS_CPU   pcpuCur = MX_CPU_GET_CUR();
    ULONG          ulMask  = (ULONG)(1 << ulIpiType);
    
    if (!MX_CPU_IS_ACTIVE(pcpuDst)) {                                   /*  CPU ���뱻����              */
        return;
    }
    
    __SMP_IPI_LOCK(pcpuDst, bIntLock);                                  /*  ����Ŀ�� CPU IPI            */
    MX_CPU_ADD_IPI_PEND(ulCpuId, ulMask);                               /*  ��� PEND λ                */
    archMpInt(ulCpuId);
    __SMP_IPI_UNLOCK(pcpuDst, bIntLock);                                /*  ����Ŀ�� CPU IPI            */
    
    if (iWait) {
        while (MX_CPU_GET_IPI_PEND(ulCpuId) & ulMask) {                 /*  �ȴ�����                    */
            __SMP_IPI_IRQ_LOCK(bIntLock);
            _SmpIpiTryProc(pcpuCur);                                    /*  ����ִ�������˷����� IPI    */
            __SMP_IPI_IRQ_UNLOCK(bIntLock);
            archSpinDelay();
        }
    }
}
/*********************************************************************************************************
** ��������: _SmpIpiSendOther
** ��������: ����һ�����Զ�������ĺ˼��жϸ��������� CPU, (�ⲿ����������ǰ CPU ����)
** �䡡��  : ulIpiType     �˼��ж����� (���Զ��������ж�����)
**           iWait         �Ƿ�ȴ�������� (MX_IPI_SCHED ��������ȴ�, ���������)
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _SmpIpiSendOther (ULONG  ulIpiType, INT  iWait)
{
    ULONG   i;
    ULONG   ulCpuId;
    
    ulCpuId = MX_CPU_GET_CUR_ID();
    
    KN_SMP_WMB();
    MX_CPU_FOREACH_EXCEPT (i, ulCpuId) {
        _SmpIpiSend(i, ulIpiType, iWait, MX_FALSE);
    }
}
/*********************************************************************************************************
** ��������: __smpIpiCall
** ��������: ����һ���Զ���˼��жϸ�ָ���� CPU. (������ȴ����ⲿ�ɲ�������ǰ CPU)
** �䡡��  : ulCpuId       CPU ID
**           pipim         �˼��жϲ���
** �䡡��  : ���÷���ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  __smpIpiCall (ULONG  ulCpuId, PMX_IPI_MSG  pipim)
{
    INTREG         iregInterLevel;
    PMX_PHYS_CPU   pcpuDst = MX_CPU_GET(ulCpuId);
    PMX_PHYS_CPU   pcpuCur = MX_CPU_GET_CUR();
    
    if (!MX_CPU_IS_ACTIVE(pcpuDst)) {                                   /*  CPU ���뱻����              */
        return  (ERROR_NONE);
    }
    
    __SMP_IPI_LOCK(pcpuDst, MX_FALSE);                                  /*  ����Ŀ�� CPU IPI            */
    _List_Ring_Add_Last(&pipim->IPIM_ringManage, &pcpuDst->CPU_pringMsg);
    pcpuDst->CPU_uiMsgCnt++;
    MX_CPU_ADD_IPI_PEND(ulCpuId, MX_IPI_CALL_MSK);
    archMpInt(ulCpuId);
    __SMP_IPI_UNLOCK(pcpuDst, MX_FALSE);                                /*  ����Ŀ�� CPU IPI            */
    
    while (pipim->IPIM_iWait) {                                         /*  �ȴ�����                    */
        __SMP_IPI_IRQ_LOCK(MX_FALSE);
        _SmpIpiTryProc(pcpuCur);
        __SMP_IPI_IRQ_UNLOCK(MX_FALSE);
        archSpinDelay();
    }
    
    return  (pipim->IPIM_iRet);
}
/*********************************************************************************************************
** ��������: __smpIpiCallOther
** ��������: ����һ���Զ���˼��жϸ��������� CPU. (�ⲿ����������ǰ CPU ����)
** �䡡��  : pipim         �˼��жϲ���
** �䡡��  : NONE (�޷�ȷ������ֵ)
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __smpIpiCallOther (PMX_IPI_MSG  pipim)
{
    ULONG   i;
    ULONG   ulCpuId;
    INT     iWaitSave = pipim->IPIM_iWait;
    
    ulCpuId = MX_CPU_GET_CUR_ID();
    
    KN_SMP_WMB();
    MX_CPU_FOREACH_EXCEPT (i, ulCpuId) {
        __smpIpiCall(i, pipim);
        
        KN_SMP_MB();
        pipim->IPIM_iWait = iWaitSave;
        KN_SMP_WMB();
    }
}
/*********************************************************************************************************
** ��������: _SmpCallFunc
** ��������: ���ú˼��ж���ָ���� CPU ����ָ���ĺ���. (�ⲿ����������ǰ CPU ����)
** �䡡��  : ulCpuId       CPU ID
**           pfunc         ͬ��ִ�к��� (�����ú����ڲ������������ں˲���, ������ܲ�������)
**           pvArg         ͬ������
**           pfuncAsync    �첽ִ�к��� (�����ú����ڲ������������ں˲���, ������ܲ�������)
**           pvAsync       �첽ִ�в���
**           iOpt          ѡ�� IPIM_OPT_NORMAL / IPIM_OPT_NOKERN
** �䡡��  : ���÷���ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  _SmpCallFunc (ULONG         ulCpuId,
                   INT_FUNCPTR   pfunc,
                   PVOID         pvArg,
                   VOID_FUNCPTR  pfuncAsync,
                   PVOID         pvAsync,
                   INT           iOpt)
{
    MX_IPI_MSG  ipim;
    
    ipim.IPIM_pfuncCall      = pfunc;
    ipim.IPIM_pvArg          = pvArg;
    ipim.IPIM_pfuncAsyncCall = pfuncAsync;
    ipim.IPIM_pvAsyncArg     = pvAsync;
    ipim.IPIM_iRet           = -1;
    ipim.IPIM_iOption        = iOpt;
    ipim.IPIM_iWait          = 1;
    
    return  (__smpIpiCall(ulCpuId, &ipim));
}
/*********************************************************************************************************
** ��������: _SmpCallFuncAllOther
** ��������: ���ú˼��ж���ָ���� CPU ����ָ���ĺ���. (�ⲿ����������ǰ CPU ����)
** �䡡��  : pfunc         ͬ��ִ�к��� (�����ú����ڲ������������ں˲���, ������ܲ�������)
**           pvArg         ͬ������
**           pfuncAsync    �첽ִ�к��� (�����ú����ڲ������������ں˲���, ������ܲ�������)
**           pvAsync       �첽ִ�в���
**           iOpt          ѡ�� IPIM_OPT_NORMAL / IPIM_OPT_NOKERN
** �䡡��  : NONE (�޷�ȷ������ֵ)
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _SmpCallFuncAllOther (INT_FUNCPTR   pfunc,
                            PVOID         pvArg,
                            VOID_FUNCPTR  pfuncAsync,
                            PVOID         pvAsync,
                            INT           iOpt)
{
    MX_IPI_MSG  ipim;
    
    ipim.IPIM_pfuncCall      = pfunc;
    ipim.IPIM_pvArg          = pvArg;
    ipim.IPIM_pfuncAsyncCall = pfuncAsync;
    ipim.IPIM_pvAsyncArg     = pvAsync;
    ipim.IPIM_iRet           = -1;
    ipim.IPIM_iOption        = iOpt;
    ipim.IPIM_iWait          = 1;
    
    __smpIpiCallOther(&ipim);
}
/*********************************************************************************************************
** ��������: __smpProcMsg
** ��������: ����˼��жϵ��ú���
** �䡡��  : pcpuCur       ��ǰ CPU
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __smpProcMsg (PMX_PHYS_CPU  pcpuCur)
{
    UINT            i, uiCnt;
    PMX_IPI_MSG     pipim;
    PMX_LIST_RING   pringTemp;
    PMX_LIST_RING   pringDelete;
    
    spinLockIgnIrq(&pcpuCur->CPU_slIpi);                                /*  ���� CPU                    */
    
    pringTemp = pcpuCur->CPU_pringMsg;
    uiCnt     = pcpuCur->CPU_uiMsgCnt;
    
    for (i = 0; i < uiCnt; i++) {
        pipim = _LIST_ENTRY(pringTemp, MX_IPI_MSG, IPIM_ringManage);
        
        pringDelete = pringTemp;
        pringTemp   = _list_ring_get_next(pringTemp);
        _List_Ring_Del(pringDelete, &pcpuCur->CPU_pringMsg);            /*  ɾ��һ���ڵ�                */
        pcpuCur->CPU_uiMsgCnt--;
        
        if (pipim->IPIM_pfuncCall) {
            pipim->IPIM_iRet = pipim->IPIM_pfuncCall(pipim->IPIM_pvArg);/*  ִ��ͬ������                */
        }
        
        KN_SMP_MB();
        pipim->IPIM_iWait = 0;                                          /*  ���ý���                    */
        KN_SMP_WMB();
        
        if (pipim->IPIM_pfuncAsyncCall) {
            pipim->IPIM_pfuncAsyncCall(pipim->IPIM_pvAsyncArg);         /*  ִ���첽����                */
        }
    }
    
    KN_SMP_MB();
    if (pcpuCur->CPU_pringMsg == MX_NULL) {
        MX_CPU_CLR_IPI_PEND2(pcpuCur, MX_IPI_CALL_MSK);                 /*  ���                        */
    }
    
    spinUnlockIgnIrq(&pcpuCur->CPU_slIpi);                              /*  ���� CPU                    */
}
/*********************************************************************************************************
** ��������: _SmpProcIpi
** ��������: ����˼��ж� (���ﲻ�����������Ϣ)
** �䡡��  : pcpuCur       ��ǰ CPU
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _SmpIpiProc (PMX_PHYS_CPU  pcpuCur)
{
    INTREG  iregInterLevel;

    pcpuCur->CPU_iIpiCnt++;                                             /*  �˼��ж����� ++             */

#if 1
    //kernelEnter();
    if (MX_CPU_GET_IPI_PEND2(pcpuCur) & MX_IPI_IRQ_MSK) {
#if 0
        pcpuCur->CPU_pvcpuCur->VCPU_pfuncIsr(pcpuCur->CPU_pvcpuCur->VCPU_uiIrqPend,
                                             pcpuCur->CPU_pvcpuCur->VCPU_puctxSP);
        pcpuCur->CPU_pvcpuCur->VCPU_uiIrqPend = 0;
#endif
        //bspDebugMsg("-%d)", archMpCur());
        vcpuDoPendIsr(pcpuCur->CPU_pvcpuCur);
    }
    //kernelExit();
#endif

    if (MX_CPU_GET_IPI_PEND2(pcpuCur) & MX_IPI_SCHED_MSK) {             /*  ���Ⱥ˼��ж�                */
        MX_CPU_CLR_SCHED_IPI_PEND(pcpuCur);
    }

    if (MX_CPU_GET_IPI_PEND2(pcpuCur) & MX_IPI_CALL_MSK) {              /*  �Զ������ ?                */
        iregInterLevel = archIntDisable();
        __smpProcMsg(pcpuCur);
        archIntEnable(iregInterLevel);
    }
}
/*********************************************************************************************************
** ��������: _SmpIpiTryProc
** ��������: ���Դ���˼��ж� (�����ڹ��ж�����µ���, �����������ִ�� call ����)
** �䡡��  : pcpuCur       ��ǰ CPU
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _SmpIpiTryProc (PMX_PHYS_CPU  pcpuCur)
{
    if (MX_CPU_GET_IPI_PEND2(pcpuCur) & MX_IPI_CALL_MSK) {              /*  �Զ������ ?                */
        __smpProcMsg(pcpuCur);
    }
}
/*********************************************************************************************************
** ��������: _SmpIpiUpdate
** ��������: ����һ�� IPI
** �䡡��  : pcpuCur   CPU ���ƿ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _SmpIpiUpdate (PMX_PHYS_CPU  pcpu)
{
    if (!MX_CPU_IS_ACTIVE(pcpu)) {                                      /*  CPU ���뱻����              */
        return;
    }

    archMpInt(pcpu->CPU_ulCpuId);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

