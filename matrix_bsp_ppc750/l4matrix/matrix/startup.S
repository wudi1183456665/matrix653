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
;** 创   建   人: Jiao.JinXing (焦进星)
;**
;** 文件创建日期: 2015 年 12 月 16 日
;**
;** 描        述: PowerPC 处理器系统启动.
;*********************************************************************************************************/

#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include "config.h"
#include <arch/arch_assembler.h>

#define __SYLIXOS_PPC604__                  0x604
#define __SYLIXOS_PPCEC604__                0xEC604
#define __SYLIXOS_PPC604__                  0x604

#define __SYLIXOS_PPC_CPU__                 __SYLIXOS_PPC604__
#define __SYLIXOS_PPC_HAVE_ALTIVEC          1
#include <arch/ppc/mm/cache/ppc60x/arch_604.h>

    FILE_BEGIN()

;/*********************************************************************************************************
;  声明外部函数和导出全局函数
;*********************************************************************************************************/

    IMPORT_LABEL(bspInit)                     //初始化函数

    IMPORT_LABEL(archDataStorageExceptionEntry)
    IMPORT_LABEL(archInstructionStorageExceptionEntry)
    IMPORT_LABEL(archExternalInterruptEntry)
    IMPORT_LABEL(archAlignmentExceptionEntry)
    IMPORT_LABEL(archProgramExceptionEntry)
    IMPORT_LABEL(archFpuUnavailableExceptionEntry)
    IMPORT_LABEL(archSystemCallEntry)
    IMPORT_LABEL(archTraceEntry)
    IMPORT_LABEL(ppc603InstructionTranslationMissEntry)
    IMPORT_LABEL(ppc603DataLoadTranslationMissEntry)
    IMPORT_LABEL(ppc603DataStoreTranslationMissEntry)
    IMPORT_LABEL(archDecrementerInterruptEntry)

   // EXPORT_LABEL(start)                       //bsp汇编入口函数
	.global start

	SECTION(.text)
;/*********************************************************************************************************
;  代码入口
;*********************************************************************************************************/

FUNC_DEF(start)
    ;/*  关闭中断 */
    XOR     R3 , R3 , R3
    ISYNC
    MTMSR   R3
    ISYNC

;/*********************************************************************************************************
;  初始化处理器
;*********************************************************************************************************/

LINE_LABEL(ppcCpuInit)

    ;/*  清零 SPRG */
    XOR     R3 , R3 , R3
    ISYNC
    MTSPR   SPRG0 , R3
    MTSPR   SPRG1 , R3
    MTSPR   SPRG2 , R3
    MTSPR   SPRG3 , R3
    ISYNC



    ;/*  初始段寄存器 */
    XOR     R3 , R3 , R3
    ISYNC

    MTSR    0 ,  R3
    MTSR    1 ,  R3
    MTSR    2 ,  R3
    MTSR    3 ,  R3
    MTSR    4 ,  R3
    MTSR    5 ,  R3
    MTSR    6 ,  R3
    MTSR    7 ,  R3
    MTSR    8 ,  R3
    MTSR    9 ,  R3
    MTSR    10 , R3
    MTSR    11 , R3
    MTSR    12 , R3
    MTSR    13 , R3
    MTSR    14 , R3
    MTSR    15 , R3
    ISYNC

    ;/*  关闭数据和指令 CACHE */
    MFSPR   R3 , HID0
    ISYNC
    XOR     R4 , R4 , R4
    ORI     R4 , R4 , (ARCH_PPC_HID0_DCE | ARCH_PPC_HID0_ICE)
    ANDC    R3 , R3 , R4
    ISYNC
    MTSPR   HID0 , R3
    ISYNC


    ;/*  清除所有的 BAT 寄存器 */
    BL      ppcBATsClear

    ;/*  无效数据和指令 CACHE */
    MFSPR   R4 , HID0
    ISYNC
    XOR     R3 , R4 , R4
    ORI     R3 , R3 , (ARCH_PPC_HID0_ICE | ARCH_PPC_HID0_DCE | ARCH_PPC_HID0_ICFI | ARCH_PPC_HID0_DCFI)
    ISYNC
    MTSPR   HID0 , R3
    MTSPR   HID0 , R4
    ISYNC

    ;/*  无效所有的 TLB */
    BL      ppcTLBsInvalidate

    ;/*  初始化 BAT 寄存器 */
    BL      ppcBATsInit

;/*********************************************************************************************************
;  在这里初始化所需的控制器
;*********************************************************************************************************/

;/*********************************************************************************************************
;  初始化堆栈
;*********************************************************************************************************/

LINE_LABEL(ppcSpInit)
    LIS     SP , HI(__stack_end)
    ORI     SP , SP , LO(__stack_end)
    SYNC
    SUBI    SP , SP , 32                                                ;/*  开辟 C 函数栈帧             */

    LIS     R4 , HI(~7)
    ORI     R4 , R4 , LO(~7)
    AND     SP , SP , R4                                                ;/*  向下 8 字节对齐             */

;/*********************************************************************************************************
;  初始化 DATA 段
;*********************************************************************************************************/

LINE_LABEL(ppcDataCopy)
    ADDIS   R3 , 0  , HI(_data)
    ORI     R3 , R3 , LO(_data)

    ADDIS   R7 , 0  , HI(_edata)
    ORI     R7 , R7 , LO(_edata)

    ADDIS   R4 , 0  , HI(_etext)
    ORI     R4 , R4 , LO(_etext)

    CMP     CR0, 0  , R3 , R4
    BEQ     ppcBssClear

LINE_LABEL(cont1)
    LWZX    R5 , 0  , R4
    STWX    R5 , 0  , R3

    ADDI    R4 , R4 , 4
    ADDI    R3 , R3 , 4

    CMP     CR0, 0  , R3 , R7
    BLE     cont1

;/*********************************************************************************************************
;  清零 BSS 段
;*********************************************************************************************************/

LINE_LABEL(ppcBssClear)
    ADDIS   R4 , 0  , HI(__bss_start)
    ORI     R4 , R4 , LO(__bss_start)

    ADDIS   R7 , 0  , HI(__bss_end)
    ORI     R7 , R7 , LO(__bss_end)

    ADDIS   R5 , 0  , 0x0000

LINE_LABEL(cont2)
    STWX    R5 , 0 , R4

    ADDI    R4 , R4 , 4

    CMP     CR0, 0  , R4 , R7
    BLE     cont2

    SYNC

;/*********************************************************************************************************
;  进入 bspInit 函数 (argc = 0 , argv = NULL , frame pointer = NULL)
;*********************************************************************************************************/

    BL      bspInit

    FUNC_END()

;/*********************************************************************************************************
;  清除所有的 BAT 寄存器
;*********************************************************************************************************/

FUNC_DEF(ppcBATsClear)
    XOR     R3 , R3 , R3
    ISYNC

    MTSPR   IBAT0U , R3                                         /*  Clear all upper BATS first          */
    MTSPR   IBAT1U , R3
    MTSPR   IBAT2U , R3
    MTSPR   IBAT3U , R3
    MTSPR   DBAT0U , R3
    MTSPR   DBAT1U , R3
    MTSPR   DBAT2U , R3
    MTSPR   DBAT3U , R3

    MTSPR   IBAT0L , R3                                         /*  Then clear lower BATS               */
    MTSPR   IBAT1L , R3
    MTSPR   IBAT2L , R3
    MTSPR   IBAT3L , R3
    MTSPR   DBAT0L , R3
    MTSPR   DBAT1L , R3
    MTSPR   DBAT2L , R3
    MTSPR   DBAT3L , R3
    ISYNC

    MFSPR   R3 , PVR                                            /*  Read PVR                            */
    ISYNC
    SRWI    R3 , R3 , 12
    ANDI.   R3 , R3 , 0xFF
    CMPWI   R3 , 0x83
    BEQ     ppcIs7x5
    BLR
LINE_LABEL(ppcIs7x5)
    XOR     R3 , R3 , R3
    ISYNC

    MTSPR   IBAT4U , R3
    MTSPR   IBAT5U , R3
    MTSPR   IBAT6U , R3
    MTSPR   IBAT7U , R3

    MTSPR   DBAT4U , R3
    MTSPR   DBAT5U , R3
    MTSPR   DBAT6U , R3
    MTSPR   DBAT7U , R3

    MTSPR   IBAT4L , R3
    MTSPR   IBAT5L , R3
    MTSPR   IBAT6L , R3
    MTSPR   IBAT7L , R3

    MTSPR   DBAT4L , R3
    MTSPR   DBAT5L , R3
    MTSPR   DBAT6L , R3
    MTSPR   DBAT7L , R3
    ISYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  无效所有 TLB
;*********************************************************************************************************/

FUNC_DEF(ppcTLBsInvalidate)
    LI      R3 , 128
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
;  初始化 BAT 寄存器
;*********************************************************************************************************/

FUNC_DEF(ppcBATsInit)

    LIS     R3 , HI(IBAT0L_VAL)
    ORI     R3 , R3 , LO(IBAT0L_VAL)
    ISYNC
    MTSPR   IBAT0L , R3
    ISYNC

    LIS     R3 , HI(IBAT0U_VAL)
    ORI     R3 , R3 , LO(IBAT0U_VAL)
    ISYNC
    MTSPR   IBAT0U , R3
    ISYNC

    LIS     R3 , HI(IBAT1L_VAL)
    ORI     R3 , R3 , LO(IBAT1L_VAL)
    ISYNC
    MTSPR   IBAT1L , R3
    ISYNC

    LIS     R3 , HI(IBAT1U_VAL)
    ORI     R3 , R3 , LO(IBAT1U_VAL)
    ISYNC
    MTSPR   IBAT1U , R3
    ISYNC

    LIS     R3 , HI(IBAT2L_VAL)
    ORI     R3 , R3 , LO(IBAT2L_VAL)
    ISYNC
    MTSPR   IBAT2L , R3
    ISYNC

    LIS     R3 , HI(IBAT2U_VAL)
    ORI     R3 , R3 , LO(IBAT2U_VAL)
    ISYNC
    MTSPR   IBAT2U , R3
    ISYNC

    LIS     R3 , HI(IBAT3L_VAL)
    ORI     R3 , R3 , LO(IBAT3L_VAL)
    ISYNC
    MTSPR   IBAT3L , R3
    ISYNC

    LIS     R3 , HI(IBAT3U_VAL)
    ORI     R3 , R3 , LO(IBAT3U_VAL)
    ISYNC
    MTSPR   IBAT3U , R3
    ISYNC

    LIS     R3 , HI(DBAT0L_VAL)
    ORI     R3 , R3 , LO(DBAT0L_VAL)
    ISYNC
    MTSPR   DBAT0L , R3
    ISYNC

    LIS     R3 , HI(DBAT0U_VAL)
    ORI     R3 , R3 , LO(DBAT0U_VAL)
    ISYNC
    MTSPR   DBAT0U,R3
    ISYNC

    LIS     R3 , HI(DBAT1L_VAL)
    ORI     R3 , R3 , LO(DBAT1L_VAL)
    ISYNC
    MTSPR   DBAT1L , R3
    ISYNC

    LIS     R3 , HI(DBAT1U_VAL)
    ORI     R3 , R3 , LO(DBAT1U_VAL)
    ISYNC
    MTSPR   DBAT1U , R3
    ISYNC

    LIS     R3 , HI(DBAT2L_VAL)
    ORI     R3 , R3 , LO(DBAT2L_VAL)
    ISYNC
    MTSPR   DBAT2L , R3
    ISYNC

    LIS     R3 , HI(DBAT2U_VAL)
    ORI     R3 , R3 , LO(DBAT2U_VAL)
    ISYNC
    MTSPR   DBAT2U , R3
    ISYNC

    LIS     R3 , HI(DBAT3L_VAL)
    ORI     R3 , R3 , LO(DBAT3L_VAL)
    ISYNC
    MTSPR   DBAT3L , R3
    ISYNC

    LIS     R3 , HI(DBAT3U_VAL)
    ORI     R3 , R3 , LO(DBAT3U_VAL)
    ISYNC
    MTSPR   DBAT3U , R3
    ISYNC

    BLR

    FUNC_END()

    FILE_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/
