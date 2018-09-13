set(COMPONENT_SRCS "esp_ota_ops.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES spi_flash)
set(COMPONENT_PRIV_REQUIRES bootloader_support)

register_component()
