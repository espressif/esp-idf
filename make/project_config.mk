# Makefile support for the menuconfig system

#Find all Kconfig files for all components
COMPONENT_KCONFIGS := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig))
COMPONENT_KCONFIGS_PROJBUILD := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig.projbuild))

#For doing make menuconfig etc
KCONFIG_TOOL_DIR=$(IDF_PATH)/tools/kconfig

# set SDKCONFIG to the project's sdkconfig,
# unless it's overriden (happens for bootloader)
SDKCONFIG ?= $(PROJECT_PATH)/sdkconfig

# SDKCONFIG_DEFAULTS is an optional file containing default
# overrides (usually used for esp-idf examples)
SDKCONFIG_DEFAULTS ?= $(PROJECT_PATH)/sdkconfig.defaults

# reset MAKEFLAGS as the menuconfig makefile uses implicit compile rules
$(KCONFIG_TOOL_DIR)/mconf $(KCONFIG_TOOL_DIR)/conf:
	MAKEFLAGS=$(ORIGINAL_MAKEFLAGS) CC=$(HOSTCC) LD=$(HOSTLD) \
	$(MAKE) -C $(KCONFIG_TOOL_DIR)

# use a wrapper environment for where we run Kconfig tools
KCONFIG_TOOL_ENV=KCONFIG_AUTOHEADER=$(abspath $(BUILD_DIR_BASE)/include/sdkconfig.h) \
	COMPONENT_KCONFIGS="$(COMPONENT_KCONFIGS)" KCONFIG_CONFIG=$(SDKCONFIG) \
	COMPONENT_KCONFIGS_PROJBUILD="$(COMPONENT_KCONFIGS_PROJBUILD)"

menuconfig: $(KCONFIG_TOOL_DIR)/mconf $(IDF_PATH)/Kconfig $(call prereq_if_explicit,defconfig)
	$(summary) MENUCONFIG
	$(KCONFIG_TOOL_ENV) $(KCONFIG_TOOL_DIR)/mconf $(IDF_PATH)/Kconfig

ifeq ("$(wildcard $(SDKCONFIG))","")
ifeq ("$(call prereq_if_explicit,defconfig)","")
# if not configuration is present and defconfig is not a target, run defconfig then menuconfig
$(SDKCONFIG): defconfig menuconfig
else
# otherwise, just defconfig
$(SDKCONFIG): defconfig
endif
endif

# defconfig creates a default config, based on SDKCONFIG_DEFAULTS if present
defconfig: $(KCONFIG_TOOL_DIR)/mconf $(IDF_PATH)/Kconfig $(BUILD_DIR_BASE)
	$(summary) DEFCONFIG
ifneq ("$(wildcard $(SDKCONFIG_DEFAULTS))","")
	cat $(SDKCONFIG_DEFAULTS) >> $(SDKCONFIG)  # append defaults to sdkconfig, will override existing values
endif
	mkdir -p $(BUILD_DIR_BASE)/include/config
	$(KCONFIG_TOOL_ENV) $(KCONFIG_TOOL_DIR)/conf --olddefconfig $(IDF_PATH)/Kconfig

$(SDKCONFIG_MAKEFILE) $(BUILD_DIR_BASE)/include/sdkconfig.h: $(SDKCONFIG) $(KCONFIG_TOOL_DIR)/conf $(COMPONENT_KCONFIGS) $(COMPONENT_KCONFIGS_PROJBUILD)
	$(summary) GENCONFIG
	mkdir -p $(BUILD_DIR_BASE)/include/config
	cd $(BUILD_DIR_BASE); $(KCONFIG_TOOL_ENV) $(KCONFIG_TOOL_DIR)/conf --silentoldconfig $(IDF_PATH)/Kconfig
	touch $(SDKCONFIG_MAKEFILE) $(BUILD_DIR_BASE)/include/sdkconfig.h
# touch to ensure both output files are newer - as 'conf' can also update sdkconfig (a dependency). Without this,
# sometimes you can get an infinite make loop on Windows where sdkconfig always gets regenerated newer
# than the target(!)

.PHONY: config-clean
config-clean:
	$(summary RM CONFIG)
	$(MAKE) -C $(KCONFIG_TOOL_DIR) clean
	rm -rf $(BUILD_DIR_BASE)/include/config $(BUILD_DIR_BASE)/include/sdkconfig.h
