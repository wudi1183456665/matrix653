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
# Source list
#*********************************************************************************************************
LOCAL_SRCS := \
./l4startup.S \
./l4main.c \
./SylixOS/bsp/bspLib.c \
./SylixOS/bsp/bspInit.c \
./SylixOS/user/main.c
#./SylixOS/driver/uart.c \
#./SylixOS/driver/sio.c \

LOCAL_SRCS += ./SylixOS/bsp/sylixos_evm/sylixos_evm.c
#LOCAL_SRCS += ./SylixOS/driver/ahci/ahciImx.c
LOCAL_SRCS += ./SylixOS/driver/ccm_pll/ccm_pll.c
#LOCAL_SRCS += ./SylixOS/driver/cpu_utility/cpu_multicore.c
#LOCAL_SRCS += ./SylixOS/driver/ecspi/ecspi.c
#LOCAL_SRCS += ./SylixOS/driver/eim/eim.c
#LOCAL_SRCS += ./SylixOS/driver/eim/eim_hal.c
#LOCAL_SRCS += ./SylixOS/driver/flexcan/flexcan.c
LOCAL_SRCS += ./SylixOS/driver/gpio/imx6q_gpio.c
#LOCAL_SRCS += ./SylixOS/driver/hdmi/hdmi_tx.c
#LOCAL_SRCS += ./SylixOS/driver/hdmi/imx6q_hdmi.c
#LOCAL_SRCS += ./SylixOS/driver/i2c/imx6q_i2c.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/imx6q_fb.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ips_csc.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_common.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_csi.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_dc.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_di.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_dmfc.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_dp.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_ic.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_idmac.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ipu_vdi.c
#LOCAL_SRCS += ./SylixOS/driver/ipu_fb/ldb.c
#LOCAL_SRCS += ./SylixOS/driver/irq/armGic.c
#LOCAL_SRCS += ./SylixOS/driver/lcd/imx6q_lcd.c
LOCAL_SRCS += ./SylixOS/driver/netif/imx6q_netif.c
LOCAL_SRCS += ./SylixOS/driver/netif/imx6q_phy.c
LOCAL_SRCS += ./SylixOS/driver/pinmux/gpio_iomux_config.c
#LOCAL_SRCS += ./SylixOS/driver/pinmux/iomux_config.c
#LOCAL_SRCS += ./SylixOS/driver/pwm/imx6q_pwm.c
#LOCAL_SRCS += ./SylixOS/driver/rtc/imx6q_rtc.c
#LOCAL_SRCS += ./SylixOS/driver/rtc_isl1208/rtc_isl1208.c
LOCAL_SRCS += ./SylixOS/driver/sdi/imx6q_sdi.c
#LOCAL_SRCS += ./SylixOS/driver/spi_nor/spi_nor.c
#LOCAL_SRCS += ./SylixOS/driver/tempmon/imx6q_tempmon.c
#LOCAL_SRCS += ./SylixOS/driver/timer/armGlobalTimer.c
#LOCAL_SRCS += ./SylixOS/driver/timer/armPrivateTimer.c
#LOCAL_SRCS += ./SylixOS/driver/touch/chip/ft5x06/ft5x06.c
#LOCAL_SRCS += ./SylixOS/driver/touch/chip/gt911/gt911.c
#LOCAL_SRCS += ./SylixOS/driver/touch/touch.c
LOCAL_SRCS += ./SylixOS/driver/uart/sio.c
#LOCAL_SRCS += ./SylixOS/driver/usb/imx6q_usb.c
#LOCAL_SRCS += ./SylixOS/driver/wdt/imx6q_wdt.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your hearder files search path")
#*********************************************************************************************************
LOCAL_INC_PATH := -I"$(MATRIX_BASE_PATH)/l4matrix/matrix"
LOCAL_INC_PATH += -I"$(MATRIX_BASE_PATH)/l4matrix/matrix/include"
LOCAL_INC_PATH += -I"$(MATRIX_BASE_PATH)/l4libc/libc"
LOCAL_INC_PATH += -I"$(MATRIX_BASE_PATH)/l4api/api"
LOCAL_INC_PATH += -I"$(SYLIXOS_BASE_PATH)/libsylixos/SylixOS"
LOCAL_INC_PATH += -I"$(SYLIXOS_BASE_PATH)/libsylixos/SylixOS/include"
LOCAL_INC_PATH += -I"$(SYLIXOS_BASE_PATH)/libsylixos/SylixOS/include/network"
LOCAL_INC_PATH += -I"./SylixOS/bsp"
LOCAL_INC_PATH += -I"./SylixOS/driver"
LOCAL_INC_PATH += -I"./SylixOS/driver/include"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := 

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB      := -lsylixos
LOCAL_DEPEND_LIB_PATH := -L"$(MATRIX_BASE_PATH)/sylixos/$(OUTDIR)"
LOCAL_DEPEND_LIB      += -ll4libc
LOCAL_DEPEND_LIB_PATH += -L"$(MATRIX_BASE_PATH)/l4libc/$(OUTDIR)"
LOCAL_DEPEND_LIB      += -ll4api
LOCAL_DEPEND_LIB_PATH += -L"$(MATRIX_BASE_PATH)/l4api/$(OUTDIR)"

#*********************************************************************************************************
# Link script file
#*********************************************************************************************************
LOCAL_LD_SCRIPT := SylixOSBSP.ld

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

include $(L4SYLIXOS_BSP_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************
