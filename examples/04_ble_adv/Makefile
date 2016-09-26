#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ble_adv

include $(IDF_PATH)/make/project.mk

# Copy some defaults into the sdkconfig by default
# so BT stack is enabled
sdkconfig: sdkconfig.defaults
	$(Q) cp $< $@

menuconfig: sdkconfig
defconfig: sdkconfig
