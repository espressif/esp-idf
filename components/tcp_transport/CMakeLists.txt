set(COMPONENT_SRCS "transport.c"
                   "transport_ssl.c"
                   "transport_tcp.c"
                   "transport_ws.c"
                   "transport_utils.c"
                   "transport_strcasestr.c")

set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES lwip esp-tls)

register_component()
