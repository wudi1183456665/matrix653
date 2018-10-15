#*********************************************************************************************************
#
#                                    �й�������Դ��֯
#
#                                   Ƕ��ʽʵʱ����ϵͳ
#
#                                SylixOS(TM)  LW : long wing
#
#                               Copyright All Rights Reserved
#
#--------------�ļ���Ϣ--------------------------------------------------------------------------------
#
# ��   ��   ��: libl4matrix.mk
#
# ��   ��   ��: RealEvo-IDE
#
# �ļ���������: 2016 �� 10 �� 08 ��
#
# ��        ��: ���ļ��� RealEvo-IDE ���ɣ��������� Makefile ���ܣ������ֶ��޸�

# BUG
# 2018.02.03  ���Ӳ�������ʱ�����ر����ļ� (matrix/test/env)
#*********************************************************************************************************

#*********************************************************************************************************
# Clear setting
#*********************************************************************************************************
include $(CLEAR_VARS_MK)

#*********************************************************************************************************
# Target
#*********************************************************************************************************
LOCAL_TARGET_NAME := libl4matrix.a

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
#*********************************************************************************************************
# ARM source
#*********************************************************************************************************

LOCAL_PPC_SRCS = \
matrix/arch/ppc/common/ppcAssert.c \
matrix/arch/ppc/common/ppcCtx.c \
matrix/arch/ppc/common/ppcCtxAsm.S \
matrix/arch/ppc/common/ppcExc.c \
matrix/arch/ppc/common/ppcExcAsm.S \
matrix/arch/ppc/common/ppcLibAsm.S \
matrix/arch/ppc/common/ppcSprAsm.S \
matrix/arch/ppc/mm/ppcMmu.c \
matrix/arch/ppc/mm/ppcMmuAsm.S \
matrix/arch/ppc/mm/ppcMmuHashPageTbl.c \
matrix/arch/ppc/mm/ppcCache.c \
matrix/arch/ppc/mm/cache/common/ppcCache.c \
matrix/arch/ppc/mm/cache/l2/ppcL2Cache750.c \
matrix/arch/ppc/mm/cache/l2/ppcL2Cache750Asm.S \
matrix/arch/ppc/mm/cache/ppc60x/ppcCache60xAsm.S \
matrix/arch/ppc/mm/cache/ppc60x/ppcCache750.c \
matrix/arch/ppc/mm/cache/ppc60x/ppcCache750Asm.S


#*********************************************************************************************************
# Kernel source
#*********************************************************************************************************
KERN_SRCS := \
matrix/core/sched/_BitmapLib.c \
matrix/core/sched/_CandTable.c \
matrix/core/sched/_ReadyTable.c \
matrix/core/sched/_RdyPrio.c \
matrix/core/sched/_Wakeup.c \
matrix/core/sched/_SmpIpi.c \
matrix/core/virq/_vIrq.c \
matrix/core/init.c \
matrix/core/ipc.c \
matrix/core/irq.c \
matrix/core/spin.c \
matrix/core/kobj.c \
matrix/core/kernel.c \
matrix/core/partition.c \
matrix/core/sched.c \
matrix/core/syscall.c \
matrix/core/vcpu.c \
matrix/core/heap.c \
matrix/list/list.c

#*********************************************************************************************************
# Debug source
#*********************************************************************************************************
DBUG_SRCS := \
matrix/debug/dump.c

#*********************************************************************************************************
# Kernel source
#*********************************************************************************************************
LOCAL_SRCS := $(KERN_SRCS)
LOCAL_SRCS += $(DBUG_SRCS)

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your hearder files search path")
#*********************************************************************************************************
LOCAL_INC_PATH := -I"./matrix"
LOCAL_INC_PATH += -I"./matrix/include"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := 

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB      := 
LOCAL_DEPEND_LIB_PATH := 

#*********************************************************************************************************
# C++ config
#*********************************************************************************************************
LOCAL_USE_CXX        := no
LOCAL_USE_CXX_EXCEPT := no

#*********************************************************************************************************
# Code coverage config
#*********************************************************************************************************
LOCAL_USE_GCOV := no

include $(L4BASE_LIBRARY_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************