
ifeq ($(GCC_NOT_5_2_0), 1)

ifdef CONFIG_NEWLIB_NANO_FORMAT
LIBC := c_nano
else  # CONFIG_NEWLIB_NANO_FORMAT
LIBC := c
endif  # CONFIG_NEWLIB_NANO_FORMAT

# Order of linking matters: libnewlib.a should go before libc.a
COMPONENT_ADD_LDFLAGS := -lnewlib -l$(LIBC) -lm
COMPONENT_ADD_INCLUDEDIRS := platform_include

ifdef CONFIG_SPIRAM_CACHE_WORKAROUND
COMPONENT_ADD_LDFRAGMENTS := esp32-spiram-rom-functions-c.lf
endif

# Forces the linker to include locks, heap, and syscalls from this component,
# instead of the implementations provided by newlib.
COMPONENT_ADD_LDFLAGS += -u newlib_include_locks_impl
COMPONENT_ADD_LDFLAGS += -u newlib_include_heap_impl
COMPONENT_ADD_LDFLAGS += -u newlib_include_syscalls_impl

else # GCC_NOT_5_2_0
# Remove this section when GCC 5.2.0 is no longer supported

ifdef CONFIG_SPIRAM_CACHE_WORKAROUND
LIBC_PATH := $(COMPONENT_PATH)/lib/libc-psram-workaround.a
LIBM_PATH := $(COMPONENT_PATH)/lib/libm-psram-workaround.a
COMPONENT_ADD_LDFRAGMENTS := esp32-spiram-rom-functions-psram-workaround.lf
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
endif  # GCC_NOT_5_2_0

COMPONENT_ADD_LDFRAGMENTS += newlib.lf

heap.o: CFLAGS += -fno-builtin
