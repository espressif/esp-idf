set(COMPONENT_ADD_INCLUDEDIRS port/include include)
set(COMPONENT_SRCDIRS library port)

set(COMPONENT_REQUIRES lwip)

register_component()

target_compile_definitions(mbedtls PUBLIC
    -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h"
)
