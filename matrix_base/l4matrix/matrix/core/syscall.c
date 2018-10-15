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
** 文   件   名: syscall.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 01 月 05 日
**
** 描        述: 系统调用.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** 函数名称: scTimeToTicks
** 功能描述: 系统调用时间转换为内核时间
** 输　入  : ui64Time      系统调用时间
**           pulTicks      转换 ticks
** 输　出  : BOOL          TRUE - 成功; FALSE - 格式错误
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  scTimeToTicks (SC_TIME  ui64Time, ULONG  *pulTicks)
{
    static  const UINT32  uiUtimes[] = {
        SC_TICK_TIME,
        SC_UTIME_DEF * 1,
        SC_UTIME_DEF * 10,
        SC_UTIME_DEF * 100,
        SC_UTIME_DEF * 1000,
        SC_UTIME_DEF * 10000,
        SC_UTIME_DEF * 100000,
        SC_UTIME_DEF * 1000000,
    };

    UINT8   ucUnit  = SC_TIME_UNIT_GET(ui64Time);
    ULONG   ulTime  = SC_TIME_VALUE_GET(ui64Time);

    if (ucUnit >= SC_UTIME_FOREVER) {
        return  (MX_FALSE);
    }

    *pulTicks = (ulTime * uiUtimes[ucUnit] / SC_TICK_TIME);

    return  (MX_TRUE);
}
/*********************************************************************************************************
** 函数名称: __scUAddrCheck
** 功能描述: 用户空间地址检查
** 输　入  : addrUser    用户空间地址
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static BOOL  __scUAddrCheck (addr_t  addrUser)
{
    /*
     *  TODO: 在有效用户地址空间内
     */
    return  (MX_TRUE);
}

/*********************************************************************************************************
** 函数名称: __scVcpuCreate
** 功能描述: 创建 VCPU
** 输　入  : pvcpuCur   系统调用当前 VCPU
** 输　出  : NONE
** 全局变量:
** 调用模块: SC_Entry
*********************************************************************************************************/
static VOID  __scVcpuCreate (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL 参数规则:
     *  [IN]  ARG0    - UINT32    - 操作码 | 调用号
     *        ARG1    - UINT32    - 进程 OBJ
     *        ARG2    - UINT32    - 线程入口地址
     *        ARG3    - UINT32    - 线程入口参数
     *        IPC_MSG - UINT32    - 线程优先级
     *                - addr_t    - 用户 IPC_MSG 地址
     *                - addr_t    - 用户栈(低地址)
     *  [OUT] ARG0    - UINT32    - 处理状态
     *        ARG1    - UINT32    - 新创建线程 OBJ
     */
    PMX_SC_PARAM        pscparam = MX_SC_PARAM_GET(pvcpuCur);           /*  系统调用参数结构            */
    PMX_IPC_MSG         pCallMsg = MX_IPC_MSG_GET(pvcpuCur);
    ULONG               ulMsgItem;

    MX_VCPU_ATTR        vcpuattr;                                       /*  新建线程属性                */
    PMX_VCPU            pNewVcpu   = MX_NULL;

    //MX_OBJECT_ID      objPt      = MX_SC_KOBJ_GET(pscparam);          /*  新建线程待绑定的进程对象    */
    PVCPU_START_ROUTINE pfuncStart = (PVCPU_START_ROUTINE)
                                     pscparam->SCPARAM_ui32Arg2;        /*  新建线程入口地址            */
    UINT32              uiPrio;                                         /*  新建线程优先级              */
    PMX_IPC_MSG         pIpcMsg    = MX_NULL;                           /*  用户层 IPC 消息体地址       */

    K_TITLE(__scVcpuCreate);

#if 0                                                                   /*  TODO: kern 还不支持进程     */
    if (!kobjCheck(objPt, _KOBJ_PARTITION)) {                           /*  检查进程对象合法性          */
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }
#endif

    if (!__scUAddrCheck((addr_t)pfuncStart)) {                          /*  检查入口地址合法性          */
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }

    ipcMsgReadOpen(pCallMsg);
    ipcMsgUlRead(pCallMsg, &ulMsgItem);
    uiPrio = (UINT32)ulMsgItem;
    if (uiPrio > MX_PRIO_LOWEST) {                                      /*  检查优先级合法性            */
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }
    vcpuattr.VCPUATTR_uiPriority = uiPrio;                              /*  设置线程优先级              */

    ipcMsgUlRead(pCallMsg, &ulMsgItem);
    pIpcMsg = (PVOID)ulMsgItem;
    if (!__scUAddrCheck((addr_t)pIpcMsg)) {                             /*  检查 MSG 地址合法性         */
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }

    ipcMsgUlRead(pCallMsg, &ulMsgItem);
    IPC_ASSERT(ulMsgItem != 0);                                         /*  检查用户栈地址合法性        */
    vcpuattr.VCPUATTR_pstkLowAddr = (PMX_STACK)ulMsgItem;

    vcpuattr.VCPUATTR_bIsUser = MX_TRUE;
    vcpuattr.VCPUATTR_pvArg   = (PVOID)pscparam->SCPARAM_ui32Arg3;      /*  设置线程参数                */

    pNewVcpu = vcpuCreate(pfuncStart, &vcpuattr);
    vcpuBindPartition(pNewVcpu, _K_pprtnKernel);                        /*  绑定进程                    */

    vcpuBindIpcMsg(pNewVcpu, pIpcMsg);                                  /*  绑定 MSG, 建立内存映射      */

    MX_SC_KOBJ_SET(pscparam, pNewVcpu->VCPU_ulVcpuId);                  /*  返回线程对象                */
    MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);

#if 0
    K_DEBUG(" vcpu pt:0x%x, arg:0x%x, prio:%d, msg:0x%x",
            objPt, vcpuattr.VCPUATTR_pvArg, vcpuattr.VCPUATTR_uiPriority, ulMsgItem);
#endif
    K_WARNING(" new_vcpu=0x%x", pNewVcpu);
}
/*********************************************************************************************************
** 函数名称: __scVcpuStart
** 功能描述: VCPU 启动
** 输　入  : pvcpuCur   系统调用当前 VCPU
** 输　出  : NONE
** 全局变量:
** 调用模块: SC_Entry
*********************************************************************************************************/
static VOID  __scVcpuStart (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL 参数规则:
     *  [IN]  ARG0    - UINT32    - 操作码 | 调用号
     *        ARG1    - UINT32    - 线程 OBJ
     *  [OUT] ARG0    - UINT32    - 处理状态
     */
    PMX_SC_PARAM     pscparam  = MX_SC_PARAM_GET(pvcpuCur);             /*  系统调用参数结构            */

    K_TITLE(__scVcpuStart);

    if (vcpuStart(kobjGet(MX_SC_KOBJ_GET(pscparam))) != ERROR_NONE) {
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_FAIL);
        K_ERROR("ERROR");
        return;
    }

    MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                        /*  设置系统调用返回参数        */
}
/*********************************************************************************************************
** 函数名称: __scVcpuSleep
** 功能描述: VCPU sleep
** 输　入  : pvcpuCur   系统调用当前 VCPU
** 输　出  : NONE
** 全局变量:
** 调用模块: SC_Entry
*********************************************************************************************************/
static VOID  __scVcpuSleep (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL 参数规则:
     *  [IN]  ARG0    - UINT32    - 操作码 | 调用号
     *        ARG2    - UINT32    - 休眠时间低 32 位
     *        ARG3    - UINT32    - 休眠时间高 32 位
     *  [OUT] ARG0    - UINT32    - 处理状态
     */
    PMX_SC_PARAM     pscparam = MX_SC_PARAM_GET(pvcpuCur);              /*  系统调用参数结构            */
    UINT64           ui64Time = MX_SC_TIME_GET(pscparam);
    ULONG            ulTicks;
    BOOL             bToTick  = scTimeToTicks(ui64Time, &ulTicks); 

    if (SC_TIME_UNIT_GET(ui64Time) == SC_UTIME_FOREVER) {
        //K_DEBUG(" PEND");
        vcpuSuspend();

    } else if (bToTick) {
        //K_DEBUG(" ticks=%d", ulTicks);
        vcpuSleep(ulTicks);

    } else {
        //K_DEBUG(" Error Param");
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }

    MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                        /*  设置系统调用返回参数        */
}
/*********************************************************************************************************
** 函数名称: __scVcpuWakeup
** 功能描述: 唤醒 VCPU
** 输　入  : pvcpuCur   系统调用当前 VCPU
** 输　出  : NONE
** 全局变量:
** 调用模块: SC_Entry
*********************************************************************************************************/
static VOID  __scVcpuWakeup (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL 参数规则:
     *  [IN]  ARG0    - UINT32    - 操作码 | 调用号
     *        ARG1    - UINT32    - 线程 OBJ
     *  [OUT] ARG0    - UINT32    - 处理状态
     */
    PMX_SC_PARAM    pscparam = MX_SC_PARAM_GET(pvcpuCur);               /*  系统调用参数结构            */
    MX_OBJECT_ID    objVcpu  = MX_SC_KOBJ_GET(pscparam);

    K_TITLE(__scVcpuWakeup);

    if (!kobjCheck(objVcpu, _KOBJ_VCPU)) {
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;

    } else {
        vcpuWakeup((PMX_VCPU)kobjGet(objVcpu));
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  设置系统调用返回参数        */
    }
}
/*********************************************************************************************************
** 函数名称: SC_PrtnHandle
** 功能描述: Partition 模块系统调用处理
** 输　入  : pvcpuCur   系统调用当前 VCPU
** 输　出  : NONE
** 全局变量:
** 调用模块: SC_Entry
*********************************************************************************************************/
static VOID  SC_PrtnHandle (PMX_VCPU  pvcpuCur)
{
    /*
     *  检查进程创建权限：当前线程是否是进程管理服务
     *  进程合法性检查：从资源配置中检索进程
     *  分配进程控制块,  分配地程ID
     *  建立页表
     *  分配进程内存空间：进程空间隔离，确定有效内存
     *  返回进程号与有效物理内存（虚拟内存可由进程管理服务确定）
     */
    PMX_SC_PARAM     pscparam = MX_SC_PARAM_GET(pvcpuCur);              /*  系统调用参数结构            */
    UINT8            ucOpCode;

    ucOpCode = MX_SC_OPCODE_GET(pscparam);

    switch (ucOpCode) {

    case MX_SC_OPCODE_PRTN_RESTART:
        prtnRestart(MX_NULL);
        break;
        
    default:
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_ILLEGAL);               /*  不合法的系统调用            */
        break;
    }
}
/*********************************************************************************************************
** 函数名称: SC_IrqHandle
** 功能描述: Irq 模块系统调用处理
** 输　入  : pvcpuCur     调用者线程控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  SC_IrqHandle (PMX_VCPU  pvcpuCur)
{
    PMX_SC_PARAM     pscparam = MX_SC_PARAM_GET(pvcpuCur);              /*  系统调用参数结构            */
    UINT8            ucOpCode;
    INTREG           iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();                            /*  进入内核同时关闭中断        */

    ucOpCode = MX_SC_OPCODE_GET(pscparam);

    switch (ucOpCode) {

    case MX_SC_OPCODE_IRQ_REGISTER:
        irqRegister((IRQ_FUNCPTR)pscparam->SCPARAM_ui32Arg1);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  设置系统调用返回参数        */
        break;

    case MX_SC_OPCODE_IRQ_ATTACH:
        irqAttach((ULONG)pscparam->SCPARAM_ui32Arg1);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  设置系统调用返回参数        */
        break;

    case MX_SC_OPCODE_IRQ_ENABLE:
        irqEnable(pscparam->SCPARAM_ui32Arg1);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  设置系统调用返回参数        */
        break;

    case MX_SC_OPCODE_IRQ_DISABLE:
        irqDisable(pscparam->SCPARAM_ui32Arg1);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  设置系统调用返回参数        */
        break;

    case MX_SC_OPCODE_SWI_REGISTER:
        swiRegister(pscparam->SCPARAM_ui32Arg1, (IRQ_FUNCPTR)pscparam->SCPARAM_ui32Arg2);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  设置系统调用返回参数        */
        break;

    default:
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_ILLEGAL);               /*  不合法的系统调用            */
        break;
    }

    kernelExitAndEnIrq(iregInterLevel);                                 /*  退出内核同时打开中断        */
}
/*********************************************************************************************************
** 函数名称: SC_IrqHandle
** 功能描述: Vcpu 模块系统调用处理
** 输　入  : pvcpuCur     调用者线程控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  SC_VcpuHandle (PMX_VCPU  pvcpuCur)
{
    PMX_SC_PARAM     pscparam = MX_SC_PARAM_GET(pvcpuCur);              /*  系统调用参数结构            */
    UINT8            ucOpCode;

    ucOpCode = MX_SC_OPCODE_GET(pscparam);

    switch (ucOpCode) {

    case MX_SC_OPCODE_VCPU_CREATE:
        __scVcpuCreate(pvcpuCur);
        break;

    case MX_SC_OPCODE_VCPU_START:
        __scVcpuStart(pvcpuCur);
        break;

    case MX_SC_OPCODE_VCPU_SLEEP:
        __scVcpuSleep(pvcpuCur);
        break;

    case MX_SC_OPCODE_VCPU_WAKEUP:
        __scVcpuWakeup(pvcpuCur);
        break;

    case MX_SC_OPCODE_VCPU_DIS_INT:
        pscparam->SCPARAM_ui32Arg1 = vcpuDisableInt();
        break;

    case MX_SC_OPCODE_VCPU_EN_INT:
        vcpuEnableInt(pscparam->SCPARAM_ui32Arg1);
        break;

    default:
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_ILLEGAL);               /*  不合法的系统调用            */
        break;
    }
}
/*********************************************************************************************************
** 函数名称: SC_DbgHandle
** 功能描述: 调试接口
** 输　入  : pvcpuCur   系统调用当前 VCPU
** 输　出  : NONE
** 全局变量:
** 调用模块: SC_Entry
*********************************************************************************************************/
static VOID  SC_DbgHandle (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL 参数规则:
     *  [IN]  ARG0    - UINT32    - 操作码 | 调用号
     *        IPC_MSG - ......    - IPC_DEBUG_BUF_OFFSET 偏移
     *                - IPC_STRING- 输出字符串结构
     *  [OUT] ARG0    - UINT32    - 处理状态
     */
    PMX_SC_PARAM    pscparam = MX_SC_PARAM_GET(pvcpuCur);
    PMX_IPC_MSG     pCallMsg = MX_IPC_MSG_GET(pvcpuCur);
    UINT8           ucOpCode = MX_SC_OPCODE_GET(pscparam);
    PIPC_STRING     pStr     = MX_NULL;
    static INT      iDbgVal  = 0;

    switch (ucOpCode) {

    case MX_SC_OPCODE_DEBUG_PRINT:
        pStr = (PVOID)(pCallMsg->IMSG_chBuff + IPC_DEBUG_BUF_OFFSET);
        dump(pStr->ISTR_ucBuf);
        break;

    case MX_SC_OPCODE_DEBUG_READ:
        pscparam->SCPARAM_ui32Arg1 = iDbgVal;
        break;

    case MX_SC_OPCODE_DEBUG_WRITE:
        iDbgVal = pscparam->SCPARAM_ui32Arg1;
        break;

    default:
        K_WARNING("unknow debug call");
        //T_funcServer2Test();
        break;
    }

    MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                        /*  设置系统调用返回参数        */
}
/*********************************************************************************************************
** 函数名称: scEntry
** 功能描述: 系统调用程序入口
** 输　入  : puctx    系统调用上下文
** 输　出  : NONE
** 全局变量:
** 调用模块: archSwiHandle()
*********************************************************************************************************/
VOID  scEntry (PARCH_USER_CTX  puctx)
{
    PMX_VCPU      pvcpuCur = MX_VCPU_GET_CUR();
    PMX_SC_PARAM  pscparam = MX_SC_PARAM_GET(pvcpuCur);
    UINT8         ucCallId;
    //UINT8         ucOpCode;

    ARCH_UCTX_COPYTO_SCPARAM(puctx, MX_SC_PARAM_GET(pvcpuCur));

    ucCallId = MX_SC_CALLID_GET(pscparam);
    //ucOpCode = MX_SC_OPCODE_GET(pscparam);

#if 0
    K_DEBUG(" start,vcpuid=%d, callid=%d, opCode=%d, arg1=%d, arg2=%d, arg3=%d",
            pvcpuCur->VCPU_ulVcpuId,
            ucCallId,
            ucOpCode,
            pscparam->SCPARAM_ui32Arg1, pscparam->SCPARAM_ui32Arg2, pscparam->SCPARAM_ui32Arg3
            );
#endif

    switch (ucCallId) {

    case MX_SC_CALLID_IPC:
        ipcDoSyscall(pvcpuCur);
        break;

    case MX_SC_CALLID_PRTN:
        SC_PrtnHandle(pvcpuCur);
        break;

    case MX_SC_CALLID_VCPU:
        SC_VcpuHandle(pvcpuCur);
        break;

    case MX_SC_CALLID_IRQ:
        SC_IrqHandle(pvcpuCur);
        break;

    case MX_SC_CALLID_DEBUG:
        SC_DbgHandle(pvcpuCur);
        break;

    default:
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_ILLEGAL);               /*  不合法的系统调用            */
        break;
    }

    ARCH_SCPARAM_COPYTO_UCTX(MX_SC_PARAM_GET(pvcpuCur), puctx);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
