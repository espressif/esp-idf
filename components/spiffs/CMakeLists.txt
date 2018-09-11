set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "." "spiffs/src")
set(COMPONENT_SRCS "esp_spiffs.c"
                   "spiffs/src/spiffs_cache.c"
                   "spiffs/src/spiffs_check.c"
                   "spiffs/src/spiffs_gc.c"
                   "spiffs/src/spiffs_hydrogen.c"
                   "spiffs/src/spiffs_nucleus.c"
                   "spiffs_api.c")

set(COMPONENT_REQUIRES spi_flash)
set(COMPONENT_PRIV_REQUIRES bootloader_support)

register_component()

