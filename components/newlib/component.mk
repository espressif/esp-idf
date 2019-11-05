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

COMPONENT_ADD_LDFRAGMENTS += newlib.lf

heap.o: CFLAGS += -fno-builtin
