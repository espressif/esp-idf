# Functionality common to both top-level project makefile (project.mk)
# and component makefiles (component_wrapper.mk)
#

# Include project config makefile, if it exists.
#
# (Note that we only rebuild this makefile automatically for some
# targets, see project_config.mk for details.)
SDKCONFIG_MAKEFILE ?= $(abspath $(BUILD_DIR_BASE)/include/config/auto.conf)
-include $(SDKCONFIG_MAKEFILE)
export SDKCONFIG_MAKEFILE  # sub-makes (like bootloader) will reuse this path

#Handling of V=1/VERBOSE=1 flag
#
# if V=1, $(summary) does nothing and $(details) will echo extra details
# if V is unset or not 1, $(summary) echoes a summary and $(details) does nothing
V ?= $(VERBOSE)
ifeq ("$(V)","1")
summary := @true
details := @echo
else
summary := @echo
details := @true

# disable echoing of commands, directory names
MAKEFLAGS += --silent
endif

# Pseudo-target to check a git submodule has been properly initialised
#
# $(eval $(call SubmoduleCheck,FILENAMES,SUBMODULE_PATH)) to create a target that
# automatically runs 'git submodule update --init SUBMODULE_PATH' if any of
# the files in FILENAMES are missing, and fails if this is not possible.
#
# Will also print a WARNING if the submodule at SUBMODULE_PATH appears
# to require an update.
define SubmoduleCheck
$(1):
	@echo "WARNING: Missing submodule $(2) for $$@..."
	[ -d ${IDF_PATH}/.git ] || ( echo "ERROR: esp-idf must be cloned from git to work."; exit 1)
	[ -x $(which git) ] || ( echo "ERROR: Need to run 'git submodule --init' in esp-idf root directory."; exit 1)
	@echo "Attempting 'git submodule update --init' in esp-idf root directory..."
	cd ${IDF_PATH} && git submodule update --init $(2)

# Parse 'git submodule status' output for out-of-date submodule.
# Status output prefixes status line with '+' if the submodule commit doesn't match
ifneq ("$(shell cd ${IDF_PATH} && git submodule status $(2) | grep '^+')","")
$$(info WARNING: git submodule $2 may be out of date. Run 'git submodule update' to update.)
endif
endef



# General make utilities

# convenience variable for printing an 80 asterisk wide separator line
SEPARATOR:="*******************************************************************************"

# macro to remove quotes from an argument, ie $(call dequote,$(CONFIG_BLAH))
define dequote
$(subst ",,$(1))
endef
# " comment kept here to keep syntax highlighting happy


# macro to keep an absolute path as-is, but resolve a relative path
# against a particular parent directory
#
# $(1) path to resolve
# $(2) directory to resolve non-absolute path against
#
# Path and directory don't have to exist (definition of a "relative
# path" is one that doesn't start with /)
#
# $(2) can contain a trailing forward slash or not, result will not
# double any path slashes.
#
# example $(call resolvepath,$(CONFIG_PATH),$(CONFIG_DIR))
define resolvepath
$(if $(filter /%,$(1)),$(1),$(subst //,/,$(2)/$(1)))
endef
