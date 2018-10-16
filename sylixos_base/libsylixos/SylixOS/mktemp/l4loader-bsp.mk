#*********************************************************************************************************
#
#                                    �й������Դ��֯
#
#                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
#
#                                SylixOS(TM)  MX : matrix
#
#                               Copyright All Rights Reserved
#
#--------------�ļ���Ϣ-------------------------------------------------------------------------------
#
# ��   ��   ��: l4loader-bsp.mk (bsp.mk)
#
# ��   ��   ��: Wang.Dongfang(������)
#
# �ļ���������: 2018 �� 01 �� 09 ��
#
# ��        ��: l4loader BSP Ŀ�� makefile ģ��.
#*********************************************************************************************************

#*********************************************************************************************************
# Include common.mk
#*********************************************************************************************************
include $(MKTEMP)/l4common.mk

#*********************************************************************************************************
# Depend and compiler parameter (cplusplus in kernel MUST NOT use exceptions and rtti)
#*********************************************************************************************************
ifeq ($($(target)_USE_CXX_EXCEPT), yes)
$(target)_CXX_EXCEPT  := $(TOOLCHAIN_NO_CXX_EXCEPT_CFLAGS)
else
$(target)_CXX_EXCEPT  := $(TOOLCHAIN_NO_CXX_EXCEPT_CFLAGS)
endif

ifeq ($($(target)_USE_GCOV), yes)
$(target)_GCOV_FLAGS  := $(TOOLCHAIN_NO_GCOV_CFLAGS)
else
$(target)_GCOV_FLAGS  := $(TOOLCHAIN_NO_GCOV_CFLAGS)
endif

ifeq ($($(target)_USE_OMP), yes)
$(target)_OMP_FLAGS   := $(TOOLCHAIN_NO_OMP_CFLAGS)
else
$(target)_OMP_FLAGS   := $(TOOLCHAIN_NO_OMP_CFLAGS)
endif

$(target)_CPUFLAGS    := $(ARCH_CPUFLAGS_NOFPU) $(ARCH_KERNEL_CFLAGS)
$(target)_COMMONFLAGS := $($(target)_CPUFLAGS) $(ARCH_COMMONFLAGS) $(TOOLCHAIN_OPTIMIZE) $(TOOLCHAIN_COMMONFLAGS) $($(target)_GCOV_FLAGS) $($(target)_OMP_FLAGS)
$(target)_ASFLAGS     := $($(target)_COMMONFLAGS) $(TOOLCHAIN_ASFLAGS) $($(target)_DSYMBOL) $($(target)_INC_PATH)
$(target)_CFLAGS      := $($(target)_COMMONFLAGS) $($(target)_DSYMBOL) $($(target)_INC_PATH) $($(target)_CFLAGS)
$(target)_CXXFLAGS    := $($(target)_COMMONFLAGS) $($(target)_DSYMBOL) $($(target)_INC_PATH) $($(target)_CXX_EXCEPT) $($(target)_CXXFLAGS)

#*********************************************************************************************************
# Depend library search paths
#*********************************************************************************************************
$(target)_DEPEND_LIB_PATH := $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libsylixos/$(OUTDIR)"
$(target)_DEPEND_LIB_PATH += $(LOCAL_DEPEND_LIB_PATH)

#*********************************************************************************************************
# Depend libraries
#*********************************************************************************************************
$(target)_DEPEND_LIB := $(LOCAL_DEPEND_LIB)
$(target)_DEPEND_LIB +=

ifeq ($($(target)_USE_CXX), yes)
$(target)_DEPEND_LIB += $(TOOLCHAIN_LINK_CXX)
endif

ifeq ($($(target)_USE_GCOV), yes)
endif

ifeq ($($(target)_USE_OMP), yes)
endif

$(target)_DEPEND_LIB += $(TOOLCHAIN_LINK_M) $(TOOLCHAIN_LINK_GCC)

#*********************************************************************************************************
# Targets
#*********************************************************************************************************
$(target)_IMG       := $(OUTPATH)/$(LOCAL_TARGET_NAME)
$(target)_STRIP_IMG := $(OUTPATH)/strip/$(LOCAL_TARGET_NAME)
$(target)_BIN       := $(OUTPATH)/$(addsuffix .bin, $(basename $(LOCAL_TARGET_NAME)))
$(target)_SIZ       := $(OUTPATH)/$(addsuffix .siz, $(basename $(LOCAL_TARGET_NAME)))
$(target)_LZO       := $(OUTPATH)/$(addsuffix .lzo, $(basename $(LOCAL_TARGET_NAME)))

#*********************************************************************************************************
# Link script files
#*********************************************************************************************************
LOCAL_LD_SCRIPT_NT := $(LOCAL_LD_SCRIPT) config.ld

#*********************************************************************************************************
# Link object files
#*********************************************************************************************************
$($(target)_IMG): $(LOCAL_LD_SCRIPT_NT) $($(target)_OBJS) $($(target)_DEPEND_TARGET)
		@rm -f $@
		touch tmp.S
		$(CC) -c tmp.S
		$(OBJCOPY) --add-section=.matrix_elf_data=$(LOCAL_MATRIX_NAME) --set-section-flags=.matrix_elf_data=load,alloc tmp.o
		$(__PRE_LINK_CMD)
		$(CPP) $(__CPUFLAGS) -E -P $(__DSYMBOL) config.ld -o config.lds
		$(LD) $(__CPUFLAGS) $(ARCH_KERNEL_LDFLAGS) -nostdlib $(addprefix -T, $<) -o $@ $(__OBJS) $(__LIBRARIES) tmp.o
		@rm -f tmp.o tmp.S
		$(__POST_LINK_CMD)

#*********************************************************************************************************
# Create bin
#*********************************************************************************************************
$($(target)_BIN): $($(target)_IMG)
		@rm -f $@
		$(OC) -O binary $< $@

#*********************************************************************************************************
# Create siz
#*********************************************************************************************************
$($(target)_SIZ): $($(target)_IMG)
		@rm -f $@
		$(SZ) --format=berkeley $< > $@

#*********************************************************************************************************
# Create lzo
#*********************************************************************************************************
$($(target)_LZO): $($(target)_BIN)
		@rm -f $@
		$(LZOCOM) -c $< $@

#*********************************************************************************************************
# Strip image
#*********************************************************************************************************
$($(target)_STRIP_IMG): $($(target)_IMG)
		@if [ ! -d "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
		@rm -f $@
		$(__PRE_STRIP_CMD)
		$(STRIP) $(TOOLCHAIN_STRIP_FLAGS) $< -o $@
		$(__POST_STRIP_CMD)

#*********************************************************************************************************
# Add targets
#*********************************************************************************************************
ifeq ($(TOOLCHAIN_COMMERCIAL), 1)
TARGETS := $(TARGETS) $($(target)_IMG) $($(target)_BIN) $($(target)_SIZ) $($(target)_STRIP_IMG) $($(target)_LZO)
else
TARGETS := $(TARGETS) $($(target)_IMG) $($(target)_BIN) $($(target)_SIZ) $($(target)_STRIP_IMG)
endif

#*********************************************************************************************************
# End
#*********************************************************************************************************

