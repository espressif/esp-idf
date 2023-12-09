set(src "src/https_server.c")
set(inc "include")

idf_component_register(SRCS ${src}
                    INCLUDE_DIRS ${inc}
                    REQUIRES esp_http_server esp-tls
                    PRIV_REQUIRES lwip)
