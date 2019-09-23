#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := expat/expat/lib port/include

COMPONENT_SRCDIRS := expat/expat/lib port

CFLAGS += -DHAVE_EXPAT_CONFIG_H -DHAVE_GETRANDOM
ifeq ($(GCC_NOT_5_2_0), 1)
# Temporary suppress "fallthrough" warnings until they are fixed in expat repo
CFLAGS += -Wno-implicit-fallthrough
endif
COMPONENT_SUBMODULES += expat
