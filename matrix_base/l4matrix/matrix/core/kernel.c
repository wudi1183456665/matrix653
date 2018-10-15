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
** ��   ��   ��: kernel.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 01 �� 03 ��
**
** ��        ��: ����ϵͳ�ں�״̬����.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** ��������: __disIrqAndLockKernel
** ��������: �ر��ж�, �����ں�����������
** �䡡��  : piregInterLevel   �ж�������Ϣ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __disIrqAndLockKernel (INTREG  *piregInterLevel)
{
    PMX_PHYS_CPU     pcpuCur;
    INT              iRet;

    for (;;) {
        *piregInterLevel = archIntDisable();
        
        iRet = archSpinTryLock(&_K_klKernel.KERN_slLock); 
        if (iRet != MX_SPIN_OK) {
            _SmpIpiTryProc(MX_CPU_GET_CUR());                           /*  ����ִ�� IPI                */
            archIntEnable(*piregInterLevel);
            archSpinDelay();
            //bspDebugMsg("x");
            
        } else {
            //bspDebugMsg("+");
            pcpuCur = MX_CPU_GET_CUR();
            pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter++;                /*  ���������ڵ�ǰ CPU          */
            KN_SMP_MB();
            break;
        }
    }
}
/*********************************************************************************************************
** ��������: __unlockKernelAndEnIrq
** ��������: �ں�����������, �����ж�
** �䡡��  : iregInterLevel    �ж�������Ϣ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __unlockKernelAndEnIrq (INTREG  iregInterLevel)
{
    PMX_PHYS_CPU    pcpuCur;
    INT             iRet;

    KN_SMP_MB();
    iRet = archSpinUnlock(&_K_klKernel.KERN_slLock);                    /*  �����ں�������              */
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);

    //bspDebugMsg("-");
    
    pcpuCur = MX_CPU_GET_CUR();
    //bspDebugMsg("[%p]ulLockCounter--\n", pcpuCur->CPU_pvcpuCur);
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                        /*  �����������                */

    archIntEnable(iregInterLevel);                                      /*  �����ж�                    */
}
/*********************************************************************************************************
** ��������: kernelEnterAndDisIrq
** ��������: �����ں�״̬, ����󱣳��жϹر�״̬
** �䡡��  : NONE
** �䡡��  : �жϼĴ���
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INTREG  kernelEnterAndDisIrq (VOID)
{
    INTREG         iregInterLevel;
    PMX_PHYS_CPU   pcpuCur;
    
    __disIrqAndLockKernel(&iregInterLevel);                             /*  �ر��ж�, �����ں� spinlock */

    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_iKernelCounter++;                                      /*  �� CPU �����ں�״̬         */
    KN_SMP_WMB();                                                       /*  �ȴ����ϲ������            */

    return  (iregInterLevel);                                           /*  ���ж�                    */
}
/*********************************************************************************************************
** ��������: kernelExitAndEnIrq
** ��������: �˳��ں�״̬
** �䡡��  : iregInterLevel     �жϼĴ���
** �䡡��  : ����������ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  kernelExitAndEnIrq (INTREG  iregInterLevel)
{
    PMX_PHYS_CPU    pcpuCur;
    INT             iRetVal;
    
    pcpuCur = MX_CPU_GET_CUR();
    if (pcpuCur->CPU_iKernelCounter) {
        pcpuCur->CPU_iKernelCounter--;                                  /*  �� CPU �˳��ں�״̬         */
        KN_SMP_WMB();                                                   /*  �ȴ����ϲ������            */

        if (pcpuCur->CPU_iKernelCounter == 0) {

            iRetVal = schedule();                                       /*  ���Ե���                    */
            __unlockKernelAndEnIrq(iregInterLevel);                     /*  �����ں� spinlock �����ж�*/
            
            return  (iRetVal);
        }
    }
    
    __unlockKernelAndEnIrq(iregInterLevel);                             /*  �����ں� spinlock �����ж�*/
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: kernelEnter
** ��������: �����ں�״̬, ������жϴ�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  kernelEnter (VOID)
{
    INTREG         iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();
    archIntEnable(iregInterLevel);                                      /*  ���ж�                    */
}
/*********************************************************************************************************
** ��������: kernelExit
** ��������: �˳��ں�״̬
** �䡡��  : NONE
** �䡡��  : ����������ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  kernelExit (VOID)
{
    INTREG         iregInterLevel;

    iregInterLevel = archIntDisable();                                  /*  �ر��ж�                    */
    return  (kernelExitAndEnIrq(iregInterLevel));
}
/*********************************************************************************************************
** ��������: kernelUnlockSched
** ��������: �ں� SMP �������л���ɺ�ר���ͷź��� (���ж�״̬�±�����)
** �䡡��  : pvcpuOwner     ���ĳ�����
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: schedSwap()
*********************************************************************************************************/
VOID  kernelUnlockSched (PMX_VCPU  pvcpuOwner)
{
    INT            iRet;

    KN_SMP_MB();
    iRet = archSpinUnlock(&_K_klKernel.KERN_slLock);
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);

    pvcpuOwner->VCPU_ulLockCounter--;                                   /*  ������������ڵ�ǰ CPU      */
}
/*********************************************************************************************************
** ��������: kernelLockIgnIrq
** ��������: �ں���������������, �����ж����� (�������жϹرյ�״̬�±�����)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: irqExit(), schedule()
*********************************************************************************************************/
 VOID  kernelLockIgnIrq (VOID)
{
    PMX_PHYS_CPU     pcpuCur;
    INT              iRet;

    pcpuCur  = MX_CPU_GET_CUR();

    iRet     = archSpinTryLock(&_K_klKernel.KERN_slLock);               /*  �״γ��������ں�������      */
    while (iRet != MX_SPIN_OK) {
        _SmpIpiTryProc(MX_CPU_GET_CUR());                               /*  ����ִ�� IPI                */

        iRet = archSpinTryLock(&_K_klKernel.KERN_slLock);               /*  �ٴγ��������ں�������      */
    }

    //bspDebugMsg("[%p]ulLockCounter++\n", pcpuCur->CPU_pvcpuCur);
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter++;                        /*  ���������ڵ�ǰ CPU          */
    KN_SMP_MB();
}
/*********************************************************************************************************
** ��������: kernelUnlockIgnIrq
** ��������: �ں���������������, �����ж����� (�������жϹرյ�״̬�±�����)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: irqExit()
*********************************************************************************************************/
VOID  kernelUnlockIgnIrq (VOID)
{
    PMX_PHYS_CPU   pcpuCur;
    INT            iRet;
    
    KN_SMP_MB();
    iRet = archSpinUnlock(&_K_klKernel.KERN_slLock);
    archAssert(iRet == MX_SPIN_OK, __func__, __FILE__, __LINE__);

    pcpuCur = MX_CPU_GET_CUR();
    pcpuCur->CPU_pvcpuCur->VCPU_ulLockCounter--;                        /*  ������������ڵ�ǰ CPU      */
}
/*********************************************************************************************************
** ��������: kernelIsEnter
** ��������: �Ƿ�����˽����ں�״̬
** �䡡��  : NONE
** �䡡��  : ���ں�״̬ MX_TRUE, ���� MX_FALSE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
BOOL  kernelIsEnter (VOID)
{
    INTREG          iregInterLevel;
    PMX_PHYS_CPU    pcpuCur;
    INT             iKernelCounter;
    
    iregInterLevel = archIntDisable();                                  /*  �ر��ж�                    */
    
    pcpuCur        = MX_CPU_GET_CUR();
    iKernelCounter = pcpuCur->CPU_iKernelCounter;                       /*  ��ȡ�ں�״̬                */
    
    archIntEnable(iregInterLevel);                                      /*  ���ж�                    */
    
    if (iKernelCounter) {
        return  (MX_TRUE);
    } else {
        return  (MX_FALSE);
    }
}
/*********************************************************************************************************
** ��������: kernelSched
** ��������: �ں˵���
** �䡡��  : NONE
** �䡡��  : ����������ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_ERROR  kernelSched (VOID)
{
    INTREG          iregInterLevel;
    MX_ERROR        ulRetVal;
    
    iregInterLevel = kernelEnterAndDisIrq();                            /*  ���ں� spinlock ���ر��ж�  */
    ulRetVal = schedule();                                              /*  ���Ե���                    */
    kernelExitAndEnIrq(iregInterLevel);                                 /*  �����ں� spinlock �����ж�*/
    
    return  (ulRetVal);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
