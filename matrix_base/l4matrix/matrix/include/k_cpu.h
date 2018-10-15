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
** ��   ��   ��: k_cpu.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 06 ��
**
** ��        ��: CPU ��Ϣ����.
*********************************************************************************************************/

#ifndef __K_CPU_H
#define __K_CPU_H

/*********************************************************************************************************
  CPU �ṹ (Ҫ�� CPU ID ��Ŵ� 0 ��ʼ���������������)
*********************************************************************************************************/
typedef struct {
    /*
     *  �����߳����
     */
    volatile PMX_VCPU        CPU_pvcpuCur;                              /*  ������ CPU ��ǰ���е� VCPU  */
    volatile PMX_VCPU        CPU_pvcpuHigh;                             /*  ��Ҫ���еĸ����� VCPU       */

    /*
     *  ��ǰ�������ȵĵ��ȷ�ʽ
     */
    BOOL                     CPU_bIsIntSwtich;                          /*  �Ƿ�Ϊ�жϵ���              */

    /*
     *  �ں�����״̬
     */
    volatile INT             CPU_iKernelCounter;                        /*  �ں�״̬������              */

    /*
     *  ��ѡ���нṹ
     */
    volatile MX_CAND         CPU_cand;                                  /*  ��ѡ���е��߳�              */

    /*
     *  ��ǰ�˾�����(��ź͵�ǰ�����׺��Ե��߳�)
     */
    MX_READY                 CPU_ready;                                 /*  ��ǰ CPU ������             */

    /*
     *  �˼��жϴ���������
     */
    spinlock_t               CPU_slIpi;                                 /*  �˼��ж���                  */
    PMX_LIST_RING            CPU_pringMsg;                              /*  �Զ���˼��жϲ�����        */
    volatile UINT            CPU_uiMsgCnt;                              /*  �Զ���˼��ж�����          */
    
    ULONG                    CPU_ulIpiVec;                              /*  �˼��ж��߼��жϺ�          */
    INT_FUNCPTR              CPU_pfuncIpiClear;                         /*  �˼��ж��������            */
    PVOID                    CPU_pvIpiArg;                              /*  �˼��ж��������            */
    
    /*
     *  �˼��жϴ������־, ��������� ULONG λ�����˼��ж�����, �� CPU Ӳ���ж�����ԭ����ͬ
     */
    INT64                    CPU_iIpiCnt;                               /*  �˼��жϴ���                */
    volatile ULONG           CPU_ulIpiPend;                             /*  �˼��жϱ�־��              */
#define MX_IPI_NOP              0                                       /*  �����ú˼��ж�����          */
#define MX_IPI_SCHED            1                                       /*  ��������                    */
#define MX_IPI_IRQ              2                                       /*  �߼��ж�����                */
#define MX_IPI_CALL             3                                       /*  �Զ������ (�в�����ѡ�ȴ�) */


#define MX_IPI_NOP_MSK          (1 << MX_IPI_NOP)
#define MX_IPI_SCHED_MSK        (1 << MX_IPI_SCHED)
#define MX_IPI_IRQ_MSK          (1 << MX_IPI_IRQ)
#define MX_IPI_CALL_MSK         (1 << MX_IPI_CALL)

    /*
     *  CPU ������Ϣ
     */
             ULONG           CPU_ulCpuId;                               /*  CPU ID ��                   */
    volatile ULONG           CPU_ulStatus;                              /*  CPU ����״̬(Ŀǰֻ֧�ּ���)*/
#define MX_CPU_STATUS_ACTIVE    0x80000000                              /*  CPU ����                    */
#define MX_CPU_STATUS_INACTIVE  0x00000000                              /*  CPU δ����                  */
    
    /*
     *  �ж���Ϣ
     */
             PMX_STACK       CPU_pstkInterBase;                         /*  �ж϶�ջ��ַ                */
    volatile ULONG           CPU_ulInterNesting;                        /*  �ж�Ƕ�׼�����              */
    volatile ULONG           CPU_ulInterNestingMax;                     /*  �ж�Ƕ�����ֵ              */

    /*
     *  ������Ϣ
     */
    volatile PMX_PRTN        CPU_pprtnCur;                              /*  ������ CPU ��ǰ���еĽ���   */

} MX_PHYS_CPU;
typedef MX_PHYS_CPU         *PMX_PHYS_CPU;
/*********************************************************************************************************
  CPU ����״̬
*********************************************************************************************************/
#define MX_CPU_IS_ACTIVE(pcpu)          ((pcpu)->CPU_ulStatus & MX_CPU_STATUS_ACTIVE)
/*********************************************************************************************************
  ��ǰ CPU ��Ϣ MX_NCPUS �����ܴ��� MX_CFG_MAX_PROCESSORS
*********************************************************************************************************/
//ULONG   archMpCur(VOID);
//#define MX_CPU_GET_CUR_ID()             archMpCur()                     /*  ��õ�ǰ CPU ID             */
#define MX_NCPUS                        (_K_ulNCpus)
/*********************************************************************************************************
  CPU �����
*********************************************************************************************************/
extern  MX_PHYS_CPU   _K_cpuTable[];                                    /*  CPU ��                      */
//#define MX_CPU_GET_CUR()                (&_K_cpuTable[archMpCur()])/*  ��õ�ǰ CPU �ṹ        */
#define MX_CPU_GET(id)                  (&_K_cpuTable[(id)])            /*  ���ָ�� CPU �ṹ           */
/*********************************************************************************************************
  CPU ����
*********************************************************************************************************/
#define MX_CPU_FOREACH(i)                       \
        for (i = 0; i < MX_NCPUS; i++)
        
#define MX_CPU_FOREACH_ACTIVE(i)                \
        for (i = 0; i < MX_NCPUS; i++)          \
        if (MX_CPU_IS_ACTIVE(MX_CPU_GET(i)))
                                    
#define MX_CPU_FOREACH_EXCEPT(i, id)            \
        for (i = 0; i < MX_NCPUS; i++)          \
        if ((i) != (id))
        
#define MX_CPU_FOREACH_ACTIVE_EXCEPT(i, id)     \
        for (i = 0; i < MX_NCPUS; i++)          \
        if ((i) != (id))                        \
        if (MX_CPU_IS_ACTIVE(MX_CPU_GET(i)))

/*********************************************************************************************************
  CPU �˼��ж�
*********************************************************************************************************/
#define MX_CPU_ADD_IPI_PEND(id, ipi_msk)    \
        (_K_cpuTable[(id)].CPU_ulIpiPend |= (ipi_msk))                  /*  ����ָ�� CPU �˼��ж� pend  */
#define MX_CPU_CLR_IPI_PEND(id, ipi_msk)    \
        (_K_cpuTable[(id)].CPU_ulIpiPend &= ~(ipi_msk))                 /*  ���ָ�� CPU �˼��ж� pend  */
#define MX_CPU_GET_IPI_PEND(id)             \
        (_K_cpuTable[(id)].CPU_ulIpiPend)                               /*  ��ȡָ�� CPU �˼��ж� pend  */
        
#define MX_CPU_ADD_IPI_PEND2(pcpu, ipi_msk)    \
        (pcpu->CPU_ulIpiPend |= (ipi_msk))                              /*  ����ָ�� CPU �˼��ж� pend  */
#define MX_CPU_CLR_IPI_PEND2(pcpu, ipi_msk)    \
        (pcpu->CPU_ulIpiPend &= ~(ipi_msk))                             /*  ���ָ�� CPU �˼��ж� pend  */
#define MX_CPU_GET_IPI_PEND2(pcpu)             \
        (pcpu->CPU_ulIpiPend)                                           /*  ��ȡָ�� CPU �˼��ж� pend  */
/*********************************************************************************************************
  CPU ������������ж� (���ж�����±�����)
*********************************************************************************************************/
#define MX_CPU_CLR_SCHED_IPI_PEND(pcpu)                             \
        do {                                                        \
            if (MX_CPU_GET_IPI_PEND2(pcpu) & MX_IPI_SCHED_MSK) {    \
                spinLockIgnIrq(&pcpu->CPU_slIpi);               \
                MX_CPU_CLR_IPI_PEND2(pcpu, MX_IPI_SCHED_MSK);       \
                spinUnlockIgnIrq(&pcpu->CPU_slIpi);             \
            }                                                       \
            archSpinNotify();                                       \
        } while (0)
/*********************************************************************************************************
  CPU �˼��ж�����
*********************************************************************************************************/
#define MX_CPU_GET_IPI_CNT(id)          (_K_cpuTable[(id)].CPU_iIpiCnt)
/*********************************************************************************************************
  CPU �ж���Ϣ
*********************************************************************************************************/
#define MX_CPU_GET_NESTING(id)          (_K_cpuTable[(id)].CPU_ulInterNesting)
#define MX_CPU_GET_NESTING_MAX(id)      (_K_cpuTable[(id)].CPU_ulInterNestingMax)

ULONG  _CpuGetNesting(VOID);
ULONG  _CpuGetMaxNesting(VOID);
#define MX_CPU_GET_CUR_NESTING()        _CpuGetNesting()
#define MX_CPU_GET_CUR_NESTING_MAX()    _CpuGetMaxNesting()

#endif                                                                  /*  __K_CPU_H                   */
/*********************************************************************************************************
  END
*********************************************************************************************************/

