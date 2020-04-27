#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src

# disable stack protection in files which are involved in initialization of that feature
src/stack_check.o: CFLAGS := $(filter-out -fstack-protector%, $(CFLAGS))
