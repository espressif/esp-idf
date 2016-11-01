COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := include_priv

ifdef IS_BOOTLOADER_BUILD
# share "private" headers with the bootloader component
COMPONENT_ADD_INCLUDEDIRS += include_priv
endif

COMPONENT_SRCDIRS := src

include $(IDF_PATH)/make/component_common.mk
