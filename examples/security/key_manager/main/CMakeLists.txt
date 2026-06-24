idf_component_register(SRCS "key_mgr_sign_main.c"
                       PRIV_REQUIRES esp_security mbedtls nvs_flash
                       INCLUDE_DIRS ".")
