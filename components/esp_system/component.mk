ifdef IS_BOOTLOADER_BUILD

ifndef CONFIG_IDF_ENV_FPGA
COMPONENT_CONFIG_ONLY := 1
else
COMPONENT_SRCDIRS := .
COMPONENT_OBJS += fpga_overrides.o
endif

else
SOC_NAME := $(IDF_TARGET)

COMPONENT_SRCDIRS := .
COMPONENT_ADD_INCLUDEDIRS := include port/public_compat
COMPONENT_PRIV_INCLUDEDIRS := port/include port
COMPONENT_ADD_LDFRAGMENTS += linker.lf app.lf
COMPONENT_DEPENDS := esp_phy

ifndef CONFIG_IDF_ENV_FPGA
COMPONENT_OBJEXCLUDE += fpga_overrides.o
endif

# Force linking UBSAN hooks. If UBSAN is not enabled, the hooks will ultimately be removed
# due to -ffunction-sections -Wl,--gc-sections options.
COMPONENT_ADD_LDFLAGS += -u __ubsan_include

include $(COMPONENT_PATH)/port/soc/$(SOC_NAME)/component.mk

# disable stack protection in files which are involved in initialization of that feature
startup.o stack_check.o: CFLAGS := $(filter-out -fstack-protector%, $(CFLAGS))

ld_input := $(COMPONENT_PATH)/ld/esp32/memory.ld.in
ld_output := $(COMPONENT_BUILD_DIR)/ld/memory.ld

sections_ld := $(COMPONENT_BUILD_DIR)/ld/sections.ld

#Linker scripts used to link the final application.
#Warning: These linker scripts are only used when the normal app is compiled; the bootloader
#specifies its own scripts.
LINKER_SCRIPTS += $(ld_output) $(sections_ld)

#ld_include_highint_hdl is added as an undefined symbol because otherwise the
#linker will ignore highint_hdl.S as it has no other files depending on any
#symbols in it.
COMPONENT_ADD_LDFLAGS += -u ld_include_highint_hdl \
                         $(addprefix -T ,$(LINKER_SCRIPTS)) \

# Preprocess memory.ld.in linker script into memory.ld
#
# The library doesn't really depend on memory.ld, but it
# saves us from having to add the target to a Makefile.projbuild
$(COMPONENT_LIBRARY): $(ld_output)

$(ld_output): $(ld_input) ../include/sdkconfig.h
	mkdir -p $(COMPONENT_BUILD_DIR)/ld
	$(CC) -I ../include -I $(COMPONENT_PATH)/ld -C -P -x c -E $< -o $@

COMPONENT_EXTRA_CLEAN := $(ld_output) $(sections_ld)
endif
