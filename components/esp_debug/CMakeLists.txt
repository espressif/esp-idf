set(COMPONENT_SRCS "src/esp_debug.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_PRIV_REQUIRES  "lwip" "app_update" "esp32")

register_component()
