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
** ��   ��   ��: boot.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 2 ��
**
** ��        ��: ������������
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
  ����
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
** ��������: T_createThread
** ��������: �ں�̬�����߳�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PMX_VCPU  _T_createThread(PVCPU_START_ROUTINE  func, UINT32  uiPrio, PVOID  pvArg, BOOL isUser)
{
    MX_OBJECT_ID objVcpu;
    MX_VCPU_ATTR vcpuattr;
    PMX_VCPU     pVcpu     = MX_NULL;

    PT_VCPU      pThread   = T_memObjAlloc(TMEM_VCPU);

    K_TITLE(T_createKernThread);


    pThread->objPt      = 0;                                            /*  TODO: ���̶���              */
    pThread->pfuncStart = (PVCPU_START_ROUTINE) T_funcRootThread;
    pThread->uiPriority = uiPrio;
    pThread->addrStack  = 0;
    pThread->pIpcMsg    = T_memObjAlloc(TMEM_IPC_MSG);

    vcpuattr.VCPUATTR_uiPriority = uiPrio;
    vcpuattr.VCPUATTR_pvArg      = pvArg;
    vcpuattr.VCPUATTR_bIsUser    = isUser;

    if (isUser) {
        pThread->addrStack  = (addr_t)T_memObjAlloc(TMEM_USE_STK);      /*  �����û���ջ                */
        T_vcpuStkInit((PT_VCPU_STK)pThread->addrStack, pThread->pIpcMsg);
        vcpuattr.VCPUATTR_pstkLowAddr =
                (PMX_STACK)((PT_VCPU_STK)pThread->addrStack)->addrStkTop;
    }

    if (!pvArg) {
            vcpuattr.VCPUATTR_pvArg = (PVOID)pThread;
        }

    pVcpu = vcpuCreate((PVCPU_START_ROUTINE)func, &vcpuattr);           /*  �����ں��߳�                */
    objVcpu = pVcpu->VCPU_ulVcpuId;

    pThread->pvArg    = (PVOID)objVcpu;

    vcpuBindUserMsg(pVcpu, pThread->pIpcMsg);                           /*  �� MSG                    */

    vcpuBindPartition(pVcpu, _K_pprtnKernel);                           /*  �󶨽���                    */

    return  (pVcpu);
}
/*********************************************************************************************************
** ��������: T_envBoot; �ں�̬
** ��������: ��������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
void T_envBoot()
{
    K_TITLE(T_envBoot);
    /*
     *  ���� root_server -> ���� log_server
     *  �����ں˵�Ԫ���� -- IPC --> root_server -> �����û���Ԫ����
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
     *  �����ں˵�Ԫ����
     */
#if 1
_G_pvcpuClient = _T_createThread((PVCPU_START_ROUTINE)T_funcKernTestThread,
                               TVCPU_PRIO_NORMAL, MX_NULL, MX_FALSE);
vcpuStart(_G_pvcpuClient);
#endif

#if 0
    /*
     *  ����ģ���������߳�
     */
    _G_pvcpuServer = _T_createThread((PVCPU_START_ROUTINE)T_funcServerTestThread,
                                   TVCPU_PRIO_NORMAL, MX_NULL, MX_FALSE);
    vcpuStart(_G_pvcpuServer);
    /*
     *  ����ģ��ͻ������߳�
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
  ��������˵����
    ���в��ԣ�
        �� syscall.c -> SC_debug() default: �е��� T_funcServer2Test

    ���Խ����
        ���������T_funcServer2Test-> kernelExitAndEnIrq ֮���߳�һֱ����
        BUG �����T_funcServer2Test-> kernelExitAndEnIrq ֮���߳������ָ�����
*********************************************************************************************************/
PMX_VCPU  T_createThread2(PVCPU_START_ROUTINE  func, UINT32  uiPrio, PMX_PRTN  pPrtn, BOOL bIsUser)
{
    MX_VCPU_ATTR vcpuattr;
    PMX_VCPU     pVcpu     = MX_NULL;
    PT_VCPU_STK  pStkUser  = MX_NULL;
    PMX_IPC_MSG  pIpcMsg  = T_memObjAlloc(TMEM_IPC_MSG);

    if (bIsUser) {
        pStkUser = T_memObjAlloc(TMEM_USE_STK);                         /*  �����û���ջ                */
        T_vcpuStkInit(pStkUser, pIpcMsg);
        vcpuattr.VCPUATTR_pstkLowAddr = (PMX_STACK)(pStkUser->addrStkTop);
    }

    vcpuattr.VCPUATTR_uiPriority = uiPrio;
    vcpuattr.VCPUATTR_pvArg      = 0;
    vcpuattr.VCPUATTR_bIsUser    = bIsUser;

    pVcpu = vcpuCreate((PVCPU_START_ROUTINE)func, &vcpuattr);           /*  �����ں��߳�                */

    vcpuBindUserMsg(pVcpu, pIpcMsg);                                    /*  �� MSG                    */
    vcpuBindPartition(pVcpu, pPrtn);                                    /*  �󶨽���                    */

    return  (pVcpu);
}

VOID  _funcUserThread (PVOID pvArg)
{
    API_debug(2); /*  syscall.c -> SC_debug() -> T_funcServer2Test */
}

VOID  _T_funcServer2Test ()
{
    /*
     *  server ģ�����
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
