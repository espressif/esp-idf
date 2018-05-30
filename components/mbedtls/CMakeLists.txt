set(COMPONENT_ADD_INCLUDEDIRS port/include mbedtls/include)
set(COMPONENT_SRCDIRS mbedtls/library port)

set(COMPONENT_SRCEXCLUDE mbedtls/library/net_sockets.c)

set(COMPONENT_REQUIRES lwip)

register_component()

target_compile_definitions(mbedtls PUBLIC
    -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h"
)
