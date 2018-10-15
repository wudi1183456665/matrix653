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
** ��   ��   ��: spin.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 01 �� 27 ��
**
** ��        ��: �� CPU ϵͳ������.
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** ��������: spinInit
** ��������: ��������ʼ��
** �䡡��  : psl           ������
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  spinInit (spinlock_t  *psl)
{
    archSpinInit(psl);
    KN_SMP_WMB();
}
/*********************************************************************************************************
** ��������: spinTryLock
** ��������: ���������Լ�������
** �䡡��  : psl           ������
** �䡡��  : MX_TRUE �������� MX_FALSE ����ʧ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
BOOL  spinTryLock (spinlock_t  *psl)
{
    INTREG          iregInterLevel;
    PMX_PHYS_CPU    pcpuCur;
    INT             iRet;
    
    iregInterLevel = archIntDisable();
    
    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter++;                        /*  ���������ڵ�ǰ CPU          */
    
    iRet = archSpinTryLock(psl);
    KN_SMP_MB();
    
    if (iRet != MX_SPIN_OK) {
        pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                    /*  ����ʧ��, �����������      */
    }
    
    archIntEnable(iregInterLevel);
    
    return  ((iRet == MX_SPIN_OK) ? (MX_TRUE) : (MX_FALSE));
}
/*********************************************************************************************************
** ��������: spinLockIgnIrq
** ��������: ��������������, �����ж����� (�������жϹرյ�״̬�±�����)
** �䡡��  : psl           ������
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  spinLockIgnIrq (spinlock_t *psl)
{
    PMX_PHYS_CPU    pcpuCur;

    archSpinLock(psl);                                                  /*  ARCH �㱣֤�����ɹ�         */

    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter++;                        /*  ���������ڵ�ǰ CPU          */
    KN_SMP_MB();
}
/*********************************************************************************************************
** ��������: spinLockAndIrq
** ��������: ��������������, ��ͬ�����ж�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  spinLockAndDisIrq (spinlock_t *psl, INTREG  *piregInterLevel)
{
    *piregInterLevel = archIntDisable();

    spinLockIgnIrq(psl);
}
/*********************************************************************************************************
** ��������: spinLock
** ��������: ��������������
** �䡡��  : psl           ������
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  spinLock (spinlock_t  *psl)
{
    INTREG          iregInterLevel;

    spinLockAndDisIrq(psl, &iregInterLevel);                            /*  ���������������ж�          */
    
    archIntEnable(iregInterLevel);                                      /*  ���ж�                      */
}
/*********************************************************************************************************
** ��������: spinUnlockIgnIrq
** ��������: ��������������, �����ж����� (�������жϹرյ�״̬�±�����)
** �䡡��  : psl           ������
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  spinUnlockIgnIrq (spinlock_t *psl)
{
    PMX_PHYS_CPU    pcpuCur;
    INT             iRet;
    
    KN_SMP_MB();
    iRet = archSpinUnlock(psl);
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);
    
    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                        /*  �����������                */
}
/*********************************************************************************************************
** ��������: spinUnlockIrq
** ��������: ��������������, ��ͬ�����ж�
** �䡡��  : psl               ������
**           iregInterLevel    �ж�������Ϣ
** �䡡��  : ����������ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_ERROR  spinUnlockAndEnIrq (spinlock_t *psl, INTREG  iregInterLevel)
{
    PMX_PHYS_CPU    pcpuCur;
    BOOL            bTrySched = MX_FALSE;
    INT             iRet;
    
    KN_SMP_MB();
    iRet = archSpinUnlock(psl);
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);
    
    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                        /*  �����������                */
    if (pcpuCur->CPU_ulInterNesting == 0 &&
        pcpuCur->CPU_iKernelCounter == 0 &&
        pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter == 0 &&
        MX_CAND_ROT(pcpuCur)) {

        bTrySched = MX_TRUE;                                            /*  ��Ҫ���Ե���                */
    }
    
    archIntEnable(iregInterLevel);
    
    if (bTrySched) {
        //K_WARNING("spinUnlockAndEnIrq: KERN SCHED");
        return  (kernelSched());
    
    } else {
        return  (ERROR_NONE);
    }
}
/*********************************************************************************************************
** ��������: spinUnlock
** ��������: ��������������
** �䡡��  : psl           ������
** �䡡��  : ����������ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_ERROR  spinUnlock (spinlock_t  *psl)
{
    INTREG          iregInterLevel;
    
    iregInterLevel = archIntDisable();
    
    return  (spinUnlockAndEnIrq(psl, iregInterLevel));
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

