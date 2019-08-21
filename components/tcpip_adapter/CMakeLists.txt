idf_component_register(SRCS "event_handlers.c" "tcpip_adapter_lwip.c"
                    INCLUDE_DIRS include
                    REQUIRES lwip esp_eth)
