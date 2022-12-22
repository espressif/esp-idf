# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS "../../../")
set(EXCLUDE_COMPONENTS examples test_app test freemodbus)

# Include parameters from common modbus folder
set(MB_PARAMS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../mb_example_common")
list(APPEND EXTRA_COMPONENT_DIRS "${MB_PARAMS_DIR}")

# This example uses an extra component for common functions such as Wi-Fi and Ethernet connection.
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(modbus_tcp_master)
