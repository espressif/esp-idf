#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/make/component.mk. By default, 
# this will take the sources in this directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#

COMPONENT_ADD_INCLUDEDIRS := include/lwip include/lwip/port

COMPONENT_SRCDIRS := api apps core/ipv4 core/ipv6 core netif port/freertos port/netif port

EXTRA_CFLAGS := -Wno-error=address -Waddress -DLWIP_ESP8266

include $(SDK_PATH)/make/component.mk
