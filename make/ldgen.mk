# Makefile to support the linker script generation mechanism
LDGEN_FRAGMENT_FILES = $(COMPONENT_LDFRAGMENTS)
LDGEN_LIBRARIES=$(foreach libcomp,$(COMPONENT_LIBRARIES),$(BUILD_DIR_BASE)/$(libcomp)/lib$(libcomp).a)

# Target to generate linker script generator from fragments presented by each of
# the components
ifeq ($(OS),Windows_NT)
define ldgen_process_template
$(BUILD_DIR_BASE)/ldgen_libraries: $(LDGEN_LIBRARIES) $(IDF_PATH)/make/ldgen.mk
	printf "$(foreach info,$(LDGEN_LIBRARIES),$(subst \,/,$(shell cygpath -m $(info)))\n)" > $(BUILD_DIR_BASE)/ldgen_libraries

$(2): $(1) $(LDGEN_FRAGMENT_FILES) $(SDKCONFIG) $(BUILD_DIR_BASE)/ldgen_libraries
	@echo 'Generating $(notdir $(2))'
	$(PYTHON) $(IDF_PATH)/tools/ldgen/ldgen.py \
		--input         $(1) \
		--config        $(SDKCONFIG) \
		--fragments     $(LDGEN_FRAGMENT_FILES) \
		--libraries-file $(BUILD_DIR_BASE)/ldgen_libraries \
		--output        $(2) \
		--kconfig       $(IDF_PATH)/Kconfig \
		--env           "COMPONENT_KCONFIGS=$(foreach k, $(COMPONENT_KCONFIGS), $(shell cygpath -m $(k)))" \
		--env           "COMPONENT_KCONFIGS_PROJBUILD=$(foreach k, $(COMPONENT_KCONFIGS_PROJBUILD), $(shell cygpath -m $(k)))" \
		--env           "COMPONENT_KCONFIGS_SOURCE_FILE=$(shell cygpath -m $(COMPONENT_KCONFIGS_SOURCE_FILE))" \
		--env           "COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE=$(shell cygpath -m $(COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE))" \
		--env           "IDF_CMAKE=n" \
		--objdump		$(OBJDUMP)
endef
else # Windows_NT
define ldgen_process_template
$(BUILD_DIR_BASE)/ldgen_libraries: $(LDGEN_LIBRARIES) $(IDF_PATH)/make/ldgen.mk
	printf "$(foreach library,$(LDGEN_LIBRARIES),$(library)\n)" > $(BUILD_DIR_BASE)/ldgen_libraries

$(2): $(1) $(LDGEN_FRAGMENT_FILES) $(SDKCONFIG) $(BUILD_DIR_BASE)/ldgen_libraries
	@echo 'Generating $(notdir $(2))'
	$(PYTHON) $(IDF_PATH)/tools/ldgen/ldgen.py \
		--input         $(1) \
		--config        $(SDKCONFIG) \
		--fragments     $(LDGEN_FRAGMENT_FILES) \
		--libraries-file $(BUILD_DIR_BASE)/ldgen_libraries \
		--output        $(2) \
		--kconfig       $(IDF_PATH)/Kconfig \
		--env           "COMPONENT_KCONFIGS=$(COMPONENT_KCONFIGS)" \
		--env           "COMPONENT_KCONFIGS_PROJBUILD=$(COMPONENT_KCONFIGS_PROJBUILD)" \
		--env           "COMPONENT_KCONFIGS_SOURCE_FILE=$(COMPONENT_KCONFIGS_SOURCE_FILE)" \
		--env           "COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE=$(COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE)" \
		--env           "IDF_CMAKE=n" \
		--objdump		$(OBJDUMP)
endef
endif # Windows_NT

define ldgen_create_commands
ldgen-clean:
	rm -f $(BUILD_DIR_BASE)/ldgen_libraries
endef
