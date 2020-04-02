set(COMPONENT_SRCS "esp_websocket_client.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES lwip esp-tls tcp_transport nghttp)

register_component()
