# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if("${IDF_TARGET}" STREQUAL "linux")
    set(COMPONENTS main esp_netif lwip protocol_examples_tapif_io startup esp_hw_support esp_system nvs_flash)
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(udp_client)
