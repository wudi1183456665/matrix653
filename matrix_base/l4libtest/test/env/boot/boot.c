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
** 文   件   名: boot.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 2 日
**
** 描        述: 测试启动代码
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
  声明
*********************************************************************************************************/
extern PMX_PRTN     _K_pprtnKernel;

void  vcpuBindUserMsg(PMX_VCPU pVcpu, PMX_IPC_MSG pUserMsg);

VOID  T_funcRootThread(PVOID pvArg);
VOID  T_funcUserTestThread(PVOID pvArg);
VOID  T_funcKernTestThread(PVOID pvArg);

extern PMX_VCPU         _G_pvcpuServer;
extern PMX_VCPU         _G_pvcpuClient;
extern MX_OBJECT_ID     G_objRoot;

VOID  T_funcServerTestThread(PVOID pvArg);
VOID  T_funcClientTestThread (PVOID  pvArg);


PMX_VCPU  _T_createThread2(PVCPU_START_ROUTINE  func, UINT32  uiPrio, PMX_PRTN  pPrtn, BOOL bIsUser);
VOID  _funcUserThread (PVOID pvArg);
/*********************************************************************************************************
** 函数名称: T_createThread
** 功能描述: 内核态创建线程
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
PMX_VCPU  _T_createThread(PVCPU_START_ROUTINE  func, UINT32  uiPrio, PVOID  pvArg, BOOL isUser)
{
    MX_OBJECT_ID objVcpu;
    MX_VCPU_ATTR vcpuattr;
    PMX_VCPU     pVcpu     = MX_NULL;

    PT_VCPU      pThread   = T_memObjAlloc(TMEM_VCPU);

    K_TITLE(T_createKernThread);


    pThread->objPt      = 0;                                            /*  TODO: 进程对象              */
    pThread->pfuncStart = (PVCPU_START_ROUTINE) T_funcRootThread;
    pThread->uiPriority = uiPrio;
    pThread->addrStack  = 0;
    pThread->pIpcMsg    = T_memObjAlloc(TMEM_IPC_MSG);

    vcpuattr.VCPUATTR_uiPriority = uiPrio;
    vcpuattr.VCPUATTR_pvArg      = pvArg;
    vcpuattr.VCPUATTR_bIsUser    = isUser;

    if (isUser) {
        pThread->addrStack  = (addr_t)T_memObjAlloc(TMEM_USE_STK);      /*  分配用户堆栈                */
        T_vcpuStkInit((PT_VCPU_STK)pThread->addrStack, pThread->pIpcMsg);
        vcpuattr.VCPUATTR_pstkLowAddr =
                (PMX_STACK)((PT_VCPU_STK)pThread->addrStack)->addrStkTop;
    }

    if (!pvArg) {
            vcpuattr.VCPUATTR_pvArg = (PVOID)pThread;
        }

    pVcpu = vcpuCreate((PVCPU_START_ROUTINE)func, &vcpuattr);           /*  创建内核线程                */
    objVcpu = pVcpu->VCPU_ulVcpuId;

    pThread->pvArg    = (PVOID)objVcpu;

    vcpuBindUserMsg(pVcpu, pThread->pIpcMsg);                           /*  绑定 MSG                    */

    vcpuBindPartition(pVcpu, _K_pprtnKernel);                           /*  绑定进程                    */

    return  (pVcpu);
}
/*********************************************************************************************************
** 函数名称: T_envBoot; 内核态
** 功能描述: 启动测试
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
void T_envBoot()
{
    K_TITLE(T_envBoot);
    /*
     *  创建 root_server -> 创建 log_server
     *  启动内核单元测试 -- IPC --> root_server -> 启动用户单元测试
     */
#if 1
    K_TITLE(" BOOT root_server");
{
    PMX_VCPU  pvcpuRoot = _T_createThread((PVCPU_START_ROUTINE)T_funcRootThread,
                                          TVCPU_PRIO_HIGH, MX_NULL, MX_TRUE);
    K_WARNING(" root_vcpu=0x%x", pvcpuRoot);

    G_objRoot = pvcpuRoot->VCPU_ulVcpuId;
    vcpuStart(pvcpuRoot);
}
#endif

    /*
     *  启动内核单元测试
     */
#if 1
_G_pvcpuClient = _T_createThread((PVCPU_START_ROUTINE)T_funcKernTestThread,
                               TVCPU_PRIO_NORMAL, MX_NULL, MX_FALSE);
vcpuStart(_G_pvcpuClient);
#endif

#if 0
    /*
     *  启动模拟服务测试线程
     */
    _G_pvcpuServer = _T_createThread((PVCPU_START_ROUTINE)T_funcServerTestThread,
                                   TVCPU_PRIO_NORMAL, MX_NULL, MX_FALSE);
    vcpuStart(_G_pvcpuServer);
    /*
     *  启动模拟客户测试线程
     */
    _G_pvcpuClient = _T_createThread((PVCPU_START_ROUTINE)T_funcClientTestThread,
                                   TVCPU_PRIO_NORMAL, MX_NULL, MX_FALSE);
    vcpuStart(_G_pvcpuClient);
#endif

#if 0
    vcpuStart( T_createThread2((PVCPU_START_ROUTINE)_funcUserThread,
               150, _K_pprtnKernel, MX_TRUE));
#endif
}

/*********************************************************************************************************
  测试用例说明：
    运行测试：
        在 syscall.c -> SC_debug() default: 中调用 T_funcServer2Test

    测试结果：
        正常结果：T_funcServer2Test-> kernelExitAndEnIrq 之后线程一直阻塞
        BUG 结果：T_funcServer2Test-> kernelExitAndEnIrq 之后线程立即恢复运行
*********************************************************************************************************/
PMX_VCPU  T_createThread2(PVCPU_START_ROUTINE  func, UINT32  uiPrio, PMX_PRTN  pPrtn, BOOL bIsUser)
{
    MX_VCPU_ATTR vcpuattr;
    PMX_VCPU     pVcpu     = MX_NULL;
    PT_VCPU_STK  pStkUser  = MX_NULL;
    PMX_IPC_MSG  pIpcMsg  = T_memObjAlloc(TMEM_IPC_MSG);

    if (bIsUser) {
        pStkUser = T_memObjAlloc(TMEM_USE_STK);                         /*  分配用户堆栈                */
        T_vcpuStkInit(pStkUser, pIpcMsg);
        vcpuattr.VCPUATTR_pstkLowAddr = (PMX_STACK)(pStkUser->addrStkTop);
    }

    vcpuattr.VCPUATTR_uiPriority = uiPrio;
    vcpuattr.VCPUATTR_pvArg      = 0;
    vcpuattr.VCPUATTR_bIsUser    = bIsUser;

    pVcpu = vcpuCreate((PVCPU_START_ROUTINE)func, &vcpuattr);           /*  创建内核线程                */

    vcpuBindUserMsg(pVcpu, pIpcMsg);                                    /*  绑定 MSG                    */
    vcpuBindPartition(pVcpu, pPrtn);                                    /*  绑定进程                    */

    return  (pVcpu);
}

VOID  _funcUserThread (PVOID pvArg)
{
    API_debug(2); /*  syscall.c -> SC_debug() -> T_funcServer2Test */
}

VOID  _T_funcServer2Test ()
{
    /*
     *  server 模拟测试
     */
    INTREG      iregIntLevel;

    K_TITLE(T_funcServer2Test);

    while(1) {
        K_DEBUG("SERVER: LOOP");

        iregIntLevel = kernelEnterAndDisIrq();

        K_DEBUG("SERVER: before PEND operate");
        vcpuSuspend();
        K_DEBUG("SERVER: after PEND operate");

        K_WARNING("SERVER: PEND ------------");
        kernelExitAndEnIrq(iregIntLevel);
        K_WARNING("SERVER: WAKEUP------------");

        continue;
        }

        kernelExitAndEnIrq(iregIntLevel);

        K_NOTE("SERVER: WAKEUP client------------");
}
/*********************************************************************************************************
   funcUserThread END
*********************************************************************************************************/

/*********************************************************************************************************
  END
*********************************************************************************************************/
