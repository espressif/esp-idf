set(COMPONENT_ADD_INCLUDEDIRS esp-mqtt/include)
set(COMPONENT_PRIV_INCLUDEDIRS "esp-mqtt/lib/include")
set(COMPONENT_SRCDIRS esp-mqtt esp-mqtt/lib)

set(COMPONENT_REQUIRES lwip nghttp mbedtls tcp_transport)

register_component()
