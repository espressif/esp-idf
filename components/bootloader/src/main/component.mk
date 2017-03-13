#
# Main bootloader Makefile.
#
# This is basically the same as a component makefile, but in the case of the bootloader
# we pull in bootloader-specific linker arguments.
#

LINKER_SCRIPTS := \
	esp32.bootloader.ld \
	$(IDF_PATH)/components/esp32/ld/esp32.rom.ld \
	$(IDF_PATH)/components/esp32/ld/esp32.peripherals.ld \
	esp32.bootloader.rom.ld

COMPONENT_ADD_LDFLAGS := -L $(COMPONENT_PATH) -lmain $(addprefix -T ,$(LINKER_SCRIPTS))

COMPONENT_ADD_LINKER_DEPS := $(LINKER_SCRIPTS)

# following lines are a workaround to link librtc into the
# bootloader, until clock setting code is in a source-based esp-idf
# component. See also rtc_printf() in bootloader_start.c
#
# See also matching COMPONENT_SUBMODULES line in Makefile.projbuild
COMPONENT_ADD_LDFLAGS += -L $(IDF_PATH)/components/esp32/lib/ -lrtc_clk -lrtc
COMPONENT_EXTRA_INCLUDES += $(IDF_PATH)/components/esp32/
