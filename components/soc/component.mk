SOC_NAME := $(IDF_TARGET)

COMPONENT_SRCDIRS := src src/hal

COMPONENT_ADD_INCLUDEDIRS := include

-include $(COMPONENT_PATH)/soc/component.mk
-include $(COMPONENT_PATH)/src/$(SOC_NAME)/component.mk

COMPONENT_ADD_LDFRAGMENTS += linker.lf
