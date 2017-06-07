#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := \
	include/lwip \
	include/lwip/port \
	include/lwip/posix \
	apps/ping

ifdef CONFIG_PPP_SUPPORT
LWIP_PPP_DIRS := netif/ppp/polarssl netif/ppp 
endif

COMPONENT_SRCDIRS := \
	api \
	apps apps/sntp apps/ping \
	core core/ipv4 core/ipv6 \
	$(LWIP_PPP_DIRS) netif \
	port/freertos port/netif port/debug port

CFLAGS += -Wno-address  # lots of LWIP source files evaluate macros that check address of stack variables

api/tcpip.o apps/dhcpserver.o: CFLAGS += -Wno-unused-variable
apps/dhcpserver.o core/pbuf.o core/tcp_in.o: CFLAGS += -Wno-unused-but-set-variable
netif/ppp/pppos.o: CFLAGS += -Wno-type-limits

