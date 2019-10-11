idf_component_register(SRCS "esp_http_client.c"
                            "lib/http_auth.c"
                            "lib/http_header.c"
                            "lib/http_utils.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "lib/include"
                    REQUIRES nghttp
                    PRIV_REQUIRES mbedtls lwip esp-tls tcp_transport)
