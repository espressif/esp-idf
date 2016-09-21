#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include include/internal include/platform include/oepnssl

COMPONENT_SRCDIRS := library platform

include $(IDF_PATH)/make/component_common.mk
