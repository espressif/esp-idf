# Functionality common to both top-level project makefile
# and component makefiles
#

# Include project config file, if it exists.
#
# (Note that we only rebuild auto.conf automatically for some targets,
# see project_config.mk for details.)
-include $(BUILD_DIR_BASE)/include/config/auto.conf

#Handling of V=1/VERBOSE=1 flag
#
# if V=1, $(summary) does nothing and $(details) will echo extra details
# if V is unset or not 1, $(summary) echoes a summary and $(details) does nothing
V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
summary := @true
details := @echo
else
Q := @
summary := @echo
details := @true
endif

# Pseudo-target to handle the case where submodules need to be
# re-initialised.
#
# $(eval $(call SubmoduleRequiredForFiles,FILENAMES)) to create a target that
# automatically runs 'git submodule update --init' if those files
# are missing, and fails if this is not possible.
define SubmoduleRequiredForFiles
$(1):
	@echo "WARNING: Missing submodule for $$@..."
	$(Q) [ -d ${IDF_PATH}/.git ] || ( echo "ERROR: esp-idf must be cloned from git to work."; exit 1)
	$(Q) [ -x $(which git) ] || ( echo "ERROR: Need to run 'git submodule --init' in esp-idf root directory."; exit 1)
	@echo "Attempting 'git submodule update --init' in esp-idf root directory..."
	cd ${IDF_PATH} && git submodule update --init
endef



# General make utilities

# convenience variable for printing an 80 asterisk wide separator line
SEPARATOR:="*******************************************************************************"

# macro to remove quotes from an argument, ie $(call dequote (CONFIG_BLAH))
define dequote
$(subst ",,$(1))
endef
# " comment kept here to keep syntax highlighting happy
