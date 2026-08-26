idf_component_register(SRCS "main.c" "misc.c" "peer.c" "scli.c"
                    PRIV_REQUIRES bt nvs_flash console esp_driver_uart cmd_system
                    INCLUDE_DIRS ".")
