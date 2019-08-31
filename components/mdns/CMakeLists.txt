idf_component_register(SRCS "mdns.c"
                            "mdns_console.c"
                            "mdns_networking.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "private_include"
                    REQUIRES lwip mbedtls console esp_netif)

