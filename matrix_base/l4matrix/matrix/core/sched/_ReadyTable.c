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
** ��   ��   ��: _ReadyTable.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 19 ��
**
** ��        ��: ���������.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** ��������: _ReadyTableGet
** ��������: ��ȡ����������ȼ��̵߳ľ������������ȼ�ֵ
** �䡡��  : pcpu         �������ؾ�����
**           puiPriority  ����������ȼ�
** �䡡��  : ����������ȼ��̵߳ľ�����(Ҫô�Ǳ��ؾ�����, Ҫô��ȫ�־�����)
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PMX_READY  _ReadyTableGet (PMX_PHYS_CPU  pcpu,  UINT32 *puiPriority)
{
    REGISTER PMX_READY  preadyReturn;
    REGISTER UINT32     uiLocal, uiGlobal;
 
    if (_BitmapIsEmpty(MX_CPU_RDY_BMAP(pcpu))) {                        /*  CPU ���ؾ������            */
        if (_BitmapIsEmpty(MX_GLOBAL_RDY_BMAP())) {                     /*  ȫ�־�����ҲΪ��            */
            return  (MX_NULL);                                          /*  ��������������              */
        }
        *puiPriority = _BitmapHigh(MX_GLOBAL_RDY_BMAP());
        return  (MX_GLOBAL_RDY());                                      /*  ����ȫ�־�����              */
    
    } else {                                                            /*  CPU ���ؾ�����ǿ�          */
        if (_BitmapIsEmpty(MX_GLOBAL_RDY_BMAP())) {                     /*  ȫ�־������                */
            *puiPriority = _BitmapHigh(MX_CPU_RDY_BMAP(pcpu));
            return  (MX_CPU_RDY(pcpu));                                 /*  ���ر��ؾ�����              */
        }

        uiLocal  = _BitmapHigh(MX_CPU_RDY_BMAP(pcpu));
        uiGlobal = _BitmapHigh(MX_GLOBAL_RDY_BMAP());
        
        if (MX_PRIO_IS_HIGH_OR_EQU(uiLocal, uiGlobal)) {                /*  ͬ���ȼ�, ����ִ�� local    */
            preadyReturn = MX_CPU_RDY(pcpu);                            /*  ���ر��ؾ�����              */
            *puiPriority = uiLocal;
        
        } else {
            preadyReturn = MX_GLOBAL_RDY();                             /*  ����ȫ�־�����              */
            *puiPriority = uiGlobal;
        }
        
        return  (preadyReturn);
    }
}
/*********************************************************************************************************
** ��������: _ReadyTableAdd
** ��������: ��ָ���̼߳��������, ���� Bitmap
** �䡡��  : pvcpu      �߳̿��ƿ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _ReadyTableAdd (PMX_VCPU  pvcpu)
{
    PMX_RDYP    prdyp;
    PMX_READY   pready;

    prdyp = _RdyPrioGet(pvcpu);                                         /*  ��ȡ�߳����������ȼ����ƿ�  */
    _RdyPrioAddVcpu(pvcpu, prdyp, MX_TRUE);                             /*  ���������ͷ, �´����ȵ���  */
    
    if (_RdyPrioIsOne(prdyp)) {
        if (pvcpu->VCPU_bCpuAffinity) {
            pready = MX_CPU_RDY(MX_CPU_GET(pvcpu->VCPU_ulCpuIdAffinity));
        } else {
            pready = MX_GLOBAL_RDY();
        }

        _BitmapAdd(&pready->RDY_bmap, pvcpu->VCPU_uiPriority);
    }
}
/*********************************************************************************************************
** ��������: _ReadyTableDel
** ��������: ��ָ���߳��˳��������, �� Bitmap
** �䡡��  : pvcpu      �߳̿��ƿ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _ReadyTableDel (PMX_VCPU  pvcpu)
{
    PMX_RDYP    prdyp;
    PMX_READY   pready;

    prdyp = _RdyPrioGet(pvcpu);                                         /*  ��ȡ�߳����������ȼ����ƿ�  */
    _RdyPrioDelVcpu(pvcpu, prdyp);                                      /*  �Ӿ�������ɾ��              */
    
    if (_RdyPrioIsEmpty(prdyp)) {
        if (pvcpu->VCPU_bCpuAffinity) {
            pready = MX_CPU_RDY(MX_CPU_GET(pvcpu->VCPU_ulCpuIdAffinity));
        } else {
            pready = MX_GLOBAL_RDY();
        }

        _BitmapDel(&pready->RDY_bmap, pvcpu->VCPU_uiPriority);
    }
}
/*********************************************************************************************************
** ��������: _ReadyTableSeek
** ��������: ���ָ���������ָ�����ȼ�����ı�ͷ�߳�
** �䡡��  : pready          ������
**           uiPriority      ���ȼ�
** �䡡��  : ��������ָ�����ȼ�����ͷ�߳�
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PMX_VCPU  _ReadyTableSeek (PMX_READY  pready, UINT32  uiPriority)
{
    REGISTER PMX_VCPU  pvcpu;
    REGISTER PMX_RDYP  prdyp;
    
    prdyp = &pready->RDY_rdyp[uiPriority];
    pvcpu = _LIST_ENTRY(prdyp->RDYP_pringReadyHeader, 
                        MX_VCPU, 
                        VCPU_ringReady);                                /*  �Ӿ�������ȡ��һ���߳�      */

    if (pvcpu->VCPU_usSchedCounter == 0) {                              /*  ʱ��Ƭ����                  */
        pvcpu->VCPU_usSchedCounter = pvcpu->VCPU_usSchedSlice;          /*  ����ʱ��Ƭ                  */
        _list_ring_next(&prdyp->RDYP_pringReadyHeader);                 /*  ��һ��                      */
        pvcpu = _LIST_ENTRY(prdyp->RDYP_pringReadyHeader, 
                           MX_VCPU, 
                           VCPU_ringReady);
    }
    
    return  (pvcpu);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

