/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: ppcMmu.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2016 年 01 月 14 日
**
** 描        述: PowerPC 体系构架 MMU 驱动.
*********************************************************************************************************/

#ifndef __ARCH_PPCMMU_H
#define __ARCH_PPCMMU_H


/*********************************************************************************************************
  FUNCPTR
*********************************************************************************************************/

#ifdef __cplusplus
typedef INT         (*FUNCPTR)(...);                                    /*  function returning int      */
typedef off_t       (*OFFTFUNCPTR)(...);                                /*  function returning off_t    */
typedef size_t      (*SIZETFUNCPTR)(...);                               /*  function returning size_t   */
typedef ssize_t     (*SSIZETFUNCPTR)(...);                              /*  function returning ssize_t  */
typedef LONG        (*LONGFUNCPTR)(...);                                /*  function returning long     */
typedef ULONG       (*ULONGFUNCPTR)(...);                               /*  function returning ulong    */
typedef VOID        (*VOIDFUNCPTR)(...);                                /*  function returning void     */
typedef PVOID       (*PVOIDFUNCPTR)(...);                               /*  function returning void *   */
typedef BOOL        (*BOOLFUNCPTR)(...);                                /*  function returning bool     */

#else
typedef INT         (*FUNCPTR)();                                       /*  function returning int      */
typedef INT64       (*OFFTFUNCPTR)();                                   /*  function returning off_t    */
typedef size_t      (*SIZETFUNCPTR)();                                  /*  function returning size_t   */
typedef LONG     (*SSIZETFUNCPTR)();                                 /*  function returning ssize_t  */
typedef LONG        (*LONGFUNCPTR)();                                   /*  function returning long     */
typedef ULONG       (*ULONGFUNCPTR)();                                  /*  function returning ulong    */
typedef VOID        (*VOIDFUNCPTR)();                                   /*  function returning void     */
typedef PVOID       (*PVOIDFUNCPTR)();                                  /*  function returning void *   */
typedef BOOL        (*BOOLFUNCPTR)();                                   /*  function returning bool     */
#endif                                                                  /*  _cplusplus                  */

/*********************************************************************************************************
  MMU 转换条目类型
*********************************************************************************************************/
#if !defined(__ASSEMBLY__) && !defined(ASSEMBLY)

typedef UINT32  MX_PGD_TRANSENTRY;                                      /*  页目录类型                  */
typedef UINT32  MX_PMD_TRANSENTRY;                                      /*  中间页目录类型              */

typedef union {
    struct {
        UINT        PTE_uiRPN       : 20;                               /*  物理页号                    */
        UINT        PTE_ucReserved1 :  3;                               /*  保留                        */
        UINT        PTE_bRef        :  1;                               /*  引用位                      */
        UINT        PTE_bChange     :  1;                               /*  修改位                      */
        UINT        PTE_ucWIMG      :  4;                               /*  内存和 CACHE 属性位         */
        UINT        PTE_bReserved2  :  1;                               /*  保留                        */
        UINT        PTE_ucPP        :  2;                               /*  页保护权限位                */
    };                                                                  /*  通用的 PPC32 PTE            */
    UINT32          PTE_uiValue;                                        /*  值                          */

    struct {
        /*
         * 以下值用于 TLB MISS 时重装到 MAS2 MAS3 寄存器
         */
        UINT        MAS3_uiRPN      : 20;                               /*  物理页号                    */

        UINT        MAS3_ucReserved0:  2;                               /*  保留                        */

        /*
         * 以下用户属性用于 TLB MISS 时重装到 MAS2 寄存器
         *
         * MAS2 寄存器还有 X0 X1 G E 位, G 和 E 位固定为 0
         * X0 X1 位由 MAS4 寄存器的 X0D X1D 自动重装
         */
#define MAS3_bValid      MAS3_bUserAttr0                                /*  是否有效                    */
#define MAS3_bWT         MAS3_bUserAttr1                                /*  是否写穿透                  */
#define MAS3_bUnCache    MAS3_bUserAttr2                                /*  是否不可 Cache              */
#define MAS3_bMemCoh     MAS3_bUserAttr3                                /*  是否多核内存一致性          */

        UINT        MAS3_bUserAttr0 :  1;                               /*  用户属性 0                  */
        UINT        MAS3_bUserAttr1 :  1;                               /*  用户属性 1                  */
        UINT        MAS3_bUserAttr2 :  1;                               /*  用户属性 2                  */
        UINT        MAS3_bUserAttr3 :  1;                               /*  用户属性 3                  */

        UINT        MAS3_bUserExec  :  1;                               /*  用户可执行权限              */
        UINT        MAS3_bSuperExec :  1;                               /*  管理员可执行权限            */

        UINT        MAS3_bUserWrite :  1;                               /*  用户可写权限                */
        UINT        MAS3_bSuperWrite:  1;                               /*  管理员可写权限              */

        UINT        MAS3_bUserRead  :  1;                               /*  用户可读权限                */
        UINT        MAS3_bSuperRead :  1;                               /*  管理员可读权限              */
    };                                                                  /*  E500 PTE                    */
    UINT32          MAS3_uiValue;                                       /*  值                          */
} MX_PTE_TRANSENTRY;                                                    /*  页表条目类型                */
#endif
/*********************************************************************************************************
  mmu 执行功能
*********************************************************************************************************/

typedef MX_PGD_TRANSENTRY  *(*PGDFUNCPTR)();
typedef MX_PMD_TRANSENTRY  *(*PMDFUNCPTR)();
typedef MX_PTE_TRANSENTRY  *(*PTEFUNCPTR)();

typedef struct {
    ULONG                    MMUOP_ulOption;                            /*  MMU 选项                    */
#define MX_VMM_MMU_FLUSH_TLB_MP     0x01                                /*  每一个核是否都要清快表      */

    FUNCPTR                  MMUOP_pfuncMemInit;                        /*  初始化内存, (页表和目录项)  */
    FUNCPTR                  MMUOP_pfuncGlobalInit;                     /*  初始化全局映射关系          */

    PGDFUNCPTR               MMUOP_pfuncPGDAlloc;                       /*  创建 PGD 空间               */
    VOIDFUNCPTR              MMUOP_pfuncPGDFree;                        /*  释放 PGD 空间               */
    PMDFUNCPTR               MMUOP_pfuncPMDAlloc;                       /*  创建 PMD 空间               */
    VOIDFUNCPTR              MMUOP_pfuncPMDFree;                        /*  释放 PMD 空间               */

    PTEFUNCPTR               MMUOP_pfuncPTEAlloc;                       /*  创建 PTE 空间               */
    VOIDFUNCPTR              MMUOP_pfuncPTEFree;                        /*  释放 PTE 空间               */

    BOOLFUNCPTR              MMUOP_pfuncPGDIsOk;                        /*  PGD 入口项是否正确          */
    BOOLFUNCPTR              MMUOP_pfuncPMDIsOk;                        /*  PMD 入口项是否正确          */

    BOOLFUNCPTR              MMUOP_pfuncPTEIsOk;                        /*  PTE 入口项是否正确          */

    PGDFUNCPTR               MMUOP_pfuncPGDOffset;                      /*  通过地址获得指定 PGD 表项   */
    PMDFUNCPTR               MMUOP_pfuncPMDOffset;                      /*  通过地址获得指定 PMD 表项   */
    PTEFUNCPTR               MMUOP_pfuncPTEOffset;                      /*  通过地址获得指定 PTE 表项   */

    FUNCPTR                  MMUOP_pfuncPTEPhysGet;                     /*  通过 PTE 条目获取物理地址   */

    ULONGFUNCPTR             MMUOP_pfuncFlagGet;                        /*  获得页面标志                */
    FUNCPTR                  MMUOP_pfuncFlagSet;                        /*  设置页面标志 (当前未使用)   */

    VOIDFUNCPTR              MMUOP_pfuncMakeTrans;                      /*  设置页面转换关系描述符      */
    VOIDFUNCPTR              MMUOP_pfuncMakeCurCtx;                     /*  激活当前的页表转换关系      */
    VOIDFUNCPTR              MMUOP_pfuncInvalidateTLB;                  /*  无效 TLB 表                 */

    VOIDFUNCPTR              MMUOP_pfuncSetEnable;                      /*  启动 MMU                    */
    VOIDFUNCPTR              MMUOP_pfuncSetDisable;                     /*  关闭 MMU                    */
} MX_MMU_OP;

typedef MX_MMU_OP           *PMX_MMU_OP;
extern  MX_MMU_OP            _G_mmuOpLib;                               /*  MMU 操作函数集              */


/*********************************************************************************************************
  mmu 信息
*********************************************************************************************************/

typedef struct __mx_mmu_context {
//    MX_VMM_AREA              MMUCTX_vmareaVirSpace;                     /*  虚拟地址空间反查表          */
    MX_PGD_TRANSENTRY       *MMUCTX_pgdEntry;                           /*  PGD 表入口地址              */                                                                 /* !MX_CFG_VMM_L4_HYPERVISOR_EN */
} MX_MMU_CONTEXT;
typedef MX_MMU_CONTEXT      *PMX_MMU_CONTEXT;
/*
 * *****************************************************************************
 */
VOID   ppcMmuInit(MX_MMU_OP *pmmuop, CPCHAR  pcMachineName);

ULONG  ppcMmuPteMissHandle(addr_t  ulAddr);
INT    ppcMmuPtePreLoad(addr_t  ulAddr);

UINT32 ppcMmuGetSRR1(VOID);
UINT32 ppcMmuGetDSISR(VOID);


#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/
