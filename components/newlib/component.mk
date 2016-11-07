
ifdef CONFIG_NEWLIB_NANO_FORMAT
LIBC_PATH := $(COMPONENT_PATH)/lib/libc_nano.a
else
LIBC_PATH := $(COMPONENT_PATH)/lib/libc.a
endif

COMPONENT_ADD_LDFLAGS := $(LIBC_PATH) $(COMPONENT_PATH)/lib/libm.a -lnewlib

COMPONENT_ADD_INCLUDEDIRS := include platform_include
