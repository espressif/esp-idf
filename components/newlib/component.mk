
ifdef CONFIG_SPIRAM_CACHE_WORKAROUND
LIBC_PATH := $(COMPONENT_PATH)/lib/libc-psram-workaround.a
LIBM_PATH := $(COMPONENT_PATH)/lib/libm-psram-workaround.a
COMPONENT_ADD_LDFRAGMENTS := esp32-spiram-rom-functions.lf
else

ifdef CONFIG_NEWLIB_NANO_FORMAT
LIBC_PATH := $(COMPONENT_PATH)/lib/libc_nano.a
else
LIBC_PATH := $(COMPONENT_PATH)/lib/libc.a
endif  # CONFIG_NEWLIB_NANO_FORMAT

LIBM_PATH := $(COMPONENT_PATH)/lib/libm.a

endif  # CONFIG_SPIRAM_CACHE_WORKAROUND

COMPONENT_ADD_LDFLAGS := $(LIBC_PATH) $(LIBM_PATH) -lnewlib

COMPONENT_ADD_LINKER_DEPS := $(LIBC_PATH) $(LIBM_PATH)

COMPONENT_ADD_INCLUDEDIRS := platform_include include

syscalls.o: CFLAGS += -fno-builtin
