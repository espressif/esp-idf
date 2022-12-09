#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include

COMPONENT_SRCDIRS := src

ifndef CONFIG_NVS_ENCRYPTION
COMPONENT_OBJEXCLUDE := src/nvs_encr.o
endif
