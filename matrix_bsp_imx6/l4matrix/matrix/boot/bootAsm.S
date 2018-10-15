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
;** 文   件   名: bootAsm.S
;**
;** 创   建   人: Wang.Dongfang (王东方)
;**
;** 文件创建日期: 2018 年 02 月 08 日
;**
;** 描        述: 必须使用汇编实现的功能函数.
;*********************************************************************************************************/
#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/arch_assembler.h>

;/*********************************************************************************************************
;  声明外部函数和导出全局函数
;*********************************************************************************************************/

    EXPORT_LABEL(bootArmMmuEnable)
    EXPORT_LABEL(bootArmMmuDisable)
    EXPORT_LABEL(bootArmMmuSetTTBase0)
    EXPORT_LABEL(bootArmMmuSetTTBase1)
    EXPORT_LABEL(bootArmMmuSetDomain)
    EXPORT_LABEL(bootArmMmuV7SetTTBCR)
    EXPORT_LABEL(bootArmMmuSetProcessId)
    EXPORT_LABEL(bootArmMmuInvalidateTLB)
    EXPORT_LABEL(bootArmControlFeatureDisable)
    EXPORT_LABEL(bootArmAuxControlFeatureEnable)
    EXPORT_LABEL(bootArmAuxControlFeatureDisable)
    EXPORT_LABEL(bootArmDCacheV7ClearAll)
    EXPORT_LABEL(bootArmDCacheV7InvalidateAll)
    EXPORT_LABEL(bootArmDCacheV7Disable)
    EXPORT_LABEL(bootArmDCacheEnable)
    EXPORT_LABEL(bootArmICacheInvalidateAll)
    EXPORT_LABEL(bootArmICacheDisable)
    EXPORT_LABEL(bootArmPrivatePeriphBaseGet)
    EXPORT_LABEL(bootArmCp15MainIdReg)
    EXPORT_LABEL(bootArmCurCpuId)
    EXPORT_LABEL(bootArmBranchPredictorInvalidate)
    EXPORT_LABEL(bootArmBranchPredictionEnable)
    EXPORT_LABEL(bootArmBranchPredictionDisable)

;/*********************************************************************************************************
;  Section .boot.text, This Matrix boot time used code
;*********************************************************************************************************/

    //FILE_BEGIN()
    .code 32
    .section .boot.text, "ax"
    .balign 8;

;/*********************************************************************************************************
;  使能 MMU
;*********************************************************************************************************/

FUNC_DEF(bootArmMmuEnable)
    ARM_DSB()
    ARM_ISB()
    MRC     p15, 0, R0, c1, c0, 0
    ORR     R0, R0, #P15_R1_M
    MCR     p15, 0, R0, c1, c0, 0
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  禁能 MMU
;*********************************************************************************************************/
FUNC_DEF(bootArmMmuDisable)
    ARM_DSB()
    ARM_ISB()
    MRC     p15, 0, R0, c1, c0, 0
    BIC     R0, R0, #P15_R1_M
    MCR     p15, 0, R0, c1, c0, 0
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()


;/*********************************************************************************************************
;  设置页表基地址 TTBR0 为进程相关页表  TTBR1 为系统全局页表
;*********************************************************************************************************/

FUNC_DEF(bootArmMmuSetTTBase0)
    ARM_DSB()
    ARM_ISB()
    MCR     p15, 0, R0, c2, c0, 0                                       ;/* Set TT Base                  */

    MOV     R0, #0
    ;/* SMP_UP_CHECK(R1) */
    MCRHI   p15, 0, R0, c7, c1, 6                                       ;/* BPIALLIS SMP                 */
    MCRLS   p15, 0, R0, c7, c5, 6                                       ;/* BPIALL UP                    */
    ARM_NOP(R0)                                                         ;/* 3 * NOP                      */
    ARM_NOP(R0)
    ARM_NOP(R0)
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()
    
FUNC_DEF(bootArmMmuSetTTBase1)
    ARM_DSB()
    ARM_ISB()
    MCR     p15, 0, R0, c2, c0, 1

    MOV     R0, #0
    ;/* SMP_UP_CHECK(R1) */
    MCRHI   p15, 0, R0, c7, c1, 6                                       ;/* BPIALLIS SMP                 */
    MCRLS   p15, 0, R0, c7, c5, 6                                       ;/* BPIALL UP                    */
    ARM_NOP(R0)                                                         ;/* 3 * NOP                      */
    ARM_NOP(R0)
    ARM_NOP(R0)
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  设置域属性
;*********************************************************************************************************/

FUNC_DEF(bootArmMmuSetDomain)
    MCR     p15, 0, R0, c3, c0, 0
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  This API set TTBCR register
;*********************************************************************************************************/

FUNC_DEF(bootArmMmuV7SetTTBCR)
    MCR     p15, 0, R0, c2, c0, 2
    BX      LR
    FUNC_END()
    
;/*********************************************************************************************************
;  设置进程 ID
;*********************************************************************************************************/

FUNC_DEF(bootArmMmuSetProcessId)
    MCR     p15, 0, R0, c13, c0, 0
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  TLB 相关操作
;*********************************************************************************************************/

FUNC_DEF(bootArmMmuInvalidateTLB)                                           ;/* 快表中的所有项无效           */
    ARM_DSB()
    ARM_ISB()
    MOV     R0, #0
    /*SMP_UP_CHECK(R1)*/
    MCRHI   p15, 0, R0, c8, c3, 0                                       ;/* TLBIALLIS SMP                */
    MCRLS   p15, 0, R0, c8, c7, 0                                       ;/* TLBIALL UP                   */
    MCRHI   p15, 0, R0, c7, c1, 6                                       ;/* BPIALLIS SMP                 */
    MCRLS   p15, 0, R0, c7, c5, 6                                       ;/* BPIALL UP                    */
    MCR     p15, 0, R0, c7, c5, 6                                       ;/* BPIALL UP                    */
    ARM_NOP(R0)                                                         ;/* 3 * NOP                      */
    ARM_NOP(R0)
    ARM_NOP(R0)
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()    

;/*********************************************************************************************************
;  This API Disables specified features in CP15 control register
;  R0 - features   Features to disable in Coprocessor 15 control register.
;                  'features' can take any OR a combination of the below values.
;                  CP15_CONTROL_TEXREMAP - TEX remap flag
;                  CP15_CONTROL_ACCESSFLAG - Access flag Control
;                  CP15_CONTROL_ALIGN_CHCK - Alignment Fault Checking
;                  CP15_CONTROL_MMU - To enable MMU
;
;  Note: Other fields of the CP15 c1 control register are not given here
;        as they are not of importance for StarterWare. However, optionally
;        they can also be ADDed.
;*********************************************************************************************************/

FUNC_DEF(bootArmControlFeatureDisable)
    ARM_DSB()
    ARM_ISB()
    MRC     p15, 0, R1, c1, c0, 0
    BIC     R0, R1, R0
    MCR     p15, 0, R0, c1, c0, 0
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  This API Sets the specified fields in Auxiliary Control Register
;  R0 - Bit Mask for the bits to be set in Auxiliary Control Register
;*********************************************************************************************************/

FUNC_DEF(bootArmAuxControlFeatureEnable)
    ARM_DSB()
    ARM_ISB()
    MRC     p15, 0, R1, c1, c0, 1
    ORR     R0,  R0, R1
    MCR     p15, 0, R0, c1, c0, 1
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  This API Clears the specified fields in Auxiliary Control Register
;  R0 - Bit Mask for the bits to be cleared in Auxiliary Control Register
;*********************************************************************************************************/

FUNC_DEF(bootArmAuxControlFeatureDisable)
    ARM_DSB()
    ARM_ISB()
    MRC     p15, 0, R1, c1, c0, 1
    BIC     R0,  R1, R0
    MCR     p15, 0, R0, c1, c0, 1
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  ARMv7 CLIDR 寄存器 level shift & mask
;*********************************************************************************************************/

#define LoUIS_MASK      0x00e00000
#define LoUIS_SHIFT     20
#define LoC_MASK        0x07000000
#define LoC_SHIFT       23
#define LoUU_MASK       0x38000000
#define LoUU_SHIFT      26

;/*********************************************************************************************************
;  ARMv7 DCACHE 操作宏
;*********************************************************************************************************/

#define _FLD_MAX_WAY    0x3ff
#define _FLD_MAX_IDX    0x7fff

MACRO_DEF(armDCacheV7Op Op CRm LoXM LoXS)
    STMFD   SP!, {R4-R11}
    ARM_DMB()
    MRC     p15, 1, R0, c0, c0, 1                                       ;/* Read CLID register           */
    ANDS    R3, R0, #\LoXM                                              ;/* Get Level of Coherency       */
    MOV     R3, R3, LSR #\LoXS
    BEQ     finished\Op
    MOV     R10, #0

LINE_LABEL(loop1\Op)
    ADD     R2, R10, R10, LSR #1
    MOV     R1, R0, LSR R2
    AND     R1, R1, #7
    CMP     R1, #2
    BLT     skip\Op
    MCR     p15, 2, R10, c0, c0, 0
    ARM_ISB()
    MRC     p15, 1, R1, c0, c0, 0
    AND     R2, R1, #7
    ADD     R2, R2, #4
    LDR     R4, =_FLD_MAX_WAY
    ANDS    R4, R4, R1, LSR #3
    CLZ     R5, R4
    LDR     R7, =_FLD_MAX_IDX
    ANDS    R7, R7, R1, LSR #13

LINE_LABEL(loop2\Op)
    MOV     R9, R4

LINE_LABEL(loop3\Op)
    ORR     R11, R10, R9, LSL R5
    ORR     R11, R11, R7, LSL R2
    MCR     p15, 0, R11, c7, \CRm, 2
    SUBS    R9, R9, #1
    BGE     loop3\Op
    SUBS    R7, R7, #1
    BGE     loop2\Op

LINE_LABEL(skip\Op)
    ADD     R10, R10, #2
    CMP     R3, R10
    BGT     loop1\Op

LINE_LABEL(finished\Op)
    MOV     R10, #0                                                     ;/*  swith back to cache lvl 0   */
    MCR     p15, 2, R10, c0, c0, 0                                      ;/*  select cache lvl in csselr  */

    ARM_DSB()
    ARM_ISB()
    LDMFD   SP!, {R4-R11}
    BX      LR
    MACRO_END()

;/*********************************************************************************************************
;  ARMv7 关闭 DCACHE
;*********************************************************************************************************/

FUNC_DEF(bootArmDCacheV7Disable)
    MRC     p15, 0, R0, c1, c0, 0
    AND     R0, R0, #P15_R1_C
    CMP     R0, #0
    BEQ     dcache_not_en

    STMFD   SP!, {LR}
    BL      bootArmDCacheV7ClearAll
    LDMFD   SP!, {LR}
    
    ARM_DSB()
    ARM_ISB()
    
    MRC     p15, 0, R0, c1, c0, 0
    BIC     R0, R0, #P15_R1_C
    MCR     p15, 0, R0, c1, c0, 0
    BX      LR
	
LINE_LABEL(dcache_not_en)
    STMFD   SP!, {LR}
    BL      bootArmDCacheV7InvalidateAll
    LDMFD   SP!, {LR}
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  ARMv7 回写并无效 DCACHE
;*********************************************************************************************************/

FUNC_DEF(bootArmDCacheV7ClearAll)
    armDCacheV7Op   ClearAll            c14     LoC_MASK    LoC_SHIFT
    FUNC_END()

;/*********************************************************************************************************
;  ARMv7 无效 DCACHE
;*********************************************************************************************************/

FUNC_DEF(bootArmDCacheV7InvalidateAll)
    armDCacheV7Op   InvalidateAll       c6      LoC_MASK    LoC_SHIFT
    FUNC_END()

;/*********************************************************************************************************
;  使能 DCACHE
;*********************************************************************************************************/

FUNC_DEF(bootArmDCacheEnable)
    MRC     p15, 0, R0, c1, c0, 0
    ORR     R0, R0, #P15_R1_C
    MCR     p15, 0, R0, c1, c0, 0
    BX      LR
    FUNC_END() 

;/*********************************************************************************************************
;  无效整个 ICACHE
;*********************************************************************************************************/

FUNC_DEF(bootArmICacheInvalidateAll)
    ARM_DSB()
    ARM_ISB()
    MOV     R0, #0
    /*SMP_UP_CHECK(R1)*/
    MCRHI   p15, 0, R0, c7, c1, 0                                       ;/* ICIALLUIS SMP                */
    MCRLS   p15, 0, R0, c7, c5, 0                                       ;/* ICIALLU UP                   */
    MCRHI   p15, 0, R0, c7, c1, 6                                       ;/* BPIALLIS SMP                 */
    MCRLS   p15, 0, R0, c7, c5, 6                                       ;/* BPIALL UP                    */
    ARM_NOP(R0)                                                         ;/* 3 * NOP                      */
    ARM_NOP(R0)
    ARM_NOP(R0)
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  禁能 ICACHE
;*********************************************************************************************************/

FUNC_DEF(bootArmICacheDisable)
    MRC     p15, 0, R0, c1, c0, 0
    AND     R0, R0, #P15_R1_I
    CMP     R0, #0
    BEQ     icache_not_en
    
    MRC     p15, 0, R0, c1, c0, 0
    BIC     R0, R0, #P15_R1_I
    MCR     p15, 0, R0, c1, c0, 0

LINE_LABEL(icache_not_en)
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  In Cortex-A9 or later uniprocessor implementations the base address is set to zero.
;  In Cortex-A9 or later MPCore implementations, the base address is reset to PERIPHBASE[31:13]
;  so that software can determine the location of the private memory region.
;*********************************************************************************************************/

FUNC_DEF(bootArmPrivatePeriphBaseGet)
    MRC     p15, 4, R0, c15, c0, 0
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  获得 CP15 基本信息
;*********************************************************************************************************/

FUNC_DEF(bootArmCp15MainIdReg)
    MRC     p15, 0, R0, c0, c0, 0
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  获得当前核 ID ARMv6/v7
;*********************************************************************************************************/

FUNC_DEF(bootArmCurCpuId)
    MRC     p15, 0, R0, c0, c0, 5
    AND     R0, R0, #0x03
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  分支预测操作
;*********************************************************************************************************/
;/*********************************************************************************************************
;  This API invalidates the branch predictor
;*********************************************************************************************************/

FUNC_DEF(bootArmBranchPredictorInvalidate)
    ARM_DSB()
    ARM_ISB()
    MOV     R0, #0
    /*SMP_UP_CHECK(R1)*/
    MCRHI   p15, 0, R0, c7, c1, 6                                       ;/* BPIALLIS SMP                 */
    MCRLS   p15, 0, R0, c7, c5, 6                                       ;/* BPIALL UP                    */

    ARM_NOP(R0)                                                         ;/* 3 * NOP                      */
    ARM_NOP(R0)
    ARM_NOP(R0)
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  This API enables the branch predictor
;*********************************************************************************************************/

FUNC_DEF(bootArmBranchPredictionEnable)
    ARM_DSB()
    ARM_ISB()
    MRC     p15, 0, R0, c1, c0, 0
    ORR     R0, R0, #(1 << 11)
    MCR     p15, 0, R0, c1, c0, 0
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()

;/*********************************************************************************************************
;  This API disables the branch predictor
;*********************************************************************************************************/

FUNC_DEF(bootArmBranchPredictionDisable)
    ARM_DSB()
    ARM_ISB()
    MRC     p15, 0, R0, c1, c0, 0
    BIC     R0, R0, #(1 << 11)
    MCR     p15, 0, R0, c1, c0, 0
    ARM_DSB()
    ARM_ISB()
    BX      LR
    FUNC_END()
    FILE_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/

