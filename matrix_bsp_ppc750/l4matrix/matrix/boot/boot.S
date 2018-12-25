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
;** 文   件   名: boot.S
;**
;** 创   建   人:
;**
;** 文件创建日期: 2018 年 12 月 17 日
;**
;** 描        述: 在开启 MMU 之前的启动代码.
;*********************************************************************************************************/
#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <../arch/arch_assembler.h>

;/*********************************************************************************************************
;  单个核心PowerPC supervisor mode of operation下堆栈大小
;  本堆栈的使用从核复位开始, 至跳入高 1G 地址结束.(作用范围是本文件夹代码)
;*********************************************************************************************************/
#define PER_CPU_STACK_SIZE  0x00001000                                  ;/*  4K                         */


	 FILE_BEGIN()
;/*********************************************************************************************************
;  声明外部函数和导出全局函数
;*********************************************************************************************************/

    IMPORT_LABEL(start)                                                 ;/*  虚拟地址入口                */
    IMPORT_LABEL(bootPrimaryMmu)                                        ;/*  设置最初的内存映射          */

	IMPORT_LABEL(ppcMmuEnable)
    IMPORT_LABEL(ppcMmuDisable)
	;.extern ppcMmuDisable

    IMPORT_LABEL(ppcMmuInvalidateTLBNr)
    IMPORT_LABEL(ppcMmuInvalidateTLBEA)

    IMPORT_LABEL(ppc60xICacheDisable)
    IMPORT_LABEL(ppc60xDCacheDisable)

    IMPORT_LABEL(ppc60xICacheInvalidate)
    IMPORT_LABEL(ppc60xDCacheInvalidate)
    IMPORT_LABEL(ppc60xDCacheClear)
    IMPORT_LABEL(ppc60xDCacheFlush)

    IMPORT_LABEL(ppc60xICacheInvalidateAll)
    .extern ppc60xDCacheInvalidateAll
    IMPORT_LABEL(ppc60xDCacheFlushAll)
    IMPORT_LABEL(ppc60xDCacheClearAll)

    IMPORT_LABEL(ppc60xBranchPredictionDisable)
    IMPORT_LABEL(ppc60xBranchPredictorInvalidate)

    ;EXPORT_LABEL(boot)                                                  ;/*  bsp 汇编入口函数            */
	.global boot

;/*
;** File begin
;*/

    .section .boot.text, "ax"
    .balign 8;

;/*********************************************************************************************************
;  启动入口
;*********************************************************************************************************/

FUNC_DEF(boot)
    ;/*  关闭中断 */
    XOR     R3 , R3 , R3
    ISYNC
    MTMSR   R3
    ISYNC

;/*********************************************************************************************************
;  初始化堆栈
;*********************************************************************************************************/

LINE_LABEL(ppcSpInit)
    LIS     SP , HI(__stack_end)
    ORI     SP , SP , LO(__stack_end)
    SYNC

;/*********************************************************************************************************
;  关闭 D-Cache(无效)
;  关闭 I-Cache(无效)
;  无效并关闭分支预测(BranchPredictor)
;  关闭 MMU(无效 TLB)
;*********************************************************************************************************/

    BL      ppc60xDCacheInvalidateAll
    BL      ppc60xDCacheDisable
    BL      ppc60xICacheInvalidateAll
    BL      ppc60xICacheDisable

    BL      ppc60xBranchPredictionDisable
    BL      ppc60xBranchPredictorInvalidate

    BL      ppcMmuInvalidateTLBNr
	BL      ppcMmuInvalidateTLBEA
    BL      ppcMmuDisable

;/*********************************************************************************************************
;  进入bootPrimaryMmu函数,初始化最初的页表
;*********************************************************************************************************/

    BL      bootPrimaryMmu

    FUNC_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/

