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
** 文   件   名: _Wakeup.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 05 日
**
** 描        述: 这是系统等待唤醒链表操作函数 (调用以下函数时, 一定要在内核状态).

** BUG:
2018.02.08 _WakeupTick 处理 IPC 线程唤醒.
2018.02.26 增加 _WakeupIsIn.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** 函数名称: _WakeupAdd
** 功能描述: 将一个 wakeup 节点加入唤醒链表
** 输　入  : pwu           唤醒链表头
**           pwnod         节点
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _WakeupAdd (PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod)
{
    PMX_LIST_LINE     plineLonger;
    PMX_WAKEUP_NODE   pwnodLonger;
    
    plineLonger = pwu->WU_plineHeader;
    while (plineLonger) {                                               /*  找出比要插入节点延时更长的  */
        pwnodLonger = _LIST_ENTRY(plineLonger, MX_WAKEUP_NODE, WUN_lineManage);
        if (pwnod->WUN_ulCounter >= pwnodLonger->WUN_ulCounter) {       /*  需要继续向后找              */
            pwnod->WUN_ulCounter -= pwnodLonger->WUN_ulCounter;
            plineLonger = _list_line_get_next(plineLonger);
        
        } else {
            if (plineLonger == pwu->WU_plineHeader) {                   /*  如果是链表头                */
                _List_Line_Add_Ahead(&pwnod->WUN_lineManage, &pwu->WU_plineHeader);
            } else {
                _List_Line_Add_Left(&pwnod->WUN_lineManage, plineLonger);
            }
            pwnodLonger->WUN_ulCounter -= pwnod->WUN_ulCounter;         /*  右侧的点从新计算计数器      */
            break;
        }
    }
    
    if (plineLonger == MX_NULL) {                                       /*  没有延时更长的              */
        if (pwu->WU_plineHeader == MX_NULL) {                           /*  当前唤醒链表为空            */
            _List_Line_Add_Ahead(&pwnod->WUN_lineManage, &pwu->WU_plineHeader);
        } else {
            _List_Line_Add_Right(&pwnod->WUN_lineManage, &pwnodLonger->WUN_lineManage);
                                                                        /*  加入唤醒链表尾              */
        }
    }
    
    pwnod->WUN_bInQ = MX_TRUE;
}
/*********************************************************************************************************
** 函数名称: _WakeupDel
** 功能描述: 从 wakeup 管理器中删除指定节点
** 输　入  : pwu           唤醒链表头
**           pwnod         节点
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _WakeupDel (PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod)
{
    PMX_LIST_LINE     plineRight;
    PMX_WAKEUP_NODE   pwnodRight;

    plineRight = _list_line_get_next(&pwnod->WUN_lineManage);
    if (plineRight) {
        pwnodRight = _LIST_ENTRY(plineRight, MX_WAKEUP_NODE, WUN_lineManage);
        pwnodRight->WUN_ulCounter += pwnod->WUN_ulCounter;              /*  右侧的点从新计算计数器      */
    }
    
    _List_Line_Del(&pwnod->WUN_lineManage, &pwu->WU_plineHeader);
    pwnod->WUN_bInQ = MX_FALSE;
}
/*********************************************************************************************************
** 函数名称: __WakeupIsIn
** 功能描述: 判断线程节点是否在唤醒表中
** 输　入  : pwnod         节点
** 输　出  : BOOL          TRUE - 节点在表中
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  _WakeupIsIn (PMX_WAKEUP_NODE  pwnod)
{
    if (_K_wuDelay.WU_plineHeader == (&pwnod->WUN_lineManage)) {        /*  表头指向此节点              */
        return  (MX_TRUE);
    }

    return  (!_LIST_LINE_IS_NOTLNK(&pwnod->WUN_lineManage));
}
/*********************************************************************************************************
** 函数名称: _WakeupTime
** 功能描述: 获得指定节点的等待时间(Tick 数)
** 输　入  : pwu           唤醒链表头
**           pwnod         节点
**           pulLeft       剩余时间
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _WakeupTime (PMX_WAKEUP  pwu, PMX_WAKEUP_NODE  pwnod, ULONG  *pulLeft)
{
    PMX_LIST_LINE     plineTemp;
    PMX_WAKEUP_NODE   pwnodTemp;
    ULONG             ulCounter = 0;
    
    for (plineTemp  = pwu->WU_plineHeader;
         plineTemp != MX_NULL;
         plineTemp  = _list_line_get_next(plineTemp)) {
        
        pwnodTemp   = _LIST_ENTRY(plineTemp, MX_WAKEUP_NODE, WUN_lineManage);
        ulCounter  += pwnodTemp->WUN_ulCounter;
        if (pwnodTemp == pwnod) {
            break;
        }
    }
    
    if (plineTemp) {
        *pulLeft = ulCounter;
    } else {
        *pulLeft = 0ul;                                                 /*  没有找到节点                */
    }
}
/*********************************************************************************************************
** 函数名称: _WakeupTick
** 功能描述: 时间片处理 (tick 中断服务程序中被调用, 进入内核且关闭中断状态)
** 输　入  : ulCounter     处理的时间长度
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _WakeupTick (ULONG  ulCounter)
{
    PMX_VCPU          pvcpu;
    PMX_LIST_LINE     plineOperate;
    PMX_WAKEUP_NODE   pwnodOperate;
    BOOL              bIpcReady;                                        /*  IPC 线程就绪状态            */

    //bspDebugMsg("_K_wuDelay.WU_plineHeader = %p\n", _K_wuDelay.WU_plineHeader);

    plineOperate = _K_wuDelay.WU_plineHeader;
    while (plineOperate) {
        //bspDebugMsg("_WakeupTick() Never be here\n");
        pwnodOperate = _LIST_ENTRY(plineOperate, MX_WAKEUP_NODE, WUN_lineManage);
        plineOperate = _list_line_get_next(plineOperate);

        if (pwnodOperate->WUN_ulCounter  > ulCounter) {                 /*  本节点未超时, 退出          */
            pwnodOperate->WUN_ulCounter -= ulCounter;
            break;
        }

        ulCounter                  -= pwnodOperate->WUN_ulCounter;
        pwnodOperate->WUN_ulCounter = 0;
        pvcpu                       = _LIST_ENTRY(pwnodOperate, MX_VCPU, VCPU_wunDelay);
        /*
         *  超时线程需要被唤醒
         */
        if (IPC_GET_IN_PROCESS(&pvcpu->VCPU_ipc)) {
            /*
             *  IPC 唤醒模式
             */
            if (ipcReadyTryTestSetSafe(&pvcpu->VCPU_ipc, &bIpcReady)) { /*  测试 IPC 线程就绪状态并置位 */

                //K_NOTE("_WakeupTick: wk_ipc_del_vcpu=0x%x", pvcpu);
                _WakeupDel(&_K_wuDelay, &pvcpu->VCPU_wunDelay);         /*  从唤醒链表中删除            */

                if (!bIpcReady) {                                       /*  线程当前未被 IPC 唤醒       */
                    ipcTickTimeoutPro(&pvcpu->VCPU_ipc);                /*  加入 IPC 事件表,异步唤醒线程*/
                }
            }                                                           /*  else (TryTest 测试阻塞):    */
                                                                        /*  跳过当前节点，下个时钟再处理*/
        } else {
            //bspDebugMsg("Wakeup : %p ", pvcpu);
            /*
             *  时钟唤醒模式
             */
            pvcpu->VCPU_uiStatus &= ~MX_VCPU_STATUS_DELAY;
            _WakeupDel(&_K_wuDelay, &pvcpu->VCPU_wunDelay);             /*  超时线程从唤醒表中移除      */

            if (pvcpu->VCPU_uiStatus == MX_VCPU_STATUS_READY) {
                if (!_CandTableTryAdd(pvcpu)) {                         /*  尝试加入候选表              */
                    _ReadyTableAdd(pvcpu);                              /*  加入就绪表                  */
                }
            }
        } 
    }
    //bspDebugMsg("_WakeupTick()\n");
} 
/*********************************************************************************************************
  END
*********************************************************************************************************/

