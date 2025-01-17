# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# (Not part of the boilerplate)
# This example uses extra components for the following -
# 1. common functions such as Wi-Fi and Ethernet connection.
# 2. managing TEE OTA updates
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common
                                 $ENV{IDF_PATH}/components/esp_tee/subproject/components/tee_ota_ops)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(tee_secure_ota)
