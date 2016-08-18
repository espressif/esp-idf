#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := bootloader
COMPONENTS := esptool_py

#We cannot include the esp32 component directly but we need its includes. This is fixed by
#adding it in the main/Makefile directory.

include $(SDK_PATH)/make/project.mk

# override configured app offset, as bootloader "app" is at 0x1000
CONFIG_APP_OFFSET := 0x1000
