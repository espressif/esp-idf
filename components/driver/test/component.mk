#
#Component Makefile
#

COMPONENT_SRCDIRS += param_test touch_sensor_test adc_test
COMPONENT_PRIV_INCLUDEDIRS += param_test/include touch_sensor_test/include

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
