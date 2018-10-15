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
** 文   件   名: api.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 02 月 01 日
**
** 描        述: 用户 API 接口
*********************************************************************************************************/
#ifndef __MXI_H
#define __MXI_H

/*********************************************************************************************************
  VCPU 系统调用
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
  Partition 系统调用
*********************************************************************************************************/
VOID            MXI_prtnRestart(MX_OBJECT_ID  objPrtn);
/*********************************************************************************************************
  IPC 系统调用
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_ipcCall(MX_OBJECT_ID  objTag, SC_TIME  ullTimeout);
MX_SC_RESULT_T  MXI_ipcSend(MX_OBJECT_ID  objTag, SC_TIME  ullTimeout);
MX_SC_RESULT_T  MXI_ipcOpenWait(MX_OBJECT_ID  *pobjSender);
MX_SC_RESULT_T  MXI_ipcReplyWait(MX_OBJECT_ID  *pobjSender);
MX_SC_RESULT_T  MXI_ipcClose(VOID);
/*********************************************************************************************************
  IRQ 系统调用
*********************************************************************************************************/
MX_SC_RESULT_T  MXI_irqRegister(IRQ_FUNCPTR  pfuncIsr);
MX_SC_RESULT_T  MXI_irqAttach(UINT32  uiIrqNum);
MX_SC_RESULT_T  MXI_irqEnable(UINT32  uiIrqNum);
MX_SC_RESULT_T  MXI_irqDisable(UINT32  uiIrqNum);
MX_SC_RESULT_T  MXI_swiRegister(UINT32  uiSwiNum, IRQ_FUNCPTR  pfuncIsr);
/*********************************************************************************************************
  DEBUG 系统调用
*********************************************************************************************************/
VOID            MXI_debug(UINT8  ucOpCode);
MX_SC_RESULT_T  MXI_debugRead(INT  *piVal);
MX_SC_RESULT_T  MXI_debugWrite(INT  iVal);

#endif                                                                  /*  __MXI_H                     */
/*********************************************************************************************************
  END
*********************************************************************************************************/
