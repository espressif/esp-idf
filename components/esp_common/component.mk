#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src

# IPC framework is not applicable if freertos unicore config is selected
ifdef CONFIG_FREERTOS_UNICORE
COMPONENT_OBJEXCLUDE := src/ipc.o
endif

# disable stack protection in files which are involved in initialization of that feature
src/stack_check.o: CFLAGS := $(filter-out -fstack-protector%, $(CFLAGS))
