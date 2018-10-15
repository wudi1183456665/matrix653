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
** 文   件   名: test.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 27 日
**
** 描        述: 测试用例.
*********************************************************************************************************/
#include <Matrix.h>
#include <api.h>
/*********************************************************************************************************
   函数声明
*********************************************************************************************************/
VOID  testStart(VOID);
VOID  T_envBoot(VOID);
VOID  syn_test(VOID);
VOID  syn_test2(VOID);
/*********************************************************************************************************
** 函数名称: testInit
** 功能描述: 测试线程初始化
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  testInit (VOID)
{
#if 1
    testStart();                                                        /*  使用本文件中测试函数测试    */
    //syn_test();
    //syn_test2();
#else
    T_envBoot();                                                        /*  启动测试框架                */
#endif
}

UINT64 _G_ui64TestBeforeSwi;
extern UINT64  armGlobalTimerCounterGet(VOID);
//extern UINT64 _G_ui64TestBeforeSwi;
//extern UINT64  armGlobalTimerCounterGet(VOID);
UINT64 _G_ui64TestCnt;
UINT64 _G_ui64TestAvg;
UINT64 _G_ui64TestMin = 1000000;
UINT64 _G_ui64TestMax;
/*********************************************************************************************************
** 函数名称: __testThread
** 功能描述: 测试线程
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static PVOID  __testThread (PVOID  pvArg)
{
    INT    i          = 0;
    INT    iArg       = (INT)pvArg;
    PCHAR  pcColor[4] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m"};

    while (1) {
        //bspDebugMsg("%s-- %d, %X ------ vCPU%d @%d\033[37m\n", pcColor[archMpCur()/*iArg*/%4], i++, archGetCpsr(), iArg, archMpCur());
        //bspDebugMsg("%s-- %d, %X ------ vCPU%d\n", pcColor[/*archMpCur()*/iArg%4], i++, archGetCpsr(), iArg);
        _G_ui64TestBeforeSwi = armGlobalTimerCounterGet();

//        __asm__ __volatile__("swi  3\n\t");

        UINT64 ui64TestEnterSwi = armGlobalTimerCounterGet();
        UINT64 ui64TestDiff = ui64TestEnterSwi - _G_ui64TestBeforeSwi;
        bspDebugMsg("B:%u-%10u, ", (UINT)(_G_ui64TestBeforeSwi >> 32), (UINT)(_G_ui64TestBeforeSwi));
        bspDebugMsg("E:%u-%10u, ", (UINT)(ui64TestEnterSwi >> 32), (UINT)(ui64TestEnterSwi));
        bspDebugMsg("%d, ", (UINT)(ui64TestDiff));
        _G_ui64TestAvg = (((_G_ui64TestAvg * _G_ui64TestCnt) + ui64TestDiff) + (_G_ui64TestCnt + 1)/2)/ (_G_ui64TestCnt + 1);
        _G_ui64TestMin = MIN(_G_ui64TestMin, ui64TestDiff);
        _G_ui64TestMax = MAX(_G_ui64TestMax, ui64TestDiff);
        _G_ui64TestCnt++;
        bspDebugMsg("-----%d AVG:%d MIN:%d MAX:%d\n", (UINT)_G_ui64TestCnt, (UINT)_G_ui64TestAvg, (UINT)_G_ui64TestMin, (UINT)_G_ui64TestMax);

        //vcpuSleep(59);
    }
    
    return  (MX_NULL);
}
/*********************************************************************************************************
** 函数名称: testStart
** 功能描述: 创建测试线程并加入到调度器中
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  testStart (VOID)
{
    INT             i;
    PMX_VCPU        pvcpu;
    MX_VCPU_ATTR    vcpuattr;

    /*
     *  构建测试内核线程
     */
    for (i = 0; i < 1; i++) {
        vcpuattr.VCPUATTR_uiPriority = 201;
        vcpuattr.VCPUATTR_pvArg      = (PVOID)i;
        vcpuattr.VCPUATTR_bIsUser    = MX_TRUE;
        //vcpuattr.VCPUATTR_bIsUser    = MX_FALSE;
        pvcpu = vcpuCreate(__testThread, &vcpuattr);                    /*  创建测试 vCPU               */

        vcpuBindPartition(pvcpu, _K_pprtnKernel);
        //vcpuSetAffinity(pvcpu, i%4);
        vcpuStart(pvcpu);
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

static volatile UINT32     G_uiCount     = 0;
static volatile BOOL       G_bSvrSuspend = MX_FALSE;

static PMX_VCPU            G_pvcpuServer;
static PMX_VCPU            G_pvcpuClient;

#if 1
#undef  K_DEBUG
#undef  K_WARNING
#define K_DEBUG(msg)
#define K_WARNING(msg)
#endif

#define UART1_Rx_TST       (*(volatile int *)(0x02020098))              /*  Receiver status reg         */
#define UART1_Rx_VAL       (*(volatile int *)(0x02020000))              /*  Receiver reg                */

VOID  funcServer (PVOID pvArg)
{
    PMX_VCPU    pvcpuClient  = G_pvcpuClient;
    INTREG      iregIntLevel;

    while (1) {
        K_WARNING("SERVER: LOOP");
        iregIntLevel = kernelEnterAndDisIrq();
        //K_TITLE("SERVER: %d", G_uiCount);

        /*
         *  没有消息请求的情况下, 服务线程休眠
         */
        if (G_uiCount == 0) {
            G_bSvrSuspend = MX_TRUE;
            K_WARNING("SERVER: SUSPEND ------------");
            vcpuSuspend(); // or  vcpuSleep(500);

            K_WARNING("SERVER: PEND ------------");
            kernelExitAndEnIrq(iregIntLevel);
            K_WARNING("SERVER: WAKEUP------------");

            continue;
        }

        /*
         *  处理消息
         */
        G_uiCount--;

        /*
         *  处理完成后唤醒客户线程
         */
        K_WARNING("SERVER: WAKEUP client------------");
        vcpuWakeup(pvcpuClient);

        kernelExitAndEnIrq(iregIntLevel);
    }
}

VOID  funcClient (PVOID pvArg)
{
    INTREG      iregIntLevel;
    PMX_VCPU    pvcpuServer  = G_pvcpuServer;

    vcpuSleep(10);

    while (1) {
#if 0
        if ((UART1_Rx_TST & (1 << 0))) {
            INT  i = UART1_Rx_VAL;

            dumpReady(MX_GLOBAL_RDY());
            dumpCpus();
        }
#endif
        K_DEBUG("CLIENT: LOOP");
        iregIntLevel = kernelEnterAndDisIrq();
        //K_NOTE("CLIENT: %d", G_uiCount);

        /*
         *  向服务线程发送 IPC,
         *  如果服务正在休眠, 则唤醒
         */
        G_uiCount++;
        if (G_bSvrSuspend == MX_TRUE) {
            G_bSvrSuspend = MX_FALSE;
            K_DEBUG("CLIENT: WAKEUP server------------");
            vcpuWakeup(pvcpuServer);
        }

        /*
         *  客户线程休眠, 等待服务线程处理完后唤醒
         */
        vcpuSuspend(); // or  vcpuSleep(500);
        K_DEBUG("CLIENT: PEND++++++++++++++");
        kernelExitAndEnIrq(iregIntLevel);
        K_DEBUG("CLIENT: WAKEUP+++++++++++++");
    }
}



static PMX_VCPU  T_createThread(PVCPU_START_ROUTINE  func, UINT32  uiPrio, PMX_PRTN  pPrtn)
{
    MX_VCPU_ATTR vcpuattr;
    PMX_VCPU     pVcpu     = MX_NULL;

    vcpuattr.VCPUATTR_uiPriority = uiPrio;
    vcpuattr.VCPUATTR_pvArg      = 0;
    vcpuattr.VCPUATTR_bIsUser    = MX_FALSE;

    pVcpu = vcpuCreate((PVCPU_START_ROUTINE)func, &vcpuattr);
    vcpuBindPartition(pVcpu, pPrtn);

    return  (pVcpu);
}

VOID  syn_test (VOID)
{
    G_pvcpuServer = T_createThread((PVCPU_START_ROUTINE)funcServer, 150, _K_pprtnKernel);
    //vcpuSetAffinity(G_pvcpuServer, 1);
    vcpuStart(G_pvcpuServer);
	
    G_pvcpuClient = T_createThread((PVCPU_START_ROUTINE)funcClient, 150, _K_pprtnKernel);
    //vcpuSetAffinity(G_pvcpuClient, 2);
    vcpuStart(G_pvcpuClient);
}



/*********************************************************************************************************
  测试用例说明：
	运行测试：
		在 syscall.c -> SC_debug() default: 中调用 T_funcServer2Test
		
    测试结果：
		正常结果：T_funcServer2Test-> kernelExitAndEnIrq 之后线程一直阻塞
		BUG 结果：T_funcServer2Test-> kernelExitAndEnIrq 之后线程立即恢复运行
*********************************************************************************************************/
VOID  funcUserThread (PVOID pvArg)
{
	//API_debug(2); /*  syscall.c -> SC_debug() -> T_funcServer2Test */
}

VOID  T_funcServer2Test ()
{
    /*
     *  server 模拟测试
     */
    INTREG      iregIntLevel;

    K_TITLE(T_funcServer2TestThread);

    while (1) {
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

VOID  syn_test2 (VOID)
{
    G_pvcpuServer = T_createThread((PVCPU_START_ROUTINE)funcUserThread, 150, _K_pprtnKernel);
    vcpuStart(G_pvcpuServer);
}
