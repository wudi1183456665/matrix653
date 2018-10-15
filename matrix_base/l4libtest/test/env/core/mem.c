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
** ��   ��   ��: mem.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 2 ��
**
** ��        ��: �ڴ����
*********************************************************************************************************/
#include "t_sys.h"

/*********************************************************************************************************
  TASK MEM
*********************************************************************************************************/
MX_IPC_MSG  __GT_poolUseMsg[T_POOL_USE_MSG_NUM];
T_VCPU_STK  __GT_poolUseStk[T_POOL_USE_STK_NUM];

UINT32      __GT_uiPoolUseMsgCur  = 0;
UINT32      __GT_uiPoolUseStkCur  = 0;
/*********************************************************************************************************
  ROOT MEM
*********************************************************************************************************/
T_VCPU      __GT_poolVcpu[T_POOL_VCPU_NUM];
T_SERVICE   __GT_poolService[T_POOL_SERVICE_NUM];

UINT32      __GT_uiPoolVcpuCur    = 0;
UINT32      __GT_uiPoolSerCur     = 0;
/*********************************************************************************************************
** ��������: T_memObjAlloc
** ��������: �����ڴ�
** �䡡��  :
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PVOID  T_memObjAlloc (UINT32  uiType)
{
    PVOID   pObj = MX_NULL;

    switch (uiType) {

    case TMEM_IPC_MSG:
                if (__GT_uiPoolUseMsgCur < T_POOL_USE_MSG_NUM) {
                    pObj = (PVOID) &__GT_poolUseMsg[__GT_uiPoolUseMsgCur++];
                    IPC_ASSERT(T_MEM_ALIGN((addr_t)pObj, 12));
                }
                break;

    case TMEM_USE_STK:
                if (__GT_uiPoolUseStkCur < T_POOL_USE_STK_NUM) {
                    pObj = (PVOID) &__GT_poolUseStk[__GT_uiPoolUseStkCur++];
                    IPC_ASSERT(T_MEM_ALIGN((addr_t)pObj, 12));
                }
                break;

    case TMEM_VCPU:
        if (__GT_uiPoolVcpuCur < T_POOL_VCPU_NUM) {
            pObj = (PVOID)&__GT_poolVcpu[__GT_uiPoolVcpuCur++];
        }
        break;

    case TMEM_SERVICE:
        if (__GT_uiPoolSerCur < T_POOL_SERVICE_NUM) {
            pObj = (PVOID) &__GT_poolService[__GT_uiPoolSerCur++];
        }
        break;
    }

    if (!pObj) {
        T_ERROR("mem not enough");
    }
    return  (pObj);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
