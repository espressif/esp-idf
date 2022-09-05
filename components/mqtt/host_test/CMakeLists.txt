cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)


set(COMPONENTS linux mqtt main)

list(APPEND EXTRA_COMPONENT_DIRS
                                 "$ENV{IDF_PATH}/tools/mocks/esp_hw_support/"
                                 "$ENV{IDF_PATH}/tools/mocks/freertos/"
                                 "$ENV{IDF_PATH}/tools/mocks/esp_timer/"
                                 "$ENV{IDF_PATH}/tools/mocks/esp_event/"
                                 "$ENV{IDF_PATH}/tools/mocks/lwip/"
                                 "$ENV{IDF_PATH}/tools/mocks/esp-tls/"
                                 "$ENV{IDF_PATH}/tools/mocks/http_parser/"
                                 "$ENV{IDF_PATH}/tools/mocks/tcp_transport/"
                                 )
project(host_mqtt_client_test)

idf_component_get_property(mqtt mqtt COMPONENT_LIB)
idf_component_get_property(linux linux COMPONENT_LIB)
target_link_libraries(${mqtt} PUBLIC ${linux})
