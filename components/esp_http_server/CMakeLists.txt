idf_component_register(SRCS "src/httpd_main.c"
                            "src/httpd_parse.c"
                            "src/httpd_sess.c"
                            "src/httpd_txrx.c"
                            "src/httpd_uri.c"
                            "src/util/ctrl_sock.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "src/port/esp32" "src/util"
                    REQUIRES nghttp # for http_parser.h
                    PRIV_REQUIRES lwip) 
