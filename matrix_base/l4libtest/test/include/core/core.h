/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: api.h
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 1 ��
**
** ��        ��: Ӧ������ʱ��������
*********************************************************************************************************/
#ifndef __T_CORE_H
#define __T_CORE_H
/*********************************************************************************************************
  �궨��
*********************************************************************************************************/
/*
 * ���ȼ�
 */
#define  TVCPU_PRIO_HIGH        150
#define  TVCPU_PRIO_NORMAL      200
#define  TVCPU_PRIO_LOW         250
/*********************************************************************************************************
  exec
*********************************************************************************************************/
struct t_vcpu_stk {
    addr_t      addrMsg;
    addr_t      addrStkTop;                                             /*  ��������ջ��������ȷ��ջ��  */
                                                                        /*  ��ַ                        */
} __attribute__((aligned(T__USE_STK_SIZE)));
typedef struct t_vcpu_stk       T_VCPU_STK;
typedef        T_VCPU_STK      *PT_VCPU_STK;

VOID        T_vcpuStkInit(PT_VCPU_STK  pStk, PMX_IPC_MSG  pMsg);
PT_VCPU_STK T_vcpuGetCurStk();
PMX_IPC_MSG T_vcpuGetCurMsg();

#define T_GET_KERN_MSG()                                \
        (IPC_VCPU_TO_IPC(vcpuGetCur())->IPC_pMsg)
/*********************************************************************************************************
  VCPU
*********************************************************************************************************/
typedef struct {
    MX_OBJECT_ID            objPt;                                      /*  ���� ID */
    MX_OBJECT_ID            objVcpu;                                    /*  �߳� ID */
    PVCPU_START_ROUTINE     pfuncStart;                                 /*  �̺߳��� */
    PVOID                   pvArg;                                      /*  �̺߳������� */
    UINT32                  uiPriority;                                 /*  �߳����ȼ�  */
    addr_t                  addrStack;
    PMX_IPC_MSG             pIpcMsg;
} T_VCPU;
typedef T_VCPU      *PT_VCPU;

PT_VCPU T_vcpuCreate(MX_OBJECT_ID  objPt, PVCPU_START_ROUTINE  pRount,
                     UINT32  uiPriority);
BOOL    T_vcpuStart(PT_VCPU  pVcpu);
VOID    T_vcpuSleep(SC_TIME  time);
VOID    T_vcpuWakeup(MX_OBJECT_ID  objVcpu);
/*********************************************************************************************************
  IPC
*********************************************************************************************************/



#endif                                                                  /*  __T_CORE_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/