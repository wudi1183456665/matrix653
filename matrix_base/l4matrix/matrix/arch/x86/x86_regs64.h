/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: arch_regs64.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2017 �� 06 �� 05 ��
**
** ��        ��: x86-64 �Ĵ������.

** BUG
2019.01.30  ����ϵͳ���ò���
*********************************************************************************************************/

#ifndef __X86_ARCH_REGS64_H
#define __X86_ARCH_REGS64_H

#ifndef ASSEMBLY

/*********************************************************************************************************
  �Ĵ�����
*********************************************************************************************************/

typedef UINT64      ARCH_REG_T;

struct __ARCH_REG_CTX {
    UINT16          REG_usGS;                                           /*  4 ���μĴ���                */
    UINT16          REG_usFS;
    UINT16          REG_usES;
    UINT16          REG_usDS;

    UINT64          REG_ulRAX;                                          /*  4 �����ݼĴ���              */
    UINT64          REG_ulRBX;
    UINT64          REG_ulRCX;
    UINT64          REG_ulRDX;

    UINT64          REG_ulRSI;                                          /*  2 ����ַ��ָ��Ĵ���        */
    UINT64          REG_ulRDI;
    UINT64          REG_ulRBP;                                          /*  ָ��Ĵ���(RBP)             */

    UINT64          REG_ulR8;                                           /*  8 �����ݼĴ���              */
    UINT64          REG_ulR9;
    UINT64          REG_ulR10;
    UINT64          REG_ulR11;
    UINT64          REG_ulR12;
    UINT64          REG_ulR13;
    UINT64          REG_ulR14;
    UINT64          REG_ulR15;

    UINT64          REG_ulError;                                        /*  ERROR CODE                  */
    UINT64          REG_ulRIP;                                          /*  ָ��ָ��Ĵ���(RIP)         */
    UINT64          REG_ulCS;                                           /*  ����μĴ���(CS)            */
    UINT64          REG_ulRFLAGS;                                       /*  ��־�Ĵ���(RFLAGS)          */
    UINT64          REG_ulRSP;                                          /*  ԭջָ��Ĵ���(RSP)         */
    UINT64          REG_ulSS;                                           /*  ջ�μĴ���(SS)              */
} __attribute__((packed));

typedef struct __ARCH_REG_CTX    ARCH_REG_CTX;
typedef          ARCH_REG_CTX   *PARCH_REG_CTX;

/*********************************************************************************************************
  ���������л�ȡ��Ϣ
*********************************************************************************************************/

#define ARCH_REG_CTX_GET_PC(ctx)    ((PVOID)(ctx).REG_ulRIP)
#define ARCH_REG_CTX_GET_FRAME(ctx) ((PVOID)(ctx).REG_ulRBP)

#endif                                                                  /*  !defined(ASSEMBLY)          */

/*********************************************************************************************************
  ��ջ�еļĴ�����Ϣ��ռ��С
*********************************************************************************************************/

#define ARCH_REG_CTX_WORD_SIZE  22
#define ARCH_STK_MIN_WORD_SIZE  512

/*********************************************************************************************************
  ��ջ����Ҫ��
*********************************************************************************************************/

#define ARCH_STK_ALIGN_SIZE     16

/*********************************************************************************************************
  �Ĵ����������������е�ƫ����
*********************************************************************************************************/

#define ARCH_REG_OFFSET_GS      0
#define ARCH_REG_OFFSET_FS      2
#define ARCH_REG_OFFSET_ES      4
#define ARCH_REG_OFFSET_DS      6
#define ARCH_REG_OFFSET_RAX     8
#define ARCH_REG_OFFSET_RBX     16
#define ARCH_REG_OFFSET_RCX     24
#define ARCH_REG_OFFSET_RDX     32
#define ARCH_REG_OFFSET_RSI     40
#define ARCH_REG_OFFSET_RDI     48
#define ARCH_REG_OFFSET_RBP     56
#define ARCH_REG_OFFSET_R8      64
#define ARCH_REG_OFFSET_R9      72
#define ARCH_REG_OFFSET_R10     80
#define ARCH_REG_OFFSET_R11     88
#define ARCH_REG_OFFSET_R12     96
#define ARCH_REG_OFFSET_R13     104
#define ARCH_REG_OFFSET_R14     112
#define ARCH_REG_OFFSET_R15     120
#define ARCH_REG_OFFSET_ERROR   128
#define ARCH_REG_OFFSET_RIP     136
#define ARCH_REG_OFFSET_CS      144
#define ARCH_REG_OFFSET_RFLAGS  152
#define ARCH_REG_OFFSET_RSP     160
#define ARCH_REG_OFFSET_SS      168

/*********************************************************************************************************
  syscall ��������
*********************************************************************************************************/

typedef ARCH_REG_CTX    SC_ARG;
typedef SC_ARG         *PSC_ARG;

#define SCARG_ulArg0    REG_ulRAX
#define SCARG_ulArg1    REG_ulRDI
#define SCARG_ulArg2    REG_ulRSI
#define SCARG_ulArg3    REG_ulRBX

/*
 * ���� syscall time
 */
#define ARCH_TIME_SET(pscArg, ullTimeout) do {                  \
            pscArg->SCARG_ulArg2 = ullTimeout;                  \
        } while(0)
/*
 * ��ȡ syscall time
 */
#define ARCH_TIME_GET(pscArg)                                   \
        (pscArg->SCARG_ulArg2)

#endif                                                                  /*  __X86_ARCH_REGS64_H         */
/*********************************************************************************************************
  END
*********************************************************************************************************/
