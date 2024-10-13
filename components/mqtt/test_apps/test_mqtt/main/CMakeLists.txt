set(srcs test_mqtt_client_broker.c test_mqtt.c)

if(CONFIG_MQTT_PROTOCOL_5)
    list(APPEND srcs test_mqtt5_client_broker.c test_mqtt5.c)
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES cmock test_utils mqtt nvs_flash app_update spi_flash common)
