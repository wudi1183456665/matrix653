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
** ��   ��   ��: irq.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 19 ��
**
** ��        ��: �жϹ���.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
  �����жϺŵ� VCPU ��ת����
*********************************************************************************************************/
static PMX_VCPU    _G_pvcpuVecMap[MX_CFG_MAX_IRQS];
static PMX_VCPU    _G_pvcpuTickAt[MX_CFG_MAX_VCPUS];
//static spinlock_t  _G_slVecMapLock;
/*********************************************************************************************************
** ��������: __irqVecToVcpu
** ��������: ���ҳ����жϺŶ�Ӧ�� VCPU �ṹ��
** �䡡��  : ulVec           �����жϺ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static PMX_VCPU  __irqVecToVcpu (ULONG  ulVec)
{
    return  (_G_pvcpuVecMap[ulVec]);
}
/*********************************************************************************************************
** ��������: irqRegister
** ��������: ע�ᵱǰ VCPU ���жϷ�����
** �䡡��  : pfuncIsr        �жϷ�����ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  irqRegister (IRQ_FUNCPTR  pfuncIsr)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();

    //bspDebugMsg("irqRegister(%p)\n", pfuncIsr);
    pvcpuCur->VCPU_pfuncIsr = pfuncIsr;
}
/*********************************************************************************************************
** ��������: irqAttach
** ��������: ��������жϺ�
** �䡡��  : ulVec           �����жϺ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_ERROR  irqAttach (ULONG  ulVec)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();
    INT         i;

    //bspDebugMsg("irqAttach(%d)\n", ulVec);
    if (ulVec != MX_CFG_TICK_VEC) {
        _G_pvcpuVecMap[ulVec] = pvcpuCur;

    } else {
        for (i = 0; i < MX_CFG_MAX_VCPUS; i++) {
            if (_G_pvcpuTickAt[i] == pvcpuCur) {
                return (ERROR_NONE);
            } else if (_G_pvcpuTickAt[i] == NULL) {
                _G_pvcpuTickAt[i] = pvcpuCur;
                return (ERROR_NONE);
            }
        }
        return (ERROR_VCPU_FULL);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: irqDisAttach
** ��������: �ջ��߳���������г����ж�
** �䡡��  : pvcpu          �߳̿��ƿ� 
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  irqDisAttach (PMX_VCPU  pvcpu)
{
    INT         i;

    for (i = 0; i < MX_CFG_MAX_IRQS; i++) {
        if (_G_pvcpuVecMap[i] == pvcpu) {
            _G_pvcpuVecMap[i] = MX_NULL;
        }
    }

    for (i = 0; i < MX_CFG_MAX_VCPUS; i++) {
        if (_G_pvcpuTickAt[i] == pvcpu) {
            _G_pvcpuTickAt[i] = MX_NULL;
        }
    }
} 
/*********************************************************************************************************
** ��������: irqEnable
** ��������: ����Ӳ��ʹ���ж�
** �䡡��  : ulVec           �����жϺ�    
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  irqEnable (ULONG  ulVec)
{
    if (ulVec != MX_CFG_TICK_VEC) {
        //bspDebugMsg("irqEnable(%d)\n", ulVec);
        bspIntVectorEnable(ulVec);
    }
}
/*********************************************************************************************************
** ��������: irqDisable
** ��������: ����Ӳ�������ж�
** �䡡��  : ulVec           �����жϺ�    
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  irqDisable (ULONG  ulVec)
{
    if (ulVec != MX_CFG_TICK_VEC) {
        bspIntVectorDisable(ulVec);
    }
}
/*********************************************************************************************************
** ��������: irqIpiVecSet
** ��������: ���� CPU �ĺ˼��ж��߼��жϺ�
** �䡡��  : ulCpuId         CPU ID
**           ulVec           �˼��ж��߼��жϺ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  irqIpiVecSet (ULONG  ulCpuId, ULONG  ulVec)
{
    PMX_PHYS_CPU    pcpu;

    if (ulCpuId < MX_CFG_MAX_PROCESSORS) {
        pcpu = MX_CPU_GET(ulCpuId);
        pcpu->CPU_ulIpiVec = ulVec;
    }
}
/*********************************************************************************************************
** ��������: irqIsr
** ��������: �ж��ܷ���
** �䡡��  : ulVec           �����жϺ�, ��Ч��Χ: 0 - 255 (arch �㺯����Ҫ��֤�˲�����ȷ)
** �䡡��  : �жϷ���ֵ
** ȫ�ֱ���: 
** ����ģ��: archIntHandle()
*********************************************************************************************************/
MX_ERROR  irqIsr (ULONG  ulVec, PARCH_USER_CTX  puctxSP)
{
    PMX_PHYS_CPU        pcpu;
    PMX_VCPU            pvcpu;
    INT                 i;
    
    //dump("-%d-", ulVec);

    pcpu = MX_CPU_GET_CUR();                                            /*  �жϴ��������, ����ı� CPU*/
    
    //__MX_CPU_INT_ENTER_HOOK(ulVec, pcpu->CPU_ulInterNesting);
    if (pcpu->CPU_ulIpiVec == ulVec) {                                  /*  �˼��ж�                    */
        _SmpIpiProc(pcpu);                                              /*  ����˼��ж�                */
        //dump("CPU %d Proc Ipi Vector %d\n", pcpu->CPU_ulCpuId, ulVec);

    } else {
        //spinLock(&_G_slVecMapLock);                                   /*  ��ס spinlock               */
        //kernelEnter();

        if (ulVec == MX_CFG_TICK_VEC) {
            for (i = 0; i < MX_CFG_MAX_VCPUS; i++) {
                pvcpu = _G_pvcpuTickAt[i];
                if (pvcpu != MX_NULL) {                                 /*  ���ж��ѱ������ĳ VCPU     */
                    vcpuOccurIrq(pvcpu, ulVec);
                } else {
                    break;
                }
            }
        } else {
            pvcpu = __irqVecToVcpu(ulVec);
            if (pvcpu != MX_NULL) {                                     /*  ���ж��ѱ������ĳ VCPU     */
                vcpuOccurIrq(pvcpu, ulVec);
            }
        }

        //kernelExit();
        //spinUnlock(&_G_slVecMapLock);                                 /*  ���� spinlock               */
    }
    //__MX_CPU_INT_EXIT_HOOK(ulVec, pcpu->CPU_ulInterNesting);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: irqEnter
** ��������: �ں��ж���ں��� (�ڹ��жϵ�����±�����)
** �䡡��  : NONE
** �䡡��  : �жϲ���
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
ULONG  irqEnter (VOID)
{
    PMX_PHYS_CPU  pcpu;

    pcpu = MX_CPU_GET_CUR();
    //bspDebugMsg("[%d]irqEnter() pcpu = %p\n", archMpCur(), pcpu);
    if (pcpu->CPU_ulInterNesting != MX_CFG_MAX_INTER_NESTING) {
        pcpu->CPU_ulInterNesting++;                                     /*  ϵͳ�ж�Ƕ�ײ���++          */
    }
    
    KN_SMP_WMB();                                                       /*  �ȴ����ϲ������            */

    return  (pcpu->CPU_ulInterNesting);
}
/*********************************************************************************************************
** ��������: irqExit
** ��������: �ں��жϳ��ں��� (�ڹ��жϵ�����±�����)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  irqExit (VOID)
{
    PMX_PHYS_CPU  pcpu;

    pcpu = MX_CPU_GET_CUR();
    //bspDebugMsg("[%d]irqExit()  pcpu = %p\n", archMpCur(), pcpu);
    //bspDebugMsg("           Nesting = %p\n", pcpu->CPU_ulInterNesting);

    if (pcpu->CPU_ulInterNesting) {                                     /*  ϵͳ�ж�Ƕ�ײ���--          */
        pcpu->CPU_ulInterNesting--;
    }

    KN_SMP_WMB();                                                       /*  �ȴ����ϲ������            */

    if (pcpu->CPU_ulInterNesting) {                                     /*  �鿴ϵͳ�Ƿ����ж�Ƕ����    */
        return;
    }

    kernelLockIgnIrq();
    schedule(); //scheduleInt();
    kernelUnlockIgnIrq();
}
/*********************************************************************************************************
** ��������: swiRegister
** ��������: ע�ᵱǰ VCPU �����жϷ�����
** �䡡��  : pfuncSwi        ���жϷ�����ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  swiRegister (UINT32  uiSwiNum, IRQ_FUNCPTR  pfuncSwi)
{
    PMX_VCPU    pvcpuCur = MX_VCPU_GET_CUR();

    pvcpuCur->VCPU_pfuncSwi  = pfuncSwi;
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
