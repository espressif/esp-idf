#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := port/include include

COMPONENT_SRCDIRS := library port

include $(IDF_PATH)/make/component_common.mk
