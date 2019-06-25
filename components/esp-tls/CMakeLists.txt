idf_component_register(SRCS "esp_tls.c"
                    INCLUDE_DIRS "."
                    REQUIRES mbedtls
                    PRIV_REQUIRES lwip nghttp)
