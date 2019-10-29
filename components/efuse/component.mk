#
# Component Makefile
# currently the only SoC supported; to be moved into Kconfig
TARGET := $(IDF_TARGET)

COMPONENT_SRCDIRS := $(TARGET) src src/$(TARGET)
COMPONENT_PRIV_INCLUDEDIRS := private_include
COMPONENT_ADD_INCLUDEDIRS := $(TARGET)/include include

-include $(COMPONENT_PATH)/$(TARGET)/component.mk