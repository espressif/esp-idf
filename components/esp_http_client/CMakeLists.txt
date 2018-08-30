set(COMPONENT_SRCDIRS "." "lib")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "lib/include")

set(COMPONENT_REQUIRES "nghttp")
set(COMPONENT_PRIV_REQUIRES "mbedtls" "lwip" "esp-tls")

register_component()
