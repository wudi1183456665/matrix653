;/*********************************************************************************************************
;**
;**                                    中国软件开源组织
;**
;**                                   嵌入式实时操作系统
;**
;**                                SylixOS(TM)  LW : long wing
;**
;**                               Copyright All Rights Reserved
;**
;**--------------文件信息--------------------------------------------------------------------------------
;**
;** 文   件   名: startup.S
;**
;** 创   建   人: Li.Jing(李靖)
;**
;** 文件创建日期: 2018 年 1 月 18 日
;**
;** 描        述: matrix i.MX6 处理器启动文件.
;*********************************************************************************************************/
#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/arch_assembler.h>
#include "config.h"

;/*********************************************************************************************************
;  配置各处理器模式堆栈空间的大小
;*********************************************************************************************************/

#define SVC_STACK_SIZE  0x00002000
#define SYS_STACK_SIZE  0x00001000
#define FIQ_STACK_SIZE  0x00001000
#define UND_STACK_SIZE  0x00001000
#define ABT_STACK_SIZE  0x00001000
#define IRQ_STACK_SIZE  0x00001000

#define CPU_STACKS_SIZE     (SVC_STACK_SIZE + \
                             SYS_STACK_SIZE + \
                             FIQ_STACK_SIZE + \
                             IRQ_STACK_SIZE + \
                             UND_STACK_SIZE + \
                             ABT_STACK_SIZE)

    FILE_BEGIN()

;/*********************************************************************************************************
;  声明外部函数和导出全局函数
;*********************************************************************************************************/
    IMPORT_LABEL(targetInit)                                            /*  i.MX6 初始化函数             */

    IMPORT_LABEL(armDCacheV7ClearAll)
    IMPORT_LABEL(armDCacheV7Disable)
    IMPORT_LABEL(armICacheInvalidateAll)
    IMPORT_LABEL(armICacheDisable)
    IMPORT_LABEL(armBranchPredictorInvalidate)
    IMPORT_LABEL(armBranchPredictionDisable)
    IMPORT_LABEL(armMmuInvalidateTLB)
    IMPORT_LABEL(armMmuDisable)

    IMPORT_LABEL(halPrimaryCpuMain)                                     ;/*  主核 C 代码                 */
    IMPORT_LABEL(halSecondaryCpuMain)                                   ;/*  从核 C 代码                 */

    EXPORT_LABEL(start)                                                 ;/*  bsp 汇编入口函数            */

;/*********************************************************************************************************
;  高地址代码入口
;*********************************************************************************************************/

FUNC_DEF(start)

;/*********************************************************************************************************
;  进入 SVC32 模式(关中断)
;*********************************************************************************************************/

    MSR     CPSR_c, #(SVC32_MODE | DIS_INT)

;/*********************************************************************************************************
;  初始化堆栈(包括主核与从核，这里不需要锁，堆栈从高地址到低地址CPU0, CPU1, CPU2, CPU3)
;*********************************************************************************************************/

    BL      archMpCur

    LDR     R1, =__stack_end                                           ;/*  栈区顶端地址                 */
LINE_LABEL(0)
    CMP     R0, #0
    SUBNE   R1, R1, #CPU_STACKS_SIZE
    SUBNE   R0, R0, #1
    BNE     0b

    MOV     R0, R1
    BL      armInitSP                                                   ;/*  调用函数初始化堆栈          */

;/*********************************************************************************************************
;  如果不是 Primary CPU, 则进入 halSecondaryCpuMain 函数
;*********************************************************************************************************/

    BL      archMpCur
    CMP     R0, #0
    BLNE    halSecondaryCpuMain

;/*********************************************************************************************************
;  清 0 BSS 段
;*********************************************************************************************************/

    MOV     R0, #0                              ;/*  get a zero                                          */
    LDR     R1, =__bss_start                    ;/*  -> bss start                                        */
    LDR     R2, =__bss_end                      ;/*  -> bss end                                          */
LINE_LABEL(3)
    CMP     R1, R2                              ;/*  check if data to clear                              */
    STRLO   R0, [R1], #4                        ;/*  clear 4 bytes                                       */
    BLO     3b                                  ;/*  loop until done                                     */

    DSB

;/*********************************************************************************************************
;  调用 targetInit 函数初始化 i.MX6DQ
;*********************************************************************************************************/

    LDR     R10, =targetInit
    MOV     LR, PC
    BX      R10

;/*********************************************************************************************************
;  跳转到 halPrimaryCpuMain 函数 (argc = 0, argv = NULL, frame pointer = NULL)
;*********************************************************************************************************/

    MOV     R0, SP
    MOV     R1, #0
    MOV     R2, #0
    MOV     FP, #0

    LDR     R10, =halPrimaryCpuMain
    MOV     LR, PC
    BX      R10

    B       .

    FUNC_END()

;/*********************************************************************************************************
;  初始化各处理器模式的堆栈指针
;*********************************************************************************************************/

FUNC_DEF(armInitSP)

    MSR     CPSR_c, #(SVC32_MODE | DIS_INT)
    MOV     SP, R0
    SUB     R0, R0, #SVC_STACK_SIZE

    MSR     CPSR_c, #(SYS32_MODE | DIS_INT)
    MOV     SP, R0
    SUB     R0, R0, #SYS_STACK_SIZE

    MSR     CPSR_c, #(FIQ32_MODE | DIS_INT)
    MOV     SP, R0
    SUB     R0, R0, #FIQ_STACK_SIZE

    MSR     CPSR_c, #(IRQ32_MODE | DIS_INT)
    MOV     SP, R0
    SUB     R0, R0, #IRQ_STACK_SIZE

    MSR     CPSR_c, #(UND32_MODE | DIS_INT)
    MOV     SP, R0
    SUB     R0, R0, #UND_STACK_SIZE

    MSR     CPSR_c, #(ABT32_MODE | DIS_INT)
    MOV     SP, R0
    SUB     R0, R0, #ABT_STACK_SIZE

    MSR     CPSR_c, #(SVC32_MODE | DIS_INT)

    BX      LR

    FUNC_END()

    FILE_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/
