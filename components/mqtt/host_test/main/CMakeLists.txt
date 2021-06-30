idf_component_register(SRCS  "test_mqtt_client.cpp"
                       INCLUDE_DIRS "$ENV{IDF_PATH}/tools/catch"
                       REQUIRES cmock mqtt)
