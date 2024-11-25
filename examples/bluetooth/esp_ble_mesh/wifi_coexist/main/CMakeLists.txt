set(srcs "main.c"
        "board.c"
        "cmd_wifi.c")

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES nvs_flash console esp_driver_gpio esp_driver_uart
                    INCLUDE_DIRS  ".")
