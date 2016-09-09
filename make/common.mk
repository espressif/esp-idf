# Functionality common to both top-level project makefile
# and component makefiles
#

# Include project config file, if it exists.
#
# (Note that we only rebuild auto.conf automatically for some targets,
# see project_config.mk for details.)
-include $(PROJECT_PATH)/build/include/config/auto.conf

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

# General make utilities

# convenience variable for printing an 80 asterisk wide separator line
SEPARATOR:="*******************************************************************************"

# macro to remove quotes from an argument, ie $(call dequote (CONFIG_BLAH))
define dequote
$(subst ",,$(1))
endef
# " comment kept here to keep syntax highlighting happy
