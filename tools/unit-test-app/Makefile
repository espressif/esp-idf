#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := unit-test-app

ifeq ($(MAKELEVEL),0)
# Set default target
all:

# Define helper targets only when not recursing

# List of unit-test-app configurations.
# Each file in configs/ directory defines a configuration. The format is the
# same as sdkconfig file. Configuration is applied on top of sdkconfig.defaults
# file from the project directory
CONFIG_NAMES := $(shell ${IDF_PATH}/tools/unit-test-app/tools/get_available_configs.sh esp32)

# Per-config targets
CONFIG_BUILD_TARGETS := $(addprefix ut-build-,$(CONFIG_NAMES))
CONFIG_CLEAN_TARGETS := $(addprefix ut-clean-,$(CONFIG_NAMES))
CONFIG_APPLY_TARGETS := $(addprefix ut-apply-config-,$(CONFIG_NAMES))

# Build (intermediate) and output (artifact) directories
PROJECT_DIR := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))
BUILDS_DIR := $(PROJECT_DIR)/builds
BINARIES_DIR := $(PROJECT_DIR)/output

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
	echo CONFIG $(BUILDS_DIR)/$*/sdkconfig
	rm -f $(BUILDS_DIR)/$*/sdkconfig
	cat sdkconfig.defaults > $(BUILDS_DIR)/$*/sdkconfig.defaults
	echo "" >> $(BUILDS_DIR)/$*/sdkconfig.defaults # in case there is no trailing newline in sdkconfig.defaults
	cat sdkconfig.defaults.esp32 >> $(BUILDS_DIR)/$*/sdkconfig.defaults
	echo "" >> $(BUILDS_DIR)/$*/sdkconfig.defaults
	cat configs/$* >> $(BUILDS_DIR)/$*/sdkconfig.defaults

	# Build, tweaking paths to sdkconfig and sdkconfig.defaults
	echo BUILD_CONFIG $(BUILDS_DIR)/$*
	# 'TEST_COMPONENTS=names' option can be added to configs/$* to limit the set
	# of tests to build for given configuration.
	# Build all tests if this option is not present.
	test_components=`sed -n 's/^TEST_COMPONENTS=\(.*\)/\1/p' configs/$*`; \
		test_exclude_components=`sed -n 's/^TEST_EXCLUDE_COMPONENTS=\(.*\)/\1/p' configs/$*`; \
		tests_all=`test -n "$${test_components}"; echo $${?}`; \
		exclude_components=`sed -n 's/^EXCLUDE_COMPONENTS=\(.*\)/\1/p' configs/$*`; \
	$(MAKE) defconfig list-components all \
		BUILD_DIR_BASE=$(BUILDS_DIR)/$* \
		SDKCONFIG=$(BUILDS_DIR)/$*/sdkconfig \
		SDKCONFIG_DEFAULTS=$(BUILDS_DIR)/$*/sdkconfig.defaults \
		TEST_COMPONENTS="$${test_components}" \
		TEST_EXCLUDE_COMPONENTS="$${test_exclude_components}" \
		TESTS_ALL=$${tests_all} \
		EXCLUDE_COMPONENTS="$${exclude_components}"
	$(MAKE) --silent print_flash_cmd \
		BUILD_DIR_BASE=$(BUILDS_DIR)/$* \
		SDKCONFIG=$(BUILDS_DIR)/$*/sdkconfig \
		| sed -e 's:'$(BUILDS_DIR)/$*/'::g' \
		| tail -n 1 > $(BINARIES_DIR)/$*/download.config
	# Copy files of interest to the output directory
	cp $(BUILDS_DIR)/$*/bootloader/bootloader.bin $(BINARIES_DIR)/$*/bootloader/
	cp $(BUILDS_DIR)/$*/$(PROJECT_NAME).elf $(BINARIES_DIR)/$*/
	cp $(BUILDS_DIR)/$*/$(PROJECT_NAME).bin $(BINARIES_DIR)/$*/
	cp $(BUILDS_DIR)/$*/$(PROJECT_NAME).map $(BINARIES_DIR)/$*/
	cp $(BUILDS_DIR)/$*/*.bin $(BINARIES_DIR)/$*/
	cp $(BUILDS_DIR)/$*/sdkconfig $(BINARIES_DIR)/$*/


ut-help:
	@echo "Additional unit-test-app specific targets:"
	@echo ""
	@echo "make ut-build-NAME - Build unit-test-app with configuration provided in configs/NAME."
	@echo "                  Build directory will be builds/NAME/, output binaries will be"
	@echo "                  under output/NAME/"
	@echo ""
	@echo "make ut-build-all-configs - Build all configurations defined in configs/ directory."
	@echo ""
	@echo "Above targets determine list of components to be built from configs/NAME files."
	@echo "To build custom subset of components use 'make ut-apply-config-NAME' and then 'make all'."
	@echo ""
	@echo "make ut-apply-config-NAME - Generates configuration based on configs/NAME in sdkconfig"
	@echo "                         file. After this, normal all/flash targets can be used."
	@echo "                         Useful for development/debugging."
	@echo ""
	@echo "make ut-clean-NAME - Remove build and output directories for configuration NAME."
	@echo ""

help: ut-help

LOCAL_TARGETS := ut-build-all-configs ut-clean-all-configs \
		$(CONFIG_BUILD_TARGETS) $(CONFIG_CLEAN_TARGETS) \
		ut-help

.PHONY: $(LOCAL_TARGETS)

NON_INTERACTIVE_TARGET += ut-apply-config-% ut-clean-% ut-build-% \
						  ut-build-all-configs ut-clean-all-configs

endif # MAKELEVEL == 0


# When targets defined in this makefile are built, don't need to include the main project makefile.
# This prevents some variables which depend on build directory from being set erroneously.
ifeq ($(filter $(LOCAL_TARGETS),$(MAKECMDGOALS)),)

include $(IDF_PATH)/make/project.mk

endif

# If recursing, print the actual list of tests being built
ifneq ($(MAKELEVEL),0)

$(info TESTS $(foreach comp,$(TEST_COMPONENT_NAMES),$(patsubst %_test,%,$(comp))))

endif # MAKELEVEL != 0
