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
** ��   ��   ��: api.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 02 �� 01 ��
**
** ��        ��: �û� API �ӿ�
*********************************************************************************************************/
#ifndef __MXI_H
#define __MXI_H

/*********************************************************************************************************
  VCPU ϵͳ����
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_vcpuCreate(PMX_IPC_MSG         pipcMsg,
                               MX_OBJECT_ID        objPt,
                               MX_OBJECT_ID       *pobjVcpu,
                               PVCPU_START_ROUTINE pRount,
                               PVOID               pvArg,
                               UINT32              uiPriority,
                               addr_t              addrStack,
                               addr_t              addrMsgTag);

MX_SC_RESULT_T  MXI_vcpuStart(MX_OBJECT_ID  objVcpu);
MX_SC_RESULT_T  MXI_vcpuSleep(SC_TIME  scTime);
MX_SC_RESULT_T  MXI_vcpuWakeup(MX_OBJECT_ID  objVcpu);
INTREG          MXI_vcpuDisableInt(VOID);
VOID            MXI_vcpuEnableInt(INTREG  iregInterLevel);
/*********************************************************************************************************
  Partition ϵͳ����
*********************************************************************************************************/
VOID            MXI_prtnRestart(MX_OBJECT_ID  objPrtn);
/*********************************************************************************************************
  IPC ϵͳ����
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_ipcCall(MX_OBJECT_ID  objTag, SC_TIME  ullTimeout);
MX_SC_RESULT_T  MXI_ipcSend(MX_OBJECT_ID  objTag, SC_TIME  ullTimeout);
MX_SC_RESULT_T  MXI_ipcOpenWait(MX_OBJECT_ID  *pobjSender);
MX_SC_RESULT_T  MXI_ipcReplyWait(MX_OBJECT_ID  *pobjSender);
MX_SC_RESULT_T  MXI_ipcClose(VOID);
/*********************************************************************************************************
  IRQ ϵͳ����
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_irqRegister(IRQ_FUNCPTR  pfuncIsr);
MX_SC_RESULT_T  MXI_irqAttach(UINT32  uiIrqNum);
MX_SC_RESULT_T  MXI_irqEnable(UINT32  uiIrqNum);
MX_SC_RESULT_T  MXI_irqDisable(UINT32  uiIrqNum);
MX_SC_RESULT_T  MXI_swiRegister(UINT32  uiSwiNum, IRQ_FUNCPTR  pfuncIsr);
/*********************************************************************************************************
  DEBUG ϵͳ����
*********************************************************************************************************/
VOID            MXI_debug(UINT8  ucOpCode);
MX_SC_RESULT_T  MXI_debugRead(INT  *piVal);
MX_SC_RESULT_T  MXI_debugWrite(INT  iVal);

#endif                                                                  /*  __MXI_H                     */
/*********************************************************************************************************
  END
*********************************************************************************************************/
