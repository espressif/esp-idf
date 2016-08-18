# Makefile support for the menuconfig system

#Find all Kconfig files for all components
COMPONENT_KCONFIGS := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig))
COMPONENT_KCONFIGS_PROJBUILD := $(foreach component,$(COMPONENT_PATHS),$(wildcard $(component)/Kconfig.projbuild))

#For doing make menuconfig etc
KCONFIG_TOOL_DIR=$(SDK_PATH)/tools/kconfig

# clear MAKEFLAGS as the menuconfig makefile uses implicit compile rules
$(KCONFIG_TOOL_DIR)/mconf $(KCONFIG_TOOL_DIR)/conf:
	MAKEFLAGS="" \
	CFLAGS="" \
	LDFLAGS="" \
	CC=$(HOSTCC) LD=$(HOSTLD) \
	$(MAKE) -C $(KCONFIG_TOOL_DIR)

menuconfig: $(KCONFIG_TOOL_DIR)/mconf $(SDK_PATH)/Kconfig $(BUILD_DIR_BASE)
	$(vecho) MENUCONFIG
	$(Q) KCONFIG_AUTOHEADER=$(PROJECT_PATH)/build/include/sdkconfig.h \
	KCONFIG_CONFIG=$(PROJECT_PATH)/sdkconfig \
	COMPONENT_KCONFIGS="$(COMPONENT_KCONFIGS)" \
	COMPONENT_KCONFIGS_PROJBUILD="$(COMPONENT_KCONFIGS_PROJBUILD)" \
	$(KCONFIG_TOOL_DIR)/mconf $(SDK_PATH)/Kconfig

ifeq ("$(wildcard $(PROJECT_PATH)/sdkconfig)","")
#No sdkconfig found. Need to run menuconfig to make this if we need it.
$(PROJECT_PATH)/sdkconfig: menuconfig
endif


# Work out of whether we have to build the Kconfig makefile
# (auto.conf), or if we're in a situation where we don't need it
NON_CONFIG_TARGETS := clean %-clean get_variable
AUTO_CONF_REGEN_TARGET := $(PROJECT_PATH)/build/include/config/auto.conf

# disable AUTO_CONF_REGEN_TARGET if all targets are non-config targets
# (and not building default target)
ifneq ("$(MAKECMDGOALS)","")
ifeq ($(filter $(NON_CONFIG_TARGETS), $(MAKECMDGOALS)),$(MAKECMDGOALS))
AUTO_CONF_REGEN_TARGET :=
# dummy target
$(PROJECT_PATH)/build/include/config/auto.conf:
endif
endif

$(AUTO_CONF_REGEN_TARGET) $(PROJECT_PATH)/build/include/sdkconfig.h: $(PROJECT_PATH)/sdkconfig $(KCONFIG_TOOL_DIR)/conf $(COMPONENT_KCONFIGS) $(COMPONENT_KCONFIGS_PROJBUILD)
	$(vecho) GENCONFIG
	$(Q) mkdir -p $(PROJECT_PATH)/build/include/config
	$(Q) cd build; KCONFIG_AUTOHEADER="$(PROJECT_PATH)/build/include/sdkconfig.h" \
	KCONFIG_CONFIG=$(PROJECT_PATH)/sdkconfig \
	COMPONENT_KCONFIGS="$(COMPONENT_KCONFIGS)" \
	COMPONENT_KCONFIGS_PROJBUILD="$(COMPONENT_KCONFIGS_PROJBUILD)" \
	$(KCONFIG_TOOL_DIR)/conf --silentoldconfig $(SDK_PATH)/Kconfig
	$(Q) touch $(AUTO_CONF_REGEN_TARGET) $(PROJECT_PATH)/build/include/sdkconfig.h
# touch to ensure both output files are newer - as 'conf' can also update sdkconfig (a dependency). Without this,
# sometimes you can get an infinite make loop on Windows where sdkconfig always gets regenerated newer
# than the target(!)

clean: config-clean
.PHONY: config-clean
EXTRA_CLEAN_TARGETS += config-clean
config-clean:
	$(MAKE) -C $(KCONFIG_TOOL_DIR) clean
