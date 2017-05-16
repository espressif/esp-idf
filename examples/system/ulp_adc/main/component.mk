#
# ULP support additions to component makefile.
#
# 1. ULP_APP_NAME must be unique (if multiple components use ULP)
#    Default value, override if necessary:
ULP_APP_NAME ?= ulp_$(COMPONENT_NAME)
#
# 2. Specify all assembly source files here.
#    Files should be placed into a separate directory (in this case, ulp/),
#    which should not be added to COMPONENT_SRCDIRS.
ULP_S_SOURCES = $(addprefix $(COMPONENT_PATH)/ulp/, \
	adc.S \
	)
#
# 3. List all the component object files which include automatically
#    generated ULP export file, $(ULP_APP_NAME).h:
ULP_EXP_DEP_OBJECTS := ulp_adc_example_main.o
#
# 4. Include build rules for ULP program 
include $(IDF_PATH)/components/ulp/component_ulp_common.mk
#
# End of ULP support additions to component makefile.
#
