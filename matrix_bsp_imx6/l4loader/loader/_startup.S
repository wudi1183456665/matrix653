;/*********************************************************************************************************
;**
;**                                    �й�������Դ��֯
;**
;**                                   Ƕ��ʽʵʱ����ϵͳ
;**
;**                                SylixOS(TM)  LW : long wing
;**
;**                               Copyright All Rights Reserved
;**
;**--------------�ļ���Ϣ--------------------------------------------------------------------------------
;**
;** ��   ��   ��: _startup.S
;**
;** ��   ��   ��: Li.Jing (�)
;**
;** �ļ���������: 2017 �� 01 �� 05 ��
;**
;** ��        ��: ��ں���.
;*********************************************************************************************************/

#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/arch_assembler.h>
#include "../config.h"

    FILE_BEGIN()

;/*********************************************************************************************************
;  �����ⲿ�����͵���ȫ�ֺ���
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
;  ת�� SVC32 ģʽ���� irq ��fiq
;*********************************************************************************************************/

    MSR     CPSR , #(0xc0 | 0x13)

;/*********************************************************************************************************
;  ��ʼ�� stack
;*********************************************************************************************************/

    LDR     SP , =__stack_end                                          ;/*  ջ�����˵�ַ                 */

;/*********************************************************************************************************
;  �� 0 BSS ��
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
;  ��ת�� bspInit ���� (VOID)
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