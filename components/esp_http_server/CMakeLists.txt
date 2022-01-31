idf_component_register(SRCS "src/httpd_main.c"
                            "src/httpd_parse.c"
                            "src/httpd_sess.c"
                            "src/httpd_txrx.c"
                            "src/httpd_uri.c"
                            "src/httpd_ws.c"
                            "src/util/ctrl_sock.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "src/port/esp32" "src/util"
                    REQUIRES http_parser # for http_parser.h
                    PRIV_REQUIRES lwip mbedtls esp_timer)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
