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
** ��   ��   ��: sched.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 19 ��
**
** ��        ��: ΢�ں˵�����.

** BUG:
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** ��������: __schedIpiLowCandCpu
** ��������: ����Ҫ���ȵ� CPU ���ͺ˼��ж�(�� CPU ��Ҫ������������: 1. Cand ��Ч, 2. Cand ���ȼ����)
** �䡡��  : ulCpuIdCur       ��ǰ CPU ID
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
** ע  ��  : ����һ���˲����˵���������, ����û���ڴ��� sched �˼��ж�, 
             �����ѡ�������ȼ���͵� CPU ���ͺ˼��ж�.
*********************************************************************************************************/
static VOID  __schedIpiLowCandCpu (ULONG  ulCpuIdCur)
{
    INT             i;
    INT             iCpuIdSend;
    UINT32          uiPrioLow = 0;
    UINT32          uiPrioCand;
    PMX_PHYS_CPU    pcpu;
    
    MX_CPU_FOREACH_ACTIVE_EXCEPT (i, ulCpuIdCur) {                      /*  ���� CPU ����Ƿ���Ҫ����   */
        pcpu = MX_CPU_GET(i);
        if (MX_CAND_ROT(pcpu) &&
            ((MX_CPU_GET_IPI_PEND(i) & MX_IPI_SCHED_MSK) == 0)) {
            uiPrioCand = MX_CAND_VCPU(pcpu)->VCPU_uiPriority;
            if (MX_PRIO_IS_HIGH(uiPrioLow, uiPrioCand)) {
                uiPrioLow  = uiPrioCand;
                iCpuIdSend = i;
            }
        }
    }
    if (uiPrioLow) {
        _SmpIpiSend(iCpuIdSend, MX_IPI_SCHED, 0, MX_TRUE);                /*  �����˼��ж�                */
    }
}
/*********************************************************************************************************
** ��������: schedSwap
** ��������: �����˵ĵ�ǰ�̸߳�ֵΪ�Ѿ�ѡ�õ�������ȼ��߳�
** �䡡��  : pcpuCur   ��ǰ���� CPU
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: �� ARCH ��������л�������ʹ��: 1.������߳�������, 2.���ô˺���, 3.�ָ����߳�������
*********************************************************************************************************/
VOID  schedSwap (PMX_PHYS_CPU  pcpuCur)
{
    PMX_VCPU  pvcpuOld = pcpuCur->CPU_pvcpuCur;
    PMX_VCPU  pvcpuNew = pcpuCur->CPU_pvcpuHigh;

    pcpuCur->CPU_pvcpuCur = pvcpuNew;                                   /*  �л�����                    */

    //bspDebugMsg("schedSwap() old stack = 0x%X, new stack = 0x%X\n", pvcpuOld->VCPU_pstkKStackNow, pvcpuNew->VCPU_pstkKStackNow);

    if (pvcpuOld->VCPU_pprtnBelong != pvcpuNew->VCPU_pprtnBelong) {
        prtnMakeCur(pvcpuNew->VCPU_pprtnBelong);
    }
    kernelUnlockSched(pvcpuOld);                                        /*  �����ں� spinlock           */
}
/*********************************************************************************************************
** ��������: schedule
** ��������: ѡȡ��ѡ���е��߳�����. �ж��˳�ʱ, ����ô˵��Ⱥ��� (�����ں�״̬�����жϱ�����)
** �䡡��  : NONE
** �䡡��  : �߳������ķ���ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_ERROR  schedule (VOID)
{
    ULONG           ulCpuId;
    PMX_PHYS_CPU    pcpuCur;
    PMX_VCPU        pvcpuCur;
    PMX_VCPU        pvcpuCand;
    MX_ERROR        iRetVal = ERROR_NONE;

    ulCpuId   = MX_CPU_GET_CUR_ID();                                    /*  ��ǰ CPU ID                 */
    pcpuCur   = MX_CPU_GET_CUR();                                       /*  ��ǰ CPU ���ƿ�             */
    pvcpuCur  = pcpuCur->CPU_pvcpuCur;                                  /*  ��ǰ vCPU ���ƿ�            */

    //if (pvcpuCur == 0xC00BE000) bspDebugMsg(".");
    //if (archMpCur() != 0) { bspDebugMsg("schedule()\n"); } while (1);
    //if (pvcpuCur->VCPU_ulLockCounter == 0) bspDebugMsg("[%d][%p]\n", archMpCur(), pvcpuCur);

    pvcpuCand = _CandTableVcpuGet(pcpuCur);                             /*  �����Ҫ���е� vCPU         */
    //bspDebugMsg("CPU%d: Cur = %X, Cand = %X\n", ulCpuId, pvcpuCur, pvcpuCand);
    //bspDebugMsg("schedule() Cur stk = 0x%X, Cand stk = 0x%X\n", pvcpuCur->VCPU_pstkKStackNow, pvcpuCand->VCPU_pstkKStackNow);
    //bspDebugMsg("schedule() Cand cpsr = 0x%X\n", ((ARCH_KERN_CTX *)(pvcpuCand->VCPU_pstkKStackNow))->KCTX_uiCpsr);
    //bspDebugMsg("schedule() Cand pc   = 0x%X\n", ((ARCH_KERN_CTX *)(pvcpuCand->VCPU_pstkKStackNow))->KCTX_uiPc);
    //bspDebugMsg("schedule() Cand fp   = 0x%X\n", ((ARCH_KERN_CTX *)(pvcpuCand->VCPU_pstkKStackNow))->KCTX_uiFp);
    if (pvcpuCand != pvcpuCur) {                                        /*  ����뵱ǰ���еĲ�ͬ, �л�  */
        pcpuCur->CPU_bIsIntSwtich = MX_FALSE;                           /*  ���жϵ���                  */
        pcpuCur->CPU_pvcpuHigh    = pvcpuCand;


        /*
         *  VCPU CTX SAVE();
         *  schedSwap();
         *  VCPU CTX LOAD();
         */
        archVcpuCtxSwitch(pcpuCur);                                     /*  ARCH ��������л�           */
        kernelLockIgnIrq();                                             /*  �ں����������¼���          */
#if 0
        if (pvcpuCur->VCPU_uiIrqPend) {
            pvcpuCur->VCPU_pfuncIsr(pvcpuCur->VCPU_uiIrqPend,
                                    pvcpuCur->VCPU_puctxSP);
            pvcpuCur->VCPU_uiIrqPend = 0;
        }
#endif
    } else {                                                            /*  ����û�����л�, ֪ͨ������  */
        //bspDebugMsg("schedule++++++++++++++\n");
        __schedIpiLowCandCpu(ulCpuId);
        return  (iRetVal);
    }

    pvcpuCur = MX_VCPU_GET_CUR();                                       /*  ����µĵ�ǰ vCPU           */

    iRetVal = pvcpuCur->VCPU_iSchedRet;                                 /*  ��õ������źŵķ���ֵ      */
    pvcpuCur->VCPU_iSchedRet = ERROR_NONE;                              /*  ���                        */

    return  (iRetVal);
}
/*********************************************************************************************************
** ��������: schedTick
** ��������: ʱ��Ƭ���� (tick �жϷ�������б�����, �����ں��ҹر��ж�״̬)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  schedTick (VOID)
{
    REGISTER PMX_PHYS_CPU   pcpu;
    REGISTER PMX_VCPU       pvcpu;
             UINT32         uiRdyPrio;
             UINT32         uiCurPrio;
             INT            i;
             INTREG         iregInterLevel;
    iregInterLevel = kernelEnterAndDisIrq();                            /*  �����ں�ͬʱ�ر��ж�        */

    _WakeupTick(1);                                                     /*  ���»��ѱ�                  */

#if 1
    /*
     *  �����ȼ����Ʊ�־��֧��ͬ���ȼ���ת����
     */
    MX_CPU_FOREACH_ACTIVE (i) {
        pcpu      = MX_CPU_GET(i);
        pvcpu     = pcpu->CPU_pvcpuCur;
        uiCurPrio = pvcpu->VCPU_uiPriority;
        if (pvcpu->VCPU_usSchedCounter == 0) {                          /*  ��ǰ�߳�ʱ��Ƭ�Ѿ��ľ�      */
            if (MX_CAND_ROT(pcpu) == MX_FALSE) {                        /*  ��ת��־δ��                */
                if (_ReadyTableGet(pcpu, &uiRdyPrio) &&
                    MX_PRIO_IS_HIGH_OR_EQU(uiRdyPrio, uiCurPrio)) {     /*  ����δ���������������ȼ�  */

                    MX_CAND_ROT(pcpu) = MX_TRUE;                        /*  �´ε���ʱ�����ת          */
                }
            }
        } else {
            pvcpu->VCPU_usSchedCounter--;
        }
    }
#else
    __asm__ __volatile__ ("NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t"
            "NOP \n\t");
#endif

    kernelExitAndEnIrq(iregInterLevel);                                 /*  �˳��ں�ͬʱ���ж�        */
}
/*********************************************************************************************************
** ��������: _SchedYield
** ��������: ָ���߳������ó� CPU ʹ��Ȩ, (ͬ���ȼ���) (�˺���������ʱ�ѽ����ں����ж��Ѿ��ر�)
** �䡡��  : pvcpu          �������е��߳�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  schedYield (PMX_VCPU  pvcpu)
{
    REGISTER PMX_PHYS_CPU   pcpu;
             UINT32         uiPriority;
             INTREG         iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();                            /*  �����ں�ͬʱ�ر��ж�        */

    if (MX_VCPU_IS_RUNNING(pvcpu)) {                                    /*  ��������ִ��                */
        pcpu = MX_CPU_GET(pvcpu->VCPU_ulCpuIdRun);
        if (_ReadyTableGet(pcpu, &uiPriority) &&                        /*  ����δ���������������ȼ�  */
            MX_PRIO_IS_HIGH_OR_EQU(uiPriority,
                                   pvcpu->VCPU_uiPriority)) {
            pvcpu->VCPU_usSchedCounter = 0;                             /*  û��ʣ��ʱ��Ƭ              */
            MX_CAND_ROT(MX_CPU_GET(pvcpu->VCPU_ulCpuIdRun)) = MX_TRUE;  /*  �´ε���ʱ�����ת          */
        }
    }

    kernelExitAndEnIrq(iregInterLevel);                                 /*  �˳��ں�ͬʱ���ж�        */ 
}
/*********************************************************************************************************
** ��������: schedActiveCpu
** ��������: ���� CPU, ʹ����������������
** �䡡��  : pcpu      CPU �ṹ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  schedActiveCpu (PMX_PHYS_CPU  pcpu)
{
    pcpu->CPU_ulStatus |= MX_CPU_STATUS_ACTIVE;

    KN_SMP_MB(); 

    _CandTableUpdate(pcpu);                                             /*  ���º�ѡ�߳�                */
    pcpu->CPU_pvcpuCur  = MX_CAND_VCPU(pcpu);
    pcpu->CPU_pvcpuHigh = MX_CAND_VCPU(pcpu);
    bspDebugMsg("-------- CPU%d Start vCPU %X ", pcpu->CPU_ulCpuId, pcpu->CPU_pvcpuCur);
    bspDebugMsg("-- Belong to %X ------ \n", pcpu->CPU_pvcpuCur->VCPU_pprtnBelong);

    prtnMakeCur(pcpu->CPU_pvcpuCur->VCPU_pprtnBelong);

#if 0
    ARCH_KERN_CTX  *pregctx = pcpu->CPU_pvcpuCur->VCPU_pstkKStackNow;
    bspDebugMsg("  sp : %X\n", pregctx);
    bspDebugMsg("  Fp : %X\n", pregctx->KCTX_uiFp);
    bspDebugMsg("Cpsr : %X\n", pregctx->KCTX_uiCpsr);
    bspDebugMsg("  Pc : %X\n", pregctx->KCTX_uiPc);
#endif

    KN_SMP_MB(); 

    bspCpuUpDone();                                                     /*  ���˳�ʼ�����, ���ȴ�������*/
}
/*********************************************************************************************************
** ��������: __schedIdle
** ��������: Idle �߳�
** �䡡��  : pvArg      CPU ID
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static PVOID  __schedIdle (PVOID  pvArg)
{
    //ULONG  ulCpuId = (ULONG)pvArg;
    
    for (;;) {
        //__MX_VCPU_IDLE_HOOK(ulCpuId);
        //bspDebugMsg("Idle %d\n", ulCpuId);
        //bspDelayUs(1000000);
        //dumpHook();
        (*(volatile int *)(0x02020098)); /* ���� */
     }

    return  (MX_NULL);
}
/*********************************************************************************************************
** ��������: schedInit
** ��������: �ں˶���ģ���ʼ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  schedInit (VOID)
{
    INT             i;
    PMX_PHYS_CPU    pcpu;
    MX_VCPU_ATTR    vcpuattr;
    PMX_VCPU        pvcpuIdle;

    MX_CPU_FOREACH (i) {
        pcpu = MX_CPU_GET(i);

        pcpu->CPU_ulCpuId = (ULONG)i;                                   /*  ���� CPU ID                 */

        /*
         *  ���� Idle �߳�
         */
        vcpuattr.VCPUATTR_uiPriority = MX_PRIO_LOWEST;
        vcpuattr.VCPUATTR_pvArg      = (PVOID)i;
        vcpuattr.VCPUATTR_bIsUser    = MX_FALSE;

        pvcpuIdle = vcpuCreate(__schedIdle, &vcpuattr);                 /*  ��������߳� VCPU �ṹ��    */

        vcpuBindPartition(pvcpuIdle, _K_pprtnKernel);                   /*  �������ں˽���              */
        vcpuSetAffinity(pvcpuIdle, i);                                  /*  �׺������� CPU              */
        vcpuStart(pvcpuIdle);
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
