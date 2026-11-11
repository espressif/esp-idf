idf_component_register(SRCS "blufi_example_main.c"
                            "blufi_security.c"
                            "blufi_init.c"
                    PRIV_REQUIRES bt nvs_flash mbedtls esp_timer bootloader_support
                    INCLUDE_DIRS ".")
