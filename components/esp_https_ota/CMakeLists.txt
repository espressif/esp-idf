idf_component_register(SRCS "src/esp_https_ota.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_client bootloader_support
                    PRIV_REQUIRES log app_update)
