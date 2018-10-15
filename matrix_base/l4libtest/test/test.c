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
** ��   ��   ��: test.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 01 �� 27 ��
**
** ��        ��: ��������.
*********************************************************************************************************/
#include <Matrix.h>
#include <api.h>
/*********************************************************************************************************
   ��������
*********************************************************************************************************/
VOID  testStart(VOID);
VOID  T_envBoot(VOID);
VOID  syn_test(VOID);
VOID  syn_test2(VOID);
/*********************************************************************************************************
** ��������: testInit
** ��������: �����̳߳�ʼ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  testInit (VOID)
{
#if 1
    testStart();                                                        /*  ʹ�ñ��ļ��в��Ժ�������    */
    //syn_test();
    //syn_test2();
#else
    T_envBoot();                                                        /*  �������Կ��                */
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
** ��������: __testThread
** ��������: �����߳�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
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
** ��������: testStart
** ��������: ���������̲߳����뵽��������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  testStart (VOID)
{
    INT             i;
    PMX_VCPU        pvcpu;
    MX_VCPU_ATTR    vcpuattr;

    /*
     *  ���������ں��߳�
     */
    for (i = 0; i < 1; i++) {
        vcpuattr.VCPUATTR_uiPriority = 201;
        vcpuattr.VCPUATTR_pvArg      = (PVOID)i;
        vcpuattr.VCPUATTR_bIsUser    = MX_TRUE;
        //vcpuattr.VCPUATTR_bIsUser    = MX_FALSE;
        pvcpu = vcpuCreate(__testThread, &vcpuattr);                    /*  �������� vCPU               */

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
         *  û����Ϣ����������, �����߳�����
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
         *  ������Ϣ
         */
        G_uiCount--;

        /*
         *  ������ɺ��ѿͻ��߳�
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
         *  ������̷߳��� IPC,
         *  ���������������, ����
         */
        G_uiCount++;
        if (G_bSvrSuspend == MX_TRUE) {
            G_bSvrSuspend = MX_FALSE;
            K_DEBUG("CLIENT: WAKEUP server------------");
            vcpuWakeup(pvcpuServer);
        }

        /*
         *  �ͻ��߳�����, �ȴ������̴߳��������
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
  ��������˵����
	���в��ԣ�
		�� syscall.c -> SC_debug() default: �е��� T_funcServer2Test
		
    ���Խ����
		���������T_funcServer2Test-> kernelExitAndEnIrq ֮���߳�һֱ����
		BUG �����T_funcServer2Test-> kernelExitAndEnIrq ֮���߳������ָ�����
*********************************************************************************************************/
VOID  funcUserThread (PVOID pvArg)
{
	//API_debug(2); /*  syscall.c -> SC_debug() -> T_funcServer2Test */
}

VOID  T_funcServer2Test ()
{
    /*
     *  server ģ�����
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
