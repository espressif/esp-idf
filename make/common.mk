# Functionality common to both top-level project makefile
# and component makefiles
#

# Include project config file, if it exists.
#
# (Note that we only rebuild auto.conf automatically for some targets,
# see project_config.mk for details.)
-include $(PROJECT_PATH)/build/include/config/auto.conf

ifeq ("$(LDFLAGS)","")
LDFLAGS = -nostdlib \
	-L$(IDF_PATH)/lib \
	-L$(IDF_PATH)/ld \
	$(addprefix -L$(BUILD_DIR_BASE)/,$(COMPONENTS) $(SRCDIRS)) \
	-u call_user_start_cpu0	\
	-Wl,--gc-sections	\
	-Wl,-static	\
	-Wl,--start-group	\
	$(COMPONENT_LDFLAGS) \
	-lgcc \
	-Wl,--end-group
endif

ifeq ("$(CFLAGS)","")
CFLAGS = -DESP_PLATFORM -Og -std=gnu99 -g3 \
	-Wpointer-arith -Werror -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable \
	-Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -Wall -ffunction-sections -fdata-sections $(EXTRA_CFLAGS)
endif

ifeq ("$(CXXFLAGS)","")
CXXFLAGS = -DESP_PLATFORM -Og -std=gnu++11 -g3 \
	-Wpointer-arith -Werror -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable \
	-Wl,-EL -nostdlib -mlongcalls -Wall -ffunction-sections -fdata-sections $(EXTRA_CFLAGS) -fno-exceptions 
endif

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
