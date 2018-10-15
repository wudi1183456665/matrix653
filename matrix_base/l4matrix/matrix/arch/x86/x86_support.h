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
** 文   件   名: x86_support.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 11 月 28 日
**
** 描        述: x86 体系构架接口, 班级支持包接口.
*********************************************************************************************************/

#ifndef __ARCH_X86_SUPPORT_H
#define __ARCH_X86_SUPPORT_H

/*********************************************************************************************************
  汇编相关头文件
*********************************************************************************************************/

#include "arch/assembler.h"
asdf
/*********************************************************************************************************
  CPU 型号 (初始化 Cache, Mmu 时使用)
*********************************************************************************************************/
#define X86_MACHINE_PC              "x86"                               /*  x86                         */

/*********************************************************************************************************
  存储器定义 (CPU 栈区操作)
*********************************************************************************************************/

#define CPU_STK_GROWTH              1                                   /*  1: 入栈从高地址向低地址     */
                                                                        /*  0: 入栈从低地址向高地址     */
/*********************************************************************************************************
  arch 已经提供的接口如下:
*********************************************************************************************************/

VOID    archKernelParam(CPCHAR  pcParam);

/*********************************************************************************************************
  x86 处理器断言
*********************************************************************************************************/

VOID    archAssert(INT  iCond, CPCHAR  pcFunc, CPCHAR  pcFile, INT  iLine);

/*********************************************************************************************************
  x86 处理器线程上下文相关接口
*********************************************************************************************************/

PMX_STACK  archVcpuCtxCreate(PTHREAD_START_ROUTINE  pfuncTask,
                             PVOID                  pvArg,
                             PMX_STACK              pstkTop,
                             ULONG                  ulOpt);
VOID    archVcpuCtxStart(PMX_CLASS_CPU  pcpuSw);
VOID    archVcpuCtxSwitch(PMX_CLASS_CPU  pcpuSw);
VOID    archVcpuCtxPrint(PMX_STACK  pstkTop);

/*********************************************************************************************************
  x86 处理器进程相关接口
*********************************************************************************************************/
VOID    archPartitionCreate(VOID);

/*********************************************************************************************************
  x86 处理器异常接口
*********************************************************************************************************/

VOID    archIntHandle(ULONG  ulVector, BOOL  bPreemptive);              /*  bspIntHandle 需要调用此函数 */

/*********************************************************************************************************
  x86 通用库
*********************************************************************************************************/

INT     archFindLsb(UINT32 ui32);
INT     archFindMsb(UINT32 ui32);

/*********************************************************************************************************
  x86 处理器标准底层库
*********************************************************************************************************/

INTREG  archIntDisable(VOID);
VOID    archIntEnable(INTREG  iregInterLevel);
VOID    archIntEnableForce(VOID);

VOID    archPageCopy(PVOID pvTo, PVOID pvFrom);

VOID    archReboot(INT  iRebootType, addr_t  ulStartAddress);

/*********************************************************************************************************
  x86 处理器 CACHE 操作
*********************************************************************************************************/

VOID    archCacheReset(CPCHAR     pcMachineName);
VOID    archCacheInit(CACHE_MODE  uiInstruction, CACHE_MODE  uiData, CPCHAR  pcMachineName);

/*********************************************************************************************************
  x86 处理器 MMU 操作 (可供给 bsp 使用)
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
  x86 处理器多核自旋锁操作
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
  x86 内存屏障
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
  bsp 需要提供的接口如下:
*********************************************************************************************************/
/*********************************************************************************************************
  x86 处理器中断向量判读
*********************************************************************************************************/

VOID    bspIntInit(VOID);
VOID    bspIntHandle(ULONG  ulVector);

VOID    bspIntVectorEnable(ULONG  ulVector);
VOID    bspIntVectorDisable(ULONG  ulVector);
BOOL    bspIntVectorIsEnable(ULONG  ulVector);

/*********************************************************************************************************
  CPU 定时器时钟
*********************************************************************************************************/

VOID    bspTickInit(VOID);
VOID    bspDelayUs(ULONG ulUs);
VOID    bspDelayNs(ULONG ulNs);

/*********************************************************************************************************
  系统重启操作 (ulStartAddress 参数用于调试, BSP 可忽略)
*********************************************************************************************************/

VOID    bspReboot(INT  iRebootType, addr_t  ulStartAddress);

/*********************************************************************************************************
  系统关键信息打印 (打印信息不可依赖任何操作系统 api)
*********************************************************************************************************/

VOID    bspDebugMsg(CPCHAR pcMsg);

/*********************************************************************************************************
  BSP 信息
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
  x86 处理器 MMU 操作
*********************************************************************************************************/

ULONG   bspMmuPgdMaxNum(VOID);
ULONG   bspMmuPteMaxNum(VOID);

/*********************************************************************************************************
  x86 处理器多核操作
*********************************************************************************************************/

VOID    bspMpInt(ULONG  ulCPUId);                                       /*  产生一个核间中断            */
VOID    bspCpuUp(ULONG  ulCPUId);                                       /*  启动一个 CPU                */
VOID    bspCpuUpDone(VOID);                                             /*  一个 CPU 启动完成           */

VOID    bspSecondaryCpusUp(VOID);                                       /*  启动所有的 Secondary Cpu    */
INT     bspSecondaryInit(VOID);                                         /*  Secondary Cpu 启动后的初始化*/

VOID    bspCpuDown(ULONG  ulCPUId);                                     /*  停止一个 CPU                */

VOID    bspCpuIpiVectorInstall(VOID);                                   /*  安装 IPI 向量               */

/*********************************************************************************************************
  x86 体系结构的初始化函数
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
  x86 指令宏定义
*********************************************************************************************************/

#define X86_PAUSE()                 __asm__ __volatile__ ("pause"  : : : "memory")
#define X86_HLT()                   __asm__ __volatile__ ("hlt"    : : : "memory")
#define X86_WBINVD()                __asm__ __volatile__ ("wbinvd" : : : "memory")

#endif                                                                  /*  __ARCH_X86_SUPPORT_H        */
/*********************************************************************************************************
  END
*********************************************************************************************************/

