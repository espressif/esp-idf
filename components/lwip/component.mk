#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include/lwip include/lwip/port include/lwip/posix

COMPONENT_SRCDIRS := api apps/sntp apps core/ipv4 core/ipv6 core netif port/freertos port/netif port

CFLAGS += -Wno-address -Wno-unused-variable -Wno-unused-but-set-variable

