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
** ��   ��   ��: x86_support.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 11 �� 28 ��
**
** ��        ��: x86 ��ϵ���ܽӿ�, �༶֧�ְ��ӿ�.
*********************************************************************************************************/

#ifndef __ARCH_X86_SUPPORT_H
#define __ARCH_X86_SUPPORT_H

/*********************************************************************************************************
  ������ͷ�ļ�
*********************************************************************************************************/

#include "arch/assembler.h"
asdf
/*********************************************************************************************************
  CPU �ͺ� (��ʼ�� Cache, Mmu ʱʹ��)
*********************************************************************************************************/
#define X86_MACHINE_PC              "x86"                               /*  x86                         */

/*********************************************************************************************************
  �洢������ (CPU ջ������)
*********************************************************************************************************/

#define CPU_STK_GROWTH              1                                   /*  1: ��ջ�Ӹߵ�ַ��͵�ַ     */
                                                                        /*  0: ��ջ�ӵ͵�ַ��ߵ�ַ     */
/*********************************************************************************************************
  arch �Ѿ��ṩ�Ľӿ�����:
*********************************************************************************************************/

VOID    archKernelParam(CPCHAR  pcParam);

/*********************************************************************************************************
  x86 ����������
*********************************************************************************************************/

VOID    archAssert(INT  iCond, CPCHAR  pcFunc, CPCHAR  pcFile, INT  iLine);

/*********************************************************************************************************
  x86 �������߳���������ؽӿ�
*********************************************************************************************************/

PMX_STACK  archVcpuCtxCreate(PTHREAD_START_ROUTINE  pfuncTask,
                             PVOID                  pvArg,
                             PMX_STACK              pstkTop,
                             ULONG                  ulOpt);
VOID    archVcpuCtxStart(PMX_CLASS_CPU  pcpuSw);
VOID    archVcpuCtxSwitch(PMX_CLASS_CPU  pcpuSw);
VOID    archVcpuCtxPrint(PMX_STACK  pstkTop);

/*********************************************************************************************************
  x86 ������������ؽӿ�
*********************************************************************************************************/
VOID    archPartitionCreate(VOID);

/*********************************************************************************************************
  x86 �������쳣�ӿ�
*********************************************************************************************************/

VOID    archIntHandle(ULONG  ulVector, BOOL  bPreemptive);              /*  bspIntHandle ��Ҫ���ô˺��� */

/*********************************************************************************************************
  x86 ͨ�ÿ�
*********************************************************************************************************/

INT     archFindLsb(UINT32 ui32);
INT     archFindMsb(UINT32 ui32);

/*********************************************************************************************************
  x86 ��������׼�ײ��
*********************************************************************************************************/

INTREG  archIntDisable(VOID);
VOID    archIntEnable(INTREG  iregInterLevel);
VOID    archIntEnableForce(VOID);

VOID    archPageCopy(PVOID pvTo, PVOID pvFrom);

VOID    archReboot(INT  iRebootType, addr_t  ulStartAddress);

/*********************************************************************************************************
  x86 ������ CACHE ����
*********************************************************************************************************/

VOID    archCacheReset(CPCHAR     pcMachineName);
VOID    archCacheInit(CACHE_MODE  uiInstruction, CACHE_MODE  uiData, CPCHAR  pcMachineName);

/*********************************************************************************************************
  x86 ������ MMU ���� (�ɹ��� bsp ʹ��)
*********************************************************************************************************/

MX_ERROR        archMmuInit(CPCHAR  pcMachineName);

PMX_PGD_TABLE   archMmuPgdTableAlloc(VOID);
VOID            archMmuPgdTableFree(PMX_PGD_TABLE  ppgdtabFree);
BOOL            archMmuPgdEntryIsOk(MX_PGD_ENTRY   pgdentCheck);
MX_PGD_ENTRY    archMmuPgdEntryGet(MX_PGD_TABLE  ppgdtabCtx, addr_t  ulVirtAddr);

PMX_PTE_TABLE   archMmuPteTableAlloc(VOID);
VOID            archMmuPteTableFree(PMX_PTE_TABLE  pptetabFree);
BOOL            archMmuPteEntryIsOk(MX_PTE_ENTRY   pteentCheck);
MX_PTE_ENTRY    archMmuPteEntryGet(MX_PGD_ENTRY  pgdentFather, addr_t  ulVirtAddr);

MX_ERROR        archMmuFlagSet(addr_t  ulAddr, ULONG  ulFlag);
ULONG           archMmuFlagGet(addr_t  ulAddr);

MX_ERROR        archMmuMap(addr_t  ulVirtAddr, addr_t  ulPhysAddr, ULONG  ulFlag);
addr_t          archMmuVirt2Phys(addr_t  ulVirtAddr);

VOID            archMmuLoad(PMX_PGD_TABLE  ppgdtabLoad);
VOID            archMmuEnable(VOID);
VOID            archMmuDisable(VOID);
VOID            archMmuInvTLB(addr_t  ulPageAddr, ULONG  ulPageNum);


/*********************************************************************************************************
  x86 �������������������
*********************************************************************************************************/

VOID    archSpinInit(spinlock_t  *psl);
VOID    archSpinDelay(VOID);
VOID    archSpinNotify(VOID);

INT     archSpinLock(spinlock_t     *psl);
INT     archSpinTryLock(spinlock_t  *psl);
INT     archSpinUnlock(spinlock_t   *psl);

INT     archSpinLockRaw(spinlock_t     *psl);
INT     archSpinTryLockRaw(spinlock_t  *psl);
INT     archSpinUnlockRaw(spinlock_t   *psl);

ULONG   archMpCur(VOID);
VOID    archMpInt(ULONG  ulCPUId);

/*********************************************************************************************************
  x86 �ڴ�����
*********************************************************************************************************/

#define KN_BARRIER()    __asm__ __volatile__ ("" : : : "memory")

#if MX_CFG_CPU_X86_NO_BARRIER > 0
#define KN_MB()         __asm__ __volatile__ ("nop" : : : "memory")
#define KN_RMB()        __asm__ __volatile__ ("nop" : : : "memory")
#define KN_WMB()        __asm__ __volatile__ ("nop" : : : "memory")
#else
#define KN_MB()         __asm__ __volatile__ ("mfence" : : : "memory")
#define KN_RMB()        __asm__ __volatile__ ("lfence" : : : "memory")
#define KN_WMB()        __asm__ __volatile__ ("sfence" : : : "memory")
#endif                                                                  /*  MX_CFG_CPU_X86_NO_BARRIER   */

#define KN_SMP_MB()     KN_MB()
#define KN_SMP_RMB()    KN_RMB()
#define KN_SMP_WMB()    KN_WMB()

/*********************************************************************************************************
  bsp ��Ҫ�ṩ�Ľӿ�����:
*********************************************************************************************************/
/*********************************************************************************************************
  x86 �������ж������ж�
*********************************************************************************************************/

VOID    bspIntInit(VOID);
VOID    bspIntHandle(ULONG  ulVector);

VOID    bspIntVectorEnable(ULONG  ulVector);
VOID    bspIntVectorDisable(ULONG  ulVector);
BOOL    bspIntVectorIsEnable(ULONG  ulVector);

/*********************************************************************************************************
  CPU ��ʱ��ʱ��
*********************************************************************************************************/

VOID    bspTickInit(VOID);
VOID    bspDelayUs(ULONG ulUs);
VOID    bspDelayNs(ULONG ulNs);

/*********************************************************************************************************
  ϵͳ�������� (ulStartAddress �������ڵ���, BSP �ɺ���)
*********************************************************************************************************/

VOID    bspReboot(INT  iRebootType, addr_t  ulStartAddress);

/*********************************************************************************************************
  ϵͳ�ؼ���Ϣ��ӡ (��ӡ��Ϣ���������κβ���ϵͳ api)
*********************************************************************************************************/

VOID    bspDebugMsg(CPCHAR pcMsg);

/*********************************************************************************************************
  BSP ��Ϣ
*********************************************************************************************************/

CPCHAR  bspInfoCpu(VOID);
CPCHAR  bspInfoCache(VOID);
CPCHAR  bspInfoPacket(VOID);
CPCHAR  bspInfoVersion(VOID);
ULONG   bspInfoHwcap(VOID);
addr_t  bspInfoRomBase(VOID);
size_t  bspInfoRomSize(VOID);
addr_t  bspInfoRamBase(VOID);
size_t  bspInfoRamSize(VOID);

/*********************************************************************************************************
  x86 ������ MMU ����
*********************************************************************************************************/

ULONG   bspMmuPgdMaxNum(VOID);
ULONG   bspMmuPteMaxNum(VOID);

/*********************************************************************************************************
  x86 ��������˲���
*********************************************************************************************************/

VOID    bspMpInt(ULONG  ulCPUId);                                       /*  ����һ���˼��ж�            */
VOID    bspCpuUp(ULONG  ulCPUId);                                       /*  ����һ�� CPU                */
VOID    bspCpuUpDone(VOID);                                             /*  һ�� CPU �������           */

VOID    bspSecondaryCpusUp(VOID);                                       /*  �������е� Secondary Cpu    */
INT     bspSecondaryInit(VOID);                                         /*  Secondary Cpu ������ĳ�ʼ��*/

VOID    bspCpuDown(ULONG  ulCPUId);                                     /*  ֹͣһ�� CPU                */

VOID    bspCpuIpiVectorInstall(VOID);                                   /*  ��װ IPI ����               */

/*********************************************************************************************************
  x86 ��ϵ�ṹ�ĳ�ʼ������
*********************************************************************************************************/

VOID    x86ExceptIrqInit(VOID);

INT     x86GdtInit(VOID);
INT     x86GdtSecondaryInit(VOID);

INT     x86TssInit(VOID);
INT     x86TssSecondaryInit(VOID);

INT     x86IdtInit(VOID);
INT     x86IdtSecondaryInit(VOID);
INT     x86IdtSetHandler(UINT8  ucX86Vector, addr_t  ulHandlerAddr, INT  iLowestPriviledge);

VOID    x86CpuIdProbe(VOID);
VOID    x86CpuIdShow(VOID);

/*********************************************************************************************************
  x86 ָ��궨��
*********************************************************************************************************/

#define X86_PAUSE()                 __asm__ __volatile__ ("pause"  : : : "memory")
#define X86_HLT()                   __asm__ __volatile__ ("hlt"    : : : "memory")
#define X86_WBINVD()                __asm__ __volatile__ ("wbinvd" : : : "memory")

#endif                                                                  /*  __ARCH_X86_SUPPORT_H        */
/*********************************************************************************************************
  END
*********************************************************************************************************/

