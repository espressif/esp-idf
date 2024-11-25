set(srcs "main.c")

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES bt nvs_flash esp_driver_uart
                    INCLUDE_DIRS ".")
