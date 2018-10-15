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
** ��   ��   ��: root_server.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 3 ��
**
** ��        ��: ϵͳ����
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
  ��������
*********************************************************************************************************/
/*
 * log �߳�
 */
VOID  T_funcLogThread(PVOID  pvArg);
/*
 * root ������
 */
VOID  T_rootServer(MX_OBJECT_ID  objRoot);
/*
 * ����Э��ӿ�
 */
BOOL  TS_rootSerRegister(PMX_IPC_MSG  pIpcMsg, MX_OBJECT_ID  objSender);
BOOL  TS_rootSerFind(PMX_IPC_MSG  pMsg);
/*
 * ������Ԫ����, ��Ԫ���������ڶ����߳���
 */
BOOL  TS_rootSerTestBoot(PMX_IPC_MSG  pIpcMsg, MX_OBJECT_ID  objRoot);
/*
 * ��Ԫ���������߳�
 */
VOID  T_funcUserTestThread(PVOID pvArg);
/*********************************************************************************************************
** ��������: T_funcRootThread
** ��������: ϵͳ�߳�
** �䡡��  : pvArg  PT_VCPU
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID T_funcRootThread (PVOID  pvArg)
{
    /*
     * ��ʼ�� ROOT ���������
     * ע�� ROOT ����
     * ���������� LOG �߳�
     * �������ģʽ���ṩע��������ַ���)
     *      T_rootService()
     */
    PT_VCPU      pvcpuRoot = (PT_VCPU)pvArg;
    MX_OBJECT_ID objRoot   = (MX_OBJECT_ID)pvcpuRoot->pvArg;
    PT_VCPU      pvcpuLog  = MX_NULL;

    T_TITLE(T_funcRootThread);

    T_serMgrInit();                                                     /* ��ʼ�����������             */
    T_serRegist(TSER_CODE_ROOT, objRoot);                               /* ע�� ROOT ����               */
    /*
     * ���������� LOG �߳�
     */
#if 1
    pvcpuLog = T_vcpuCreate(0,
                            (PVCPU_START_ROUTINE)T_funcLogThread,
                            TVCPU_PRIO_HIGH+1);

    if (!pvcpuLog) {
        T_ERROR("log.I_vcpuCreate error");
    }
    else {
        pvcpuLog->pvArg = (PVOID)objRoot;

        if(!T_vcpuStart(pvcpuLog)) {
            T_ERROR("log_server start error");
        }
    }
#endif

#if 0                                                                   /*  ���� wakeup log �߳�        */
    T_vcpuSleep(SC_USER_TIME(1, SC_UTIME_L));
    T_NOTE("wakeup log_server");
    T_vcpuWakeup(pvcpuLog->objVcpu);
#endif

#if 0                                                                   /*  ���Դ��� unit_test �߳�     */
    TS_rootSerTestBoot(pvcpuRoot->pIpcMsg, MX_NULL);
#endif

    T_rootServer(objRoot);                                              /*  IPC ����                    */
}
/*********************************************************************************************************
** ��������: T_rootService
** ��������: ϵͳ����
** �䡡��  :
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL T_rootHandle (MX_OBJECT_ID  objRoot, MX_OBJECT_ID  objSender)
{
    /*
     * ����Э��
     *
     * T_ROOT_SER_REG:
     *      [IN]  UINT32           - Э���
     *            UINT32           - ��ע�����ID
     *      [OUT] BOOL             - ִ�н��: 1 �ɹ�; 0 ʧ��
     *
     * T_ROOT_SER_FIND:
     *      [IN]  UINT32           - Э���
     *            UINT32           - ����ID
     *      [OUT] BOOL             - 1 �ɹ�; 0 ʧ��
     *            MX_OBJECT_ID     - �������
     *
     * T_ROOT_SER_CLOSE:
     *      [IN]  UINT32           - Э���
     *      [OUT]
     *
     * T_ROOT_SER_TEST_BOOT:
     *      [IN]  UINT32           - Э���
     *      [OUT]
     */
    ULONG           ulOpCode = 0;
    BOOL            result   = MX_TRUE;
    PMX_IPC_MSG     pIpcMsg  = T_vcpuGetCurMsg();

    T_TITLE(T_rootHandle);

    ipcMsgReadOpen(pIpcMsg);
    ipcMsgUlRead(pIpcMsg, &ulOpCode);

    switch (ulOpCode) {

    case    T_ROOT_SER_REG:
        TS_rootSerRegister(pIpcMsg, objSender);
        break;

    case    T_ROOT_SER_FIND:
        TS_rootSerFind(pIpcMsg);
        break;

    case    T_ROOT_SER_CLOSE:
        T_DEBUG(": close service");
        result = MX_FALSE;
        break;

    case    T_ROOT_SER_TEST_BOOT:
        TS_rootSerTestBoot(pIpcMsg, objRoot);
        break;

    default:
        T_ERROR("unknow code");

        ipcMsgWriteOpen(pIpcMsg);
        ipcMsgUlWrite(pIpcMsg, 0);
        ipcMsgFlush(pIpcMsg);
        break;
    }
    T_DEBUG("SUCCESS");
    return  (result);
}

BOOL T_rootErrHandle (MX_OBJECT_ID  objRoot, MX_OBJECT_ID  objSender)
{
    T_ERROR("IPC error");
    return  (MX_FALSE);
}

VOID T_rootServer (MX_OBJECT_ID  objRoot)
{
#if 1
    MX_OBJECT_ID    objSender = 0;
    MX_SC_RESULT_T  errCode;

    T_TITLE("API_ipcOpenWait");

    errCode = API_ipcOpenWait(&objSender);

    do {
        if (errCode == MX_SC_RESULT_NEED_REPLY) {
            if (!T_rootHandle(objRoot, objSender)) {
               break;
            }
            T_DEBUG("API_ipcReplyWait");
            errCode = API_ipcReplyWait(&objSender);

        } else if(errCode == MX_SC_RESULT_OK) {
            if (!T_rootHandle(objRoot, objSender)) {
                break;
            }
            T_DEBUG("API_ipcOpenWait");
            errCode = API_ipcOpenWait(&objSender);

        } else {
            if (!T_rootErrHandle(errCode, objSender)) {
                break;
            }
        }
    } while(1);

    T_DEBUG("API_ipcClose");
    API_ipcClose();
#endif

    do {
        T_vcpuSleep(SC_TIME_MAKE(3, SC_UTIME_L));                       /* BUG: bootload ��̬����δ����;  */
                                                                        /* ����ʽ��������λΪ 100ms    */
    } while(1);
}
/*********************************************************************************************************
    service handle
*********************************************************************************************************/
/*********************************************************************************************************
** ��������: TS_rootSerRegister
** ��������: ע�����
** �䡡��  : pIpcMsg
**           objSender   ��ע��ķ������
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL TS_rootSerRegister (PMX_IPC_MSG pIpcMsg, MX_OBJECT_ID objSender)
{
    ULONG       ulSerId;
    BOOL        result  =MX_TRUE;

    T_TITLE(TS_rootSerRegister);

    if (!ipcMsgUlRead(pIpcMsg, &ulSerId)) {
        T_ERROR("ipcMsgUlRead fail");
        result = MX_FALSE;
    } else {
        T_serRegist((UINT32)ulSerId, objSender);
    }

    ipcMsgWriteOpen(pIpcMsg);
    ipcMsgUlWrite(pIpcMsg, (result ? 1 : 0));
    ipcMsgFlush(pIpcMsg);

    return  (result);
}
/*********************************************************************************************************
** ��������: TS_rootSerFind
** ��������: ���ҷ���
** �䡡��  : pIpcMsg
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  TS_rootSerFind (PMX_IPC_MSG pIpcMsg)
{
    ULONG           ulSerId;
    MX_OBJECT_ID    objFind;
    BOOL            result  =MX_TRUE;

    if (!ipcMsgUlRead(pIpcMsg, &ulSerId)) {
        T_ERROR("ipcMsgUlRead fail");
        result = MX_FALSE;
    } else {
        objFind = T_serFind((UINT32)ulSerId);
        result = (objFind==0) ? MX_FALSE : MX_TRUE;
    }

    ipcMsgWriteOpen(pIpcMsg);
    ipcMsgUlWrite(pIpcMsg, (result ? 1 : 0));
    ipcMsgUlWrite(pIpcMsg, (ULONG)objFind);
    ipcMsgFlush(pIpcMsg);

    return  (result);
}
/*********************************************************************************************************
** ��������: TS_rootSerTestBoot
** ��������: �����û���Ԫ���Կ��
** �䡡��  : objRoot   root �������
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  TS_rootSerTestBoot (PMX_IPC_MSG pIpcMsg, MX_OBJECT_ID objRoot)
{
    /*
     * ������Ԫ����, ��Ԫ���������ڶ����߳���
     */
    PT_VCPU pvcpuTest = MX_NULL;

    T_TITLE(TS_rootSerTestBoot);

    /*
     * ���������� test �߳�
     */
    pvcpuTest = T_vcpuCreate(0,
                             (PVCPU_START_ROUTINE)T_funcUserTestThread,
                             TVCPU_PRIO_NORMAL);

    if (!pvcpuTest) {
        T_ERROR("T_vcpuCreate fail");

    } else {
        pvcpuTest->pvArg = (PVOID) objRoot;
#if 1
        if (!T_vcpuStart(pvcpuTest)) {
            T_ERROR( "T_vcpuStart fail");
        }
#endif
    }
    return  (MX_TRUE);
}
/*********************************************************************************************************
    root service user interface
*********************************************************************************************************/
/*********************************************************************************************************
** ��������: TI_rootSerRegister
** ��������: ע�����
** �䡡��  : pIpcMsg
**           objRoot    ROOT �������
**           uiSerId    ��ע��ķ��� ID (����)
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL TI_rootSerRegister (MX_OBJECT_ID objRoot, UINT32 uiSerId)
{
    /*
     * T_ROOT_SER_REG:
     *      [IN]  UINT32           - Э���
     *            UINT32           - ��ע�����ID
     *      [OUT] BOOL             - ִ�н��: 1 �ɹ�; 0 ʧ��
     */
    MX_SC_RESULT_T  error;
    ULONG           result  = 0;

    PMX_IPC_MSG     pIpcMsg = T_vcpuGetCurMsg();

    ipcMsgWriteOpen(pIpcMsg);
    ipcMsgUlWrite(pIpcMsg, T_ROOT_SER_REG);
    ipcMsgUlWrite(pIpcMsg, uiSerId);
    ipcMsgFlush(pIpcMsg);

    error = API_ipcCall(objRoot, SC_TIME_MAKE(1, SC_UTIME_L));
    if (error == MX_SC_RESULT_OK) {
        ipcMsgReadOpen(pIpcMsg);
        ipcMsgUlRead(pIpcMsg, &result);

    } else {
        T_ERROR("API_ipcCall fail");
    }

    return  (result);
}
/*********************************************************************************************************
** ��������: TI_rootSerFind
** ��������: ���ҷ���
** �䡡��  : pIpcMsg
**           objRoot
**           uiSerId    �����ҵķ��� ID (����)
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_OBJECT_ID TI_rootSerFind (MX_OBJECT_ID objRoot, UINT32 uiSerId)
{
    /*
     * T_ROOT_SER_FIND:
     *      [IN]  UINT32           - Э���
     *            UINT32           - ����ID
     *      [OUT] BOOL             - 1 �ɹ�; 0 ʧ��
     *            MX_OBJECT_ID     - �������
    */
    MX_SC_RESULT_T  error;
    ULONG           result   = 0;
    ULONG           ulSerObj = 0;

    PMX_IPC_MSG     pIpcMsg  = T_vcpuGetCurMsg();

    ipcMsgWriteOpen(pIpcMsg);
    ipcMsgUlWrite(pIpcMsg, T_ROOT_SER_FIND);
    ipcMsgUlWrite(pIpcMsg, uiSerId);
    ipcMsgFlush(pIpcMsg);

    error = API_ipcCall(objRoot, SC_TIME_MAKE(20, SC_UTIME_L));
    if (error == MX_SC_RESULT_OK) {
        ipcMsgReadOpen(pIpcMsg);
        ipcMsgUlRead(pIpcMsg, &result);
        ipcMsgUlRead(pIpcMsg, &ulSerObj);

    } else {
        T_ERROR("API_ipcCall fail");
    }

    return  ((MX_OBJECT_ID)ulSerObj);
}
/*********************************************************************************************************
** ��������: TI_rootSerClose
** ��������: �ر� ROOT ���� (��ȫ�ر�)
** �䡡��  : pIpcMsg
**           objRoot
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  TI_rootSerClose (MX_OBJECT_ID objRoot)
{
    /*
     * T_ROOT_SER_CLOSE:
     *      [IN]  UINT32           - Э���
     *      [OUT]
     */
    MX_SC_RESULT_T  error;

    PMX_IPC_MSG     pIpcMsg = T_vcpuGetCurMsg();

    ipcMsgWriteOpen(pIpcMsg);
    ipcMsgUlWrite(pIpcMsg, T_ROOT_SER_CLOSE);
    ipcMsgFlush(pIpcMsg);

    error = API_ipcSend(objRoot, 0);
    if (error != MX_SC_RESULT_OK) {
        T_ERROR("API_ipcSend fail");
        return  (MX_FALSE);
    }

    return  (MX_TRUE);
}
/*********************************************************************************************************
** ��������: TI_rootSerTestBoot
** ��������: ������Ԫ����, ��Ԫ���������ڶ����߳���
** �䡡��  : pIpcMsg
**           objRoo     troot �������
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  TI_rootSerTestBoot (MX_OBJECT_ID objRoot)
{
    /*
     * T_ROOT_SER_TEST_BOOT:
     *      [IN]  UINT32           - Э���
     *      [OUT]
     */
    MX_SC_RESULT_T  error;
    PMX_IPC_MSG     pIpcMsg = T_vcpuGetCurMsg();

    ipcMsgWriteOpen(pIpcMsg);
    ipcMsgUlWrite(pIpcMsg, T_ROOT_SER_TEST_BOOT);
    ipcMsgFlush(pIpcMsg);

    // error = API_ipcSend(objRoot, 0);
    error = API_ipcSend(objRoot, SC_TIME_MAKE(1, SC_UTIME_L));
    if (error != MX_SC_RESULT_OK) {
        T_ERROR("API_ipcSend fail");
        return  (MX_FALSE);
    }

    return  (MX_TRUE);
}
/*********************************************************************************************************
    root service kern interface
*********************************************************************************************************/
/*********************************************************************************************************
** ��������: TKI_rootSerFind
** ��������: ���ҷ���
** �䡡��  : pIpcMsg
**           objRoot
**           uiSerId    �����ҵķ��� ID (����)
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_OBJECT_ID TKI_rootSerFind (MX_OBJECT_ID objRoot, UINT32 uiSerId)
{
    /*
     * T_ROOT_SER_FIND:
     *      [IN]  UINT32           - Э���
     *            UINT32           - ����ID
     *      [OUT] BOOL             - 1 �ɹ�; 0 ʧ��
     *            MX_OBJECT_ID     - �������
    */
    BOOL            error    = MX_FALSE;
    ULONG           result   = 0;
    ULONG           ulSerObj = 0;
    PMX_IPC_MSG     pIpcMsg  = T_GET_KERN_MSG();

    ipcMsgWriteOpen(pIpcMsg);
    ipcMsgUlWrite(pIpcMsg, T_ROOT_SER_FIND);
    ipcMsgUlWrite(pIpcMsg, uiSerId);
    ipcMsgFlush(pIpcMsg);

    error = ipcKernSend(objRoot, MX_FALSE);
    if (error == MX_TRUE) {
        ipcMsgReadOpen(pIpcMsg);
        ipcMsgUlRead(pIpcMsg, &result);
        ipcMsgUlRead(pIpcMsg, &ulSerObj);

    } else {
        T_ERROR("ipcKernSend fail");
    }

    return  ((MX_OBJECT_ID)ulSerObj);
}
/*********************************************************************************************************
** ��������: TKI_rootSerClose
** ��������: �ر� ROOT ���� (��ȫ�ر�)
** �䡡��  : pIpcMsg
**           objRoot
** �䡡��  : BOOL
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  TKI_rootSerClose (MX_OBJECT_ID objRoot)
{
    // PMX_IPC_MSG     pIpcMsg  = T_GET_KERN_MSG();

    return  (MX_TRUE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
