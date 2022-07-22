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
    "lwip/netif/wlanif.c"
    "lwip/netif/ethernetif.c"
    "lwip/netif/esp_pbuf_ref.c"    )

set(include_dirs "include")
set(priv_include_dirs "lwip" "private_include")

if(CONFIG_PPP_SUPPORT)
list(APPEND srcs
    "lwip/esp_netif_lwip_ppp.c")
endif()

if(CONFIG_LWIP_NETIF_LOOPBACK)
list(APPEND srcs
    "loopback/esp_netif_loopback.c")
endif()

if(CONFIG_ESP_NETIF_L2_TAP)
list(APPEND srcs
    "vfs_l2tap/esp_vfs_l2tap.c")
endif()

if(CONFIG_ESP_NETIF_BRIDGE_EN)
list(APPEND srcs
    "lwip/esp_netif_br_glue.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                    REQUIRES esp_event
                    PRIV_REQUIRES lwip
                    LDFRAGMENTS linker.lf)

if(CONFIG_ESP_NETIF_L2_TAP OR CONFIG_ESP_NETIF_BRIDGE_EN)
    idf_component_optional_requires(PRIVATE esp_eth vfs)
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
