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
;** 文   件   名: _startup.S
;**
;** 创   建   人: Li.Jing (李靖)
;**
;** 文件创建日期: 2017 年 01 月 05 日
;**
;** 描        述: 入口函数.
;*********************************************************************************************************/

#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/arch_assembler.h>
#include "../config.h"

    FILE_BEGIN()

;/*********************************************************************************************************
;  声明外部函数和导出全局函数
;*********************************************************************************************************/

    IMPORT_LABEL(bspInit)

    EXPORT_LABEL(_start)

;/*********************************************************************************************************
;  The bsp header. It must come first.
;*********************************************************************************************************/

    SECTION(.bsp.header)

FUNC_DEF(start)
LINE_LABEL(_start)

;/*********************************************************************************************************
;  转入 SVC32 模式，关 irq ，fiq
;*********************************************************************************************************/

    MSR     CPSR , #(0xc0 | 0x13)

;/*********************************************************************************************************
;  初始化 stack
;*********************************************************************************************************/

    LDR     SP , =__stack_end                                          ;/*  栈区顶端地址                 */

;/*********************************************************************************************************
;  清 0 BSS 段
;*********************************************************************************************************/

    MOV     R0, #0                              ;/*  get a zero                                          */
    LDR     R1, =__bss_start                    ;/*  -> bss start                                        */
    LDR     R2, =__bss_end                      ;/*  -> bss end                                          */
LINE_LABEL(2)
    CMP     R1, R2                              ;/*  check if data to clear                              */
    STRLO   R0, [R1], #4                        ;/*  clear 4 bytes                                       */
    BLO     2b                                  ;/*  loop until done                                     */

    DSB

;/*********************************************************************************************************
;  跳转到 bspInit 函数 (VOID)
;*********************************************************************************************************/

    LDR     R10 , =bspInit
    MOV     LR  , PC
    BX      R10

    B       .

    FUNC_END(start)

    FILE_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/
