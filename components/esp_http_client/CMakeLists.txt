idf_component_register(SRCS "esp_http_client.c"
                            "lib/http_auth.c"
                            "lib/http_header.c"
                            "lib/http_utils.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "lib/include"
                    # lwip is a public requirement because esp_http_client.h includes sys/socket.h
                    REQUIRES lwip
                    PRIV_REQUIRES tcp_transport http_parser)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
