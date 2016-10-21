#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := bootloader
COMPONENTS := esptool_py bootloader log

# The bootloader pseudo-component is also included in this build, for its Kconfig.projbuild to be included.
#
# IS_BOOTLOADER_BUILD tells the component Makefile.projbuild to be a no-op
IS_BOOTLOADER_BUILD := 1
export IS_BOOTLOADER_BUILD

#We cannot include the esp32 component directly but we need its includes. 
#This is fixed by adding CFLAGS from Makefile.projbuild

include $(IDF_PATH)/make/project.mk
