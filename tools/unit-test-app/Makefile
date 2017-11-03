#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := unit-test-app

include $(IDF_PATH)/make/project.mk

# List of unit-test-app configurations.
# Each file in configs/ directory defines a configuration. The format is the
# same as sdkconfig file. Configuration is applied on top of sdkconfig.defaults
# file from the project directory
CONFIG_NAMES := $(notdir $(wildcard configs/*))

# Per-config targets
CONFIG_BUILD_TARGETS := $(addprefix ut-build-,$(CONFIG_NAMES))
CONFIG_CLEAN_TARGETS := $(addprefix ut-clean-,$(CONFIG_NAMES))
CONFIG_APPLY_TARGETS := $(addprefix ut-apply-config-,$(CONFIG_NAMES))

# Build (intermediate) and output (artifact) directories
BUILDS_DIR := $(PROJECT_PATH)/builds
BINARIES_DIR := $(PROJECT_PATH)/output

# This generates per-config targets (clean, build, apply-config).
define GenerateConfigTargets
# $(1) - configuration name
ut-clean-$(1):
	rm -rf $$(BUILDS_DIR)/$(1) $$(BINARIES_DIR)/$(1)

ut-build-$(1): $$(BINARIES_DIR)/$(1)/$$(PROJECT_NAME).bin

ut-apply-config-$(1):
	cat sdkconfig.defaults > sdkconfig
	echo "" >> sdkconfig
	cat configs/$(1) >> sdkconfig
	$(call RunConf,conf --olddefconfig)
endef

$(foreach config_name,$(CONFIG_NAMES), $(eval $(call GenerateConfigTargets,$(config_name))))

ut-build-all-configs: $(CONFIG_BUILD_TARGETS)
ut-clean-all-configs: $(CONFIG_CLEAN_TARGETS)

# This target builds the configuration. It does not currently track dependencies,
# but is good enough for CI builds if used together with clean-all-configs.
# For local builds, use 'apply-config-NAME' target and then use normal 'all'
# and 'flash' targets.
$(BINARIES_DIR)/%/bootloader.bin \
$(BINARIES_DIR)/%/$(PROJECT_NAME).elf \
$(BINARIES_DIR)/%/$(PROJECT_NAME).map \
$(BINARIES_DIR)/%/$(PROJECT_NAME).bin: configs/%
	# Create build and output directories
	mkdir -p $(BINARIES_DIR)/$*/bootloader
	mkdir -p $(BUILDS_DIR)/$*
	# Prepare configuration: top-level sdkconfig.defaults file plus the current configuration (configs/$*)
	$(summary) CONFIG $(BUILDS_DIR)/$*/sdkconfig
	rm -f $(BUILDS_DIR)/$*/sdkconfig
	cat sdkconfig.defaults > $(BUILDS_DIR)/$*/sdkconfig.defaults
	echo "" >> $(BUILDS_DIR)/$*/sdkconfig.defaults # in case there is no trailing newline in sdkconfig.defaults
	cat configs/$* >> $(BUILDS_DIR)/$*/sdkconfig.defaults
	# Build, tweaking paths to sdkconfig and sdkconfig.defaults
	$(summary) BUILD_CONFIG $(BUILDS_DIR)/$*
	$(MAKE) defconfig all \
		BUILD_DIR_BASE=$(BUILDS_DIR)/$* \
		SDKCONFIG=$(BUILDS_DIR)/$*/sdkconfig \
		SDKCONFIG_DEFAULTS=$(BUILDS_DIR)/$*/sdkconfig.defaults
	$(MAKE) print_flash_cmd \
		BUILD_DIR_BASE=$(BUILDS_DIR)/$* \
		SDKCONFIG=$(BUILDS_DIR)/$*/sdkconfig \
		| sed -e 's:'$(BUILDS_DIR)/$*/'::g' \
		| tail -n 1 > $(BINARIES_DIR)/$*/download.config
	# Copy files of interest to the output directory
	cp $(BUILDS_DIR)/$*/bootloader/bootloader.bin $(BINARIES_DIR)/$*/bootloader/
	cp $(BUILDS_DIR)/$*/$(PROJECT_NAME).elf $(BINARIES_DIR)/$*/
	cp $(BUILDS_DIR)/$*/$(PROJECT_NAME).bin $(BINARIES_DIR)/$*/
	cp $(BUILDS_DIR)/$*/$(PROJECT_NAME).map $(BINARIES_DIR)/$*/
	cp $(BUILDS_DIR)/$*/partition_table*.bin $(BINARIES_DIR)/$*/
	cp $(BUILDS_DIR)/$*/sdkconfig $(BINARIES_DIR)/$*/


ut-help:
	@echo "Additional unit-test-app specific targets:"
	@echo ""
	@echo "make ut-build-NAME - Build unit-test-app with configuration provided in configs/NAME."
	@echo "                  Build directory will be builds/NAME/, output binaries will be"
	@echo "                  under output/NAME/"
	@echo "make ut-clean-NAME - Remove build and output directories for configuration NAME."
	@echo ""
	@echo "make ut-build-all-configs - Build all configurations defined in configs/ directory."
	@echo ""
	@echo "make ut-apply-config-NAME - Generates configuration based on configs/NAME in sdkconfig"
	@echo "                         file. After this, normal all/flash targets can be used."
	@echo "                         Useful for development/debugging."
	@echo ""

help: ut-help

.PHONY: ut-build-all-configs ut-clean-all-configs \
		$(CONFIG_BUILD_TARGETS) $(CONFIG_CLEAN_TARGETS) $(CONFIG_APPLY_TARGETS) \
		ut-help
