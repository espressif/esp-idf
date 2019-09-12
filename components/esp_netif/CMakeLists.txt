idf_component_register(SRCS "esp_netif_handlers.c"
                            "esp_netif_objects.c"
                            "esp_netif_defaults.c"
                            "lwip/esp_netif_lwip.c"
                            "loopback/esp_netif_loopback.c"
                            "lwip/esp_netif_lwip_defaults.c"
                            "lwip/esp_netif_sta_list.c"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS lwip private_include
                    REQUIRES lwip esp_eth tcpip_adapter)
