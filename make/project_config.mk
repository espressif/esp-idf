# Makefile support for the menuconfig system

#Find all Kconfig files for all components
COMPONENT_KCONFIGS := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig))
COMPONENT_KCONFIGS_PROJBUILD := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig.projbuild))
COMPONENT_SDKCONFIG_RENAMES := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/sdkconfig.rename))

ifeq ($(OS),Windows_NT)
# kconfiglib requires Windows-style paths for kconfig files
COMPONENT_KCONFIGS := $(shell cygpath -w $(COMPONENT_KCONFIGS))
COMPONENT_KCONFIGS_PROJBUILD := $(shell cygpath -w $(COMPONENT_KCONFIGS_PROJBUILD))
endif

#For doing make menuconfig etc
KCONFIG_TOOL_DIR=$(IDF_PATH)/tools/kconfig

# set SDKCONFIG to the project's sdkconfig,
# unless it's overriden (happens for bootloader)
SDKCONFIG ?= $(PROJECT_PATH)/sdkconfig

SDKCONFIG_RENAME ?= $(IDF_PATH)/sdkconfig.rename

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

# macro for running confgen.py
define RunConfGen
	mkdir -p $(BUILD_DIR_BASE)/include/config
	$(PYTHON) $(IDF_PATH)/tools/kconfig_new/confgen.py \
		--kconfig $(IDF_PATH)/Kconfig \
		--config $(SDKCONFIG) \
		--sdkconfig-rename $(SDKCONFIG_RENAME) \
		--env "COMPONENT_KCONFIGS=$(strip $(COMPONENT_KCONFIGS))" \
		--env "COMPONENT_KCONFIGS_PROJBUILD=$(strip $(COMPONENT_KCONFIGS_PROJBUILD))" \
		--env "COMPONENT_SDKCONFIG_RENAMES=$(strip $(COMPONENT_SDKCONFIG_RENAMES))" \
		--env "IDF_CMAKE=n" \
		--output config ${SDKCONFIG} \
		--output makefile $(SDKCONFIG_MAKEFILE) \
		--output header $(BUILD_DIR_BASE)/include/sdkconfig.h
endef

# macro for the commands to run kconfig tools conf-idf or mconf-idf.
# $1 is the name (& args) of the conf tool to run
# Note: Currently only mconf-idf is used for compatibility with the CMake build system. The header file used is also
# the same.
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

menuconfig: $(KCONFIG_TOOL_DIR)/mconf-idf | check_python_dependencies
	$(summary) MENUCONFIG
ifdef BATCH_BUILD
	@echo "Can't run interactive configuration inside non-interactive build process."
	@echo ""
	@echo "Open a command line terminal and run 'make menuconfig' from there."
	@echo "See esp-idf documentation for more details."
	@exit 1
else
	$(call RunConfGen)
	# RunConfGen before mconf-idf ensures that deprecated options won't be ignored (they've got renamed)
	$(call RunConf,mconf-idf)
	# RunConfGen after mconf-idf ensures that deprecated options are appended to $(SDKCONFIG) for backward compatibility
	$(call RunConfGen)
endif

# defconfig creates a default config, based on SDKCONFIG_DEFAULTS if present
defconfig: | check_python_dependencies
	$(summary) DEFCONFIG
ifneq ("$(wildcard $(SDKCONFIG_DEFAULTS))","")
	cat $(SDKCONFIG_DEFAULTS) >> $(SDKCONFIG)  # append defaults to sdkconfig, will override existing values
endif
	$(call RunConfGen)

# if neither defconfig or menuconfig are requested, use the GENCONFIG rule to
# ensure generated config files are up to date
$(SDKCONFIG_MAKEFILE) $(BUILD_DIR_BASE)/include/sdkconfig.h: $(SDKCONFIG) $(COMPONENT_KCONFIGS) $(COMPONENT_KCONFIGS_PROJBUILD) | check_python_dependencies $(call prereq_if_explicit,defconfig) $(call prereq_if_explicit,menuconfig)
	$(summary) GENCONFIG
	$(call RunConfGen)
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
