idf_component_register(SRCS "esp_websocket_client.c"
                    INCLUDE_DIRS "include"
                    REQUIRES lwip esp-tls tcp_transport nghttp)
