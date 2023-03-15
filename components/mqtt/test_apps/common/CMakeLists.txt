
idf_component_register(SRCS test_mqtt_connection.c
                    INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include
                    PRIV_REQUIRES unity esp_event esp_netif esp_eth)
