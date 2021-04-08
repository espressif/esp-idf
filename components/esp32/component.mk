#
# Component Makefile
#

COMPONENT_ADD_LINKER_DEPS := $(COMPONENT_PATH) ld/esp32.peripherals.ld
COMPONENT_ADD_LDFLAGS := -T $(COMPONENT_PATH)/ld/esp32.peripherals.ld
