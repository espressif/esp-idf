SOC_NAME := $(IDF_TARGET)

COMPONENT_SRCDIRS := src src/hal
COMPONENT_ADD_INCLUDEDIRS :=

# Since there can be chip-specific HAL headers which can include the common
# HAL header via include_next, process the build scripts here first so that
# include directories appear first in the compile command.
-include $(COMPONENT_PATH)/src/$(SOC_NAME)/component.mk
COMPONENT_ADD_INCLUDEDIRS += include

-include $(COMPONENT_PATH)/soc/component.mk

COMPONENT_ADD_LDFRAGMENTS += linker.lf

ifdef IS_BOOTLOADER_BUILD
	COMPONENT_OBJEXCLUDE += src/regi2c_ctrl.o
endif
