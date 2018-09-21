#
# Component Makefile
#

ifeq ($(GCC_NOT_5_2_0), 1)
unity.o: CFLAGS += -Wno-unused-const-variable
endif