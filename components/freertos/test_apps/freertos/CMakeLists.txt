# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# FreeRTOS tests of different types (e.g., kernel, port, performance etc.)are
# split into different directores in the test app's root directory. Each test
# type is treated as separate component
set(test_types)

list(APPEND EXTRA_COMPONENT_DIRS
     ${test_types}) # Add each test type as a component

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(freertos_test)
