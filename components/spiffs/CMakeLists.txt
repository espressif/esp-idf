set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "." "spiffs/src")
set(COMPONENT_SRCDIRS "." "spiffs/src")

set(COMPONENT_REQUIRES spi_flash)
set(COMPONENT_PRIV_REQUIRES bootloader_support)

register_component()

