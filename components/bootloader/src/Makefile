#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := bootloader

#We cannot include the esp32 component directly but we need its includes.
#This is fixed by adding CFLAGS from Makefile.projbuild
COMPONENTS := esptool_py bootloader bootloader_support log spi_flash micro-ecc soc

# The bootloader pseudo-component is also included in this build, for its Kconfig.projbuild to be included.
#
# IS_BOOTLOADER_BUILD tells the component Makefile.projbuild to be a no-op
IS_BOOTLOADER_BUILD := 1
export IS_BOOTLOADER_BUILD

# include the top-level "project" include directory, for sdkconfig.h
CFLAGS += -I$(BUILD_DIR_BASE)/../include

include $(IDF_PATH)/make/project.mk
