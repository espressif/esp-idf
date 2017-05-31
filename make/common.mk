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

# BATCH_BUILD flag disables interactive terminal features, defaults to verbose build
ifdef BATCH_BUILD
V ?= 1
endif

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
$(foreach dir,$(1),$(if $(filter /%,$(dir)),$(dir),$(subst //,/,$(2)/$(dir))))
endef


# macro to include a target only if it's on the list of targets that make
# was invoked with
#
# This allows you to have something like an "order-only phony prerequisite",
# ie a prerequisite that determines an order phony targets have to run in.
#
# Because normal order-only prerequisites don't work with phony targets.
#
# example $(call prereq_if_explicit,erase_flash)
define prereq_if_explicit
$(filter $(1),$(MAKECMDGOALS))
endef

# macro to kill duplicate items in a list without messing up the sort order of the list.
# Will only keep the unique items; if there are non-unique items in the list, it will remove
# the later recurring ones so only the first one remains.
# Copied from http://stackoverflow.com/questions/16144115/makefile-remove-duplicate-words-without-sorting
define uniq
$(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))
endef
