#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/Makefile. By default, 
# this will take the sources in this directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#

COMPONENT_SRCDIRS := library port

EXTRA_CFLAGS += -DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"'

include $(SDK_PATH)/Makefile
