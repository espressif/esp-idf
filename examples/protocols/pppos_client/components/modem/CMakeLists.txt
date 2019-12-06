set(srcs "src/esp_modem.c"
        "src/esp_modem_dce_service"
        "src/sim800.c"
        "src/bg96.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    REQUIRES driver)