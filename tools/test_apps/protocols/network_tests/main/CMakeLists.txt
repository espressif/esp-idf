idf_component_register(SRCS "net_suite.c" "stdinout.c"
                    PRIV_REQUIRES esp_event esp_netif vfs esp_driver_uart console
                    INCLUDE_DIRS ".")
