set(priv_req mbedtls lwip esp_timer)
set(priv_inc_dir "src/util" "src/port/esp32")
set(requires http_parser esp_event)

idf_component_register(SRCS "src/httpd_main.c"
                            "src/httpd_parse.c"
                            "src/httpd_sess.c"
                            "src/httpd_txrx.c"
                            "src/httpd_uri.c"
                            "src/httpd_ws.c"
                            "src/util/ctrl_sock.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS ${priv_inc_dir}
                    REQUIRES ${requires}
                    PRIV_REQUIRES ${priv_req})
