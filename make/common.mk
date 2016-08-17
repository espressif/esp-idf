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
	-L$(SDK_PATH)/lib \
	-L$(SDK_PATH)/ld \
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
V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

# General make utilities

# convenience variable for printing an 80 asterisk wide separator line
SEPARATOR:="*******************************************************************************"

# macro to remove quotes from an argument, ie $(call dequote (CONFIG_BLAH))
define dequote
$(subst ",,$(1))
endef
# " comment kept here to keep syntax highlighting happy
