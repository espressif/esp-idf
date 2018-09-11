set(COMPONENT_SRCS "mdns.c"
                   "mdns_console.c"
                   "mdns_networking.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "private_include")
set(COMPONENT_REQUIRES lwip mbedtls console tcpip_adapter)

register_component()

