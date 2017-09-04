

ifeq ("$(CONFIG_SPIRAM_CACHE_WORKAROUND)","y")
LIBC_PATH := $(COMPONENT_PATH)/lib/libc-psram-workaround.a
LIBM_PATH := $(COMPONENT_PATH)/lib/libm-psram-workaround.a

else

ifeq ("$(CONFIG_NEWLIB_NANO_FORMAT)","y")
LIBC_PATH := $(COMPONENT_PATH)/lib/libc_nano.a
else
LIBC_PATH := $(COMPONENT_PATH)/lib/libc.a
endif

LIBM_PATH := $(COMPONENT_PATH)/lib/libm.a

endif

COMPONENT_ADD_LDFLAGS := $(LIBC_PATH) $(LIBM_PATH) -lnewlib

COMPONENT_ADD_LINKER_DEPS := $(LIBC_PATH) $(LIBM_PATH)

COMPONENT_ADD_INCLUDEDIRS := platform_include include
