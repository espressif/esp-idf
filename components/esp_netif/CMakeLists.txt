idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    # Header only library for linux
    idf_component_register(INCLUDE_DIRS include)
    return()
endif()

set(srcs
    "esp_netif_handlers.c"
    "esp_netif_objects.c"
    "esp_netif_defaults.c"
    "lwip/esp_netif_lwip.c"
    "lwip/esp_netif_lwip_defaults.c"
    "lwip/esp_netif_sta_list.c")

set(include_dirs "include")
set(priv_include_dirs "lwip" "private_include")

if(CONFIG_LWIP_SLIP_SUPPORT)
list(APPEND srcs
    "lwip/esp_netif_lwip_slip.c")
endif()

if(CONFIG_PPP_SUPPORT)
list(APPEND srcs
    "lwip/esp_netif_lwip_ppp.c")
endif()

if(CONFIG_LWIP_NETIF_LOOPBACK)
list(APPEND srcs
    "loopback/esp_netif_loopback.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                    REQUIRES lwip esp_eth tcpip_adapter)
