#
# Component Makefile
# currently the only SoC supported; to be moved into Kconfig
SOC_NAME := $(IDF_TARGET)

COMPONENT_SRCDIRS := $(SOC_NAME) src

COMPONENT_ADD_INCLUDEDIRS := $(SOC_NAME)/include include

-include $(COMPONENT_PATH)/$(SOC_NAME)/component.mk