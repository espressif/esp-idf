idf_component_register(SRCS "app_main.c" "custom_nvs_its_backend.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES mbedtls nvs_flash)
