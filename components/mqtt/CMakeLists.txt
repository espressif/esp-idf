set(srcs esp-mqtt/mqtt_client.c esp-mqtt/lib/mqtt_msg.c esp-mqtt/lib/mqtt_outbox.c esp-mqtt/lib/platform_esp32_idf.c)

if(CONFIG_MQTT_PROTOCOL_5)
    list(APPEND srcs esp-mqtt/lib/mqtt5_msg.c esp-mqtt/mqtt5_client.c)
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS esp-mqtt/include
                    PRIV_INCLUDE_DIRS "esp-mqtt/lib/include"
                    REQUIRES esp_event tcp_transport
                    PRIV_REQUIRES esp_timer http_parser esp_hw_support
                    )
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
