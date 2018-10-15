/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                嵌入式 L4 微内核操作系统
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: ser_mgr.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: 服务管理
*********************************************************************************************************/
#include "t_sys.h"
/*********************************************************************************************************
  ser_mgr 单例
*********************************************************************************************************/
T_SER_MGR   __TG_serMgr;
/*
 * 服务名称
 */
CPCHAR   TG_strSerName[] = {
        "root_service",
        "log_service",
};
/*********************************************************************************************************
  ser_mgr 接口
*********************************************************************************************************/
/*
 * 初始化
 */
void T_serMgrInit ()
{
    __TG_serMgr.lineService = MX_NULL;
}
/*
 * 注册服务
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
 * 查找服务
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
  服务模型
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
