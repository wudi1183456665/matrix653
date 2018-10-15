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
** ��   ��   ��: syscall.c
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 01 �� 05 ��
**
** ��        ��: ϵͳ����.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** ��������: scTimeToTicks
** ��������: ϵͳ����ʱ��ת��Ϊ�ں�ʱ��
** �䡡��  : ui64Time      ϵͳ����ʱ��
**           pulTicks      ת�� ticks
** �䡡��  : BOOL          TRUE - �ɹ�; FALSE - ��ʽ����
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __scUAddrCheck
** ��������: �û��ռ��ַ���
** �䡡��  : addrUser    �û��ռ��ַ
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static BOOL  __scUAddrCheck (addr_t  addrUser)
{
    /*
     *  TODO: ����Ч�û���ַ�ռ���
     */
    return  (MX_TRUE);
}

/*********************************************************************************************************
** ��������: __scVcpuCreate
** ��������: ���� VCPU
** �䡡��  : pvcpuCur   ϵͳ���õ�ǰ VCPU
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: SC_Entry
*********************************************************************************************************/
static VOID  __scVcpuCreate (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL ��������:
     *  [IN]  ARG0    - UINT32    - ������ | ���ú�
     *        ARG1    - UINT32    - ���� OBJ
     *        ARG2    - UINT32    - �߳���ڵ�ַ
     *        ARG3    - UINT32    - �߳���ڲ���
     *        IPC_MSG - UINT32    - �߳����ȼ�
     *                - addr_t    - �û� IPC_MSG ��ַ
     *                - addr_t    - �û�ջ(�͵�ַ)
     *  [OUT] ARG0    - UINT32    - ����״̬
     *        ARG1    - UINT32    - �´����߳� OBJ
     */
    PMX_SC_PARAM        pscparam = MX_SC_PARAM_GET(pvcpuCur);           /*  ϵͳ���ò����ṹ            */
    PMX_IPC_MSG         pCallMsg = MX_IPC_MSG_GET(pvcpuCur);
    ULONG               ulMsgItem;

    MX_VCPU_ATTR        vcpuattr;                                       /*  �½��߳�����                */
    PMX_VCPU            pNewVcpu   = MX_NULL;

    //MX_OBJECT_ID      objPt      = MX_SC_KOBJ_GET(pscparam);          /*  �½��̴߳��󶨵Ľ��̶���    */
    PVCPU_START_ROUTINE pfuncStart = (PVCPU_START_ROUTINE)
                                     pscparam->SCPARAM_ui32Arg2;        /*  �½��߳���ڵ�ַ            */
    UINT32              uiPrio;                                         /*  �½��߳����ȼ�              */
    PMX_IPC_MSG         pIpcMsg    = MX_NULL;                           /*  �û��� IPC ��Ϣ���ַ       */

    K_TITLE(__scVcpuCreate);

#if 0                                                                   /*  TODO: kern ����֧�ֽ���     */
    if (!kobjCheck(objPt, _KOBJ_PARTITION)) {                           /*  �����̶���Ϸ���          */
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }
#endif

    if (!__scUAddrCheck((addr_t)pfuncStart)) {                          /*  �����ڵ�ַ�Ϸ���          */
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }

    ipcMsgReadOpen(pCallMsg);
    ipcMsgUlRead(pCallMsg, &ulMsgItem);
    uiPrio = (UINT32)ulMsgItem;
    if (uiPrio > MX_PRIO_LOWEST) {                                      /*  ������ȼ��Ϸ���            */
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }
    vcpuattr.VCPUATTR_uiPriority = uiPrio;                              /*  �����߳����ȼ�              */

    ipcMsgUlRead(pCallMsg, &ulMsgItem);
    pIpcMsg = (PVOID)ulMsgItem;
    if (!__scUAddrCheck((addr_t)pIpcMsg)) {                             /*  ��� MSG ��ַ�Ϸ���         */
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;
    }

    ipcMsgUlRead(pCallMsg, &ulMsgItem);
    IPC_ASSERT(ulMsgItem != 0);                                         /*  ����û�ջ��ַ�Ϸ���        */
    vcpuattr.VCPUATTR_pstkLowAddr = (PMX_STACK)ulMsgItem;

    vcpuattr.VCPUATTR_bIsUser = MX_TRUE;
    vcpuattr.VCPUATTR_pvArg   = (PVOID)pscparam->SCPARAM_ui32Arg3;      /*  �����̲߳���                */

    pNewVcpu = vcpuCreate(pfuncStart, &vcpuattr);
    vcpuBindPartition(pNewVcpu, _K_pprtnKernel);                        /*  �󶨽���                    */

    vcpuBindIpcMsg(pNewVcpu, pIpcMsg);                                  /*  �� MSG, �����ڴ�ӳ��      */

    MX_SC_KOBJ_SET(pscparam, pNewVcpu->VCPU_ulVcpuId);                  /*  �����̶߳���                */
    MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);

#if 0
    K_DEBUG(" vcpu pt:0x%x, arg:0x%x, prio:%d, msg:0x%x",
            objPt, vcpuattr.VCPUATTR_pvArg, vcpuattr.VCPUATTR_uiPriority, ulMsgItem);
#endif
    K_WARNING(" new_vcpu=0x%x", pNewVcpu);
}
/*********************************************************************************************************
** ��������: __scVcpuStart
** ��������: VCPU ����
** �䡡��  : pvcpuCur   ϵͳ���õ�ǰ VCPU
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: SC_Entry
*********************************************************************************************************/
static VOID  __scVcpuStart (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL ��������:
     *  [IN]  ARG0    - UINT32    - ������ | ���ú�
     *        ARG1    - UINT32    - �߳� OBJ
     *  [OUT] ARG0    - UINT32    - ����״̬
     */
    PMX_SC_PARAM     pscparam  = MX_SC_PARAM_GET(pvcpuCur);             /*  ϵͳ���ò����ṹ            */

    K_TITLE(__scVcpuStart);

    if (vcpuStart(kobjGet(MX_SC_KOBJ_GET(pscparam))) != ERROR_NONE) {
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_FAIL);
        K_ERROR("ERROR");
        return;
    }

    MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                        /*  ����ϵͳ���÷��ز���        */
}
/*********************************************************************************************************
** ��������: __scVcpuSleep
** ��������: VCPU sleep
** �䡡��  : pvcpuCur   ϵͳ���õ�ǰ VCPU
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: SC_Entry
*********************************************************************************************************/
static VOID  __scVcpuSleep (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL ��������:
     *  [IN]  ARG0    - UINT32    - ������ | ���ú�
     *        ARG2    - UINT32    - ����ʱ��� 32 λ
     *        ARG3    - UINT32    - ����ʱ��� 32 λ
     *  [OUT] ARG0    - UINT32    - ����״̬
     */
    PMX_SC_PARAM     pscparam = MX_SC_PARAM_GET(pvcpuCur);              /*  ϵͳ���ò����ṹ            */
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

    MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                        /*  ����ϵͳ���÷��ز���        */
}
/*********************************************************************************************************
** ��������: __scVcpuWakeup
** ��������: ���� VCPU
** �䡡��  : pvcpuCur   ϵͳ���õ�ǰ VCPU
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: SC_Entry
*********************************************************************************************************/
static VOID  __scVcpuWakeup (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL ��������:
     *  [IN]  ARG0    - UINT32    - ������ | ���ú�
     *        ARG1    - UINT32    - �߳� OBJ
     *  [OUT] ARG0    - UINT32    - ����״̬
     */
    PMX_SC_PARAM    pscparam = MX_SC_PARAM_GET(pvcpuCur);               /*  ϵͳ���ò����ṹ            */
    MX_OBJECT_ID    objVcpu  = MX_SC_KOBJ_GET(pscparam);

    K_TITLE(__scVcpuWakeup);

    if (!kobjCheck(objVcpu, _KOBJ_VCPU)) {
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_PARAM);
        return;

    } else {
        vcpuWakeup((PMX_VCPU)kobjGet(objVcpu));
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  ����ϵͳ���÷��ز���        */
    }
}
/*********************************************************************************************************
** ��������: SC_PrtnHandle
** ��������: Partition ģ��ϵͳ���ô���
** �䡡��  : pvcpuCur   ϵͳ���õ�ǰ VCPU
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: SC_Entry
*********************************************************************************************************/
static VOID  SC_PrtnHandle (PMX_VCPU  pvcpuCur)
{
    /*
     *  �����̴���Ȩ�ޣ���ǰ�߳��Ƿ��ǽ��̹������
     *  ���̺Ϸ��Լ�飺����Դ�����м�������
     *  ������̿��ƿ�,  ����س�ID
     *  ����ҳ��
     *  ��������ڴ�ռ䣺���̿ռ���룬ȷ����Ч�ڴ�
     *  ���ؽ��̺�����Ч�����ڴ棨�����ڴ���ɽ��̹������ȷ����
     */
    PMX_SC_PARAM     pscparam = MX_SC_PARAM_GET(pvcpuCur);              /*  ϵͳ���ò����ṹ            */
    UINT8            ucOpCode;

    ucOpCode = MX_SC_OPCODE_GET(pscparam);

    switch (ucOpCode) {

    case MX_SC_OPCODE_PRTN_RESTART:
        prtnRestart(MX_NULL);
        break;
        
    default:
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_ILLEGAL);               /*  ���Ϸ���ϵͳ����            */
        break;
    }
}
/*********************************************************************************************************
** ��������: SC_IrqHandle
** ��������: Irq ģ��ϵͳ���ô���
** �䡡��  : pvcpuCur     �������߳̿��ƿ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  SC_IrqHandle (PMX_VCPU  pvcpuCur)
{
    PMX_SC_PARAM     pscparam = MX_SC_PARAM_GET(pvcpuCur);              /*  ϵͳ���ò����ṹ            */
    UINT8            ucOpCode;
    INTREG           iregInterLevel;

    iregInterLevel = kernelEnterAndDisIrq();                            /*  �����ں�ͬʱ�ر��ж�        */

    ucOpCode = MX_SC_OPCODE_GET(pscparam);

    switch (ucOpCode) {

    case MX_SC_OPCODE_IRQ_REGISTER:
        irqRegister((IRQ_FUNCPTR)pscparam->SCPARAM_ui32Arg1);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  ����ϵͳ���÷��ز���        */
        break;

    case MX_SC_OPCODE_IRQ_ATTACH:
        irqAttach((ULONG)pscparam->SCPARAM_ui32Arg1);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  ����ϵͳ���÷��ز���        */
        break;

    case MX_SC_OPCODE_IRQ_ENABLE:
        irqEnable(pscparam->SCPARAM_ui32Arg1);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  ����ϵͳ���÷��ز���        */
        break;

    case MX_SC_OPCODE_IRQ_DISABLE:
        irqDisable(pscparam->SCPARAM_ui32Arg1);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  ����ϵͳ���÷��ز���        */
        break;

    case MX_SC_OPCODE_SWI_REGISTER:
        swiRegister(pscparam->SCPARAM_ui32Arg1, (IRQ_FUNCPTR)pscparam->SCPARAM_ui32Arg2);
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                    /*  ����ϵͳ���÷��ز���        */
        break;

    default:
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_ILLEGAL);               /*  ���Ϸ���ϵͳ����            */
        break;
    }

    kernelExitAndEnIrq(iregInterLevel);                                 /*  �˳��ں�ͬʱ���ж�        */
}
/*********************************************************************************************************
** ��������: SC_IrqHandle
** ��������: Vcpu ģ��ϵͳ���ô���
** �䡡��  : pvcpuCur     �������߳̿��ƿ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  SC_VcpuHandle (PMX_VCPU  pvcpuCur)
{
    PMX_SC_PARAM     pscparam = MX_SC_PARAM_GET(pvcpuCur);              /*  ϵͳ���ò����ṹ            */
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
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_ILLEGAL);               /*  ���Ϸ���ϵͳ����            */
        break;
    }
}
/*********************************************************************************************************
** ��������: SC_DbgHandle
** ��������: ���Խӿ�
** �䡡��  : pvcpuCur   ϵͳ���õ�ǰ VCPU
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: SC_Entry
*********************************************************************************************************/
static VOID  SC_DbgHandle (PMX_VCPU  pvcpuCur)
{
    /* 
     *  SYSCALL ��������:
     *  [IN]  ARG0    - UINT32    - ������ | ���ú�
     *        IPC_MSG - ......    - IPC_DEBUG_BUF_OFFSET ƫ��
     *                - IPC_STRING- ����ַ����ṹ
     *  [OUT] ARG0    - UINT32    - ����״̬
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

    MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_OK);                        /*  ����ϵͳ���÷��ز���        */
}
/*********************************************************************************************************
** ��������: scEntry
** ��������: ϵͳ���ó������
** �䡡��  : puctx    ϵͳ����������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: archSwiHandle()
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
        MX_SC_RESULT_SET(pscparam, MX_SC_RESULT_ILLEGAL);               /*  ���Ϸ���ϵͳ����            */
        break;
    }

    ARCH_SCPARAM_COPYTO_UCTX(MX_SC_PARAM_GET(pvcpuCur), puctx);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
