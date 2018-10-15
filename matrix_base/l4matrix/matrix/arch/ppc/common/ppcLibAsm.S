;/*********************************************************************************************************
;**
;**                                    中国软件开源组织
;**
;**                                   嵌入式实时操作系统
;**
;**                                       SylixOS(TM)
;**
;**                               Copyright  All Rights Reserved
;**
;**--------------文件信息--------------------------------------------------------------------------------
;**
;** 文   件   名: ppcLibAsm.S
;**
;** 创   建   人: Li.Yanqi
;**
;** 文件创建日期: 2018 年 07 月 06 日
;**
;** 描        述: PowerPC 体系构架内部库.
;*********************************************************************************************************/

#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/arch_assembler.h>
;#include <matrix/ppc/ppc_regs.h>

	FILE_BEGIN()
	EXPORT_LABEL(archIntDisable)
	EXPORT_LABEL(archIntEnable)
	EXPORT_LABEL(archIntEnableForce)

	EXPORT_LABEL(archFindLsb)
    EXPORT_LABEL(archFindMsb)


;/*********************************************************************************************************
;  PowerPC 关闭总中断
;  注意: 这里没有关闭机器检查异常和临界输入异常(E500 才有)
;*********************************************************************************************************/
FUNC_DEF(archIntDisable)
	MFMSR	R3
	ISYNC
	OR		R4 , R3 , R3
	ARCH_PPC_INT_MASK(R4, R4)
	ISYNC
	MTMSR	R4
	ISYNC
	BLR
	FUNC_END()

FUNC_DEF(archIntEnable)
	SYNC
	RLWINM	R4 , R3 , 0 , ARCH_PPC_MSR_BIT_EE , ARCH_PPC_MSR_BIT_EE
	CMPWI	CR0, R4 , 0
	BEQ		CR0, __archIntIsDisable
	MFMSR	R3
	ISYNC
	ORI		R3 , R3 , ARCH_PPC_MSR_EE
	ISYNC
	MTMSR	R3
	ISYNC
LINE_LABEL(__archIntIsDisable)
	BLR
	FUNC_END()

FUNC_DEF(archIntEnableForce)
    MFMSR   R3
    ISYNC
    ORI     R3 , R3 , ARCH_PPC_MSR_EE
    ISYNC
    MTMSR   R3
    ISYNC
    BLR
    FUNC_END()

;/*********************************************************************************************************
;  PowerPC 前导零运算
;*********************************************************************************************************/

FUNC_DEF(archFindLsb)
    NEG     R4 , R3
    AND     R3 , R3 , R4
    CNTLZW  R3 , R3                                                     ;/*  R3 = # leading zeros        */
    SUBFIC  R3 , R3 , 32                                                ;/*  R3 = 32 - R3                */
    BLR
    FUNC_END()

FUNC_DEF(archFindMsb)
    CNTLZW  R3 , R3                                                     ;/*  R3 = # leading zeros        */
    SUBFIC  R3 , R3 , 32                                                ;/*  R3 = 32 - R3                */
    BLR
    FUNC_END()


    FILE_END()
;/*********************************************************************************************************
;  END
;*********************************************************************************************************/
