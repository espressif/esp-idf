#
# Component Makefile
#
COMPONENT_SUBMODULES += lwip

COMPONENT_ADD_INCLUDEDIRS := \
	include/apps \
	include/apps/sntp \
	lwip/src/include \
	port/esp32/include \
	port/esp32/include/arch

COMPONENT_SRCDIRS := \
	apps/dhcpserver \
	apps/ping \
	apps/sntp \
	lwip/src/api \
	lwip/src/apps/sntp \
	lwip/src/core \
	lwip/src/core/ipv4 \
	lwip/src/core/ipv6 \
	lwip/src/netif \
	port/esp32 \
	port/esp32/freertos \
	port/esp32/netif \
	port/esp32/debug

ifdef CONFIG_LWIP_PPP_SUPPORT
    COMPONENT_SRCDIRS += lwip/src/netif/ppp lwip/src/netif/ppp/polarssl
endif

CFLAGS += -Wno-address  # lots of LWIP source files evaluate macros that check address of stack variables

ifeq ($(GCC_NOT_5_2_0), 1)
lwip/src/netif/ppp/ppp.o: CFLAGS += -Wno-uninitialized
lwip/src/netif/ppp/pppos.o: CFLAGS += -Wno-implicit-fallthrough
endif

lwip/src/core/tcp.o: CFLAGS += -Wno-type-limits

COMPONENT_ADD_LDFRAGMENTS += linker.lf
