set(priv_req mbedtls lwip esp_timer)
set(priv_inc_dir "src/util" "src/port/esp32")
set(requires http_parser esp_event)

if(CONFIG_HTTPD_SERVER_PSA_CRYPTO_MIGRATE)
    set(HTTPD_CRYPTO_SRC "src/httpd_crypto_psa.c")
else()
    set(HTTPD_CRYPTO_SRC "src/httpd_crypto_mbedtls.c")
endif()

idf_component_register(SRCS "src/httpd_main.c"
                            "src/httpd_parse.c"
                            "src/httpd_sess.c"
                            "src/httpd_txrx.c"
                            "src/httpd_uri.c"
                            "src/httpd_ws.c"
                            ${HTTPD_CRYPTO_SRC}
                            "src/util/ctrl_sock.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS ${priv_inc_dir}
                    REQUIRES ${requires}
                    PRIV_REQUIRES ${priv_req})
