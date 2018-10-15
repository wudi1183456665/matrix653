/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: arch_regs32.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2016 年 06 月 25 日
**
** 描        述: x86 寄存器相关.
*********************************************************************************************************/

#ifndef __X86_ARCH_REGS32_H
#define __X86_ARCH_REGS32_H

#ifndef ASSEMBLY

/*********************************************************************************************************
  寄存器表
*********************************************************************************************************/

typedef UINT32      ARCH_REG_T;

struct __ARCH_REG_CTX {
    UINT16          REG_usGS;                                           /*  6 个段寄存器                */
    UINT16          REG_usFS;
    UINT16          REG_usES;
    UINT16          REG_usDS;
    UINT16          REG_usSS;
    UINT16          REG_usPad;

    UINT32          REG_uiEAX;                                          /*  4 个数据寄存器              */
    UINT32          REG_uiEBX;
    UINT32          REG_uiECX;
    UINT32          REG_uiEDX;

    UINT32          REG_uiESI;                                          /*  2 个变址和指针寄存器        */
    UINT32          REG_uiEDI;
    UINT32          REG_uiEBP;                                          /*  指针寄存器(RBP)             */

    UINT32          REG_uiError;                                        /*  ERROR CODE                  */
    UINT32          REG_uiEIP;                                          /*  指令指针寄存器(EIP)         */
    UINT32          REG_uiCS;                                           /*  代码段寄存器(CS)            */
    UINT32          REG_uiEFLAGS;                                       /*  标志寄存器(EFLAGS)          */
} __attribute__((packed));

typedef struct __ARCH_REG_CTX   ARCH_REG_CTX;
typedef          ARCH_REG_CTX  *PARCH_REG_CTX;

/*********************************************************************************************************
  从上下文中获取信息
*********************************************************************************************************/

#define ARCH_REG_CTX_GET_PC(ctx)    ((PVOID)(ctx).REG_uiEIP)
#define ARCH_REG_CTX_GET_FRAME(ctx) ((PVOID)(ctx).REG_uiEBP)

#endif                                                                  /*  !defined(ASSEMBLY)          */

/*********************************************************************************************************
  堆栈中的寄存器信息所占大小
*********************************************************************************************************/

#define ARCH_REG_CTX_WORD_SIZE  14
#define ARCH_STK_MIN_WORD_SIZE  256

/*********************************************************************************************************
  堆栈对齐要求
*********************************************************************************************************/

#define ARCH_STK_ALIGN_SIZE     8

/*********************************************************************************************************
  syscall 参数定义
*********************************************************************************************************/

typedef ARCH_REG_CTX    SC_ARG;
typedef SC_ARG         *PSC_ARG;

#define SCARG_ulArg0    REG_uiEAX
#define SCARG_ulArg1    REG_uiEDI
#define SCARG_ulArg2    REG_uiESI
#define SCARG_ulArg3    REG_uiEBX

/*
 * 设置 syscall time
 */
#define ARCH_TIME_SET(pscArg, ullTimeout) do {                  \
            pscArg->SCARG_ulArg2 = (UINT32)ullTimeout;          \
            pscArg->SCARG_ulArg3 = (UINT32)(ullTimeout >> 32);  \
        } while(0)

/*
 * 获取 syscall time
 */
#define ARCH_TIME_GET(pscArg)                                   \
        (((0ULL | pscArg->SCARG_ulArg3) << 32) |                \
          (0ULL | pscArg->SCARG_ulArg2))

#endif                                                                  /*  __X86_ARCH_REGS32_H         */
/*********************************************************************************************************
  END
*********************************************************************************************************/
