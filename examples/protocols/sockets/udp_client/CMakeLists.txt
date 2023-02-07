# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if("${IDF_TARGET}" STREQUAL "linux")
    # This example uses an extra component with common functionality for lwip's port on linux target
    set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_tapif_io)
    set(COMPONENTS main esp_netif protocol_examples_tapif_io startup esp_hw_support esp_system nvs_flash)
else()
    # This example uses an extra component for common functions such as Wi-Fi and Ethernet connection on ESP target
    set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(udp_client)
