set(srcs "main.c"
        "board.c")

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES esp_psram nvs_flash esp_driver_gpio
                    INCLUDE_DIRS  ".")
