# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/protocols/modbus/mb_example_common)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(modbus_slave)
