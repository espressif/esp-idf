cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
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
