set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCS "src/https_server.c")

set(COMPONENT_REQUIRES esp_http_server openssl)
set(COMPONENT_PRIV_REQUIRES lwip)

register_component()
