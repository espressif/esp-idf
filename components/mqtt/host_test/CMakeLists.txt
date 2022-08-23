cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/"
                                 "$ENV{IDF_PATH}/tools/mocks/esp_timer/"
                                 )

option(TEST_BUILD "" ON)
project(host_mqtt_client_test)
