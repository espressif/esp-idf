#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include/lwip include/lwip/port

COMPONENT_SRCDIRS := api apps/sntp apps core/ipv4 core/ipv6 core netif port/freertos port/netif port

EXTRA_CFLAGS := -Wno-error=address -Waddress -DLWIP_ESP8266

include $(IDF_PATH)/make/component.mk
