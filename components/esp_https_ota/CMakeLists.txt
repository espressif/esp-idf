set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCS "src/esp_https_ota.c")

set(COMPONENT_REQUIRES esp_http_client bootloader_support)
set(COMPONENT_PRIV_REQUIRES log app_update)

register_component()
