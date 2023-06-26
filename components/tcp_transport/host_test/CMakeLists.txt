cmake_minimum_required(VERSION 3.16)

set(COMPONENTS esp_timer lwip tcp_transport main)

list(APPEND EXTRA_COMPONENT_DIRS
     "$ENV{IDF_PATH}/tools/mocks/lwip/"
     "$ENV{IDF_PATH}/tools/mocks/freertos/"
     "$ENV{IDF_PATH}/tools/mocks/esp_timer/"
     "$ENV{IDF_PATH}/tools/mocks/esp-tls/"
    )

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(host_tcp_transport_test)
