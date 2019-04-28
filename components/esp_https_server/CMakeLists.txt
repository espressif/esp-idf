idf_component_register(SRCS "src/https_server.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_server openssl
                    PRIV_REQUIRES lwip)
