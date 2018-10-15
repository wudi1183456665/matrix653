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
** 文   件   名: k_unit_boot.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 3 月 1 日
**
** 描        述: 内核单元测试驱动代码
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
  定义与声明
*********************************************************************************************************/
MX_OBJECT_ID    G_objRoot;

INT  kernelExitAndEnIrq2(INTREG  iregInterLevel);
/*********************************************************************************************************
** 函数名称: T_funcKernTestThread
** 功能描述: 内核单元测试驱动线程; 内核态
** 输　入  : pvArg
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  T_funcKernTestThread(PVOID pvArg)
{
    PT_VCPU      pvcpuUnit = (PT_VCPU)pvArg;
    MX_OBJECT_ID objRoot   = G_objRoot;
    PMX_IPC_MSG  pIpcMsg   = pvcpuUnit->pIpcMsg;
    // PMX_IPC_MSG  pIpcMsg   = T_GET_KERN_MSG();

    K_TITLE(T_funcKernTestThread);

    /*
     * TODO: 内核单元测试
     */

    /*
     *  启动用户单元测试
     *  向 root 服务发送启动测试 IPC 请求
     */
#if 1
    K_TITLE("T_funcKernTestThread: BOOT unit_test");

    ipcMsgWriteOpen(pIpcMsg);
    ipcMsgUlWrite(pIpcMsg, T_ROOT_SER_TEST_BOOT);
    ipcMsgFlush(pIpcMsg);
    /*
     *  TEST:
     *  1) ipcKernSend 内核 IPC 同步请求、异步请求
     *  2) 服务未启动时向其发出请求
     */
    K_SLEEP(10);
    K_DEBUG(" IPC call boot unit_test");
    while(!ipcKernSend(objRoot, MX_TRUE)) {
        K_NOTE(" ipcKernSend fail, try again");
        K_SLEEP(1);
    }
#endif

#if 1   /*  DEBUG: 并发测试 IPC        */
{
    MX_OBJECT_ID objLog;
    K_SLEEP(10);

    do {
        K_DEBUG(" FIND log_service id**********");
        objLog = TKI_rootSerFind(objRoot, TSER_CODE_LOG);
        K_DEBUG(" log_service_id=0x%x******************", objLog);
        K_SLEEP(200);
    } while(1);
}
#endif

    do {
        K_DEBUG("(^_^)KERN UNIT TEST LOOP***********(^_^)");
        //K_SLEEP(20);
        vcpuSleep(10);

    } while(1);
}
/*********************************************************************************************************
  client & server
*********************************************************************************************************/
UINT32          _G_uiCount = 0;
BOOL            _G_bPend = MX_FALSE;

PMX_VCPU        _G_pvcpuServer;
PMX_VCPU        _G_pvcpuClient;

VOID  T_funcClientTestThread (PVOID  pvArg)
{
    /*
     *  client 模拟测试
     */
    INTREG      iregIntLevel;
    PMX_VCPU    pvcpuServer  = _G_pvcpuServer;

    K_TITLE(client);
    IPC_ASSERT(_G_pvcpuServer != MX_NULL);

    K_DEBUG("CLIENT: test SP=0x%x", T_execGetCurSp());
    K_SLEEP(1000);

    while(1) {
        K_DEBUG("CLIENT: LOOP");
        iregIntLevel = kernelEnterAndDisIrq();
        K_DEBUG("CLIENT: in SPINLOCK");

        _G_uiCount++;

        if (_G_bPend == MX_TRUE) {
            _G_bPend = MX_FALSE;
            K_NOTE("CLIENT: WAKEUP server+++++++++++++");
            vcpuWakeup(pvcpuServer);
        }

        vcpuSuspend();
        K_WARNING("CLIENT: PEND++++++++++++++");
        kernelExitAndEnIrq(iregIntLevel);
        K_WARNING("CLIENT: WAKEUP+++++++++++++");

        vcpuSleep(50);
    }
}

VOID  T_funcServerTestThread (PVOID  pvArg)
{
    /*
     *  server 模拟测试
     */
    PMX_VCPU    pvcpuClient  = _G_pvcpuClient;
    INTREG      iregIntLevel;

    K_TITLE(T_funcServerTestThread);
    IPC_ASSERT(_G_pvcpuClient != MX_NULL);

    while(1) {
        K_DEBUG("SERVER: LOOP");

        iregIntLevel = kernelEnterAndDisIrq();
        K_DEBUG("SERVER: in SPINLOCK");

        if(_G_uiCount == 0) {
            _G_bPend = MX_TRUE;
            K_DEBUG("SERVER: before PEND operate");
            vcpuSuspend();
            K_DEBUG("SERVER: after PEND operate");

            K_WARNING("SERVER: PEND ------------");
            kernelExitAndEnIrq(iregIntLevel);
            K_WARNING("SERVER: WAKEUP------------");

            continue;
        }

        kernelExitAndEnIrq(iregIntLevel);

        _G_uiCount--;

        K_NOTE("SERVER: WAKEUP client------------");
        vcpuWakeup(pvcpuClient);
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
