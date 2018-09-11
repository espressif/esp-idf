set(COMPONENT_SRCS "esp_http_client.c"
                   "lib/http_auth.c"
                   "lib/http_header.c"
                   "lib/http_utils.c"
                   "lib/transport.c"
                   "lib/transport_ssl.c"
                   "lib/transport_tcp.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "lib/include")

set(COMPONENT_REQUIRES "nghttp")
set(COMPONENT_PRIV_REQUIRES "mbedtls" "lwip" "esp-tls")

register_component()
