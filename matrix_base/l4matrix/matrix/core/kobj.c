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
** ��   ��   ��: kobj.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 19 ��
**
** ��        ��: �ں˶������.

** BUG
2018.01.30  ���������麯�� kobjCheck; TODO: ���� _KOBJ_IPC_MSG ���ͼ��
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** ��������: kobjInit
** ��������: �ں˶���ģ���ʼ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  kobjInit (VOID)
{
}
/*********************************************************************************************************
** ��������: __kobjGetClass
** ��������: ��ȡ�ں˶�������
** �䡡��  : ulKobjId           �ں˶��� ID
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static UINT8  __kobjGetClass (MX_OBJECT_ID  ulKobjId)
{
    return  ((UINT8)((ulKobjId >> 16) & 0xFF));
}
/*********************************************************************************************************
** ��������: __kobjGetIndex
** ��������: ��ȡ�ں˶�������
** �䡡��  : ulKobjId           �ں˶��� ID
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static UINT16  __kobjGetIndex (MX_OBJECT_ID  ulKobjId)
{
    return  ((UINT16)(ulKobjId & 0xFFFF));
}
/*********************************************************************************************************
** ��������: kobjAlloc
** ��������: ����һ���ں˶���
** �䡡��  : ucClass            �ں˶�������
** �䡡��  : �ں˶��� ID
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_OBJECT_ID  kobjAlloc (UINT8  ucClass)
{
    MX_OBJECT_ID      ulReturnId = MX_OBJECT_INVALID;
    UINT32            i = 0;

    switch (ucClass) {

    case _KOBJ_VCPU:
        for (i = 0; i < MX_CFG_MAX_VCPUS; i++) {                        /*  ��������δʹ��Ԫ��          */
            if (!_K_vcpuTable[i].VCPUKSTACK_vcpu.VCPU_bIsUsed) {
                _K_vcpuTable[i].VCPUKSTACK_vcpu.VCPU_bIsUsed = MX_TRUE;
                ulReturnId = (ucClass << 16) | i;
                break;
            }
        }
        break;

    case _KOBJ_IPC_MSG:
        break;

    case _KOBJ_PRTN:
        for (i = 0; i < MX_CFG_MAX_PRTNS; i++) {                        /*  ��������δʹ��Ԫ��          */
            if (!_K_prtnTable[i].PRTN_bIsUsed) {
                _K_prtnTable[i].PRTN_bIsUsed = MX_TRUE;
                ulReturnId = (ucClass << 16) | i;
                break;
            }
        }
        break;

    default:
        break;
    }

    return  (ulReturnId);
}
/*********************************************************************************************************
** ��������: kobjFree
** ��������: �ͷ�һ���ں˶���
** �䡡��  : ulKobjId           �ں˶��� ID
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  kobjFree (MX_OBJECT_ID  ulKobjId)
{
    UINT8      ucClass = __kobjGetClass(ulKobjId);
    UINT32     uiIndex = __kobjGetIndex(ulKobjId);

    switch (ucClass) {

    case _KOBJ_VCPU:
        if (uiIndex < MX_CFG_MAX_VCPUS) {
            _K_vcpuTable[uiIndex].VCPUKSTACK_vcpu.VCPU_bIsUsed = MX_FALSE;
        }
        break;

    case _KOBJ_IPC_MSG:
        break;

    case _KOBJ_PRTN:
        if (uiIndex < MX_CFG_MAX_PRTNS) {
            _K_prtnTable[uiIndex].PRTN_bIsUsed = MX_FALSE;
        }
        break;

    default:
        break;
    }

}
/*********************************************************************************************************
** ��������: kobjGet
** ��������: ��ȡһ���ں˶���ṹ��ָ��
** �䡡��  : ulKobjId           �ں˶��� ID
** �䡡��  : �ں˶���ṹ��ָ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PVOID  kobjGet (MX_OBJECT_ID  ulKobjId)
{
    UINT8      ucClass = __kobjGetClass(ulKobjId);
    UINT32     uiIndex = __kobjGetIndex(ulKobjId);
    VOID      *pvKobj  = MX_NULL;

    switch (ucClass) {

    case _KOBJ_VCPU:
        if (uiIndex < MX_CFG_MAX_VCPUS) {
            pvKobj = &_K_vcpuTable[uiIndex];
        }
        break;

    case _KOBJ_IPC_MSG:
        break;

    case _KOBJ_PRTN:
        if (uiIndex < MX_CFG_MAX_PRTNS) {
            pvKobj = &_K_prtnTable[uiIndex];
        }
        break;

    default:
        break;
    }

    return  (pvKobj);
}
/*********************************************************************************************************
** ��������: kobjCheck
** ��������: �ں˶���Ϸ��Լ��
** �䡡��  : ulKobjId   ����
** �䡡��  : ucObjClass ������������
** ȫ�ֱ���: FALSE - �����ʽ����
** ����ģ��:
*********************************************************************************************************/
BOOL  kobjCheck (MX_OBJECT_ID ulKobjId, UINT8 ucObjClass)
{
    UINT8      ucClass = __kobjGetClass(ulKobjId);
    UINT32     uiIndex = __kobjGetIndex(ulKobjId);

    if (ucObjClass != ucClass) {
        return  (MX_FALSE);
    }

    switch (ucClass) {
    case _KOBJ_VCPU:
        if (uiIndex >= MX_CFG_MAX_VCPUS) {
            return  (MX_FALSE);
        }
        break;

    case _KOBJ_IPC_MSG:
        break;

    case _KOBJ_PRTN:
        if (uiIndex >= MX_CFG_MAX_PRTNS) {
            return  (MX_FALSE);
        }
        break;

    default:
        return  (MX_FALSE);
    }
    return  (MX_TRUE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
