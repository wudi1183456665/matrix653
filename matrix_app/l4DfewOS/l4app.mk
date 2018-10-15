#*********************************************************************************************************
#
#                                    中国软件开源组织
#
#                                   嵌入式实时操作系统
#
#                                SylixOS(TM)  LW : long wing
#
#                               Copyright All Rights Reserved
#
#--------------文件信息--------------------------------------------------------------------------------
#
# 文   件   名: libl4loader.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2016 年 10 月 08 日
#
# 描        述: 本文件由 RealEvo-IDE 生成，用于配置 Makefile 功能，请勿手动修改
#*********************************************************************************************************

#*********************************************************************************************************
# Clear setting
#*********************************************************************************************************
include $(CLEAR_VARS_MK)

#*********************************************************************************************************
# Target
#*********************************************************************************************************
LOCAL_TARGET_NAME := app.elf

#*********************************************************************************************************
# DfewOS Source list
#*********************************************************************************************************
DF_SRCS=./DfewOS/main.c \
	    ./DfewOS/atomic.c \
 	    ./DfewOS/cmd.c \
 	    ./DfewOS/delayQ.c \
	    ./DfewOS/dlist.c \
	    ./DfewOS/fifo.c \
	    ./DfewOS/memH.c \
	    ./DfewOS/msgQ.c \
 	    ./DfewOS/readyQ.c \
 	    ./DfewOS/semB.c \
 	    ./DfewOS/semC.c \
 	    ./DfewOS/semM.c \
 	    ./DfewOS/serial.c \
 	    ./DfewOS/string.c \
 	    ./DfewOS/shell.c \
 	    ./DfewOS/task.c \
 	    ./DfewOS/tick.c \
		\
 	    ./DfewOS/bsp/cpu.c \
 	    ./DfewOS/bsp/uart.c \
 	    \
 	    ./DfewOS/calculator/cal_cmd.c \
 	    ./DfewOS/calculator/calculator.c \
 	    \
 	    ./DfewOS/date/date_cmd.c \
 	    ./DfewOS/date/nongli.c \
 	    ./DfewOS/date/date.c \
 	    ./DfewOS/date/timeLib.c \
 	    \
 	    ./DfewOS/lianliankan/lian_cmd.c \
 	    ./DfewOS/lianliankan/src/bridge.c \
 	    ./DfewOS/lianliankan/src/lian.c \
 	    ./DfewOS/lianliankan/src/tty.c \
 	    \
 	    ./DfewOS/tetris/tetris_cmd.c \
 	    ./DfewOS/tetris/tty_tetris.c \
 	    ./DfewOS/tetris/src/auto.c \
 	    ./DfewOS/tetris/src/bridge.c \
 	    ./DfewOS/tetris/src/os.c \
 	    ./DfewOS/tetris/src/tetris.c \
		\
		./DfewOS/snake/snake_cmd.c \
		./DfewOS/snake/snake.c \
		\
 	    ./DfewOS/wuzi/wuzi.c

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS := \
./startup.S \
./main.c

LOCAL_SRCS  += $(DF_SRCS)

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your hearder files search path")
#*********************************************************************************************************
LOCAL_INC_PATH := -I"$(MATRIX_BASE_PATH)/l4matrix/matrix"
LOCAL_INC_PATH += -I"$(MATRIX_BASE_PATH)/l4matrix/matrix/include"
LOCAL_INC_PATH += -I"$(MATRIX_BASE_PATH)/l4libc/libc"
LOCAL_INC_PATH += -I"$(MATRIX_BASE_PATH)/l4api/api"
LOCAL_INC_PATH += -I"./DfewOS/include"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := 

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB      := -ll4libc
LOCAL_DEPEND_LIB_PATH := -L"$(MATRIX_BASE_PATH)/l4libc/$(OUTDIR)"
LOCAL_DEPEND_LIB      += -ll4api
LOCAL_DEPEND_LIB_PATH += -L"$(MATRIX_BASE_PATH)/l4api/$(OUTDIR)"

#*********************************************************************************************************
# Link script file
#*********************************************************************************************************
LOCAL_LD_SCRIPT := App.ld

#*********************************************************************************************************
# C++ config
#*********************************************************************************************************
LOCAL_USE_CXX        := no
LOCAL_USE_CXX_EXCEPT := no

#*********************************************************************************************************
# Code coverage config
#*********************************************************************************************************
LOCAL_USE_GCOV := no

#*********************************************************************************************************
# User link command
#*********************************************************************************************************
LOCAL_PRE_LINK_CMD   := 
LOCAL_POST_LINK_CMD  := 
LOCAL_PRE_STRIP_CMD  := 
LOCAL_POST_STRIP_CMD := 

include $(L4APP_BSP_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************
