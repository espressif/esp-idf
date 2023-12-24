set(srcs_lwip
        "lwip/esp_netif_lwip.c"
        "lwip/esp_netif_sntp.c"
        "lwip/esp_netif_lwip_defaults.c"
        "lwip/netif/wlanif.c"
        "lwip/netif/ethernetif.c"
        "lwip/netif/esp_pbuf_ref.c")


set(srcs
    "esp_netif_handlers.c"
    "esp_netif_objects.c"
    "esp_netif_defaults.c")

set(include_dirs "include")
set(priv_include_dirs "private_include")

if(CONFIG_PPP_SUPPORT)
    list(APPEND srcs_lwip lwip/esp_netif_lwip_ppp.c lwip/netif/ppp.c)
endif()


if(CONFIG_ESP_NETIF_L2_TAP)
    list(APPEND srcs vfs_l2tap/esp_vfs_l2tap.c)
endif()

if(CONFIG_ESP_NETIF_BRIDGE_EN)
    list(APPEND srcs_lwip lwip/esp_netif_br_glue.c)
endif()

if(CONFIG_ESP_NETIF_LOOPBACK)
    list(APPEND srcs loopback/esp_netif_loopback.c)
elseif(CONFIG_ESP_NETIF_TCPIP_LWIP)
    list(APPEND srcs ${srcs_lwip})
    list(APPEND priv_include_dirs lwip)
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                    REQUIRES esp_event
                    PRIV_REQUIRES esp_netif_stack
                    LDFRAGMENTS linker.lf)

if(CONFIG_ESP_NETIF_L2_TAP OR CONFIG_ESP_NETIF_BRIDGE_EN)
    set(optional_requires "")
    if(CONFIG_ESP_NETIF_L2_TAP)
        list(APPEND optional_requires "vfs")
    endif()

    if(CONFIG_ESP_NETIF_BRIDGE_EN)
        list(APPEND optional_requires "esp_wifi")
    endif()

    list(APPEND optional_requires "esp_eth")
    idf_component_optional_requires(PRIVATE "${optional_requires}")
endif()


target_compile_definitions(${COMPONENT_LIB} PRIVATE ESP_NETIF_COMPONENT_BUILD)
