set(COMPONENT_ADD_INCLUDEDIRS
    include/apps
    lwip/src/include
    port/esp32/include
    port/esp32/include/arch
    )

set(COMPONENT_SRCDIRS
    apps/dhcpserver
	apps/ping
	lwip/src/api
	lwip/src/apps/sntp
	lwip/src/core
	lwip/src/core/ipv4
	lwip/src/core/ipv6
	lwip/src/netif
	lwip/src/netif/ppp
	port/esp32
	port/esp32/freertos
	port/esp32/netif
	port/esp32/debug
    )

if(CONFIG_PPP_SUPPORT)
    list(APPEND COMPONENT_SRCDIRS
        lwip/src/netif/ppp
        lwip/src/netif/ppp/polarssl
        )
endif()

set(COMPONENT_REQUIRES vfs)
set(COMPONENT_PRIV_REQUIRES ethernet tcpip_adapter)

register_component()

# lots of LWIP source files evaluate macros that check address of stack variables
component_compile_options(-Wno-address)
