# Makefile support for the menuconfig system

#Find all Kconfig files for all components
COMPONENT_KCONFIGS := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig))
COMPONENT_KCONFIGS_PROJBUILD := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig.projbuild))
COMPONENT_SDKCONFIG_RENAMES := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/sdkconfig.rename))
COMPONENT_KCONFIGS_SOURCE_FILE:=$(BUILD_DIR_BASE)/kconfigs.in
COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE:=$(BUILD_DIR_BASE)/kconfigs_projbuild.in


ifeq ($(OS),Windows_NT)
# kconfiglib requires Windows-style paths for kconfig files
COMPONENT_KCONFIGS := $(shell cygpath -m $(COMPONENT_KCONFIGS))
COMPONENT_KCONFIGS_PROJBUILD := $(shell cygpath -m $(COMPONENT_KCONFIGS_PROJBUILD))
COMPONENT_SDKCONFIG_RENAMES := $(shell cygpath -m $(COMPONENT_SDKCONFIG_RENAMES))
COMPONENT_KCONFIGS_SOURCE_FILE := $(shell cygpath -m $(COMPONENT_KCONFIGS_SOURCE_FILE))
COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE := $(shell cygpath -m $(COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE))
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
ifeq ($(OS),Windows_NT)
	# mconf-idf is used only in MSYS
	MAKEFLAGS="" CC=$(HOSTCC) LD=$(HOSTLD) \
	$(MAKE) -C $(KCONFIG_TOOL_DIR)
else
	@echo "mconf-idf is not required on this platform"
endif

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

ifeq ("$(PYTHON)","")
# fallback value when menuconfig is called without a build directory and sdkconfig file
PYTHON=python
endif

SDKCONFIG_DEFAULTS_FILES := $(foreach f,$(SDKCONFIG_DEFAULTS),$(wildcard $(f)))
# for each sdkconfig.defaults file, also add sdkconfig.defaults.IDF_TARGET, if it exists
SDKCONFIG_DEFAULTS_FILES += $(foreach f,$(SDKCONFIG_DEFAULTS_FILES),$(wildcard $(f).$(IDF_TARGET)))

ifeq ($(OS),Windows_NT)
SDKCONFIG_DEFAULTS_FILES := $(shell cygpath -m $(SDKCONFIG_DEFAULTS_FILES))
endif
DEFAULTS_ARG := $(foreach f,$(SDKCONFIG_DEFAULTS_FILES),--defaults $(f))

# macro for running confgen.py
define RunConfGen
	mkdir -p $(BUILD_DIR_BASE)/include/config
	$(PYTHON) $(IDF_PATH)/tools/kconfig_new/confgen.py \
		--kconfig $(IDF_PATH)/Kconfig \
		--config $(SDKCONFIG) \
		--sdkconfig-rename $(SDKCONFIG_RENAME) \
		--env "COMPONENT_KCONFIGS=$(strip $(COMPONENT_KCONFIGS))" \
		--env "COMPONENT_KCONFIGS_PROJBUILD=$(strip $(COMPONENT_KCONFIGS_PROJBUILD))" \
		--env "COMPONENT_KCONFIGS_SOURCE_FILE=$(COMPONENT_KCONFIGS_SOURCE_FILE)" \
		--env "COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE=$(COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE)" \
		--env "COMPONENT_SDKCONFIG_RENAMES=$(strip $(COMPONENT_SDKCONFIG_RENAMES))" \
		--env "IDF_CMAKE=n" \
		$(DEFAULTS_ARG) \
		--output config ${SDKCONFIG} \
		--output makefile $(SDKCONFIG_MAKEFILE) \
		--output header $(BUILD_DIR_BASE)/include/sdkconfig.h
endef

ifeq ($(OS),Windows_NT)
MENUCONFIG_CMD := $(KCONFIG_TOOL_DIR)/mconf-idf
else
MENUCONFIG_CMD := MENUCONFIG_STYLE=aquatic $(PYTHON) $(IDF_PATH)/tools/kconfig_new/menuconfig.py
endif

# macro for running menuconfig
define RunMenuConf
	mkdir -p $(BUILD_DIR_BASE)/include/config
	cd $(BUILD_DIR_BASE); KCONFIG_AUTOHEADER=$(abspath $(BUILD_DIR_BASE)/include/sdkconfig.h) \
	KCONFIG_CONFIG=$(SDKCONFIG) \
	COMPONENT_KCONFIGS_SOURCE_FILE="$(COMPONENT_KCONFIGS_SOURCE_FILE)" \
	COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE="$(COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE)" \
	IDF_CMAKE=n \
	$(MENUCONFIG_CMD) $(IDF_PATH)/Kconfig
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
	# RunConfGen before menuconfig ensures that deprecated options won't be ignored (they've got renamed)
	$(call RunMenuConf)
	# RunConfGen after menuconfig ensures that deprecated options are appended to $(SDKCONFIG) for backward compatibility
	$(call RunConfGen)
endif

# defconfig creates a default config, based on SDKCONFIG_DEFAULTS if present
defconfig: | check_python_dependencies
	$(summary) DEFCONFIG
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
	rm -rf $(BUILD_DIR_BASE)/include/config $(BUILD_DIR_BASE)/include/sdkconfig.h \
		$(COMPONENT_KCONFIGS_SOURCE_FILE) $(COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE)
