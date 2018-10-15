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
** ��   ��   ��: log_server.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 3 ��
**
** ��        ��: ��־����
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
  ��������
*********************************************************************************************************/
VOID T_logService (MX_OBJECT_ID  objRoot);
/*********************************************************************************************************
 *  ע�����
 *
 *  �������״̬: T_logService()
*********************************************************************************************************/
VOID T_funcLogThread (PVOID  pvArg)
{
    PT_VCPU      pvcpuLog = (PT_VCPU)pvArg;
    MX_OBJECT_ID objRoot  = (MX_OBJECT_ID)pvcpuLog->pvArg;

    T_TITLE(T_funcLogThread);

    T_vcpuSleep(SC_TIME_MAKE(1, SC_UTIME_L));                           /*  ������������                */

#if 1
    T_DEBUG(": REGISTER log_server");
    TI_rootSerRegister(objRoot, TSER_CODE_LOG);                         /*  ע����־����                */
#endif

#if 1   /*  DEBUG: ���Բ��ҷ���         */
{
    MX_OBJECT_ID objLog;
    T_DEBUG(": FIND log_service id");
    objLog = TI_rootSerFind(objRoot, TSER_CODE_LOG);
    T_DEBUG(": log_service_id=0x%x", objLog);
}
#endif

#if 0   /*  DEBUG: ����������Ԫ����     */
    T_DEBUG(": TEST boot test_unit");
    if (!TI_rootSerTestBoot(objRoot)) {
        T_ERROR("boot test_unit fail");
    }
#endif

    T_logService(objRoot);
}

VOID T_logService (MX_OBJECT_ID objRoot)
{
#if 1   /*  DEBUG: �������� IPC        */
    do {
        MX_OBJECT_ID objLog;
        T_DEBUG(": FIND log_service id**********");
        objLog = TI_rootSerFind(objRoot, TSER_CODE_LOG);
        T_DEBUG(": log_service_id=0x%x*********", objLog);
        T_vcpuSleep(SC_TIME_MAKE(4, SC_UTIME_L));
    } while(1);
#endif

    do {
        T_DEBUG("(^_^)LOG SERVICE LOOP*******************(^_^)");
        T_vcpuSleep(SC_TIME_MAKE(2, SC_UTIME_L));
    } while (1);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
