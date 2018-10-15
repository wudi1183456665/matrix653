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
** ��   ��   ��: unit_boot.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 6 ��
**
** ��        ��: ��Ԫ������������
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
** ��������: T_funcUserTestThread
** ��������: Ӧ�õ�Ԫ���������߳�; �û�̬
** �䡡��  : pvArg
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  T_funcUserTestThread(PVOID pvArg)
{
    PT_VCPU      pvcpuUnit = (PT_VCPU)pvArg;
    MX_OBJECT_ID objRoot   = (MX_OBJECT_ID)pvcpuUnit->pvArg;

    // PMX_IPC_MSG  pIpcMsg   = T_vcpuGetCurMsg();

    T_TITLE(T_funcUserTestThread);

#if 1   /*  DEBUG: �������� IPC        */
    T_vcpuSleep(SC_TIME_MAKE(1, SC_UTIME_L));
    do {
        MX_OBJECT_ID objLog;
        T_DEBUG(": FIND log_service id**********");
        objLog = TI_rootSerFind(objRoot, TSER_CODE_LOG);
        T_DEBUG(": log_service_id=0x%x*********************", objLog);
        T_vcpuSleep(SC_TIME_MAKE(3, SC_UTIME_L));
    } while(1);
#endif

    do {
        T_DEBUG("(^_^)USER UNIT TEST LOOP*******************(^_^)");
        T_vcpuSleep(SC_TIME_MAKE(2, SC_UTIME_L));

    } while(1);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
