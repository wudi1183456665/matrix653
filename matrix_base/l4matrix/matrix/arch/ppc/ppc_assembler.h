/*
 * ppc_assembler.h
 *
 *  Created on: Jun 29, 2018
 *      Author: Liyanqi
 */

#ifndef __PPC_ASSEMBLER_H
#define __PPC_ASSEMBLER_H


/*********************************************************************************************************
  PowerPC architecture assembly special code
*********************************************************************************************************/

#if defined(__ASSEMBLY__) || defined(ASSEMBLY)


#define ARCH_PPC_MMU_MASK(src, des) \
    RLWINM  des, src, 0, 28, 25

/*********************************************************************************************************
  assembler define
*********************************************************************************************************/

#  define EXPORT_LABEL(lable)		.global label
#  define IMPORT_LABEL(lable)		.extern label

#  define FUNC_LABEL(func)			func:
#  define LINE_LABEL(line)			line:
#  define FUNC_DEF(func)	\
		.balign	4;	\
		.type func, %function;	\
func:

#  define FUNC_END()

#  define MACRO_DEF(mfunc...)	\
		.macro  mfunc

#  define MACRO_END()	\
		.endm

#  define FILE_BEGIN()	\
		.text;	\
		.balign	4;

#  define FILE_END()	\
		.end

#  define SECTION(sec)	\
		.section sec

#  define WEAK(sym)		\
		.weak sym
/*********************************************************************************************************
  PowerPC memory barrier
*********************************************************************************************************/

#define PPC_ISYNC()     ISYNC
#define PPC_SYNC()      SYNC

/*********************************************************************************************************
  Macro for hiadjust and lo
*********************************************************************************************************/

#define HIADJ(arg)      (arg)@ha
#define HI(arg)         (arg)@h
#define LO(arg)         (arg)@l

/*********************************************************************************************************
  GP register names
*********************************************************************************************************/

#define R0          0
#define R1          1
#define R2          2
#define R3          3
#define R4          4
#define R5          5
#define R6          6
#define R7          7
#define R8          8
#define R9          9
#define R10         10
#define R11         11
#define R12         12
#define R13         13
#define R14         14
#define R15         15
#define R16         16
#define R17         17
#define R18         18
#define R19         19
#define R20         20
#define R21         21
#define R22         22
#define R23         23
#define R24         24
#define R25         25
#define R26         26
#define R27         27
#define R28         28
#define R29         29
#define R30         30
#define R31         31

/*********************************************************************************************************
  GP register definitions
*********************************************************************************************************/
/*********************************************************************************************************
  Volatile registers that are not saved across subroutine calls
*********************************************************************************************************/

#define P0          R3                      /*  Argument register, volatile                             */
#define P1          R4                      /*  Argument register, volatile                             */
#define P2          R5                      /*  Argument register, volatile                             */
#define P3          R6                      /*  Argument register, volatile                             */
#define P4          R7                      /*  Argument register, volatile                             */
#define P5          R8                      /*  Argument register, volatile                             */
#define P6          R9                      /*  Argument register, volatile                             */
#define P7          R10                     /*  Argument register, volatile                             */
#define GLR0        R0                      /*  Prologs(PO,EABI), epilogs, glink routines(EABI) /
                                             *  language specific purpose(SVR4), volatile               */
#define GLR1        R11                     /*  Prologs, epilogs, as Pascal environment pointer(EABI)
                                             *  language specific purpose (SVR4)
                                             *  calls by pointer, as Pascal environment(PO),
                                             *  volatile                                                */
#define GLR2        R12                     /*  Prologs, epilogs, glink routines, calls by
                                             *  pointer(EABI), language specific purpose (SVR4),
                                             *  glue code, exception handling (PO), volatile            */
#define RETVAL0     R3                      /*  Return register 0, volatile                             */
#define RETVAL1     R4                      /*  Return register 1, volatile                             */

/*********************************************************************************************************
  Non-volatile and dedicated registers saved across subroutine calls
*********************************************************************************************************/

#define SP          R1                      /*  Stack pointer, dedicated                                */
#define T0          R14                     /*  Temporary registers, non-volatile                       */
#define T1          R15                     /*  Temporary registers, non-volatile                       */
#define T2          R16                     /*  Temporary registers, non-volatile                       */
#define T3          R17                     /*  Temporary registers, non-volatile                       */
#define T4          R18                     /*  Temporary registers, non-volatile                       */
#define T5          R19                     /*  Temporary registers, non-volatile                       */
#define T6          R20                     /*  Temporary registers, non-volatile                       */
#define T7          R21                     /*  Temporary registers, non-volatile                       */
#define T8          R22                     /*  Temporary registers, non-volatile                       */
#define T9          R23                     /*  Temporary registers, non-volatile                       */
#define T10         R24                     /*  Temporary registers, non-volatile                       */
#define T11         R25                     /*  Temporary registers, non-volatile                       */
#define T12         R26                     /*  Temporary registers, non-volatile                       */
#define T13         R27                     /*  Temporary registers, non-volatile                       */
#define T14         R28                     /*  Temporary registers, non-volatile                       */
#define T15         R29                     /*  Temporary registers, non-volatile                       */
#define T16         R30                     /*  Temporary registers, non-volatile                       */
#define FP          R31                     /*  Frame pointer, non-volatile                             */

/*********************************************************************************************************
  FP register names
*********************************************************************************************************/

#define FPR0        0
#define FPR1        1
#define FPR2        2
#define FPR3        3
#define FPR4        4
#define FPR5        5
#define FPR6        6
#define FPR7        7
#define FPR8        8
#define FPR9        9
#define FPR10       10
#define FPR11       11
#define FPR12       12
#define FPR13       13
#define FPR14       14
#define FPR15       15
#define FPR16       16
#define FPR17       17
#define FPR18       18
#define FPR19       19
#define FPR20       20
#define FPR21       21
#define FPR22       22
#define FPR23       23
#define FPR24       24
#define FPR25       25
#define FPR26       26
#define FPR27       27
#define FPR28       28
#define FPR29       29
#define FPR30       30
#define FPR31       31

/*********************************************************************************************************
  Condition register names
*********************************************************************************************************/

#define CR0         0
#define CR1         1
#define CR2         2
#define CR3         3
#define CR4         4
#define CR5         5
#define CR6         6
#define CR7         7

/*********************************************************************************************************
  ALTIVEC vector register names
*********************************************************************************************************/

#define V0          0
#define V1          1
#define V2          2
#define V3          3
#define V4          4
#define V5          5
#define V6          6
#define V7          7
#define V8          8
#define V9          9
#define V10         10
#define V11         11
#define V12         12
#define V13         13
#define V14         14
#define V15         15
#define V16         16
#define V17         17
#define V18         18
#define V19         19
#define V20         20
#define V21         21
#define V22         22
#define V23         23
#define V24         24
#define V25         25
#define V26         26
#define V27         27
#define V28         28
#define V29         29
#define V30         30
#define V31         31

/*********************************************************************************************************
  Special register definitions
*********************************************************************************************************/

#define XER         1                       /*  External exception register                             */
#define LR          8                       /*  Link register (return address)                          */
#define CTR         9                       /*  Counter register                                        */

#define DSISR       18                      /*  Data storage interrupt status                           */
#define DAR         19                      /*  Data address register                                   */
#define SDR1        25                      /*  Storage description register 1                          */

#define DEC         22                      /*  Decrement register                                      */

#define SRR0        26                      /*  Save and restore register 0                             */
#define SRR1        27                      /*  Save and restore register 1                             */

#define SPRG0       272                     /*  Software program register 0                             */
#define SPRG1       273                     /*  Software program register 1                             */
#define SPRG2       274                     /*  Software program register 2                             */
#define SPRG3       275                     /*  Software program register 3                             */

#define ASR         280                     /*  Address space register
                                             *  (64 bit implementation only)                            */
#define EAR         282                     /*  External address register                               */

#define TBL         284                     /*  Lower time base register                                */
#define TBU         285                     /*  Upper time base register                                */

#define PVR         287                     /*  Processor version register                              */

#define IBAT0U      528                     /*  Instruction BAT register                                */
#define IBAT0L      529                     /*  Instruction BAT register                                */
#define IBAT1U      530                     /*  Instruction BAT register                                */
#define IBAT1L      531                     /*  Instruction BAT register                                */
#define IBAT2U      532                     /*  Instruction BAT register                                */
#define IBAT2L      533                     /*  Instruction BAT register                                */
#define IBAT3U      534                     /*  Instruction BAT register                                */
#define IBAT3L      535                     /*  Instruction BAT register                                */

#define DBAT0U      536                     /*  Data BAT register                                       */
#define DBAT0L      537                     /*  Data BAT register                                       */
#define DBAT1U      538                     /*  Data BAT register                                       */
#define DBAT1L      539                     /*  Data BAT register                                       */
#define DBAT2U      540                     /*  Data BAT register                                       */
#define DBAT2L      541                     /*  Data BAT register                                       */
#define DBAT3U      542                     /*  Data BAT register                                       */
#define DBAT3L      543                     /*  Data BAT register                                       */

#define IBAT4U      560                     /*  Instruction BAT register                                */
#define IBAT4L      561                     /*  Instruction BAT register                                */
#define IBAT5U      562                     /*  Instruction BAT register                                */
#define IBAT5L      563                     /*  Instruction BAT register                                */
#define IBAT6U      564                     /*  Instruction BAT register                                */
#define IBAT6L      565                     /*  Instruction BAT register                                */
#define IBAT7U      566                     /*  Instruction BAT register                                */
#define IBAT7L      567                     /*  Instruction BAT register                                */

#define DBAT4U      568                     /*  Data BAT register                                       */
#define DBAT4L      569                     /*  Data BAT register                                       */
#define DBAT5U      570                     /*  Data BAT register                                       */
#define DBAT5L      571                     /*  Data BAT register                                       */
#define DBAT6U      572                     /*  Data BAT register                                       */
#define DBAT6L      573                     /*  Data BAT register                                       */
#define DBAT7U      574                     /*  Data BAT register                                       */
#define DBAT7L      575                     /*  Data BAT register                                       */



/*********************************************************************************************************
  Lower Machine State Register (MSR) mask
*********************************************************************************************************/
#define ARCH_PPC_INT_MASK(src, des) \
    RLWINM  des, src, 0, ARCH_PPC_MSR_BIT_EE + 1, ARCH_PPC_MSR_BIT_EE - 1

#define ARCH_PPC_MSR_BIT_EE         16              /*  MSR Ext. Intr. Enable bit - EE                  */
#define ARCH_PPC_MSR_BIT_PR         17              /*  MSR Privilege Level bit - PR                    */
#define ARCH_PPC_MSR_BIT_ME         19              /*  MSR Machine Check Enable bit - ME               */
#define ARCH_PPC_MSR_BIT_LE         31              /*  MSR Little Endian mode bit - LE                 */


#define ARCH_PPC_MSR_EE             0x8000          /*  External interrupt enable                       */
#define ARCH_PPC_MSR_PR             0x4000          /*  Privilege level                                 */
#define ARCH_PPC_MSR_FP             0x2000          /*  Floating-point available                        */
#define ARCH_PPC_MSR_ME             0x1000          /*  Machine check enable                            */
#define ARCH_PPC_MSR_FE0            0x0800          /*  Floating-point exception mode 0                 */
#define ARCH_PPC_MSR_SE             0x0400          /*  Single-step trace enable                        */
#define ARCH_PPC_MSR_BE             0x0200          /*  Branch trace enable                             */
#define ARCH_PPC_MSR_FE1            0x0100          /*  Floating-point exception mode 1                 */
#define ARCH_PPC_MSR_IP             0x0040          /*  Exception prefix                                */
#define ARCH_PPC_MSR_IR             0x0020          /*  Instruction address translation                 */
#define ARCH_PPC_MSR_DR             0x0010          /*  Data address translation                        */
#define ARCH_PPC_MSR_RI             0x0002          /*  Recoverable interrupt                           */
#define ARCH_PPC_MSR_LE             0x0001          /*  Little-endian mode                              */

#define ARCH_PPC_MSR_POWERUP        0x0040          /*  State of MSR at powerup                         */



#endif                                                                  /*  __ASSEMBLY__                */

#endif 																		/* __PPC_ASSEMBLER_H 		*/
/*********************************************************************************************************
  END
*********************************************************************************************************/
