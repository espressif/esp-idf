cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(COMPONENTS main)
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/tools/mocks/esp_wifi)
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/tools/mocks/esp_netif)
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/tools/mocks/esp_event)
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/tools/mocks/lwip)

project(wifi_build_test)
