idf_component_register(SRCS "blufi_example_main.c"
                            "blufi_security.c"
                            "blufi_init.c"
                    PRIV_REQUIRES bt nvs_flash mbedtls
                    INCLUDE_DIRS ".")
