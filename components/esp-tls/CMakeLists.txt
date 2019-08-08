idf_component_register(SRCS "esp_tls.c"
                    INCLUDE_DIRS "."
                    PRIVATE_INCLUDE_DIRS "private_include"
                    REQUIRES mbedtls
                    PRIV_REQUIRES lwip nghttp)
