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
** 文   件   名: root_server.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 3 日
**
** 描        述: 系统服务
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
  函数声明
*********************************************************************************************************/
/*
 * log 线程
 */
VOID  T_funcLogThread(PVOID  pvArg);
/*
 * root 服务器
 */
VOID  T_rootServer(MX_OBJECT_ID  objRoot);
/*
 * 服务协议接口
 */
BOOL  TS_rootSerRegister(PMX_IPC_MSG  pIpcMsg, MX_OBJECT_ID  objSender);
BOOL  TS_rootSerFind(PMX_IPC_MSG  pMsg);
/*
 * 启动单元测试, 单元测试运行在独立线程中
 */
BOOL  TS_rootSerTestBoot(PMX_IPC_MSG  pIpcMsg, MX_OBJECT_ID  objRoot);
/*
 * 单元测试运行线程
 */
VOID  T_funcUserTestThread(PVOID pvArg);
/*********************************************************************************************************
** 函数名称: T_funcRootThread
** 功能描述: 系统线程
** 输　入  : pvArg  PT_VCPU
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID T_funcRootThread (PVOID  pvArg)
{
    /*
     * 初始化 ROOT 服务管理器
     * 注册 ROOT 服务
     * 创建并启动 LOG 线程
     * 进入服务模式（提供注册服务，名字服务)
     *      T_rootService()
     */
    PT_VCPU      pvcpuRoot = (PT_VCPU)pvArg;
    MX_OBJECT_ID objRoot   = (MX_OBJECT_ID)pvcpuRoot->pvArg;
    PT_VCPU      pvcpuLog  = MX_NULL;

    T_TITLE(T_funcRootThread);

    T_serMgrInit();                                                     /* 初始化服务管理器             */
    T_serRegist(TSER_CODE_ROOT, objRoot);                               /* 注册 ROOT 服务               */
    /*
     * 创建并启动 LOG 线程
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

#if 0                                                                   /*  测试 wakeup log 线程        */
    T_vcpuSleep(SC_USER_TIME(1, SC_UTIME_L));
    T_NOTE("wakeup log_server");
    T_vcpuWakeup(pvcpuLog->objVcpu);
#endif

#if 0                                                                   /*  测试创建 unit_test 线程     */
    TS_rootSerTestBoot(pvcpuRoot->pIpcMsg, MX_NULL);
#endif

    T_rootServer(objRoot);                                              /*  IPC 服务                    */
}
/*********************************************************************************************************
** 函数名称: T_rootService
** 功能描述: 系统服务
** 输　入  :
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL T_rootHandle (MX_OBJECT_ID  objRoot, MX_OBJECT_ID  objSender)
{
    /*
     * 服务协议
     *
     * T_ROOT_SER_REG:
     *      [IN]  UINT32           - 协议号
     *            UINT32           - 待注册服务ID
     *      [OUT] BOOL             - 执行结果: 1 成功; 0 失败
     *
     * T_ROOT_SER_FIND:
     *      [IN]  UINT32           - 协议号
     *            UINT32           - 服务ID
     *      [OUT] BOOL             - 1 成功; 0 失败
     *            MX_OBJECT_ID     - 服务对象
     *
     * T_ROOT_SER_CLOSE:
     *      [IN]  UINT32           - 协议号
     *      [OUT]
     *
     * T_ROOT_SER_TEST_BOOT:
     *      [IN]  UINT32           - 协议号
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
        T_vcpuSleep(SC_TIME_MAKE(3, SC_UTIME_L));                       /* BUG: bootload 静态数据未加载;  */
                                                                        /* 非正式参数，单位为 100ms    */
    } while(1);
}
/*********************************************************************************************************
    service handle
*********************************************************************************************************/
/*********************************************************************************************************
** 函数名称: TS_rootSerRegister
** 功能描述: 注册服务
** 输　入  : pIpcMsg
**           objSender   待注册的服务对象
** 输　出  : BOOL
** 全局变量:
** 调用模块:
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
** 函数名称: TS_rootSerFind
** 功能描述: 查找服务
** 输　入  : pIpcMsg
** 输　出  : BOOL
** 全局变量:
** 调用模块:
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
** 函数名称: TS_rootSerTestBoot
** 功能描述: 启动用户单元测试框架
** 输　入  : objRoot   root 服务对象
** 输　出  : BOOL
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  TS_rootSerTestBoot (PMX_IPC_MSG pIpcMsg, MX_OBJECT_ID objRoot)
{
    /*
     * 启动单元测试, 单元测试运行在独立线程中
     */
    PT_VCPU pvcpuTest = MX_NULL;

    T_TITLE(TS_rootSerTestBoot);

    /*
     * 创建并启动 test 线程
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
** 函数名称: TI_rootSerRegister
** 功能描述: 注册服务
** 输　入  : pIpcMsg
**           objRoot    ROOT 服务对象
**           uiSerId    待注册的服务 ID (名字)
** 输　出  : BOOL
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL TI_rootSerRegister (MX_OBJECT_ID objRoot, UINT32 uiSerId)
{
    /*
     * T_ROOT_SER_REG:
     *      [IN]  UINT32           - 协议号
     *            UINT32           - 待注册服务ID
     *      [OUT] BOOL             - 执行结果: 1 成功; 0 失败
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
** 函数名称: TI_rootSerFind
** 功能描述: 查找服务
** 输　入  : pIpcMsg
**           objRoot
**           uiSerId    待查找的服务 ID (名字)
** 输　出  : BOOL
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_OBJECT_ID TI_rootSerFind (MX_OBJECT_ID objRoot, UINT32 uiSerId)
{
    /*
     * T_ROOT_SER_FIND:
     *      [IN]  UINT32           - 协议号
     *            UINT32           - 服务ID
     *      [OUT] BOOL             - 1 成功; 0 失败
     *            MX_OBJECT_ID     - 服务对象
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
** 函数名称: TI_rootSerClose
** 功能描述: 关闭 ROOT 服务 (安全关闭)
** 输　入  : pIpcMsg
**           objRoot
** 输　出  : BOOL
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  TI_rootSerClose (MX_OBJECT_ID objRoot)
{
    /*
     * T_ROOT_SER_CLOSE:
     *      [IN]  UINT32           - 协议号
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
** 函数名称: TI_rootSerTestBoot
** 功能描述: 启动单元测试, 单元测试运行在独立线程中
** 输　入  : pIpcMsg
**           objRoo     troot 服务对象
** 输　出  : BOOL
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  TI_rootSerTestBoot (MX_OBJECT_ID objRoot)
{
    /*
     * T_ROOT_SER_TEST_BOOT:
     *      [IN]  UINT32           - 协议号
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
** 函数名称: TKI_rootSerFind
** 功能描述: 查找服务
** 输　入  : pIpcMsg
**           objRoot
**           uiSerId    待查找的服务 ID (名字)
** 输　出  : BOOL
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_OBJECT_ID TKI_rootSerFind (MX_OBJECT_ID objRoot, UINT32 uiSerId)
{
    /*
     * T_ROOT_SER_FIND:
     *      [IN]  UINT32           - 协议号
     *            UINT32           - 服务ID
     *      [OUT] BOOL             - 1 成功; 0 失败
     *            MX_OBJECT_ID     - 服务对象
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
** 函数名称: TKI_rootSerClose
** 功能描述: 关闭 ROOT 服务 (安全关闭)
** 输　入  : pIpcMsg
**           objRoot
** 输　出  : BOOL
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  TKI_rootSerClose (MX_OBJECT_ID objRoot)
{
    // PMX_IPC_MSG     pIpcMsg  = T_GET_KERN_MSG();

    return  (MX_TRUE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
