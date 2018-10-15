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
** ��   ��   ��: k_vcpu.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 11 �� 28 ��
**
** ��        ��: ����΢�ں��߳����Ͷ����ļ�.
*********************************************************************************************************/

#ifndef __K_VCPU_H
#define __K_VCPU_H

/*********************************************************************************************************
  �߳���ں������Ͷ���
*********************************************************************************************************/
typedef PVOID           (*PVCPU_START_ROUTINE)(PVOID  pvArg);           /*  ϵͳ�߳����Ͷ���            */
/*********************************************************************************************************
  ���ѱ�ڵ� (���ʱ��)
*********************************************************************************************************/
typedef struct {
    MX_LIST_LINE          WUN_lineManage;                               /*  ɨ������                    */
    BOOL                  WUN_bInQ;                                     /*  �Ƿ���ɨ������              */
    ULONG                 WUN_ulCounter;                                /*  ��Եȴ�ʱ��                */
} MX_WAKEUP_NODE;
typedef MX_WAKEUP_NODE   *PMX_WAKEUP_NODE;
/*********************************************************************************************************
  �߳̿��ƿ�
*********************************************************************************************************/
typedef struct __mx_vcpu {
    PMX_STACK             VCPU_pstkKStackNow;                           /*  �̵߳�ǰ��ջָ��            */
    PMX_STACK             VCPU_pstkUStackStart;                         /*  �߳��û���ջ��ʼֵ          */
    UINT32                VCPU_uiPriority;                              /*  �߳����ȼ�                  */
    UINT32                VCPU_uiStatus;                                /*  �̵߳�ǰ״̬                */

    PVCPU_START_ROUTINE   VCPU_pfuncStart;                              /*  �߳���ں���                */
    PVOID                 VCPU_pvArg;                                   /*  �߳���ڲ���                */
    BOOL                  VCPU_bIsUser;                                 /*  �Ƿ����û��߳�              */

    MX_LIST_RING          VCPU_ringReady;                               /*  ���ȼ����ƿ��л���ڵ�      */
    MX_WAKEUP_NODE        VCPU_wunDelay;                                /*  �ȴ����Ա��нڵ�            */
#define VCPU_ulDelay      VCPU_wunDelay.WUN_ulCounter

    BOOL                  VCPU_bCpuAffinity;                            /*  �Ƿ��׺����� CPU            */
    ULONG                 VCPU_ulCpuIdAffinity;                         /*  �׺����е� CPU ID           */

    volatile ULONG        VCPU_ulCpuIdRun;                              /*  �������е� CPU ID           */
    volatile BOOL         VCPU_bIsCand;                                 /*  �Ƿ��ں�ѡ���б���          */

    INT                   VCPU_iSchedRet;                               /*  ���������ص�ֵ, signal      */
    UINT8                 VCPU_ucSchedPolicy;                           /*  ���Ȳ���                    */
    volatile ULONG        VCPU_ulLockCounter;                           /*  �߳�����������, ������ֵ����*/
                                                                        /*  1 ʱ, ���̲߳�������� CPU  */

    UINT16                VCPU_usSchedSlice;                            /*  �߳�ʱ��Ƭ����ֵ            */
    UINT16                VCPU_usSchedCounter;                          /*  �̵߳�ǰʣ��ʱ��Ƭ          */

    PMX_PRTN              VCPU_pprtnBelong;                             /*  ���߳����ڵĽ���            */

    MX_SC_PARAM           VCPU_scparam;                                 /*  ϵͳ���ò����ṹ��          */
    MX_IPC                VCPU_ipc;                                     /*  IPC ���ƿ�                  */

    volatile ULONG        VCPU_ulStatus;                                /*  VCPU ״̬                   */
#define VCPU_STAT_DIS_INT 0x00000001                                    /*  ���߼��ж�״̬              */

#if 1
    UINT32                VCPU_uiIrqMask[8];                            /*  �߼��ж� Mask λ            */
    UINT32                VCPU_uiIrqPend[8];                            /*  �߼��ж� Pend λ            */
#else
    UINT32                VCPU_uiIrqPend;
#endif
    PARCH_USER_CTX        VCPU_puctxSP;                                 /*  �жϷ���ʱջ�е��û�������  */
    IRQ_FUNCPTR           VCPU_pfuncIsr;                                /*  �жϷ��������            */
    IRQ_FUNCPTR           VCPU_pfuncSwi;                                /*  ϵͳ���ú������            */

    MX_OBJECT_ID          VCPU_ulVcpuId;                                /*  ���߳��ں˶��� ID           */

    BOOL                  VCPU_bIsUsed;                                 /*  ���ṹ���Ƿ�����ʹ��        */
    //ARCH_REG_CTX        VCPU_regctx;
} MX_VCPU;
typedef MX_VCPU          *PMX_VCPU;
/*********************************************************************************************************
  �߳����Կ�
*********************************************************************************************************/
typedef struct {
    PMX_STACK             VCPUATTR_pstkLowAddr;                         /*  ȫ����ջ�����ڴ���ʼ��ַ    */
    ULONG                 VCPUATTR_stStackByteSize;                     /*  ȫ����ջ����С(�ֽ�)        */
    UINT32                VCPUATTR_uiPriority;                          /*  �߳����ȼ�                  */
    ULONG                 VCPUATTR_ulOption;                            /*  ����ѡ��                    */
    PVOID                 VCPUATTR_pvArg;                               /*  �̲߳���                    */
    BOOL                  VCPUATTR_bIsUser;                             /*  �Ƿ����û��߳�              */
} MX_VCPU_ATTR;
typedef MX_VCPU_ATTR     *PMX_VCPU_ATTR;
/*********************************************************************************************************
  MX_VCPU ���ں˶�ջ��������
*********************************************************************************************************/
typedef union {
    MX_VCPU               VCPUKSTACK_vcpu;
    UINT8                 VCPUKSTACK_uiKernelStack[MX_CFG_KSTACK_SIZE];
} MX_VCPU_KSTACK;
/*********************************************************************************************************
  �߳�״̬
*********************************************************************************************************/
#define  MX_VCPU_STATUS_READY         0x0000                            /*  �������                    */
#define  MX_VCPU_STATUS_DELAY         0x0001                            /*  �ӳ�                        */
#define  MX_VCPU_STATUS_IPC           0x0002                            /*  �ȴ� IPC ͨ��               */
#define  MX_VCPU_STATUS_INIT          0x0040                            /*  ��ʼ��                      */
#define  MX_VCPU_STATUS_SUSPEND       0x0080                            /*  ���񱻹���                  */

#define  MX_VCPU_IS_RUNNING(pvcpu)    (MX_CPU_GET(pvcpu->VCPU_ulCpuIdRun)->CPU_pvcpuCur == pvcpu)
#endif                                                                  /*  __K_VCPU_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/

