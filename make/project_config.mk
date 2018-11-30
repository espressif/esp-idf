# Makefile support for the menuconfig system

#Find all Kconfig files for all components
COMPONENT_KCONFIGS := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig))
COMPONENT_KCONFIGS_PROJBUILD := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig.projbuild))

ifdef MSYSTEM
# kconfiglib requires Windows-style paths for kconfig files
COMPONENT_KCONFIGS := $(shell cygpath -w $(COMPONENT_KCONFIGS))
COMPONENT_KCONFIGS_PROJBUILD := $(shell cygpath -w $(COMPONENT_KCONFIGS_PROJBUILD))
endif

#For doing make menuconfig etc
KCONFIG_TOOL_DIR=$(IDF_PATH)/tools/kconfig

# set SDKCONFIG to the project's sdkconfig,
# unless it's overriden (happens for bootloader)
SDKCONFIG ?= $(PROJECT_PATH)/sdkconfig

# SDKCONFIG_DEFAULTS is an optional file containing default
# overrides (usually used for esp-idf examples)
SDKCONFIG_DEFAULTS ?= $(PROJECT_PATH)/sdkconfig.defaults

# Workaround to run make parallel (-j). mconf-idf and conf-idf cannot be made simultaneously
$(KCONFIG_TOOL_DIR)/mconf-idf: $(KCONFIG_TOOL_DIR)/conf-idf

# reset MAKEFLAGS as the menuconfig makefile uses implicit compile rules
$(KCONFIG_TOOL_DIR)/mconf-idf $(KCONFIG_TOOL_DIR)/conf-idf: $(wildcard $(KCONFIG_TOOL_DIR)/*.c)
	MAKEFLAGS="" CC=$(HOSTCC) LD=$(HOSTLD) \
	$(MAKE) -C $(KCONFIG_TOOL_DIR)

ifeq ("$(wildcard $(SDKCONFIG))","")
# if no configuration file is present we need a rule for it
ifeq ("$(filter $(NON_INTERACTIVE_TARGET), $(MAKECMDGOALS))","")
# if special non-interactive item is not a named target (eg. 'defconfig', 'clean')
# run defconfig then menuconfig to get the initial config
$(SDKCONFIG): menuconfig
menuconfig: defconfig
else
# otherwise, just run defconfig
$(SDKCONFIG): defconfig
endif
endif

# macro for the commands to run kconfig tools conf-idf or mconf-idf.
# $1 is the name (& args) of the conf tool to run
define RunConf
	mkdir -p $(BUILD_DIR_BASE)/include/config
	cd $(BUILD_DIR_BASE); KCONFIG_AUTOHEADER=$(abspath $(BUILD_DIR_BASE)/include/sdkconfig.h) \
	COMPONENT_KCONFIGS="$(COMPONENT_KCONFIGS)" KCONFIG_CONFIG=$(SDKCONFIG) \
	COMPONENT_KCONFIGS_PROJBUILD="$(COMPONENT_KCONFIGS_PROJBUILD)" \
	IDF_CMAKE=n \
	$(KCONFIG_TOOL_DIR)/$1 $(IDF_PATH)/Kconfig
endef

ifndef MAKE_RESTARTS
# menuconfig, defconfig and "GENCONFIG" configuration generation only
# ever run on the first make pass, subsequent passes don't run these
# (make often wants to re-run them as the conf tool can regenerate the
# sdkconfig input file as an output file, but this is not what the
# user wants - a single config pass is enough to produce all output
# files.)
#
# To prevent problems missing genconfig, ensure none of these targets
# depend on any prerequisite that may cause a make restart as part of
# the prerequisite's own recipe.

menuconfig: $(KCONFIG_TOOL_DIR)/mconf-idf
	$(summary) MENUCONFIG
ifdef BATCH_BUILD
	@echo "Can't run interactive configuration inside non-interactive build process."
	@echo ""
	@echo "Open a command line terminal and run 'make menuconfig' from there."
	@echo "See esp-idf documentation for more details."
	@exit 1
else
	$(call RunConf,mconf-idf)
endif

# defconfig creates a default config, based on SDKCONFIG_DEFAULTS if present
defconfig: $(KCONFIG_TOOL_DIR)/conf-idf
	$(summary) DEFCONFIG
ifneq ("$(wildcard $(SDKCONFIG_DEFAULTS))","")
	cat $(SDKCONFIG_DEFAULTS) >> $(SDKCONFIG)  # append defaults to sdkconfig, will override existing values
endif
	$(call RunConf,conf-idf --olddefconfig)

# if neither defconfig or menuconfig are requested, use the GENCONFIG rule to
# ensure generated config files are up to date
$(SDKCONFIG_MAKEFILE) $(BUILD_DIR_BASE)/include/sdkconfig.h: $(KCONFIG_TOOL_DIR)/conf-idf $(SDKCONFIG) $(COMPONENT_KCONFIGS) $(COMPONENT_KCONFIGS_PROJBUILD) | $(call prereq_if_explicit,defconfig) $(call prereq_if_explicit,menuconfig)
	$(summary) GENCONFIG
ifdef BATCH_BUILD  # can't prompt for new config values like on terminal
	$(call RunConf,conf-idf --olddefconfig)
endif
	$(call RunConf,conf-idf --silentoldconfig)
	touch $(SDKCONFIG_MAKEFILE) $(BUILD_DIR_BASE)/include/sdkconfig.h  # ensure newer than sdkconfig

else  # "$(MAKE_RESTARTS)" != ""
# on subsequent make passes, skip config generation entirely
defconfig:
menuconfig:
endif

.PHONY: config-clean defconfig menuconfig
config-clean:
	$(summary) RM CONFIG
	MAKEFLAGS="" $(MAKE) -C $(KCONFIG_TOOL_DIR) clean
	rm -rf $(BUILD_DIR_BASE)/include/config $(BUILD_DIR_BASE)/include/sdkconfig.h
