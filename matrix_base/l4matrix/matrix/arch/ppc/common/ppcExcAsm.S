/*
 * ppcExcAsm.S
 *
 *  Created on: Jun 29, 2018
 *      Author: Liyanqi
 */


#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/arch_assembler.h>
#include "ppcCtxAsm.h"

    FILE_BEGIN()

    IMPORT_LABEL(API_ThreadTcbInter)
    IMPORT_LABEL(API_InterStackBaseGet)
    IMPORT_LABEL(API_InterEnter)
    IMPORT_LABEL(API_InterExit)

    IMPORT_LABEL(archMachineCheckExceptionHandle)
    IMPORT_LABEL(archDataStorageExceptionHandle)
    IMPORT_LABEL(archInstructionStorageExceptionHandle)
    IMPORT_LABEL(bspIntHandle)
    IMPORT_LABEL(archAlignmentExceptionHandle)
    IMPORT_LABEL(archProgramExceptionHandle)
    IMPORT_LABEL(archFpuUnavailableExceptionHandle)
    IMPORT_LABEL(archDecrementerInterruptHandle)
    IMPORT_LABEL(archSystemCallHandle)
    IMPORT_LABEL(archTraceHandle)
    IMPORT_LABEL(archFpAssistExceptionHandle)

    EXPORT_LABEL(archMachineCheckExceptionEntry)
    EXPORT_LABEL(archDataStorageExceptionEntry)
    EXPORT_LABEL(archInstructionStorageExceptionEntry)
    EXPORT_LABEL(archExternalInterruptEntry)
    EXPORT_LABEL(archAlignmentExceptionEntry)
    EXPORT_LABEL(archProgramExceptionEntry)
    EXPORT_LABEL(archFpuUnavailableExceptionEntry)
    EXPORT_LABEL(archDecrementerInterruptEntry)
    EXPORT_LABEL(archSystemCallEntry)
    EXPORT_LABEL(archTraceEntry)
    EXPORT_LABEL(archFpAssistExceptionEntry)

;/*********************************************************************************************************
;  异常入口宏
;*********************************************************************************************************/

MACRO_DEF(EXC_ENTRY, handle)
    ;/*
    ; * 使用异常临时栈, 并在异常临时栈开辟临时上下文保存区, 将 volatile 寄存器保存到临时上下文保存区
    ; */
    EXC_SAVE_VOLATILE

    SUBI    SP , SP , 16                                                ;/*  开辟 C 函数栈帧             */

    ;/*
    ; * API_InterEnter()
    ; */
    BL      API_InterEnter                                              ;/*  获取中断层数放到返回值 R3   */

    CMPWI   CR0, R3 , 1
    BNE     CR0, 1f

    ;/*
    ; * 获得当前任务 TCB 的 ARCH_REG_CTX 地址
    ; */
    BL      API_ThreadTcbInter                                          ;/*  get current tcb             */

    ;/*
    ; * 拷贝 volatile 寄存器到当前任务 TCB 的 ARCH_REG_CTX 里
    ; */
    ADDI    SP , SP , 16                                                ;/*  回收 C 函数栈帧             */

    EXC_COPY_VOLATILE

    ;/*
    ; * 保存 non volatile 寄存器到当前任务 TCB 的 ARCH_REG_CTX 里
    ; */
    EXC_SAVE_NON_VOLATILE

    SUBI    SP , SP , 16                                                ;/*  开辟 C 函数栈帧             */

    ;/*
    ; * 第一次进入中断: 获得当前 CPU 中断堆栈栈顶, 并设置 SP
    ; */
    BL      API_InterStackBaseGet
    SUBI    SP , R3 , 16                                                ;/*  开辟 C 函数栈帧             */

2:
    ;/*
    ; * handle(返回地址)
    ; */
    MFSPR   R3 , SRR0                                                   ;/*  传递返回地址                */
    ISYNC
    BL      \handle                                                     ;/*  调用处理函数                */

    ;/*
    ; * API_InterExit()
    ; * 如果没有发生中断嵌套, 则 API_InterExit 会调用 archIntCtxLoad 函数
    ; */
    BL      API_InterExit

    ;/*
    ; * 来到这里, 说明发生了中断嵌套
    ; */
    ADDI    R4 , SP , 16                                                ;/*  回收 C 函数栈帧             */
    RESTORE_REGS                                                        ;/*  恢复所有寄存器              */

1:
    ;/*
    ; * 不是第一次进入中断
    ; */
    ADDI    SP , SP , 16                                                ;/*  回收 C 函数栈帧             */

    MFSPR   R3 , SPRG0                                                  ;/*  异常前 SP(R1)               */
    SUBI    R3 , R3 , ARCH_REG_CTX_SIZE                                 ;/*  在异常堆栈开辟上下文保存区  */

    ;/*
    ; * 拷贝 volatile 寄存器到异常堆栈里的上下文保存区
    ; */
    EXC_COPY_VOLATILE

    ;/*
    ; * 保存 non volatile 寄存器到异常堆栈里
    ; */
    EXC_SAVE_NON_VOLATILE

    SUBI    SP , R3 , 16                                                ;/*  使用异常堆栈并开辟C函数栈帧 */

    B       2b
    MACRO_END()

;/*********************************************************************************************************
;  机器检查异常入口(Vector 0x200)
;*********************************************************************************************************/

FUNC_DEF(archMachineCheckExceptionEntry)
    EXC_ENTRY   archMachineCheckExceptionHandle
    FUNC_END()

;/*********************************************************************************************************
;  数据存储异常入口(Vector 0x300)
;*********************************************************************************************************/

FUNC_DEF(archDataStorageExceptionEntry)
    EXC_ENTRY   archDataStorageExceptionHandle
    FUNC_END()

;/*********************************************************************************************************
;  指令访问异常入口(Vector 0x400)
;*********************************************************************************************************/

FUNC_DEF(archInstructionStorageExceptionEntry)
    EXC_ENTRY   archInstructionStorageExceptionHandle
    FUNC_END()

;/*********************************************************************************************************
;  外部中断入口(Vector 0x500)
;*********************************************************************************************************/

FUNC_DEF(archExternalInterruptEntry)
    EXC_ENTRY   bspIntHandle
    FUNC_END()

;/*********************************************************************************************************
;  非对齐异常入口(Vector 0x600)
;*********************************************************************************************************/

FUNC_DEF(archAlignmentExceptionEntry)
    EXC_ENTRY   archAlignmentExceptionHandle
    FUNC_END()

;/*********************************************************************************************************
;  程序异常入口(Vector 0x700)
;*********************************************************************************************************/

FUNC_DEF(archProgramExceptionEntry)
    EXC_ENTRY   archProgramExceptionHandle
    FUNC_END()


;/*********************************************************************************************************
;  Decrementer 中断入口(Vector 0x900)
;*********************************************************************************************************/

FUNC_DEF(archDecrementerInterruptEntry)
    EXC_ENTRY   archDecrementerInterruptHandle
    FUNC_END()

;/*********************************************************************************************************
;  系统调用入口(Vector 0xC00)
;*********************************************************************************************************/

FUNC_DEF(archSystemCallEntry)
    EXC_ENTRY   archSystemCallHandle
    FUNC_END()

;/*********************************************************************************************************
;  Trace 入口(Vector 0xD00)
;*********************************************************************************************************/

FUNC_DEF(archTraceEntry)
    EXC_ENTRY   archTraceHandle
    FUNC_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/
