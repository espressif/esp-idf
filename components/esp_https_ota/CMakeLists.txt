set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCDIRS "src")

set(COMPONENT_REQUIRES esp_http_client)
set(COMPONENT_PRIV_REQUIRES log app_update)

register_component()
