# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# FreeRTOS tests of different types (e.g., kernel, port, performance etc.)are
# split into different directores in the test app's root directory. Each test
# type is treated as separate component
set(test_types
    "kernel"
    "misc"
    "performance"
    "port")

list(APPEND EXTRA_COMPONENT_DIRS
     ${test_types}  # Add each test type as a component
     "$ENV{IDF_PATH}/tools/unit-test-app/components")   # For test_utils component

#"Trim" the build. Include the minimal set of components, main, and anything it depends on.
# Note: This is commented out for now due to pthread depending on vPortCleanUpTCB provided by "test_freertos_hooks.c".
# pthread is no used in FreeRTOS unit tests, but is pulled in by esp_system due to another dependency.
# Todo: Resolve this by either moving the "test_freertos_hooks.c" out, or solving the component dependencies.
#set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(freertos_test)
