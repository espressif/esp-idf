set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS src/port/esp32 src/util)
set(COMPONENT_SRCS "src/httpd_main.c"
                   "src/httpd_parse.c"
                   "src/httpd_sess.c"
                   "src/httpd_txrx.c"
                   "src/httpd_uri.c"
                   "src/util/ctrl_sock.c")

set(COMPONENT_REQUIRES nghttp)  # for http_parser.h
set(COMPONENT_PRIV_REQUIRES lwip)

register_component()
