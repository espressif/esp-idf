idf_component_register(SRCS "main.c"
                    SRCS "app_spp_msg_prs.c"
                    SRCS "app_spp_msg_set.c"
                    SRCS "console_uart.c"
                    PRIV_REQUIRES bt nvs_flash esp_driver_uart
                    INCLUDE_DIRS ".")
