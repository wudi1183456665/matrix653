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
** ��   ��   ��: test_config.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 1 ��
**
** ��        ��: ������������
*********************************************************************************************************/
#include "../test/iunit/t_unit.h"

/*********************************************************************************************************
  ��������
*********************************************************************************************************/
BOOL unitDemo()
{
    return  (MX_FALSE);
}

/*********************************************************************************************************
   ���Բ���
*********************************************************************************************************/
#define SCT_VCPUID_KERN     0
#define SCT_VCPUID_ROOT     1
#define SCT_VCPUID_SER      2
#define SCT_VCPUID_USER     3

/*********************************************************************************************************
   ��������1: ģ��ϵͳ�߳�(root�߳� - sigma0 + moe)
*********************************************************************************************************/
PVOID  __SCT_kernThread(PVOID  pvArg)
{
    INT  i = 0;

    SCT_TITLE("%s---__SCT_kernThread() start. %X\n", MX_VCPU_GET_CUR());

    while (1) {
        SCT_DEBUG("-- %d, %X ------ vCPU%d @%d\n", i++, archGetCpsr(), (INT)pvArg, archMpCur());
        SCT_SLEEP(30);
    }

    return  (MX_NULL);
}


PVOID  __SCT_serverThread(PVOID  pvArg)
{
    INT  i = 0;

    SCT_TITLE("%s---__SCT_rootThread() start. %X\n", MX_VCPU_GET_CUR());

    while (1) {
        SCT_DEBUG("-- %d, %X ------ vCPU%d @%d\n", i++, archGetCpsr(), (INT)pvArg, archMpCur());
        SCT_SLEEP(30);
    }

    return  (MX_NULL);
}

PVOID  __SCT_rootThread(PVOID  pvArg)
{
    INT  i = 0;

    SCT_TITLE("%s---__SCT_rootThread() start. %X\n", MX_VCPU_GET_CUR());

    while (1) {
        SCT_DEBUG("-- %d, %X ------ vCPU%d @%d\n", i++, archGetCpsr(), (INT)pvArg, archMpCur());
        SCT_SLEEP(30);
    }

    return  (MX_NULL);
}

VOID  __SCT_sysStartUnit()
{
    MX_OBJECT_ID ulVcpuId;
    MX_VCPU_ATTR vcpuattr;

    SCT_TITLE("%s---__SCT_kernThread CREATE. \n");
    vcpuattr.VCPUATTR_uiPriority = 200;
    vcpuattr.VCPUATTR_pvArg = (PVOID) SCT_VCPUID_KERN;
    vcpuattr.VCPUATTR_bIsUser = MX_FALSE;
    ulVcpuId = vcpuCreate(__SCT_kernThread, &vcpuattr);                /*  ���� �ں� �߳�              */
    vcpuStart(ulVcpuId);

    SCT_TITLE("%s---__SCT_rootThread CREATE. \n");
    vcpuattr.VCPUATTR_uiPriority = 200;
    vcpuattr.VCPUATTR_pvArg = (PVOID) SCT_VCPUID_ROOT;
    vcpuattr.VCPUATTR_bIsUser = MX_TRUE;
    ulVcpuId = vcpuCreate(__SCT_rootThread, &vcpuattr);                /*  ���� ROOT �߳�              */
    vcpuStart(ulVcpuId);
}

/*********************************************************************************************************
   ��������2: ģ��ϵͳ�߳�(root�߳� - sigma0 + moe)
*********************************************************************************************************/



/*********************************************************************************************************
  END
*********************************************************************************************************/
