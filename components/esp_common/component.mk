#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src
ifndef CONFIG_IDF_ENV_FPGA
COMPONENT_OBJEXCLUDE += src/fpga_overrides.o
endif