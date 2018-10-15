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
** ��   ��   ��: ser_mgr.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 1 ��
**
** ��        ��: �������
*********************************************************************************************************/
#include "t_sys.h"
/*********************************************************************************************************
  ser_mgr ����
*********************************************************************************************************/
T_SER_MGR   __TG_serMgr;
/*
 * ��������
 */
CPCHAR   TG_strSerName[] = {
        "root_service",
        "log_service",
};
/*********************************************************************************************************
  ser_mgr �ӿ�
*********************************************************************************************************/
/*
 * ��ʼ��
 */
void T_serMgrInit ()
{
    __TG_serMgr.lineService = MX_NULL;
}
/*
 * ע�����
 */
void T_serRegist (UINT32  uiSerCode, MX_OBJECT_ID  objSer)
{
    PT_SERVICE pserNew = (PT_SERVICE)T_memObjAlloc(TMEM_SERVICE);
    if (!pserNew) {
        T_ERROR("T_memObjAlloc FAIL \n");
        return;
    }

    pserNew->objSer = objSer;
    pserNew->uiSerCode = uiSerCode;
    _LIST_LINE_INIT_NODE(pserNew->lineNode);

    _List_Line_Add_Ahead(&pserNew->lineNode, &__TG_serMgr.lineService);
}
/*
 * ���ҷ���
 */
MX_OBJECT_ID T_serFind (UINT32  uiSerCode)
{
    PMX_LIST_LINE   pNodeTem = MX_NULL;
    PT_SERVICE      pserTem  = MX_NULL;

    pNodeTem = __TG_serMgr.lineService;

    while (pNodeTem) {
        pserTem = _LIST_ENTRY(pNodeTem, T_SERVICE, lineNode);
        if (pserTem->uiSerCode == uiSerCode) {
            return  (pserTem->objSer);
        }
        pNodeTem = _list_line_get_next(pNodeTem);
    }

    return  (0);
}

/*********************************************************************************************************
  ����ģ��
*********************************************************************************************************/
#if 1
BOOL T_serHandle(PMX_IPC_MSG pIpcMsg)
{
    return  (MX_TRUE);
}

BOOL T_serErrHandle(MX_SC_RESULT_T err, PMX_IPC_MSG pIpcMsg)
{
    return  (MX_TRUE);
}

VOID T_serFrame (PMX_IPC_MSG pIpcMsg)
{
    MX_OBJECT_ID    objSender = 0;
    MX_SC_RESULT_T  errCode;

    errCode = API_ipcOpenWait(&objSender);

    do {
        if (errCode == MX_SC_RESULT_NEED_REPLY) {
            if (!T_serHandle(pIpcMsg)) {
               break;
            }
            errCode = API_ipcReplyWait(&objSender);

        } else if(errCode == MX_SC_RESULT_OK) {
            if (!T_serHandle(pIpcMsg)) {
                break;
            }
            errCode = API_ipcOpenWait(&objSender);

        } else {
            if (!T_serErrHandle(errCode, pIpcMsg)) {
                break;
            }
        }
    } while(1);

    API_ipcClose();

}
#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/
