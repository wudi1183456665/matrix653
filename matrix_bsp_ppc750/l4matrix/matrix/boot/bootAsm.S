;/*********************************************************************************************************
;**
;**                                    中国软件开源组织
;**
;**                                嵌入式 L4 微内核操作系统
;**
;**                                SylixOS(TM)  MX : matrix
;**
;**                               Copyright All Rights Reserved
;**
;**--------------文件信息--------------------------------------------------------------------------------
;**
;** 文   件   名: bootAsm750.S
;**
;** 创   建   人:
;**
;** 文件创建日期: 2018 年 12 月 17 日
;**
;** 描        述: 必须使用汇编实现的功能函数.
;*********************************************************************************************************/
#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <../arch/arch_assembler.h>

;/*********************************************************************************************************
;  声明外部函数和导出全局函数
;*********************************************************************************************************/

    .global ppcMmuEnable
    //EXPORT_LABEL(ppcMmuDisable)
	.global ppcMmuDisable

    .global ppcMmuInvalidateTLBNr
    .global ppcMmuInvalidateTLBEA

    .global ppcMmuSetSDR1
    .global ppcMmuSetSR

    .global ppcMmuGetSRR1
    .global ppcMmuGetDSISR
    .global ppcHashPageTblPteSet

    .global ppc750ICacheEnable
    .global ppc750DCacheEnable

    .global ppc750ICacheDisable
    .global ppc750DCacheDisable

    .global ppc60xICacheInvalidate

    .global ppc60xDCacheInvalidate
    .global ppc60xDCacheClear
    .global ppc60xDCacheFlush

    .global ppc750ICacheInvalidateAll

    .global ppc750DCacheInvalidateAll
    .global ppc60xDCacheFlushAll
    .global ppc60xDCacheClearAll

    .global ppc750BranchPredictionDisable
    .global ppc60xBranchPredictionEnable
    .global ppc750BranchPredictorInvalidate

    IMPORT_LABEL(G_pucPpcCacheReadBuffer)

;/*********************************************************************************************************
;  Section .boot.text, This Matrix boot time used code
;*********************************************************************************************************/

    //FILE_BEGIN()
    //.code 32
    .section .boot.text, "ax"
    .balign 8;

;/*********************************************************************************************************
;  使能 MMU
;*********************************************************************************************************/

FUNC_DEF(ppcMmuEnable)
    MFMSR   R3
    ISYNC
    ORI     R3 , R3 , ARCH_PPC_MSR_DR | ARCH_PPC_MSR_IR
    ISYNC
    MTMSR   R3
    ISYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  禁能 MMU
;*********************************************************************************************************/
FUNC_DEF(ppcMmuDisable)
    MFMSR   R3
    ISYNC
    ARCH_PPC_MMU_MASK(R3 , R3)
    ISYNC
    MTMSR   R3
    ISYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  无效所有TLB
;*********************************************************************************************************/

FUNC_DEF(ppcMmuInvalidateTLBNr)
    ISYNC
    MTCTR   R3                                                          ;/*  CTR = R3                    */
    XOR     R4 , R4 , R4                                                ;/*  R4 = 0                      */

LINE_LABEL(tlbloop)
    ISYNC
    TLBIE   R4
    EIEIO
    TLBSYNC
    SYNC
    ADDI    R4 , R4 , 0x1000                                            ;/*  Increment bits 15-19        */
    BDNZ    tlbloop                                                     ;/*  Dec CTR, branch if CTR != 0 */
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  无效指定 EA 的 TLB
;*********************************************************************************************************/

FUNC_DEF(ppcMmuInvalidateTLBEA)
    SYNC
    TLBIE   R3
    EIEIO
    TLBSYNC
    SYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  设置 PTE，并无效 TLB
;  VOID  ppcHashPageTblPteSet(PTE    *pPte,
;                             UINT32  uiWord0,
;                             UINT32  uiWord1,
;                             UINT32  uiEffectiveAddr);
;*********************************************************************************************************/

FUNC_DEF(ppcHashPageTblPteSet)
    SYNC
    STW     R5, 4(R3)
    EIEIO
    STW     R4, 0(R3)
    SYNC
    TLBIE   R6
    EIEIO
    TLBSYNC
    SYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  设置 SDR1 寄存器
;*********************************************************************************************************/

FUNC_DEF(ppcMmuSetSDR1)
    SYNC
    MTSPR   SDR1 , R3
    ISYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  设置段寄存器
;  VOID  ppcMmuSetSR(UINT32  uiSRn, UINT32  uiValue);
;*********************************************************************************************************/

FUNC_DEF(ppcMmuSetSR)
    SYNC
    RLWINM  R3 , R3 , 28 , 0 , 3
    ISYNC
    MTSRIN  R4 , R3
    ISYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  获得 SRR1 寄存器
;*********************************************************************************************************/

FUNC_DEF(ppcMmuGetSRR1)
    MFSPR   R3 , SRR1
    ISYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  获得 DSISR 寄存器
;*********************************************************************************************************/

FUNC_DEF(ppcMmuGetDSISR)
    MFSPR   R3 , DSISR
    ISYNC
    BLR
    FUNC_END()

    FILE_END()

;/*********************************************************************************************************
;  定义
;*********************************************************************************************************/

#define PPC_PAGE_ALIGN_SIZE         4096
#define PPC_PAGE_ALIGN_SHIFT        12              /*  Page size == 2**12                              */

#if (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC604__)
#define PPC_DCACHE_SIZE             32768
#else
#ifdef __SYLIXOS_PPC83XX__
#define PPC_DCACHE_SIZE             (16384 * 3)     /*  32K cache plus need to flush 3/2 times          */
#else
#define PPC_DCACHE_SIZE             16384
#endif
#endif  /* __SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC604__ */

#define PPC_CACHE_BLOCK_SIZE        ARCH_PPC_CACHE_ALIGN_SIZE

#define PPC_CACHE_OP_DISABLE        0
#define PPC_CACHE_OP_CLEAR_ALL      1
#define PPC_CACHE_OP_STORE_ALL      2
#define PPC_CACHE_OP_INVALID_ALL    3

    FILE_BEGIN()

;/*********************************************************************************************************
;  使能 ICACHE
;*********************************************************************************************************/

FUNC_DEF(ppc60xICacheEnable)

    MFSPR   R4 , HID0

    ;/* enable and invalidate the Instruction cache */

    ANDI.   R5 , R4 , ARCH_PPC_HID0_ICE             ;/*  if insn cache already                           */
    BNE     cache60xRetOK                           ;/*  enabled just return OK                          */

    ORI     R4 , R4 , ARCH_PPC_HID0_ICE | ARCH_PPC_HID0_ICFI

#if ((__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC603__) || (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPCEC603__))
    RLWINM  R5 , R4 , 0 , ARCH_PPC_HID0_BIT_ICFI + 1 , ARCH_PPC_HID0_BIT_ICFI - 1
#endif

    ;/*
     ;* The setting of the instruction cache enable (ICE) bit must be
     ;* preceded by an isync instruction to prevent the cache from being
     ;* enabled or disabled while an instruction access is in progress.
     ;* XXX TPR to verify.
     ;*/

    ISYNC                                           ;/*  Synchronize for ICE enable                      */
    MTSPR   HID0 , R4                               ;/*  Enable Instr Cache & Inval cache                */
#if ((__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC603__) || (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPCEC603__))
    MTSPR   HID0 , R5                               ;/*  using 2 consec instructions                     */
    ISYNC                                           ;/*  PPC603 recommendation                           */
#else
#ifdef __SYLIXOS_PPC745X__
    ISYNC                                           ;/*  must immediately follow mtspr HID0              */
#else       /* __SYLIXOS_PPC745X__ */
    SYNC                                            ;/*  sync here makes no sense, but isync
                                                     ;*  does not seem to work properly on
                                                     ;*  any of 604e, 750CX, 750FX, 7400.
                                                     ;*  See also SPR 8976.
                                                     ;*                                                  */
#endif      /* __SYLIXOS_PPC745X__ */
#endif

    B       cache60xRetOK                           ;/*  return OK                                       */

    FUNC_END()

;/*********************************************************************************************************
;  使能 DCACHE
;*********************************************************************************************************/

FUNC_DEF(ppc750DCacheEnable)
    MFSPR   R4 , HID0

    ANDI.   R5 , R4 , ARCH_PPC_HID0_DCE             ;/*  if data cache already enabled                   */
    BNE     cache60xRetOK                           ;/*  then exit with OK                               */

    ORI     R4 , R4 , ARCH_PPC_HID0_DCE | ARCH_PPC_HID0_DCFI;/*  set DCE & DCFI bit                      */

#if ((__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC603__) || (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPCEC603__))
    RLWINM  R5 , R4 , 0 , ARCH_PPC_HID0_BIT_DCFI + 1 , ARCH_PPC_HID0_BIT_DCFI - 1
#endif

    ;/*
     ;* The setting of the data cache enable (DCE) bit must be
     ;* preceded by a sync instruction to prevent the cache from
     ;* being enabled or disabled during a data access.
     ;*/

#ifdef __SYLIXOS_PPC745X__
    DSSALL
#endif      /* __SYLIXOS_PPC745X__ */
    SYNC                                            ;/*  Synchronize for DCE enable                      */
    MTSPR   HID0 , R4                               ;/*  Enable Data Cache & Inval cache                 */
#if ((__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC603__) || (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPCEC603__))
    MTSPR   HID0 , R5                               ;/*  using 2 consecutive instructions                */
                                                    ;/*  PPC603 recommendation                           */
#endif
    SYNC
#ifdef __SYLIXOS_PPC745X__
    ISYNC
#endif      /* __SYLIXOS_PPC745X__ */

    B       cache60xRetOK                           ;/*  return OK                                       */

    FUNC_END()

;/*********************************************************************************************************
;  禁能 ICACHE
;*********************************************************************************************************/

FUNC_DEF(ppc60xICacheDisable)

    MFSPR   R4 , HID0

    ;/* disable instruction cache */
    RLWINM. R5 , R4 , 0 , ARCH_PPC_HID0_BIT_ICE , ARCH_PPC_HID0_BIT_ICE
    BEQ     cache60xRetOK                           ;/*  return OK if cache already off                  */

#if ((__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC603__) || (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPCEC603__))
    RLWINM  R4 , R4 , 0 , ARCH_PPC_HID0_BIT_ICFI + 1 , ARCH_PPC_HID0_BIT_ICFI - 1
    RLWINM  R4 , R4 , 0 , ARCH_PPC_HID0_BIT_ICE  + 1 , ARCH_PPC_HID0_BIT_ICE  - 1
#else
    ORI     R5 , R4 , ARCH_PPC_HID0_ICFI
    RLWINM  R4 , R5 , 0 , ARCH_PPC_HID0_BIT_ICE + 1 , ARCH_PPC_HID0_BIT_ICE - 1
                                                    ;/*  but leave ARCH_PPC_HID0_ICFI on                 */
#endif

    ISYNC                                           ;/*  Synchronize for ICE disable                     */
#if ((__SYLIXOS_PPC_CPU__ != __SYLIXOS_PPC603__) && (__SYLIXOS_PPC_CPU__ != __SYLIXOS_PPCEC603__))
    MTSPR   HID0 , R5                               ;/*  Invalidate Instr Cache                          */
#ifdef  __SYLIXOS_PPC745X__
    ISYNC
#endif      /* __SYLIXOS_PPC745X__ */
#endif
    MTSPR   HID0 , R4                               ;/*  using 2 consec instructions                     */
    ISYNC                                           ;/*  PPC603 recommendation                           */
#if ((__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC604__) && !defined(__SYLIXOS_PPC745X__))
    SYNC                                            ;/*  It makes no sense for sync to be
                                                     ;*  needed here, and it is not required
                                                     ;*  for 745x per the User's Manual, but
                                                     ;*  isync alone does not seem to work
                                                     ;*  properly on any of 604e, 750CX,
                                                     ;*  750FX, 7400.  See also SPR 8976.
                                                     ;*                                                  */
#endif      /* __SYLIXOS_PPC604__ && !__SYLIXOS_PPC745X__ */

    B       cache60xRetOK                           ;/*  return OK                                       */

    FUNC_END()

;/*********************************************************************************************************
;  禁能 DCACHE
;*********************************************************************************************************/

FUNC_DEF(ppc60xDCacheDisable)
    LI      R8 , PPC_CACHE_OP_DISABLE
    B       ppc60xDCacheAllOps
    FUNC_END()

;/*********************************************************************************************************
;  D-CACHE 所有数据回写并无效
;*********************************************************************************************************/

FUNC_DEF(ppc60xDCacheClearAll)
    LI      R8 , PPC_CACHE_OP_CLEAR_ALL
    B       ppc60xDCacheAllOps
    FUNC_END()

;/*********************************************************************************************************
;  D-CACHE 所有数据回写
;*********************************************************************************************************/

FUNC_DEF(ppc60xDCacheFlushAll)
    LI      R8 , PPC_CACHE_OP_STORE_ALL
    B       ppc60xDCacheAllOps
    FUNC_END()

;/*********************************************************************************************************
;  D-CACHE 所有数据无效
;*********************************************************************************************************/

FUNC_DEF(ppc60xDCacheInvalidateAll)
    LI      R8 , PPC_CACHE_OP_INVALID_ALL

;/*********************************************************************************************************
;  D-CACHE 所有数据操作
;*********************************************************************************************************/

FUNC_DEF(ppc60xDCacheAllOps)
    MFSPR   R4 , HID0

    ;/* if cache already disabled, just return OK */

    RLWINM. R5 , R4 , 0 , ARCH_PPC_HID0_BIT_DCE , ARCH_PPC_HID0_BIT_DCE
    BEQ     cache60xRetOK

    ;/* "invalidate all" skips over loops */
    CMPWI   R8 , PPC_CACHE_OP_INVALID_ALL
    BEQ     cache60xInvalidateD

#if (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC604__)
    ;/* set up cache line count in R6:  32KB => 1024 */
    LI      R6 , PPC_DCACHE_SIZE / PPC_CACHE_BLOCK_SIZE

    MFSPR   R5 , PVR
    RLWINM  R5 , R5 , 16 , 16 , 31                  ;/*  extract MS half of PVR                          */

    ;/* All 744x/745x/8641 fall through to cache60xDataDisableLTLB */
#ifndef  __SYLIXOS_PPC745X__
#ifdef ARCH_PPC604_USE_DCFA
    ;/*
     ;* Recognize processors which implement HID0[DCFA], which enables the
     ;* 8-way PLRU cache to be flushed by loading one cache worth of data.
     ;*/
    CMPLWI  R5 , ARCH_PPC_PVR_PPC750_U
    BEQ     cache60xDataDisableDCFA

    CMPLWI  R5 , ARCH_PPC_PVR_PPC750FX_0_U          ;/*  lowest PPC750FX                                 */
    BEQ     cache60xDataDisableDCFA

    BLT     notPPC750FX
    CMPLWI  R5 , ARCH_PPC_PVR_PPC750FX_f_U          ;/*  highest PPC750FX                                */
    BLE     cache60xDataDisableDCFA
LINE_LABEL(notPPC750FX)
    CMPLWI  R5 , ARCH_PPC_PVR_MPC755_U
    BEQ     cache60xDataDisableDCFA

    CMPLWI  R5 , ARCH_PPC_PVR_MPC745_U
    BEQ     cache60xDataDisableDCFA
#endif      /* ARCH_PPC604_USE_DCFA */

    ;/*
     ;* Recognize processors which implement true LRU, or modified PLRU.
     ;* These caches also require loading only one cache worth of data,
     ;* but there is no DCFA bit to manage.
     ;*/
    CMPLWI  R5 , ARCH_PPC_PVR_PPC604E_U
    BEQ     cache60xDataDisableLTLB

    CMPLWI  R5 , ARCH_PPC_PVR_PPC604R_U
    BEQ     cache60xDataDisableLTLB

    CMPLWI  R5 , ARCH_PPC_PVR_PPC604_U
    BEQ     cache60xDataDisableLTLB

    ;/*
     ;* Remaining cases are 7400/7410, and unrecognized PVR (including DCFA
     ;* processors if USE_DCFA is not selected).  7400/7410 h/w flush will
     ;* not use the count value in R6; assume others have a 750-style PLRU
     ;* cache.  To cover optimized replacement which uses invalid lines
     ;* first, 7xx manuals say count must be 1.5 * total # of lines in
     ;* cache:  32KB => 1536.
     ;*/
    LI      R6 , (3 * PPC_DCACHE_SIZE) / (2 * PPC_CACHE_BLOCK_SIZE)

    RLWINM  R5 , R5 , 0 , 17 , 31                   ;/*  mask off bit 0x8000                             */
    CMPLWI  R5 , ARCH_PPC_PVR_MPC7400_U             ;/*  MPC7400 or MPC7410                              */
    BNE     cache60xDataDisableLTLB

    ;/*
     ;* Since 7400/7410 h/w flush invalidates the cache, don't use it if
     ;* only a "store" was requested.  These processors also support DCFA,
     ;* so use it if selected -- after re-establishing the proper R6 value.
     ;*/
    CMPWI   R8 , PPC_CACHE_OP_STORE_ALL

#ifdef ARCH_PPC604_USE_DCFA
    LI      R6 , PPC_DCACHE_SIZE / PPC_CACHE_BLOCK_SIZE
    BEQ     cache60xDataDisableDCFA
#else       /* ARCH_PPC604_USE_DCFA */
    BEQ     cache60xDataDisableLTLB
#endif      /* ARCH_PPC604_USE_DCFA */

    ;/*
     ;* 7400 and 7410 (but not 744x or 745x) have a hardware L1 flush.
     ;* Code sequence described in sec. 3.5.2 "Data Cache Hardware Flush
     ;* Parameter in MSSCR0" of the MPC7400 and MPC7410 User's Manuals.
     ;*/

    .long   0x7e00066c                              ;/*  dssall                                          */
    SYNC
    MFSPR   R5 , MSSCR0
    ORIS    R5 , R5 , ARCH_PPC_MSSCR0_DL1HWF_U
    MTSPR   MSSCR0 , R5
LINE_LABEL(waitForL1Hw)
    MFSPR   R5 , MSSCR0
    RLWINM. R5 , R5 , 0 , ARCH_PPC_MSSCR0_BIT_DL1HWF , ARCH_PPC_MSSCR0_BIT_DL1HWF
    BNE     waitForL1Hw
    SYNC
    B       cache60xInvalidateD                     ;/*  DCFI unneeded, but harmless                     */

#ifdef ARCH_PPC604_USE_DCFA
LINE_LABEL(cache60xDataDisableDCFA)
    ;/*
     ;* Set HID0[DCFA] for 7xx.  R4 already contains HID0, and interrupts
     ;* are already disabled.  This is the officially recommended method
     ;* for all 7xx.
     ;*/
    ORI     R5 , R4 , ARCH_PPC_HID0_DCFA
    MTSPR   HID0 , R5
#endif      /* ARCH_PPC604_USE_DCFA */
#endif      /* __SYLIXOS_PPC745X__ */

LINE_LABEL(cache60xDataDisableLTLB)
    ;/*
     ;* Interrupts have been disabled, and HID0[DCFA] has been set if
     ;* required.  Former contents of MSR and HID0 are in R3 and R4
     ;* respectively, and will be restored even if unchanged.  This also
     ;* works for 7450 since its PLRU always acts as if DCFA were set.
     ;*/

#else       /* __SYLIXOS_PPC604__ */
    ;/*
     ;* All supported __SYLIXOS_PPCEC603__ (incl MPC82xx) have 16KB or smaller DCache,
     ;* and no DCFA or hardware flush facility.
     ;*/
    LI      R6 , PPC_DCACHE_SIZE / PPC_CACHE_BLOCK_SIZE   ;/*  16KB => 512                               */
#endif      /* __SYLIXOS_PPC604__ */

    ;/*
     ;* R6 contains the count of cache lines to be fetched & flushed.
     ;* Convert to a count of pages covered, and fetch a word from
     ;* each page to ensure that all addresses involved are in
     ;* the TLB so that reloads do not disrupt the flush loop.
     ;* A simple shift without round-up is sufficient because
     ;* the R6 value is always a multiple of the shift count.
     ;*/
    SRWI    R5 , R6 , PPC_PAGE_ALIGN_SHIFT - ARCH_PPC_CACHE_ALIGN_SHIFT
    MTSPR   CTR , R5
    LIS     R9 , HIADJ(G_pucPpcCacheReadBuffer)
    LWZ     R9 , LO(G_pucPpcCacheReadBuffer)(R9)
    SUBI    R5 , R9 , PPC_PAGE_ALIGN_SIZE

    ;/*
     ;* There might be a page boundary between here and the end of
     ;* the function, so make sure both pages are in the I-TLB.
     ;*/
    B       cache60xDisableLoadItlb
LINE_LABEL(cache60xDisableLoadDtlb)
    LBZU    R7 , PPC_PAGE_ALIGN_SIZE(R5)
    BDNZ    cache60xDisableLoadDtlb
    SYNC                                            ;/*  ensure loads, hence TLB loads, complete         */

    ;/*
     ;* Now load up the cache from the buffer
     ;*/
    MTSPR   CTR , R6                                ;/*  load CTR with the count of cache lines          */

    ;/* load up R5 with the buffer address minus one cache block size */
    SUBI    R5 , R9 , PPC_CACHE_BLOCK_SIZE

LINE_LABEL(cache60xDisableLoad)
    LBZU    R7 , PPC_CACHE_BLOCK_SIZE(R5)           ;/*  cast out old line if modified                   */
    BDNZ    cache60xDisableLoad                     ;/*  repeat for all sets and ways                    */
    SYNC                                            ;/*  ensure loads complete                           */

    MTSPR   CTR , R6                                ;/*  reload CTR; base still in R9                    */

    ;/*
     ;* All modified lines not in the buffer range have been cast out,
     ;* but any within the range would not have been affected by the
     ;* above (load) loop.  Fix by running through the range with dcbst.
     ;*/
LINE_LABEL(cache60xDisableStore)
    DCBST   0 , R9                                  ;/*  store line if "modified"                        */
    ADDI    R9 , R9 , PPC_CACHE_BLOCK_SIZE          ;/*  point to next cache line                        */
    BDNZ    cache60xDisableStore                    ;/*  repeat for all sets and ways                    */

    ;/* "store all" operation is complete */
    CMPWI   R8 , PPC_CACHE_OP_STORE_ALL
    BEQ     cache60xAllOpsDone

LINE_LABEL(cache60xInvalidateD)
    ;/* remaining cases must invalidate the cache */

    ORI     R5 , R4 , ARCH_PPC_HID0_DCFI
    RLWINM  R4 , R5 , 0 , ARCH_PPC_HID0_BIT_DCFI + 1 , ARCH_PPC_HID0_BIT_DCFI - 1

    ;/* registers are now set to invalidate but leave enabled */
    CMPWI   R8 , PPC_CACHE_OP_DISABLE
    BNE     cache60xFlashInv

    ;/* for "disable", second write to HID0 turns off both DCE and DCFI */
    RLWINM  R4 , R4 , 0 , ARCH_PPC_HID0_BIT_DCE + 1 , ARCH_PPC_HID0_BIT_DCE - 1

LINE_LABEL(cache60xFlashInv)
#ifdef __SYLIXOS_PPC745X__
    DSSALL
#endif      /* __SYLIXOS_PPC745X__ */
    SYNC                                            ;/*  Synchronize for invalidate                      */
    MTSPR   HID0 , R5                               ;/*  Invalidate cache                                */
#ifdef __SYLIXOS_PPC745X__
    SYNC
    ISYNC
#endif      /* __SYLIXOS_PPC745X__ */
    MTSPR   HID0 , R4                               ;/*  Turn off DCFI, retain or clear DCE              */
    SYNC
#ifdef __SYLIXOS_PPC745X__
    ISYNC
#endif      /* __SYLIXOS_PPC745X__ */

LINE_LABEL(cache60xAllOpsDone)
    B       cache60xRetOK                           ;/*  return OK                                       */

;/*
 ;* This label and branch must not precede the "MTSPR HID0 , R4"
 ;* in cache60xFlashInv.
 ;*/
LINE_LABEL(cache60xDisableLoadItlb)
    B       cache60xDisableLoadDtlb

    FUNC_END()

;/*********************************************************************************************************
;  CACHE 操作返回成功
;*********************************************************************************************************/

FUNC_DEF(cache60xRetOK)
    SYNC                                            ;/*  SYNC for good measure (multiprocessor?)         */
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  I-CACHE 所有数据无效
;*********************************************************************************************************/

FUNC_DEF(ppc60xICacheInvalidateAll)
    MFSPR   R4 , HID0

    ;/* disable instruction cache */
    RLWINM. R5 , R4 , 0 , ARCH_PPC_HID0_BIT_ICE , ARCH_PPC_HID0_BIT_ICE
    BEQ     ICacheInvalidDone                       ;/*  return OK if cache already off                  */

    ORI     R5 , R4 , ARCH_PPC_HID0_ICFI            ;/*  set the ICFI bit                                */
    RLWINM  R4 , R4 , 0 , ARCH_PPC_HID0_BIT_ICFI + 1 , ARCH_PPC_HID0_BIT_ICFI - 1

    ISYNC                                           ;/*  Synchronize for IC invalidate                   */
    MTSPR   HID0 , R5                               ;/*  Invalidate Instr Cache                          */
#if ((__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC603__) || (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPCEC603__))
    MTSPR   HID0 , R4                               ;/*  using 2 consec instructions                     */
#endif
    ISYNC                                           ;/*  PPC603 recommendation                           */
#if ((__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC604__) && !defined(__SYLIXOS_PPC745X__))
    SYNC                                            ;/*  It makes no sense for sync to be
                                                     ;*  needed here, and it is not required
                                                     ;*  for 745x per the User's Manual, but
                                                     ;*  isync alone does not seem to work
                                                     ;*  properly on any of 604e, 750CX,
                                                     ;*  750FX, 7400.  See also SPR 8976.
                                                     ;*                                                  */
#endif      /* __SYLIXOS_PPC604__ && !__SYLIXOS_PPC745X__ */

LINE_LABEL(ICacheInvalidDone)
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  无效 ICACHE R3=void *start; R4=void *end; R5=cache line size
;*********************************************************************************************************/

FUNC_DEF(ppc60xICacheInvalidate)
    ICBI    R0 , R3
    ADD     R3 , R3 , R5
    CMPLW   R3 , R4
    BLT     ppc60xICacheInvalidate

    ISYNC
    SYNC                                            ;/*  SYNC for good measure (multiprocessor?)         */
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  将指定虚拟地址的 DCACHE 无效  R3=void *start; R4=void *end; R5=cache line size
;*********************************************************************************************************/

FUNC_DEF(ppc60xDCacheInvalidate)
#if (__SYLIXOS_PPC_CPU__ == __SYLIXOS_PPC603__)
    DCBF    R0 , R3                                 ;/*  PPC603 User's Manual says "don't use dcbi"      */
#else
    DCBI    R0 , R3
#endif
    ADD     R3 , R3 , R5
    CMPLW   R3 , R4
    BLT     ppc60xDCacheInvalidate

    SYNC                                            ;/*  XXX confirm needed XXX                          */
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  将指定虚拟地址的 DCACHE 回写并无效  R3=void *start; R4=void *end; R5=cache line size
;*********************************************************************************************************/

FUNC_DEF(ppc60xDCacheClear)
    DCBF    R0 , R3                                 ;/*  PPC "flush" == SylixOS "clear"                  */
    ADD     R3 , R3 , R5
    CMPLW   R3 , R4
    BLT     ppc60xDCacheClear

    SYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  将指定虚拟地址的 DCACHE 回写  R3=void *start; R4=void *end; R5=cache line size
;*********************************************************************************************************/

FUNC_DEF(ppc60xDCacheFlush)
    DCBST   R0 , R3                                 ;/*  PPC "store" == SylixOS "flush"                  */
    ADD     R3 , R3 , R5
    CMPLW   R3 , R4
    BLT     ppc60xDCacheFlush

    SYNC                                            ;/*  Ensure all stores complete                      */
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  禁能分支预测
;*********************************************************************************************************/

FUNC_DEF(ppc60xBranchPredictionDisable)
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  使能分支预测
;*********************************************************************************************************/

FUNC_DEF(ppc60xBranchPredictionEnable)
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  无效分支预测
;*********************************************************************************************************/

FUNC_DEF(ppc60xBranchPredictorInvalidate)
    BLR
    FUNC_END()

    FILE_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/

