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
** 文   件   名: ipc.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2017 年 12 月 19 日
**
** 描        述: 进程间通信.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
  判断 IPC_ES 状态是否正常
  返回 IPC_ES_T 类型
*********************************************************************************************************/
#define IPC_ES_TRUE(ipcEs)           (ipcEs == IPC_ES_OK)
#define IPC_ES_FALSE(ipcEs)          (ipcEs != IPC_ES_OK)
/*********************************************************************************************************
** 函数名称: __ipcArgTargetSet
** 功能描述: 设置 IPC 参数对象
** 输　入  : pIpc      IPC 结构指针
             pipcTag   返回对象
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcArgTargetSet (PMX_IPC  pIpc, PMX_IPC  pipcTag)
{
    MX_SC_KOBJ_SET(MX_SCPARAM_GET(pIpc), IPC_VCPU_TO_OBJ(IPC_TO_VCPU(pipcTag)));
}
/*********************************************************************************************************
** 函数名称: __ipcErrorSet
** 功能描述: 设置 IPC 错误码
** 输　入  : pIpc      IPC 结构指针
             ucErrCode 错误码
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcErrorSet (PMX_IPC  pIpc, MX_SC_RESULT_T  ucErrCode)
{
    MX_SC_RESULT_SET(MX_SCPARAM_GET(pIpc), ucErrCode);
}
/*********************************************************************************************************
** 函数名称: __ipcStateAdd
** 功能描述: 增加 IPC 状态
** 输　入  : pIpc      IPC 结构指针
** 输  入  : uiAddBits 待增加的状态位
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcStateAdd (PMX_IPC  pIpc, UINT32  uiAddBits)
{
    pIpc->IPC_uiState |= uiAddBits;
}
/*********************************************************************************************************
** 函数名称: __ipcStateDel
** 功能描述: 删除 IPC 状态
** 输　入  : pIpc      IPC 结构指针
** 输  入  : uiDelBits 待删除的状态位
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcStateDel (PMX_IPC  pIpc, UINT32  uiDelBits)
{
    pIpc->IPC_uiState &= ~uiDelBits;
}
/*********************************************************************************************************
** 函数名称: __ipcStateSet
** 功能描述: 设置 IPC 状态
** 输　入  : pIpc      IPC 结构指针
** 输  入  : uiDelBits 待清理的状态位
** 输  入  : uiAddBits 待增加的状态位
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcStateSet (PMX_IPC  pIpc, UINT32  uiDelBits, UINT32  uiAddBits)
{
    pIpc->IPC_uiState &= ~uiDelBits;
    pIpc->IPC_uiState |= uiAddBits;
}
/*********************************************************************************************************
** 函数名称: __ipcStateFind
** 功能描述: 查找 IPC 状态
** 输　入  : pIpc      IPC 结构指针
** 输  入  : uiBits    待查找的状态位
** 输　出  : 返回查找到的位; 返回值: =0  没有指定状态；==uiBits 存在指定的状态
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE UINT32  __ipcStateFind (PMX_IPC  pIpc, UINT32  uiBits)
{
    return  (pIpc->IPC_uiState & uiBits);
}
/*********************************************************************************************************
** 函数名称: __ipcRmStateChange
** 功能描述: remote state 变更
** 输　入  : pIpc      IPC 结构指针
** 输  入  : uiDelBits 待清理的状态位
** 输  入  : uiAddBits 待增加的状态位
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcRmStateChange (PMX_IPC  pIpc, UINT32  uiDelBits, UINT32  uiAddBits)
{
    pIpc->IPC_uiRmStateDel |= uiDelBits;
    pIpc->IPC_uiRmStateAdd &= ~uiDelBits;
    pIpc->IPC_uiRmStateAdd |= uiAddBits;
}
/*********************************************************************************************************
** 函数名称: __ipcRmStateClean
** 功能描述: remote state 变更
** 输　入  : pIpc      IPC 结构指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcRmStateClean (PMX_IPC  pIpc)
{
    pIpc->IPC_uiRmStateDel = 0;
    pIpc->IPC_uiRmStateAdd = 0;
}
/*********************************************************************************************************
** 函数名称: __ipcStateUpdate
** 功能描述: 将 remote state 更新到本地状态
** 输　入  : pIpc      IPC 结构指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcStateUpdate (PMX_IPC  pIpc)
{
    __ipcStateSet(pIpc, pIpc->IPC_uiRmStateDel, pIpc->IPC_uiRmStateAdd);
    __ipcRmStateClean(pIpc);
}
/*********************************************************************************************************
** 函数名称: __ipcPartnerSet
** 功能描述: 设置 IPC 目标 IPC 对象
** 输　入  : pIpc      IPC 结构指针
**           pipcTag   目标 IPC 指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcTargetSet (PMX_IPC  pIpc, PMX_IPC  pipcTag)
{
    pIpc->IPC_pipcTag = pipcTag;
}
/*********************************************************************************************************
** 函数名称: __ipcPartnerGet
** 功能描述: 获取 IPC 目标 IPC 对象
** 输　入  : pIpc      IPC 结构指针
** 输　出  : 目标 IPC 指针
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE PMX_IPC  __ipcTargetGet (PMX_IPC  pIpc)
{
    return  (pIpc->IPC_pipcTag);
}
/*********************************************************************************************************
** 函数名称: __ipcTargetClean
** 功能描述: 清目标 VCPU 指针
** 输　入  : pIpc      IPC 结构指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcTargetClean (PMX_IPC  pIpc)
{
    pIpc->IPC_pipcTag = MX_NULL;
}
/*********************************************************************************************************
** 函数名称: __ipcReadySet
** 功能描述: 设置 IPC 过程中的 VCPU 就绪
** 输　入  : pIpc      IPC 结构指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcReadySet (PMX_IPC  pIpc)
{
    __ipcStateAdd(pIpc, IPC_TS_READY);
}
/*********************************************************************************************************
** 函数名称: __ipcTargetClean
** 功能描述: 设置 IPC 过程中的 VCPU 非就绪
** 输　入  : pIpc      IPC 结构指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_INLINE VOID  __ipcReadyClean (PMX_IPC  pIpc)
{
    __ipcStateDel(pIpc, IPC_TS_READY);
}
/*********************************************************************************************************
** 函数名称: ipcReadyTestSafe
** 功能描述: 获取 IPC_TS_READY 状态，函数线程安全模式
** 输　入  : pIpc      IPC 结构指针
** 输　出  : BOOL: TRUE - VCPU 就绪；FALSE - VCPU 未就绪
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  ipcReadyTestSafe (PMX_IPC  pIpc)
{
    UINT            uiState;
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);
    uiState = __ipcStateFind(pIpc, IPC_TS_READY);
    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);

    return  (uiState == IPC_TS_READY);
}
/*********************************************************************************************************
** 函数名称: ipcReadyTryTestSetSafe
** 功能描述: 测试并置位 IPC_TS_READY 状态, 线程安全模式; 非阻塞调用(必须在中断关闭的状态下被调用)
** 输　入  : pIpc        IPC 结构指针
**           pbIpcReady  传出参数，测试 IPC 是否就绪态，TRUE - 就绪
** 输　出  : FALSE       阻塞,操作失败
**           TRUE        操作成功
** 全局变量:
** 调用模块: 仅由 tick 处理函数调用
*********************************************************************************************************/
BOOL  ipcReadyTryTestSetSafe (PMX_IPC  pIpc, BOOL  *pbIpcReady)
{
    UINT uiState;                                                       /*  IPC 测试状态                */

    if (!spinTryLock(&pIpc->IPC_spStateLock)) {                         /*  阻塞返回                    */
        return  (MX_FALSE);
    }

    uiState = __ipcStateFind(pIpc, IPC_TS_READY);
    if (uiState != IPC_TS_READY) {
        __ipcReadySet(pIpc);                                            /*  置就绪态, 避免再次唤醒      */
    }
    spinUnlock(&pIpc->IPC_spStateLock);

    *pbIpcReady = (uiState == IPC_TS_READY) ? MX_TRUE : MX_FALSE;
    return  (MX_TRUE);
}
/*********************************************************************************************************
** 函数名称: __ipcReadyTestSetSf
** 功能描述: 测试并置位 IPC_TS_READY 状态；线程安全模式; 阻塞调用
** 输　入  : pIpc      IPC 结构指针
** 输　出  : 状态为非就绪则设置为就绪态，返回FALSE; 就绪返回 TRUE
** 全局变量:
** 调用模块: IPC 处理函数内部调用
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcReadyTestSetSf (PMX_IPC  pIpc)
{
    UINT            uiState;                                            /*  IPC 测试状态                */
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);

    uiState = __ipcStateFind(pIpc, IPC_TS_READY);
    if (uiState != IPC_TS_READY) {
        __ipcReadySet(pIpc);                                            /*  置就绪态, 避免再次唤醒      */
    }

    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);
    return  (uiState == IPC_TS_READY);
}
/*********************************************************************************************************
** 函数名称: __ipcServiceTestSf
** 功能描述: 测试 IPC 是否处于服务状态(可接收 IPC 请求)
** 输　入  : pIpc      IPC 结构指针
** 输　出  : BOOL: TRUE - 服务状态；FALSE - 非服务状态
** 全局变量:
** 调用模块: __ipcSendHandle
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcServiceTestSf (PMX_IPC  pIpc)
{
    UINT            uiState;
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);
    uiState = __ipcStateFind(pIpc, IPC_TS_SER);
    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);

    return  (uiState == IPC_TS_SER);
}
/*********************************************************************************************************
** 设置访问状态
*********************************************************************************************************/
#define IPC_MSG_WRITEABLE_SET(pipcMsg, bWrite)  do {            \
            (pipcMsg->IMSG_uiAccess &= ~0x1);                   \
            (pipcMsg->IMSG_uiAccess |= (bWrite ? 0x1 : 0));     \
        } while (0)
/*********************************************************************************************************
** 函数名称: ipcMsgReadOpen
** 功能描述: 设置只读状态
** 输　入  : pipcMsg
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  ipcMsgReadOpen (PMX_IPC_MSG  pipcMsg)
{
    pipcMsg->IMSG_uiOffset = 0;
    IPC_MSG_WRITEABLE_SET(pipcMsg, MX_FALSE);
}
/*********************************************************************************************************
** 函数名称: ipcMsgWriteOpen
** 功能描述: 设置只写状态
** 输　入  : pipcMsg
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  ipcMsgWriteOpen (PMX_IPC_MSG  pipcMsg)
{
    pipcMsg->IMSG_uiOffset = 0;
    pipcMsg->IMSG_uiCount  = 0;
    IPC_MSG_WRITEABLE_SET(pipcMsg, MX_TRUE);
}
/*********************************************************************************************************
** 函数名称: ipcMsgFlush
** 功能描述: 提交更新, 写结束后调用此操作
** 输　入  : pipcMsg
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  ipcMsgFlush (PMX_IPC_MSG  pipcMsg)
{
    IPC_MSG_WRITEABLE_SET(pipcMsg, MX_FALSE);                           /*  设置只读状态                */
    pipcMsg->IMSG_uiCount  = pipcMsg->IMSG_uiOffset;                    /*  将游标值更新到消息计数      */
    pipcMsg->IMSG_uiOffset = 0;                                         /*  游标归 0                    */
}
/*********************************************************************************************************
** 函数名称: ipcMsgUlWrite
** 功能描述: 写入数据
** 输　入  : pipcMsg
**           ulData       待写数据
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  ipcMsgUlWrite (PMX_IPC_MSG  pipcMsg, ULONG  ulData)
{
    if (IPC_MSG_WRITEABLE(pipcMsg)) {
        pipcMsg->IMSG_ulBuff[pipcMsg->IMSG_uiOffset++] = ulData;

        if (pipcMsg->IMSG_uiOffset >= IPC_MSG_ITEMS) {
            ipcMsgFlush(pipcMsg);
        }
        return  (MX_TRUE);
    }

    return  (MX_FALSE);
}
/*********************************************************************************************************
** 函数名称: ipcMsgUlRead
** 功能描述: 读出数据
** 输　入  : pipcMsg
**           pulData       记录读出数据
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  ipcMsgUlRead (PMX_IPC_MSG  pipcMsg, ULONG  *pulData)
{
    if (!IPC_MSG_WRITEABLE(pipcMsg)) {

        if (pipcMsg->IMSG_uiOffset >= pipcMsg->IMSG_uiCount) {
            ipcMsgWriteOpen(pipcMsg);
            return  (MX_FALSE);
        }

        *pulData = pipcMsg->IMSG_ulBuff[pipcMsg->IMSG_uiOffset++];
        return  (MX_TRUE);
    }

    return  (MX_FALSE);
}
/*********************************************************************************************************
** 函数名称: __ipcMsgCopy
** 功能描述: IPC 消息拷贝
** 输　入  : pmsgSrc      源消息结构指针
** 输　入  : pmsgTag      目标消息结构指针
** 输　出  : TRUE - 拷贝成功; FALSE - 拷贝失败
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcMsgCopy (PMX_IPC_MSG  pmsgSrc, PMX_IPC_MSG  pmsgTag)
{
    UINT32  index;

    if (!pmsgSrc || !pmsgTag || (pmsgSrc->IMSG_uiCount >= IPC_MSG_ITEMS)) {
        K_ERROR(" msg_count=%d, IPC_MSG_ITEMS=%d", pmsgSrc->IMSG_uiCount, IPC_MSG_ITEMS);
        return  (MX_FALSE);
    }

    for (index = 0; index < pmsgSrc->IMSG_uiCount; index++) {
        pmsgTag->IMSG_ulBuff[index] = pmsgSrc->IMSG_ulBuff[index];
    }

    pmsgTag->IMSG_uiCount = pmsgSrc->IMSG_uiCount;
    pmsgTag->IMSG_uiFlags = pmsgSrc->IMSG_uiFlags;

    return  (MX_TRUE);
}
/*********************************************************************************************************
** 函数名称: __ipcEvtqInit
** 功能描述: IPC 事件队列初始化
** 输　入  : pevtqQueue   事件队列
** 输　出  : NONE
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_INLINE VOID  __ipcEvtqInit (PIPC_EVTQ  pevtqQueue)
{
    pevtqQueue->IEQ_pringHead = MX_NULL;
}
/*********************************************************************************************************
** 函数名称: __ipcEvtqInsert
** 功能描述: 将 一个 IPC 对象加入另一个 IPC 对象事件队列
** 输　入  : pipcTag       提供事件队列的 IPC
**           pipcInsert    待加入队列的 IPC
**           uiIpcEvent    IPC 事件码
** 输　出  : NONE
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcEvtqInsert (PMX_IPC  pipcTag,
                                 PMX_IPC  pipcInsert,
                                 UINT32   uiIpcEvent)
{
    if (_LIST_RING_IS_NOTLNK(&pipcInsert->IPC_ringEvtqNode)) {         /*  判断节点是否已经加入队列     */
        _List_Ring_Add_Last(&pipcInsert->IPC_ringEvtqNode,             /*  在事件队列中加入节点         */
                            &(pipcTag->IPC_evtqQueue.IEQ_pringHead));
    }

    __ipcStateAdd(pipcInsert, uiIpcEvent);                             /*  设置事件状态                 */
}
/*********************************************************************************************************
** 函数名称: __ipcEvtqNext
** 功能描述: 从事件队列中取出一个节点
** 输　入  : pIpc          提供事件队列的 IPC
** 输　出  : 从事件队列中取得的优先节点关联的 IPC 对象
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_LOCAL PMX_IPC  __ipcEvtqNext (PMX_IPC  pIpc)
{
    PMX_LIST_RING  pringNext = pIpc->IPC_evtqQueue.IEQ_pringHead;

    if (pringNext != MX_NULL) {                                         /*  当前事件队列非空            */
        _List_Ring_Del(pringNext,
                       (&pIpc->IPC_evtqQueue.IEQ_pringHead));           /*  删除表头节点                */
        //K_DEBUG("get a event");
        return  (_LIST_ENTRY(pringNext, MX_IPC, IPC_ringEvtqNode));
    }

    return  (MX_NULL);
}
/*********************************************************************************************************
** 函数名称: ipcTickTimeoutPro
** 功能描述: tick 对超时线程的处理 (必须在中断关闭的状态下被调用)
** 输　入  : pIpc    超时的 IPC
** 输　出  :
** 全局变量:
** 调用模块: tick
*********************************************************************************************************/
VOID  ipcTickTimeoutPro (PMX_IPC  pIpc)
{
    PMX_IPC     pipcRecver = MX_NULL;

    __ipcRmStateChange(pIpc, 0, IPC_ES_TIMEOUT);                        /*  设置为超时状态              */

    IPC_ASSERT((pipcRecver = pIpc->IPC_pipcTag) != MX_NULL);
    spinLockIgnIrq(&pipcRecver->IPC_evtqQueue.IEQ_spLock);
    __ipcEvtqInsert(pipcRecver, pIpc, IPC_EVTS_TIMEOUT);                /*  加入超时事件                */
    spinUnlockIgnIrq(&pipcRecver->IPC_evtqQueue.IEQ_spLock);
}
/*********************************************************************************************************
** 函数名称: __ipcPlistInit
** 功能描述: IPC 优先级请求列表初始化
** 输　入  : piplList     优先级请求列表指针
** 输　出  : NONE
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcPlistInit (PIPC_PLIST  piplList)
{
    UINT32  index;

    for ( index = 0; index < MX_CFG_IPC_PRIO_MAX; index++ ) {
        piplList->IPL_pringHead[index] = MX_NULL;
    }

    piplList->IPL_uiPrioCur  = MX_CFG_IPC_PRIO_MAX - 1;
    piplList->IPL_uiSignal   = 0;
}
/*********************************************************************************************************
** 函数名称: __ipcPlistPrioFlush
** 功能描述: 更新优先级请求列表中当前最高优先级
** 输　入  : piplList     优先级请求列表指针
** 输　出  : TRUE - 列表不为空; FLASE - 列表为空
** 全局变量:
** 调用模块: PRIO_LIST 内部函数
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcPlistPrioFlush (PIPC_PLIST  piplList)
{
    UINT32      index;
    UINT32      uiPrioCur = piplList->IPL_uiPrioCur;                    /*  当前最高优先级              */

    IPC_ASSERT(uiPrioCur < MX_CFG_IPC_PRIO_MAX);

    if (piplList->IPL_pringHead[uiPrioCur] != MX_NULL) {                /*  当前优先级链表非空          */
        return  (MX_TRUE);
    }

    for (index = ++uiPrioCur; index < MX_CFG_IPC_PRIO_MAX; index++) {   /*  向优先级低方向搜索          */
        if (piplList->IPL_pringHead[index] != MX_NULL) {
            piplList->IPL_uiPrioCur = index;
            return  (MX_TRUE);
        }
    }

    for (index = 0; index < uiPrioCur; index++) {                       /*  如果优先级低方向未搜索到    */
                                                                        /*  有效链表，再从数组头开始搜索*/
        if (piplList->IPL_pringHead[index] != MX_NULL) {
            piplList->IPL_uiPrioCur = index;
            return  (MX_TRUE);
        }
    }

    piplList->IPL_uiPrioCur = MX_CFG_IPC_PRIO_MAX - 1;                  /*  当前列表为空                */
    return  (MX_FALSE);
}
/*********************************************************************************************************
** 函数名称: __ipcPlistInsert
** 功能描述: 将 一个 IPC 对象加入另一个 IPC 对象优先级请求表
** 输　入  : pipcTag       提供优先级表的 IPC
** 输　入  : uiPrio        pipcInsert 关联优先级
** 输　入  : pipcInsert    待加入表的 IPC
** 输　出  : NONE
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcPlistInsert (PMX_IPC pipcTag,  UINT32 uiPrio,
                                  PMX_IPC pipcInsert)
{
    IPC_ASSERT(uiPrio < MX_CFG_IPC_PRIO_MAX);

    _List_Ring_Add_Last(&pipcInsert->IPC_ringPlistNode,                 /*  在相应优先级的表中加入节点  */
        &(pipcTag->IPC_plistPrio.IPL_pringHead[uiPrio]));

    /*
     *  新加入节点优先级高于当前最高优先级，更新当前记录
     */
    if (uiPrio < pipcTag->IPC_plistPrio.IPL_uiPrioCur) {
        pipcTag->IPC_plistPrio.IPL_uiPrioCur = uiPrio;
    }

    //K_DEBUG("uiPrioCur=%d", uiPrio);
}
/*********************************************************************************************************
** 函数名称: __ipcPlistInList
** 功能描述: 判断 IPC 是否在表中
** 输　入  : pipc     待检查的 IPC
** 输　出  : BOOL     TRUE - 在表中
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_INLINE BOOL  __ipcPlistInList (PMX_IPC  pIpc)
{
    return  (!_LIST_RING_IS_NOTLNK(&pIpc->IPC_ringPlistNode));
}
/*********************************************************************************************************
** 函数名称: __ipcPlistRemove
** 功能描述: 将 一个 IPC 对象从另一个 IPC 对象优先级请求表中移除
** 输　入  : pipcTag       提供优先级表的 IPC
** 输　入  : uiPrio        pipcRemove 关联优先级
** 输　入  : pipcInsert    待移除的 IPC
** 输　出  : NONE
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcPlistRemove (PMX_IPC  pipcTag, UINT32 uiPrio, PMX_IPC  pipcRemove)
{
    IPC_ASSERT(uiPrio < MX_CFG_IPC_PRIO_MAX);

    _List_Ring_Del(&pipcRemove->IPC_ringPlistNode,                      /*  在相应优先级的表中删除节点  */
                   &(pipcTag->IPC_plistPrio.IPL_pringHead[uiPrio]));

    __ipcPlistPrioFlush(&pipcTag->IPC_plistPrio);                       /*  更新优先级表当前最高优先级  */
}
/*********************************************************************************************************
** 函数名称: __ipcPlistNext
** 功能描述: 寻找优先级最高的节点并从表中移除
** 输　入  : pIpc          提供优先级表的 IPC
** 输　出  : 从优先级列表中取得的最高优先级节点关联的 IPC 对象
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_LOCAL PMX_IPC  __ipcPlistNext (PMX_IPC  pIpc)
{
    PIPC_PLIST           piplList  = &pIpc->IPC_plistPrio;
    UINT32               uiPrioCur = piplList->IPL_uiPrioCur;           /*  当前最高优先级              */

    PMX_LIST_RING        pringNext = MX_NULL;

    IPC_ASSERT(uiPrioCur < MX_CFG_IPC_PRIO_MAX);
    //K_DEBUG("uiPrioCur=%d", uiPrioCur);

    pringNext  = piplList->IPL_pringHead[uiPrioCur];                    /*  获取链表表头节点为候选节点  */

    if (pringNext != MX_NULL) {                                         /*  当前优先级链表非空          */
        _List_Ring_Del(pringNext,
                       (&piplList->IPL_pringHead[uiPrioCur]));          /*  删除表头节点                */
        __ipcPlistPrioFlush(piplList);                                  /*  更新优先级列表当前优先级    */
        return  (_LIST_ENTRY(pringNext, MX_IPC, IPC_ringPlistNode));
    }

    return  (MX_NULL);
}
/*********************************************************************************************************
** 函数名称: __ipcPlistSignalUpSf
** 功能描述: 信号计数累加
** 输　入  : pIpc          提供优先级表的 IPC
** 输　出  :
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_INLINE VOID  __ipcPlistSignalAddSf (PMX_IPC  pIpc)
{
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);
    pIpc->IPC_plistPrio.IPL_uiSignal++;
    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);
}
/*********************************************************************************************************
** 函数名称: __ipcPlistSignalDelSf
** 功能描述: 信号计数累减
** 输　入  : pIpc          提供优先级表的 IPC
** 输　出  :
** 全局变量:
** 调用模块: IPC 模块接口函数
*********************************************************************************************************/
IPC_INLINE VOID  __ipcPlistSignalDelSf (PMX_IPC  pIpc)
{
    INTREG          iregInterLevel;

    spinLockAndDisIrq(&pIpc->IPC_spStateLock, &iregInterLevel);
    pIpc->IPC_plistPrio.IPL_uiSignal--;
    spinUnlockAndEnIrq(&pIpc->IPC_spStateLock, iregInterLevel);
}
/**********************************************************************************************************
** 函数名称: ipcInit
** 功能描述: IPC 初始化接口，创建VCPU时进行初始化
** 输　入  : pIpc      IPC 结构指针
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  ipcInit (PMX_IPC  pIpc)
{
    pIpc->IPC_pMsg = kobjGet(kobjAlloc(_KOBJ_IPC_MSG));                 /*  分配 IPC_MSG                */
#if 0
    IPC_ASSERT(pIpc->IPC_pMsg);
#endif
    __ipcStateDel(pIpc, IPC_S_ALL_MASK);                                /*  状态清空                    */
    __ipcRmStateClean(pIpc);                                            /*  异步状态清空                */

    _LIST_RING_INIT_NODE_PTR(&pIpc->IPC_ringEvtqNode);                  /*  事件队列结点初始化          */
    __ipcEvtqInit(&pIpc->IPC_evtqQueue);                                /*  事件队列初始化              */

    _LIST_RING_INIT_NODE_PTR(&pIpc->IPC_ringPlistNode);                 /*  优先级请求表结点初始化      */
    __ipcPlistInit(&pIpc->IPC_plistPrio);                               /*  优先级请求表初始化          */

    spinInit(&pIpc->IPC_spStateLock);                                   /*  初始化自旋锁                */
    spinInit(&pIpc->IPC_spRmStateLock);
    spinInit(&pIpc->IPC_evtqQueue.IEQ_spLock);
    spinInit(&pIpc->IPC_plistPrio.IPL_spLock);
}
/*********************************************************************************************************
  IPC 过程
              +---> SEND --!R--> SEND_WT ----> SEND_FI -----------------+
              |       |             |                                   |
      ENTRY --+       +-RECV[O]--+  +---------RECV[C]---------+         +----> RETURN
              |                  |                            |         |
              +---> RECV ------> RECV_WT ----> RECV_IN -----> RECV_FI --+

*********************************************************************************************************/
typedef   IPC_ES_T (*IPC_HANDLE) (PMX_IPC);

IPC_LOCAL IPC_ES_T __ipcSendHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcSendWaitHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcRecvHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcRecvWaitHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcRecvInHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcSendFiHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcRecvFiHandle (PMX_IPC pIpc);
IPC_LOCAL IPC_ES_T __ipcCloseHandle (PMX_IPC pIpc);
/*********************************************************************************************************
** 函数名称: __ipcStateCheckPro
** 功能描述: 检查 IPC 状态，设置异常与错误码
** 输　入  : pIpc          IPC 结构指针
**           uiIpcState    IPC 当前状态
** 输　出  : IPC_ES_T      异常码; IPC_ES_OK - 操作成功
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_ES_T  __ipcStateCheckPro (PMX_IPC  pIpc, UINT32  uiIpcStage)
{
    IPC_ES_T    ucResult    = IPC_ES_OK;
    PMX_IPC     pipcTag     = MX_NULL;                                  /*  目标 IPC                    */
    UINT32      uiIpcState;                                             /*  IPC 状态                    */

    /*
     *  检查异常状态
     */
    ucResult = __ipcStateFind(pIpc, IPC_ES_MASK);
    if (IPC_ES_FALSE(ucResult)) {
        K_ERROR(" ES=0x%x",
                (pIpc->IPC_uiState & IPC_ES_MASK));
        return  (ucResult);
    }

    /*
     *  检查 IPC 状态
     */
    switch (uiIpcStage) {

    case IPC_S_SEND:
        if (!__ipcServiceTestSf(pIpc->IPC_pipcTag)) {                   /*  预检查目标 IPC 是否开启服务 */
                                                                        /*  在加入 IPC 请求时需再次检查 */
            K_ERROR("SEND, no service");
            __ipcStateAdd(pIpc, IPC_ES_ERROR);
            __ipcErrorSet(pIpc, MX_SC_RESULT_NOT_SER);
            ucResult = IPC_ES_ERROR;
        }
        break;

    case IPC_S_SEND | IPC_S_REPLY:
        break;

    case IPC_S_REPLY:
        pipcTag = __ipcTargetGet(pIpc);
        if (pipcTag == MX_NULL                      ||
            __ipcTargetGet(pipcTag) != pIpc         ||                  /*  target 检查                 */
            __ipcStateFind(pipcTag, IPC_S_MASK) !=
             (IPC_S_SEND_WT | IPC_S_RECV) )           {                 /*  target IPC 为等待接收状态   */
            K_ERROR("REPLY");
            ucResult = IPC_ES_ERROR;
        }
        break;

    case IPC_S_SEND_WT:
        uiIpcState  = __ipcStateFind(pIpc, IPC_S_NONE);
        //K_DEBUG(":SEND_WT, ipcState=0x%x", uiIpcState);
        if (uiIpcState != IPC_S_SEND_FI &&                              /*  IPC状态: IPC_S_SEND_FI 或   */
            uiIpcState != IPC_S_RECV_FI)  {                             /*  IPC_S_RECV_FI 状态          */
            __ipcStateAdd(pIpc, IPC_ES_ERROR);
            __ipcErrorSet(pIpc, MX_SC_RESULT_STATE);
            ucResult = IPC_ES_ERROR;
        }
        break;

    case IPC_S_RECV:
    case IPC_S_RECV_WT:
    case IPC_S_RECV_IN:
    case IPC_S_RECV_FI:
    default:
        break;
    }
    return  (ucResult);
}
/*********************************************************************************************************
** 函数名称: __ipcErrorPro
** 功能描述: IPC 错误处理
** 输　入  : pIpc          IPC 结构指针
**           uiIpcState    IPC 当前状态
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  __ipcErrorPro (PMX_IPC  pIpc, UINT32  uiIpcStage)
{
    K_ERROR("__ipcErrorPro");

    __ipcStateDel(pIpc, IPC_S_IPC_MASK);
    __ipcTargetClean(pIpc);
    return;
}
/*********************************************************************************************************
** 函数名称: __ipcSendPro
** 功能描述: IPC 发送子程序, 发送消息
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendPro (PMX_IPC  pIpc)
{
    PMX_IPC     pipcTag      = pIpc->IPC_pipcTag;
    PMX_VCPU    pvcpuTag     = IPC_TO_VCPU(pipcTag);
    PIPC_EVTQ   pevtqTag     = &pipcTag->IPC_evtqQueue;
    ULONG       ulTicks      = 0;

    INTREG      iregIntLevel;

    //K_DEBUG(__ipcSendPro);
    /*
     *  NOTE: IPC_TS_SER 状态同步
     */
#if 1
    if (__ipcStateFind(pipcTag, IPC_TS_SER) != IPC_TS_SER) {            /*  目标线程不在 IPC 服务状态   */
        K_WARNING(" server not in serivce");
        __ipcStateAdd(pIpc, IPC_ES_ERROR);
        __ipcErrorSet(pIpc, MX_SC_RESULT_NOT_SER);
        return  (IPC_ES_ERROR);
    }
#endif

    __ipcStateSet(pIpc, IPC_S_SEND, IPC_S_SEND_WT);                     /*  IPC 将进入 SEND_WT 状态     */
    __ipcReadyClean(pIpc);                                              /*  IPC 将进入非就绪状态        */
    scTimeToTicks(MX_SC_TIME_GET(MX_SCPARAM_GET(pIpc)), &ulTicks);

    spinLockAndDisIrq(&pevtqTag->IEQ_spLock, &iregIntLevel);            /*  加事件队列锁                */
    __ipcEvtqInsert(pipcTag, pIpc, IPC_EVTS_REQUEST);                   /*  插入 IPC 请求事件           */
    spinUnlockAndEnIrq(&pevtqTag->IEQ_spLock, iregIntLevel);            /*  释放事件队列锁              */

    iregIntLevel = kernelEnterAndDisIrq();                              /*  加内核锁                    */

    __ipcPlistSignalAddSf(pipcTag);                                     /*  消息计数累加                */
                                                                        /*  与service 之间需消息锁同步  */

    if (__ipcStateFind(pipcTag, IPC_TS_SER_PEND) == IPC_TS_SER_PEND) {
         __ipcStateSet(pipcTag, IPC_TS_SER_PEND, 0);                    /*  清除服务阻塞状态            */
         K_NOTE(" wakeup vcpu=0x%x", pvcpuTag);
         vcpuWakeup(pvcpuTag);                                          /*  之前无 IPC 请求则唤醒接收方 */
    }

    vcpuSleep(ulTicks);                                                 /*  发送方休眠, 重新调度时阻塞  */
    IPC_SET_IN_PROCESS(pIpc, MX_TRUE);                                  /*  IPC 休眠                    */
    K_WARNING(" SLEEP=0x%x, tick=%d", IPC_TO_VCPU(pIpc), ulTicks);

    kernelExitAndEnIrq(iregIntLevel);                                   /*  阻塞点:释放内核锁, 执行调度 */

    return  (IPC_ES_OK);
}
/*********************************************************************************************************
** 函数名称: __ipcSendReplyPro
** 功能描述: IPC 发送子程序，回复消息
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendReplyPro (PMX_IPC  pIpc)
{
    PMX_IPC     pipcTag   = __ipcTargetGet(pIpc);
    PMX_VCPU    pvcpuTag  = IPC_TO_VCPU(pipcTag);

    IPC_ASSERT(pvcpuTag != MX_NULL);
    //K_DEBUG("start, pvcpuTag=0x%x", pvcpuTag);
    /*
     *  请求线程超时，超时事件己加入事件表处理
     *  未超时则正常处理，回复 IPC 请求
     */
    if (!__ipcReadyTestSetSf(pipcTag)) {                                /*  未超时处理                  */

        if (IPC_ES_FALSE(__ipcStateCheckPro(pIpc, IPC_S_REPLY))) {      /*  检查 IPC 状态合法性         */
            K_ERROR(" target ERROR");
            __ipcRmStateChange(pipcTag, IPC_S_NULL, IPC_ES_ERROR);      /*  设置目标 IPC 错误           */

        } else if (!__ipcMsgCopy(pIpc->IPC_pMsg, pipcTag->IPC_pMsg)) {  /*  IPC 消息传递                */
            /*
             *  NOTE: 记录日志或向上级反馈，内存问题可能是不可恢复
             */
            K_ERROR(" __ipcMsgCopy ERROR");
            __ipcRmStateChange(pipcTag, IPC_S_NULL, IPC_ES_ERROR);      /*  设置目标 IPC 错误           */

        } else {
            __ipcRmStateChange(pipcTag, IPC_S_RECV_WT, IPC_S_RECV_FI);  /*  设置目标 IPC 接收完成态     */
        }

        K_NOTE("__ipcSendReplyPro: wakeup vcpu=0x%x", pvcpuTag);
        vcpuWakeup(pvcpuTag);                                           /*  唤醒目标 VCPU; 执行调度     */
    }

    __ipcTargetClean(pIpc);                                             /*  清除目标 IPC                */
    __ipcStateDel(pIpc, IPC_S_SEND | IPC_S_REPLY);                      /*  IPC 清除发送状态            */

    if (__ipcStateFind(pIpc, IPC_S_RECV) == IPC_S_RECV) {               /*  判断是否还有接收操作        */
        __ipcStateSet(pIpc, IPC_S_RECV, IPC_S_RECV_WT);                 /*  IPC 设置接收等待状态        */

    } else {
        __ipcStateAdd(pIpc, IPC_S_SEND_FI);                             /*  IPC 设置接收完成状态        */
    }
    return  (IPC_ES_OK);
}
/*********************************************************************************************************
** 函数名称: __ipcSendHandle
** 功能描述: IPC 发送阶段子程序
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendHandle (PMX_IPC  pIpc)
{
    IPC_ES_T    uiResult    = IPC_ES_OK;
    UINT32      uiIpcStage;                                             /*  IPC 阶段                    */
    UINT32      uiIpcState;

    K_TITLE(__ipcSendHandle);

    if (__ipcStateFind(pIpc, IPC_S_REPLY) == IPC_S_REPLY) {             /*  IPC 回复消息处理            */
        uiIpcStage = IPC_S_SEND | IPC_S_REPLY;
        uiResult   = __ipcStateCheckPro(pIpc, uiIpcStage);

        if (IPC_ES_FALSE(uiResult)) {                                   /*  检查 IPC 状态合法性         */
            K_ERROR("TAG1, result=%x", uiResult);
            __ipcErrorPro (pIpc, IPC_S_SEND);                           /*  错误处理                    */
            return  (uiResult);
        }
        /*
         *  IPC 请求线程休眠状态不确定, 不能预先检查其状态
         *  __ipcSendReplyPro 中需要检查请求 IPC 状态
         */
        uiIpcStage = IPC_S_REPLY;
        uiResult = __ipcSendReplyPro(pIpc);

    } else {                                                            /*  IPC 发送消息处理            */
        uiIpcStage = IPC_S_SEND;
        uiResult   = __ipcStateCheckPro(pIpc, uiIpcStage);

        if (IPC_ES_FALSE(uiResult)) {                                   /*  检查 IPC 状态合法性         */
            K_ERROR("TAG2, result=%x", uiResult);
            __ipcErrorPro (pIpc, IPC_S_SEND);                           /*  错误处理                    */
            return  (uiResult);
        }

        uiResult = __ipcSendPro(pIpc);
        //K_DEBUG("__ipcSendPro RESULT=%x", uiResult);
    }

    if (IPC_ES_FALSE(uiResult)) {
        K_ERROR("TAG3, result=%x", uiResult);
         __ipcErrorPro (pIpc, uiIpcStage);                              /*  错误处理                    */
         return  (uiResult);
     }

    uiIpcState = __ipcStateFind(pIpc, IPC_S_NONE);
    switch (uiIpcState) {
    case IPC_S_SEND_WT:
    case IPC_S_SEND_WT | IPC_S_RECV:
        uiResult = __ipcSendWaitHandle(pIpc);                           /*  进入 IPC_SEND_WT 阶段       */
        break;

    case IPC_S_RECV_WT:
        uiResult = __ipcRecvWaitHandle(pIpc);                           /*  进入 IPC_RECV_WT 阶段       */
        break;

    case IPC_S_RECV_FI:
        uiResult = __ipcRecvFiHandle(pIpc);                             /*  进入 IPC_RECV_FI 阶段       */
        break;

    default:
        IPC_ASSERT(MX_FALSE);
        __ipcErrorSet(pIpc, MX_SC_RESULT_FAIL);
        K_ERROR("TAG4, result=%d", uiResult);
        __ipcErrorPro (pIpc, uiIpcStage);
        uiResult = IPC_ES_ERROR;
    }
    return  (uiResult);
}
/*********************************************************************************************************
** 函数名称: __ipcSendWaitHandle
** 功能描述: IPC 发送等待阶段子程序
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendWaitHandle (PMX_IPC  pIpc)
{
    IPC_ES_T uiResult    = IPC_ES_OK;

    K_TITLE(__ipcSendWaitHandle);

    /*
     *  未就绪态, 阻塞等待; 可能的情况: sender sleep 还未调度线程
     */
    while (!ipcReadyTestSafe(pIpc)) {
        K_NOTE(" RE_SCHED");
        kernelSched();
    }
    K_WARNING(" WAKEUP=0x%x", IPC_TO_VCPU(pIpc));
    __ipcStateUpdate(pIpc);                                             /*  IPC 唤醒，更新本地状态      */
    /*
     *  如果被时钟唤醒，则已标识 TIMEOUT 异常
     */
    uiResult = __ipcStateCheckPro(pIpc, IPC_S_SEND_WT);                 /*  检查 IPC 状态合法性         */
    if (uiResult == IPC_ES_TIMEOUT) {
        K_WARNING(" TIMEOUT");

    } else if (IPC_ES_FALSE(uiResult)) {
        K_ERROR(" uiResult=%x", uiResult);
        __ipcErrorPro (pIpc, IPC_S_SEND_WT);                            /*  通用错误处理                */
        return  (uiResult);
    }

    if (__ipcStateFind(pIpc, IPC_S_SEND_FI) == IPC_S_SEND_FI) {         /*  进入 IPC_SEND_FI 阶段       */
        uiResult = __ipcSendFiHandle(pIpc);

    } else {                                                            /*  进入 IPC_RECV_FI 阶段       */
        uiResult = __ipcRecvFiHandle(pIpc);
    }
    return  (uiResult);
}
/*********************************************************************************************************
** 函数名称: __ipcRecvHandle
** 功能描述: IPC 接收阶段子程序
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcRecvHandle (PMX_IPC  pIpc)
{
    INTREG          iregIntLevel;

    K_TITLE(__ipcRecvHandle);
    if (IPC_ES_FALSE(__ipcStateCheckPro(pIpc, IPC_S_RECV))) {           /*  检查 IPC 状态合法性         */
        K_ERROR(__ipcRecvHandle);
        __ipcErrorPro (pIpc, IPC_S_RECV);                               /*  异常处理                    */
        return  (IPC_ES_ERROR);
    }

    spinLockAndDisIrq(&pIpc->IPC_plistPrio.IPL_spLock, &iregIntLevel);
    __ipcStateAdd(pIpc, IPC_TS_SER);                                    /*  开启 IPC 服务模式           */
    spinUnlockAndEnIrq(&pIpc->IPC_plistPrio.IPL_spLock, iregIntLevel);

    __ipcStateSet(pIpc, IPC_S_RECV, IPC_S_RECV_WT);                     /*  设置为 IPC_RECV_WT 状态     */
    return  (__ipcRecvWaitHandle(pIpc));                                /*  进入 IPC_RECV_WT 阶段       */
}
/*********************************************************************************************************
** 函数名称: __ipcRecvEventPro
** 功能描述: 处理 IPC 异步事件
** 输　入  : pIpc      IPC 结构指针
** 输　出  : pIpcEvt   产生事件的 IPC
** 全局变量:
** 调用模块: __ipcRecvWaitHandle
*********************************************************************************************************/
IPC_LOCAL VOID  __ipcRecvEventPro (PMX_IPC  pIpc, PMX_IPC  pIpcEvt)
{
    UINT32 uiRequest = __ipcStateFind(pIpcEvt,
                                     IPC_EVTS_REQUEST |
                                     IPC_EVTS_TIMEOUT);
    UINT32 uiEvtPrio = 0;

    //K_DEBUG("start");
    vcpuGetPriority(IPC_TO_VCPU(pIpcEvt), &uiEvtPrio);

    if (uiRequest == IPC_EVTS_REQUEST) {                                /*  只有 IPC 请求事件           */
        __ipcPlistInsert(pIpc, uiEvtPrio, pIpcEvt);                     /*  IPC 加入接收方优先级请求表  */
        __ipcRmStateChange(pIpcEvt, IPC_EVTS_MASK, IPC_S_NULL);         /*  清除事件                    */

    } else if (uiRequest) {                                             /*  IPC 超时事件                */
        if (__ipcPlistInList(pIpcEvt)) {                                /*  如果超时 IPC 已加入请求表   */
            __ipcPlistRemove(pIpc, uiEvtPrio, pIpcEvt);                 /*  将超时 IPC 从请求表移除     */
            __ipcPlistSignalDelSf(pIpc);                                /*  信号计数累减                */
        }

        __ipcRmStateChange(pIpcEvt, IPC_EVTS_MASK, IPC_S_NULL);         /*  清除事件                    */

        K_NOTE(" wakeup timeout vcpu=0x%x", IPC_TO_VCPU(pIpcEvt));
        vcpuWakeup(IPC_TO_VCPU(pIpcEvt));                               /*  唤醒超时线程;其超时状态     */
                                                                        /*  由触发其超时的线程设置      */
    } else {                                                            /*  其它事件暂不处理            */
        IPC_ASSERT(FALSE);
    }
}
/*********************************************************************************************************
** 函数名称: __ipcRecvNextPro
** 功能描述: 从 IPC 优先级列表中取出优先级最高的 IPC
** 输　入  : pIpc      IPC 结构指针
** 输　出  : PMX_IPC   NULL - 列表为空
** 全局变量:
** 调用模块: __ipcRecvWaitHandle
*********************************************************************************************************/
IPC_LOCAL PMX_IPC  __ipcRecvNextPro (PMX_IPC  pIpc)
{
    PMX_IPC  pNext = __ipcPlistNext (pIpc);                             /*  从列表中取出一个待处理项    */
#if 0
    pNext = pNext ? pNext : pIpc->IPC_pipcTag;                          /*  NOTE: 只有单个IPC请求时     */
#endif                                                                  /*  直接入 ipcTarget            */

    if (pNext) {
        __ipcPlistSignalDelSf(pIpc);                                    /*  信号计数累减                */
    }
    //K_DEBUG("next_vcpu=0x%x", IPC_TO_VCPU(pNext));

    return  (pNext);
}
/*********************************************************************************************************
** 函数名称: __ipcRecvWaitHandle
** 功能描述: IPC 接收等待阶段子程序
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcRecvWaitHandle (PMX_IPC  pIpc)
{
    IPC_ES_T    uiResult   = IPC_ES_OK;
    PIPC_EVTQ   pevtqThis  = &pIpc->IPC_evtqQueue;
    PIPC_PLIST  piplThis   = &pIpc->IPC_plistPrio;
    PMX_IPC     pIpcNext   = MX_NULL;

    INTREG      iregIntLevel;

    while (1) {
        K_TITLE(__ipcRecvWaitHandle);
        /*
         *  事件处理
         */
        do {                                                            /*  先处理完所有异步事件        */
            spinLockAndDisIrq(&pevtqThis->IEQ_spLock, &iregIntLevel);
            pIpcNext = __ipcEvtqNext(pIpc);                             /*  取出一个事件                */
            spinUnlockAndEnIrq(&pevtqThis->IEQ_spLock, iregIntLevel);

            if (pIpcNext) {
                __ipcRecvEventPro(pIpc, pIpcNext);                      /*  处理异步事件                */
            }
        } while (pIpcNext);
        /*
         *  获取 IPC 请求
         */
        iregIntLevel = kernelEnterAndDisIrq();                          /*  内核锁                      */
        if (piplThis->IPL_uiSignal == 0) {                              /*  无 IPC 请求空则阻塞等待     */
                                                                        /*  此处计数只需内核锁同步      */
            __ipcStateSet(pIpc, 0, IPC_TS_SER_PEND);                    /*  置阻塞状态                  */
            vcpuSuspend();                                              /*  之前无 IPC 请求则唤醒接收方 */

            K_WARNING(" PENDING=0x%x", IPC_TO_VCPU(pIpc));
            kernelExitAndEnIrq(iregIntLevel);                           /*  阻塞点: 发生调度            */
            K_WARNING(" WAKEUP=0x%x", IPC_TO_VCPU(pIpc));
            continue;
        }
        kernelExitAndEnIrq(iregIntLevel);

        pIpcNext = __ipcRecvNextPro(pIpc);                              /*  从优先级列表中获取待处理IPC */
        if (pIpcNext == MX_NULL) {                                      /*  IPC 请求在事件表中, 重询    */
            continue;
        }
        /*
         *  处理 IPC 请求
         */
        __ipcStateSet(pIpc, IPC_S_RECV_WT, IPC_S_RECV_IN);              /*  设置为 IPC_S_RECV_IN 状态   */
        __ipcTargetSet(pIpc, pIpcNext);                                 /*  设置为 IPC 目标对象         */
        uiResult = __ipcRecvInHandle(pIpc);                             /*  进入 IPC_RECV_IN 阶段       */

        if (IPC_ES_TRUE(uiResult)                          &&
            __ipcStateFind(pIpc, IPC_S_NONE) == IPC_S_RECV_WT) {        /*  IPC 继续处理下一请求        */
            //K_DEBUG("handle next ipc");
            continue;
        }

        return  (uiResult);
    }
}
/*********************************************************************************************************
** 函数名称: __ipcRecvPro
** 功能描述: IPC 接收处理子程序；进入之前已检测就绪态（就绪锁）
** 输　入  : pIpc      IPC 结构指针
**           pIpcNext  待处理的 IPC 请求对象
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块: __ipcRecvInHandle
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcRecvPro (PMX_IPC  pIpc, PMX_IPC  pipcNext)
{
    INTREG          iregIntLevel;

    spinLockAndDisIrq(&pipcNext->IPC_spStateLock, &iregIntLevel);

    if (__ipcStateFind(pipcNext, IPC_TS_READY) == IPC_TS_READY) {       /*  已超时，超时事件待处理      */
        spinUnlockAndEnIrq(&pipcNext->IPC_spStateLock, iregIntLevel);
        return  (IPC_ES_TIMEOUT);                                       /*  返回超时状态, 不注册异常    */
    }

    if (__ipcStateFind(pipcNext, IPC_S_RECV) == 0) {
        /*
         *  目标 VCPU 没有接收阶段, 将进入 SEND_FI 状态
         */
        __ipcReadySet(pipcNext);                                        /*  目标 IPC 设为就绪           */
        spinUnlockAndEnIrq(&pipcNext->IPC_spStateLock, iregIntLevel);   /*  及时释放锁, 避免 Copy 阻塞  */

        IPC_ASSERT(__ipcMsgCopy(pipcNext->IPC_pMsg, pIpc->IPC_pMsg));   /*  IPC 消息传递                */

        __ipcRmStateChange(pipcNext, IPC_S_SEND_WT, IPC_S_SEND_FI);     /*  目标 IPC 设为 SEND_FI 态    */

        K_NOTE(" wakeup vcpu=0x%x", IPC_TO_VCPU(pipcNext));
        vcpuWakeup(IPC_TO_VCPU(pipcNext));                          /*  唤醒目标 VCPU; 执行调度     */

    } else {
        /*
         *  目标 VCPU 有接收阶段, 目标 VCPU 继续等待
         *  NOTE: 可优化 - 消息拷贝占用中断时间过长;
         *        一种简单方案是直接将受理 IPC 从唤醒表移除
         */
        IPC_ASSERT(__ipcMsgCopy(pipcNext->IPC_pMsg, pIpc->IPC_pMsg));   /*  IPC 消息传递                */
        __ipcStateAdd(pIpc, IPC_S_REPLY);                               /*  IPC 回复状态,RECV_FI 预判   */
        __ipcRmStateChange(pipcNext, IPC_S_SEND_WT | IPC_S_RECV,        /*  目标 IPC 设为 RECV_WT 态    */
                           IPC_S_RECV_WT);

        spinUnlockAndEnIrq(&pipcNext->IPC_spStateLock, iregIntLevel);
    }

    return  (IPC_ES_OK);
}
/*********************************************************************************************************
** 函数名称: __ipcRecvInHandle
** 功能描述: IPC 接收处理阶段子程序
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcRecvInHandle (PMX_IPC  pIpc)
{
    UINT32  uiResult = IPC_ES_OK;

    K_TITLE(__ipcRecvInHandle);

    IPC_ASSERT (pIpc->IPC_pipcTag != NULL);
    uiResult = __ipcRecvPro(pIpc, pIpc->IPC_pipcTag);

    if (IPC_ES_FALSE(uiResult)) {                                       /*  状态异常                    */
        if (IPC_ES_TRUE(__ipcStateFind(pIpc, IPC_ES_MASK))) {           /*  可恢复异常，恢复至 RECV_WT  */
            __ipcStateSet(pIpc, IPC_S_RECV_IN, IPC_S_RECV_WT);
            __ipcTargetClean(pIpc);

        } else {                                                        /*  错误，中止 IPC              */
            __ipcErrorPro(pIpc, IPC_S_RECV_IN);                         /*  错误处理                    */
        }

        return  (uiResult);
    }

    __ipcStateSet(pIpc, IPC_S_RECV_IN, IPC_S_RECV_FI);                  /*  设置为 IPC_S_RECV_FI 状态   */
    return  (__ipcRecvFiHandle(pIpc));                                  /*  进入 IPC_S_RECV_FI 阶段     */
}
/*********************************************************************************************************
** 函数名称: __ipcSendFiHandle
** 功能描述: IPC 发送完成阶段子程序
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcSendFiHandle (PMX_IPC  pIpc)
{
    K_TITLE(__ipcSendFiHandle);

    __ipcTargetClean(pIpc);                                             /*  清除目标 IPC                */
    __ipcStateDel(pIpc, IPC_S_IPC_MASK);                                /*  清除 IPC 状态               */

    __ipcErrorSet(pIpc, MX_SC_RESULT_OK);                               /*  设置 IPC 结果               */
    return (IPC_ES_OK);
}
/*********************************************************************************************************
** 函数名称: __ipcRecvFiHandle
** 功能描述: IPC 接收完成阶段子程序
** 输　入  : pIpc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcRecvFiHandle (PMX_IPC  pIpc)
{
    PMX_IPC pipcTag = __ipcTargetGet(pIpc);
    IPC_ASSERT(pipcTag != MX_NULL);

    K_TITLE(__ipcRecvFiHandle);

    if (__ipcStateFind(pIpc, IPC_S_REPLY) == IPC_S_REPLY) {
        __ipcErrorSet(pIpc, MX_SC_RESULT_NEED_REPLY);                   /*  需要回复 IPC 请求           */
        __ipcArgTargetSet(pIpc, pipcTag);                               /*  设置 IPC 通信方             */

    } else {
        __ipcTargetClean(pIpc);                                         /*  清除目标 IPC                */
        __ipcErrorSet(pIpc, MX_SC_RESULT_OK);
    }
    __ipcStateDel(pIpc, IPC_S_IPC_MASK);                                /*  清除 IPC 状态               */
    return  (IPC_ES_OK);
}
/*********************************************************************************************************
** 函数名称: __ipcCloseHandle
** 功能描述: 关闭 IPC 服务
** 输　入  : pipc      IPC 结构指针
** 输　出  : IPC_ES_T  异常结果; IPC_ES_OK - 无异常
** 全局变量:
** 调用模块:
*********************************************************************************************************/
IPC_LOCAL IPC_ES_T  __ipcCloseHandle (PMX_IPC  pIpc)
{
    PIPC_EVTQ   pevtqThis  = &pIpc->IPC_evtqQueue;                      /*  IPC 事件队列                */
    BOOL        bReCheck   =  MX_TRUE;                                  /*  开关量, 标识对事件队列重检  */
    PMX_IPC     pipcNext   =  MX_NULL;
    PMX_IPC     pipcTag    =  __ipcTargetGet(pIpc);                     /*  目标 IPC                    */
    IPC_ES_T    uiResult   =  IPC_ES_OK;

    INTREG      iregIntLevel;

    K_TITLE(__ipcCloseHandle);
    spinLockAndDisIrq(&pIpc->IPC_plistPrio.IPL_spLock, &iregIntLevel);
    __ipcStateDel(pIpc, IPC_TS_SER);                                    /*  关闭 IPC 服务模式           */
    spinUnlockAndEnIrq(&pIpc->IPC_plistPrio.IPL_spLock, iregIntLevel);
    /*
     *  锁定待回复线程，如果已超时则按超时处理
     *  CLOSE 回复的对象, 一般是通过 IPC 向服务发出 CLOSE 命令,
     *  服务接受命令并处理完成 CLOSE 事务后向该对象反馈
     */
    if (__ipcReadyTestSetSf(pipcTag)) {
        __ipcTargetClean(pIpc);
        pipcTag = MX_NULL;
    }
    /*
     *  清空消息列表
     */
    while ((pipcNext = __ipcRecvNextPro(pIpc)) != MX_NULL) {            /*  从优先级列表中获取待处理IPC */
        if (!__ipcReadyTestSetSf(pipcNext)) {                           /*  未超时 IPC                  */
            __ipcRmStateChange(pipcNext, 0, IPC_ES_ERROR);              /*  设置异常状态                */
            vcpuWakeup(IPC_TO_VCPU(pipcNext));                          /*  唤醒线程                    */
        }
    }
    /*
     *  清空异步事件
     */
    do {                                                                /*  处理异步事件                */
        spinLockAndDisIrq(&pevtqThis->IEQ_spLock, &iregIntLevel);
        pipcNext = __ipcEvtqNext(pIpc);                                 /*  取出一个事件                */
        spinUnlockAndEnIrq(&pevtqThis->IEQ_spLock, iregIntLevel);
        /*
         *  事件队列已清空, 重检事件队列, 确保时钟提交最后一个超时事件;
         *  此处采用延迟方案, 而没有在 TICK 中利用 IPC_TS_SER 状态进行同步,
         *  是考虑简化逻辑，不在 TICK 中使用过多同步操作和访问 IPC 内部状态
         */
        if (!pipcNext) {
            if (bReCheck) {
                vcpuSleep(1);                                           /*  sleep 1 TICK                */
                bReCheck = MX_FALSE;
                continue;                                               /*  重检一次事件队列            */
            }
            break;
        }

        if (__ipcStateFind(pipcNext, IPC_EVTS_REQUEST) ==               /*  有请求事件则修改信号计数    */
                                     IPC_EVTS_REQUEST)    {
            pIpc->IPC_plistPrio.IPL_uiSignal--;                         /*  信号累减, 不需同步          */
        }
        __ipcRmStateChange(pipcNext, IPC_ES_MASK, IPC_ES_ERROR);        /*  设置异常状态                */
        vcpuWakeup(IPC_TO_VCPU(pipcNext));                              /*  唤醒线程                    */

    } while (1);

    if (pIpc->IPC_plistPrio.IPL_uiSignal > 0) {                         /*  IPC 异常处理                */
                                                                        /*  IPC 服务信号计数错误        */
        __ipcErrorSet(pIpc, MX_SC_RESULT_SER_SIGNAL);
        pIpc->IPC_plistPrio.IPL_uiSignal = 0;
        uiResult = IPC_ES_ERROR;
    }
    __ipcTargetClean(pIpc);                                             /*  清除目标 IPC                */
    __ipcStateDel(pIpc, IPC_S_IPC_MASK);                                /*  清除 IPC 状态               */

    if (pipcTag) {                                                      /*  IPC REPLY 处理              */
        vcpuWakeup(IPC_TO_VCPU(pipcNext));                              /*  唤醒线程                    */
    }
    return  (uiResult);
}
/*********************************************************************************************************
** 函数名称: __ipcScTargetCheckPro
** 功能描述: 检查并设置 IPC 目标
** 输　入  : pvcpuIpc  IPC 系统调用 VCPU
**           pvcpuTag  IPC 通信目标 VCPU
** 输　出  : NONE
** 全局变量:
** 调用模块: SC_ipcEntry
*********************************************************************************************************/
IPC_LOCAL BOOL  __ipcScTargetCheckPro (PMX_VCPU  pvcpuIpc,
                                       PMX_VCPU  pvcpuTag)
 {
    PMX_IPC     pIpc = IPC_VCPU_TO_IPC(pvcpuIpc);

    if ((pvcpuTag == MX_NULL) || (pvcpuTag == pvcpuIpc)) {              /*  未设置目标或向自己发送消息  */
        __ipcErrorSet(pIpc, MX_SC_RESULT_INVALID_OBJ);
        return  (MX_FALSE);
    }
    __ipcTargetSet(pIpc, IPC_VCPU_TO_IPC(pvcpuTag));                    /*  设置 IPC 目标               */
    return  (MX_TRUE);
}
/*********************************************************************************************************
** 函数名称: ipcDoSyscall
** 功能描述: IPC 系统调用入口函数
** 输　入  : pvcpu
** 输　出  : NONE
** 全局变量:
** 调用模块: SC_Entry, ipcKernSend
*********************************************************************************************************/
VOID  ipcDoSyscall (PMX_VCPU  pvcpu)
{
    PMX_IPC       pIpc      = IPC_VCPU_TO_IPC(pvcpu);
    PMX_SC_PARAM  pScParam  = MX_SC_PARAM_GET(pvcpu);
    UINT8         ucOpCode  = MX_SC_OPCODE_GET(pScParam);               /*  IPC 调用操作码              */
    MX_OBJECT_ID  ulTagId   = MX_SC_KOBJ_GET(pScParam);                 /*  IPC 目标 VCPU 内核对象 ID   */
    PMX_VCPU      pvcpuTag  = IPC_OBJ_TO_VCPU(ulTagId);                 /*  目标 VCPU                   */
    BOOL          bResult   = MX_TRUE;                                  /*  系统调用接口检查结果        */

    UINT32        uiState   = 0;                                        /*  IPC 初始状态                */
    IPC_HANDLE    ipcHandle;                                            /*  IPC 子程序                  */

    K_TITLE(ipcDoSyscall);

    switch (ucOpCode) {

    case IPC_OP_CLOSE:
        ipcHandle = __ipcCloseHandle;
        break;

    case IPC_OP_SEND:
        if (!(bResult = __ipcScTargetCheckPro(pvcpu, pvcpuTag))) {
            K_ERROR(" ipc tag1_vcpu=0x%x is error", pvcpuTag);
            break;
        }
        uiState = IPC_S_SEND | IPC_TS_READY;
        ipcHandle = __ipcSendHandle;
        break;

    case (IPC_OP_SEND | IPC_OP_RECV):
        if (!(bResult = __ipcScTargetCheckPro(pvcpu, pvcpuTag))) {
            K_ERROR(" ipc tag2_vcpu=0x%x is error", pvcpuTag);
            break;
        }
        uiState = IPC_S_SEND | IPC_S_RECV | IPC_TS_READY;
        ipcHandle = __ipcSendHandle;
        break;

    case (IPC_OP_SEND | IPC_OP_RECV | IPC_OP_OPEN | IPC_OP_REPLY):
        uiState = IPC_S_SEND | IPC_S_RECV | IPC_S_OPEN | IPC_S_REPLY | IPC_TS_READY;
        /*
         *  回复 IPC 时, 需同步检查 target, 因此不在调用入口检查
         */
        ipcHandle = __ipcSendHandle;
        break;

    case (IPC_OP_RECV | IPC_OP_OPEN):
        uiState = IPC_S_RECV | IPC_S_OPEN | IPC_TS_READY;
        ipcHandle = __ipcRecvHandle;
        break;

    default:                                                           /*  IPC 操作不合法               */
        bResult = MX_FALSE;
        K_ERROR(" ipc opcode=0x%x is error", ucOpCode);
        __ipcErrorSet(pIpc, MX_SC_RESULT_ILLEGAL);
    }

    if (bResult) {
        __ipcStateAdd(pIpc, uiState);                                  /*  设置 IPC 状态                */
        ipcHandle(pIpc);                                               /*  调用 IPC 子程序              */
        __ipcStateDel(pIpc, IPC_ES_MASK);                              /*  清除 IPC 异常状态            */
    }
}
/*********************************************************************************************************
** 函数名称: ipcKernSend
** 功能描述: 内核发送 IPC 消息接口; 发送之前在当前 IPC_MSG 中填充消息
** 输　入  : objTarget  IPC 接收对象
**           bSyn       发送模式: TRUE - 同步模式, 发送消息即返回
**                                FALSE- 异步模式, 发送后等待回复消息
** 输　出  : BOOL       TRUE-发送成功
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  ipcKernSend (MX_OBJECT_ID  objTarget, BOOL  bSyn)
{
    UINT8       ucOpCode = bSyn ? IPC_OP_SEND :                         /*  同步模式                    */
                              (IPC_OP_SEND | IPC_OP_RECV);              /*  异步模式                    */
    PMX_VCPU    pvcpu    = MX_VCPU_GET_CUR();


    PMX_SC_PARAM   pscparam  = MX_SC_PARAM_GET(pvcpu);

    K_TITLE(ipcKernSend);
    MX_SC_KOBJ_SET(pscparam, objTarget);                                /*  设置 IPC 目标               */
    MX_SC_OPCODE_SET(pscparam, ucOpCode);                               /*  设置 IPC 操作码             */
    MX_SC_TIME_SET(pscparam, SC_DTIME_DEF);                             /*  设置 IPC timeout 时间       */
    ipcDoSyscall(pvcpu);                                                /*  IPC 调用                    */

    if (MX_SC_RESULT_GET(pscparam) != MX_SC_RESULT_OK) {
        return  (MX_FALSE);
    }
    return  (MX_TRUE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
