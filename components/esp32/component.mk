#
# Component Makefile
#

COMPONENT_SRCDIRS := .

ifdef CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
   # This linker script must come before esp32.project.ld
   LINKER_SCRIPTS += esp32.extram.bss.ld
endif

#Linker scripts used to link the final application.
#Warning: These linker scripts are only used when the normal app is compiled; the bootloader
#specifies its own scripts.
LINKER_SCRIPTS += $(COMPONENT_BUILD_DIR)/esp32.project.ld esp32.peripherals.ld

#ld_include_panic_highint_hdl is added as an undefined symbol because otherwise the
#linker will ignore panic_highint_hdl.S as it has no other files depending on any
#symbols in it.
COMPONENT_ADD_LDFLAGS += -L $(COMPONENT_PATH)/ld \
                         -T esp32_out.ld \
                         -u ld_include_panic_highint_hdl \
                         $(addprefix -T ,$(LINKER_SCRIPTS)) \

COMPONENT_ADD_LDFRAGMENTS += ld/esp32_fragments.lf linker.lf

# final linking of project ELF depends on all binary libraries, and
# all linker scripts (except esp32_out.ld, as this is code generated here.)
COMPONENT_ADD_LINKER_DEPS := $(addprefix ld/, $(filter-out $(COMPONENT_BUILD_DIR)/esp32.project.ld, $(LINKER_SCRIPTS))) \
                            $(COMPONENT_BUILD_DIR)/esp32.project.ld

# Preprocess esp32.ld linker script into esp32_out.ld
#
# The library doesn't really depend on esp32_out.ld, but it
# saves us from having to add the target to a Makefile.projbuild
$(COMPONENT_LIBRARY): esp32_out.ld

esp32_out.ld: $(COMPONENT_PATH)/ld/esp32.ld ../include/sdkconfig.h
	$(CC) -I ../include -C -P -x c -E $< -o $@

COMPONENT_EXTRA_CLEAN := esp32_out.ld $(COMPONENT_BUILD_DIR)/esp32.project.ld

# disable stack protection in files which are involved in initialization of that feature
cpu_start.o: CFLAGS := $(filter-out -fstack-protector%, $(CFLAGS))
