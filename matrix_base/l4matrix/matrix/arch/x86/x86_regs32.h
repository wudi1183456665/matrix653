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
** ��   ��   ��: arch_regs32.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2016 �� 06 �� 25 ��
**
** ��        ��: x86 �Ĵ������.
*********************************************************************************************************/

#ifndef __X86_ARCH_REGS32_H
#define __X86_ARCH_REGS32_H

#ifndef ASSEMBLY

/*********************************************************************************************************
  �Ĵ�����
*********************************************************************************************************/

typedef UINT32      ARCH_REG_T;

struct __ARCH_REG_CTX {
    UINT16          REG_usGS;                                           /*  6 ���μĴ���                */
    UINT16          REG_usFS;
    UINT16          REG_usES;
    UINT16          REG_usDS;
    UINT16          REG_usSS;
    UINT16          REG_usPad;

    UINT32          REG_uiEAX;                                          /*  4 �����ݼĴ���              */
    UINT32          REG_uiEBX;
    UINT32          REG_uiECX;
    UINT32          REG_uiEDX;

    UINT32          REG_uiESI;                                          /*  2 ����ַ��ָ��Ĵ���        */
    UINT32          REG_uiEDI;
    UINT32          REG_uiEBP;                                          /*  ָ��Ĵ���(RBP)             */

    UINT32          REG_uiError;                                        /*  ERROR CODE                  */
    UINT32          REG_uiEIP;                                          /*  ָ��ָ��Ĵ���(EIP)         */
    UINT32          REG_uiCS;                                           /*  ����μĴ���(CS)            */
    UINT32          REG_uiEFLAGS;                                       /*  ��־�Ĵ���(EFLAGS)          */
} __attribute__((packed));

typedef struct __ARCH_REG_CTX   ARCH_REG_CTX;
typedef          ARCH_REG_CTX  *PARCH_REG_CTX;

/*********************************************************************************************************
  ���������л�ȡ��Ϣ
*********************************************************************************************************/

#define ARCH_REG_CTX_GET_PC(ctx)    ((PVOID)(ctx).REG_uiEIP)
#define ARCH_REG_CTX_GET_FRAME(ctx) ((PVOID)(ctx).REG_uiEBP)

#endif                                                                  /*  !defined(ASSEMBLY)          */

/*********************************************************************************************************
  ��ջ�еļĴ�����Ϣ��ռ��С
*********************************************************************************************************/

#define ARCH_REG_CTX_WORD_SIZE  14
#define ARCH_STK_MIN_WORD_SIZE  256

/*********************************************************************************************************
  ��ջ����Ҫ��
*********************************************************************************************************/

#define ARCH_STK_ALIGN_SIZE     8

/*********************************************************************************************************
  syscall ��������
*********************************************************************************************************/

typedef ARCH_REG_CTX    SC_ARG;
typedef SC_ARG         *PSC_ARG;

#define SCARG_ulArg0    REG_uiEAX
#define SCARG_ulArg1    REG_uiEDI
#define SCARG_ulArg2    REG_uiESI
#define SCARG_ulArg3    REG_uiEBX

/*
 * ���� syscall time
 */
#define ARCH_TIME_SET(pscArg, ullTimeout) do {                  \
            pscArg->SCARG_ulArg2 = (UINT32)ullTimeout;          \
            pscArg->SCARG_ulArg3 = (UINT32)(ullTimeout >> 32);  \
        } while(0)

/*
 * ��ȡ syscall time
 */
#define ARCH_TIME_GET(pscArg)                                   \
        (((0ULL | pscArg->SCARG_ulArg3) << 32) |                \
          (0ULL | pscArg->SCARG_ulArg2))

#endif                                                                  /*  __X86_ARCH_REGS32_H         */
/*********************************************************************************************************
  END
*********************************************************************************************************/
