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
;** 创   建   人: L Y.Q.
;**
;** 文件创建日期: 2018 年 08 月 09 日
;**
;** 描        述: 在开启 MMU 之前的启动代码.
;*********************************************************************************************************/
#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/arch_assembler.h>

;/*********************************************************************************************************
;  单个核心 SVC 模式下堆栈大小
;  本堆栈的使用从核复位开始, 至跳入高 1G 地址结束.(作用范围是本文件夹代码)
;*********************************************************************************************************/
#define PER_CPU_STACK_SIZE  0x00001000                                  ;/*  4K                         */

;/*********************************************************************************************************
;  声明外部函数和导出全局函数
;*********************************************************************************************************/

    IMPORT_LABEL(start)                                                 ;/*  虚拟地址入口                */
    IMPORT_LABEL(bootPrimaryMmu)                                        ;/*  设置最初的内存映射          */
    IMPORT_LABEL(bootSecondaryMmu)                                      ;/*  设置最初的内存映射          */
    IMPORT_LABEL(bootPpcCurCpuId)                                       ;/*  获取当前 CPU ID 号 [0 - 3]  */
    IMPORT_LABEL(bootPpcDCacheV7InvalidateAll)
    IMPORT_LABEL(bootPpcDCacheV7Disable)
    IMPORT_LABEL(bootPpcICacheInvalidateAll)
    IMPORT_LABEL(bootPpcICacheDisable)
    IMPORT_LABEL(bootPpcBranchPredictorInvalidate)
    IMPORT_LABEL(bootPpcBranchPredictionDisable)
    IMPORT_LABEL(bootPpcMmuInvalidateTLB)
    IMPORT_LABEL(bootPpcMmuDisable)

    EXPORT_LABEL(boot)                                                  ;/*  bsp 汇编入口函数            */

;/*********************************************************************************************************
;  异常向量表
;*********************************************************************************************************/

    .code 32
    .section .vector, "ax"
    .balign 32

FUNC_DEF(vector)
    B       boot
    LDR     PC, undEntry                                                ;/*  未定义指令                 */
    LDR     PC, swiEntry                                                ;/*  软件中断                   */
    LDR     PC, preEntry                                                ;/*  指令预取中止               */
    LDR     PC, datEntry                                                ;/*  数据访问中止               */
    LDR     PC, reserveEntry
    LDR     PC, irqEntry                                                ;/*  外部中断请求               */
    LDR     PC, fiqEntry                                                ;/*  快速中断请求               */
    FUNC_END()

FUNC_LABEL(undEntry)
    .word   archUndEntry

FUNC_LABEL(swiEntry)
    .word   archSwiEntry

FUNC_LABEL(preEntry)
    .word   archPreEntry

FUNC_LABEL(datEntry)
    .word   archDatEntry

FUNC_LABEL(reserveEntry)
    .word   archResHandle

FUNC_LABEL(irqEntry)
    .word   archIntEntry

FUNC_LABEL(fiqEntry)
    .word   archFiqHandle

;/*********************************************************************************************************
;  Section .boot.text, This Matrix boot time used code
;*********************************************************************************************************/

    //FILE_BEGIN()
    .code 32
    .section .boot.text, "ax"
    .balign 8;

FUNC_DEF(boot)

;/*********************************************************************************************************
;  进入 SVC32 模式(关中断)
;*********************************************************************************************************/

    MSR     CPSR_c, #(SVC32_MODE | DIS_INT)

;/*********************************************************************************************************
;  初始化堆栈(包括主核与从核, 这里仅设置 SVC 模式的堆栈)
;*********************************************************************************************************/

    BL      bootArmCurCpuId                                            ;/*  获取当前 CPU ID 存入 R0      */

    LDR     R1 , =_boot_stack_end                                      ;/*  栈区顶端地址                 */
LINE_LABEL(0)
    CMP     R0 , #0
    SUBNE   R1 , R1 , #PER_CPU_STACK_SIZE
    SUBNE   R0 , R0 , #1
    BNE     0b

    MOV     SP , R1                                                    ;/*  设置堆栈                     */

;/*********************************************************************************************************
;  Patch for i.MX6 errarta
;*********************************************************************************************************/
;/*********************************************************************************************************
;  ARM Errarta: 743622, 751472, 743623
;*********************************************************************************************************/
    MRC     p15, 0 , R0, c15, c0, 1
    ORR     R0 , R0, #0x00000040
    ORR     R0 , R0, #0x00000800
    ORR     R0 , R0, #0x00100000
    MCR     p15, 0 , R0, c15, c0, 1
;/*********************************************************************************************************
;  ARM Errarta: 845369 (patch will reduce speed of memset/memcpy)
;*********************************************************************************************************/
    MRC     p15, 0 , R0, c15, c0, 1
    ORR     R0 , R0, #0x00400000
    MCR     p15, 0 , R0, c15, c0, 1
;/*********************************************************************************************************
;  ARM Errarta: 761320 (patch will reduce bus performance )
;  - CPU0 performs a full cache line write to address A, then a full cache line write to address B
;  - CPU1 performs a full cache line write to address B, then a full cache line write to address A
;  - CPU2 performs read accesses to addresses A and B
;*********************************************************************************************************/
    MRC     p15, 0 , R0, c15, c0, 1
    ORR     R0 , R0, #0x00200000
    MCR     p15, 0 , R0, c15, c0, 1
;/*********************************************************************************************************
;  WARNING: ARM Errarta: 743626 is NOT handled yet
;*********************************************************************************************************/

;/*********************************************************************************************************
;  关闭 D-Cache(无效)
;  关闭 I-Cache(无效)
;  无效并关闭分支预测(BranchPredictor)
;  关闭 MMU(无效 TLB)
;*********************************************************************************************************/

    BL      bootArmDCacheV7InvalidateAll
    BL      bootArmDCacheV7Disable
    BL      bootArmICacheInvalidateAll
    BL      bootArmICacheDisable

    BL      bootArmBranchPredictorInvalidate
    BL      bootArmBranchPredictionDisable

    BL      bootArmMmuInvalidateTLB
    BL      bootArmMmuDisable

;/*********************************************************************************************************
;  如果不是 Primary CPU, 则进入 bootSecondaryMmu 函数
;*********************************************************************************************************/

    BL      bootArmCurCpuId
    CMP     R0, #0
    BLNE    bootSecondaryMmu

;/*********************************************************************************************************
;  如果是 Primary CPU, 则进入 bootPrimaryMmu 函数, 初始化最初的页表
;*********************************************************************************************************/

    LDR     R10, =bootPrimaryMmu
    MOV     LR , PC
    BX      R10

    B       .
    FUNC_END()

    FILE_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/

