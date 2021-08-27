#
# Component Makefile
#

COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS := include

COMPONENT_ADD_LDFLAGS := -lpthread

ifdef CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP
COMPONENT_ADD_LDFLAGS += -Wl,--wrap=vPortCleanUpTCB
endif

# Forces the linker to include pthread implementation from this component,
# instead of the weak implementations provided by libgcc.
COMPONENT_ADD_LDFLAGS += -u pthread_include_pthread_impl
COMPONENT_ADD_LDFLAGS += -u pthread_include_pthread_cond_impl
COMPONENT_ADD_LDFLAGS += -u pthread_include_pthread_local_storage_impl
COMPONENT_ADD_LDFLAGS += -u pthread_include_pthread_rwlock_impl

CFLAGS += -D_POSIX_READER_WRITER_LOCKS
