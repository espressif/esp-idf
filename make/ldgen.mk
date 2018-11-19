# Makefile to support the linker script generation mechanism

LDGEN_SECTIONS_INFO_FILES = $(foreach lib, $(COMPONENT_LIBRARIES), $(BUILD_DIR_BASE)/$(lib)/lib$(lib).a.sections_info)
LDGEN_FRAGMENT_FILES = $(COMPONENT_LDFRAGMENTS)

# Target to generate linker script generator from fragments presented by each of
# the components
define ldgen_process_template
$(2): $(1) $(LDGEN_FRAGMENT_FILES) $(SDKCONFIG) $(LDGEN_SECTIONS_INFO_FILES)
	@echo 'Generating $(notdir $(2))'
	$(PYTHON) $(IDF_PATH)/tools/ldgen/ldgen.py \
		--input         $(1) \
		--config        $(SDKCONFIG) \
		--fragments     $(LDGEN_FRAGMENT_FILES) \
		--output        $(2) \
		--sections      $(LDGEN_SECTIONS_INFO_FILES) \
		--kconfig       $(IDF_PATH)/Kconfig \
		--env           "COMPONENT_KCONFIGS=$(COMPONENT_KCONFIGS)" \
		--env           "COMPONENT_KCONFIGS_PROJBUILD=$(COMPONENT_KCONFIGS_PROJBUILD)" \
		--env           "IDF_CMAKE=n" \
		--env           "IDF_TARGET=$(IDF_TARGET)"
endef

define ldgen_create_commands
$(foreach lib, $(COMPONENT_LIBRARIES), \
	$(eval $(call ldgen_generate_target_sections_info, $(BUILD_DIR_BASE)/$(lib)/lib$(lib).a)))

ldgen-clean:
	rm -f $(LDGEN_SECTIONS_INFO_FILES)
endef

# Target to generate sections info file from objdump of component archive
define ldgen_generate_target_sections_info
$(1).sections_info: $(1)
	@echo 'Generating $(notdir $(1).sections_info)' 
	$(OBJDUMP) -h $(1) > $(1).sections_info
endef
