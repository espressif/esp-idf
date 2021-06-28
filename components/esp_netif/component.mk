#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := private_include lwip
COMPONENT_SRCDIRS := . lwip loopback

ifndef CONFIG_LWIP_SLIP_SUPPORT
    COMPONENT_OBJEXCLUDE := lwip/esp_netif_lwip_slip.o
endif

ifndef CONFIG_PPP_SUPPORT
    COMPONENT_OBJEXCLUDE += lwip/esp_netif_lwip_ppp.o
endif

ifndef CONFIG_LWIP_NETIF_LOOPBACK
    COMPONENT_OBJEXCLUDE += loopback/esp_netif_loopback.o
endif
