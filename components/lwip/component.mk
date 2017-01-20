#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := include/lwip include/lwip/port include/lwip/posix apps/ping

COMPONENT_SRCDIRS := api apps/sntp apps/ping apps core/ipv4 core/ipv6 core netif netif/ppp netif/ppp/polarssl port/freertos port/netif port/debug port

CFLAGS += -Wno-address  # lots of LWIP source files evaluate macros that check address of stack variables

api/tcpip.o apps/dhcpserver.o: CFLAGS += -Wno-unused-variable
apps/dhcpserver.o core/pbuf.o core/tcp_in.o: CFLAGS += -Wno-unused-but-set-variable
netif/ppp/pppos.o: CFLAGS += -Wno-type-limits

