idf_component_register(SRCS "esp-mqtt/mqtt_client.c"
                            "esp-mqtt/lib/mqtt_msg.c"
                            "esp-mqtt/lib/mqtt_outbox.c"
                            "esp-mqtt/lib/platform_esp32_idf.c"
                    INCLUDE_DIRS esp-mqtt/include
                    PRIV_INCLUDE_DIRS "esp-mqtt/lib/include"
                    REQUIRES lwip nghttp mbedtls tcp_transport)
