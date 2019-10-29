#
# Component Makefile
#
COMPONENT_SRCDIRS := . $(IDF_TARGET)

COMPONENT_ADD_INCLUDEDIRS := include $(IDF_TARGET)/include

COMPONENT_PRIV_INCLUDEDIRS := include/driver

ifeq ($(GCC_NOT_5_2_0), 1)
# uses C11 atomic feature
spi_master.o: CFLAGS += -std=gnu11
endif
