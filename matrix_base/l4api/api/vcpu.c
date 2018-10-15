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
** 文   件   名: vcpu.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 03 月 02 日
**
** 描        述: VCPU 相关 API 接口.
*********************************************************************************************************/
#include <Matrix.h>
#include "abi.h"
/*********************************************************************************************************
** 函数名称: MXI_vcpuCreate
** 功能描述: 创建 VCPU
** 输　入  : pVcpu   系统调用当前 VCPU
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_SC_RESULT_T MXI_vcpuCreate (PMX_IPC_MSG         pipcMsg,
                            MX_OBJECT_ID        objPt,
                            MX_OBJECT_ID       *pobjVcpu,
                            PVCPU_START_ROUTINE pRount,
                            PVOID               pvArg,
                            UINT32              uiPriority,
                            addr_t              addrStack,
                            addr_t              addrMsgTag)
{
    /*
     * syscall 参数说明：
     *     [IN]
     *          ARG1    - UINT32    - 进程 OBJ
     *          ARG2    - ULONG     - 线程入口地址
     *          ARG3    - ULONG     - 线程入口参数
     *          IPC_MSG - UINT32    - 线程优先级
     *                  - addr_t    - 用户 IPC_MSG 地址
     *                  - addr_t    _ 用户堆栈地址(栈顶)
     *     [OUT]
     *          ARG1    - UINT32    - 新创建线程 OBJ
     */
    ULONG   ulArg0      = 0;
    ULONG   ulOobjVcpu  = 0;

    //T_DEBUG(": prio=%d, pMsg=0x%x", uiPriority, addrMsgTag);

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_CREATE);
#if 0
    ipcMsgWriteOpen(pipcMsg);
    ipcMsgUlWrite(pipcMsg, uiPriority);
    ipcMsgUlWrite(pipcMsg, (ULONG)addrMsgTag);
    ipcMsgUlWrite(pipcMsg, (ULONG)addrStack);
    ipcMsgFlush(pipcMsg);
#endif

    MX_Syscall42(ulArg0, (ULONG)objPt, (ULONG)pRount, (ULONG)pvArg,
                &ulArg0, &ulOobjVcpu);

    *pobjVcpu = (MX_OBJECT_ID)ulOobjVcpu;
    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** 函数名称: MXI_vcpuStart
** 功能描述: 创建 VCPU
** 输　入  : pVcpu   系统调用当前 VCPU
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_SC_RESULT_T MXI_vcpuStart (MX_OBJECT_ID  objVcpu)
{
    /*
     * syscall 参数说明：
     *     [IN]
     *         ARG1    - UINT32    - 线程 OBJ
     */
    ULONG   ulArg0      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_START);

    MX_Syscall41(ulArg0, (ULONG)objVcpu, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** 函数名称: MXI_vcpuSleep
** 功能描述: 创建 VCPU
** 输　入  : pVcpu   系统调用当前 VCPU
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_vcpuSleep (SC_TIME  scTime)
{
    /*
     * syscall 参数说明：
     *     [IN]
     *          ARG2,ARG3     - TIMEOUT    - 休眠时间
     */
    ULONG   ulArg0      = 0;
    ULONG   ulArg2      = 0;
    ULONG   ulArg3      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_SLEEP);
    ABI_SCTIME_SET(ulArg2, ulArg3, scTime);

    MX_Syscall41(ulArg0, 0, ulArg2, ulArg3, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** 函数名称: MXI_vcpuWakeup
** 功能描述: 唤醒线程
** 输　入  : objVcpu
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_vcpuWakeup (MX_OBJECT_ID  objVcpu)
{
    /*
     * syscall 参数说明：
     *     [IN]
     *          ARG1    - MX_OBJECT_ID   - 线程ID
     */
    ULONG   ulArg0      = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_WAKEUP);

    MX_Syscall41(ulArg0, (ULONG)objVcpu, 0, 0, &ulArg0);

    return  (ABI_RESULT_GET(&ulArg0));
}
/*********************************************************************************************************
** 函数名称: MXI_vcpuDisableInt
** 功能描述: 禁能当前 VCPU 所有中断
** 输　入  : NONE
** 输　出  : 当前中断状态
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
INTREG  MXI_vcpuDisableInt (VOID)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = 0;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_DIS_INT);

    MX_Syscall42(ulArg0, 0, 0, 0, &ulArg0, &ulArg1);

    return  (ulArg1);
}
/*********************************************************************************************************
** 函数名称: MXI_vcpuEnableInt
** 功能描述: 使能(恢复)当前 VCPU 所有中断
** 输　入  : iregInterLevel  禁能时保存的状态
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  MXI_vcpuEnableInt (INTREG  iregInterLevel)
{
    ULONG    ulArg0 = 0;
    ULONG    ulArg1 = (ULONG)iregInterLevel;

    ABI_CALLID_SET(&ulArg0, MX_SC_CALLID_VCPU);
    ABI_OPCODE_SET(&ulArg0, MX_SC_OPCODE_VCPU_EN_INT);

    MX_Syscall41(ulArg0, ulArg1, 0, 0, &ulArg0);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/


