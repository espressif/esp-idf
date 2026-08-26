cmake_minimum_required(VERSION 3.16)

set(SDKCONFIG ${CMAKE_CURRENT_BINARY_DIR}/sdkconfig)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# Disabling minimal build so that "main" depends on all components in the build,
# allowing all the public headers to be extracted.
idf_build_set_property(MINIMAL_BUILD OFF)
project(test_api_check)

# Define __DOXYGEN__ and IDF_DOC_BUILD so that the API checker can see
# declarations hidden behind these guards in ESP-IDF headers.
add_compile_definitions(__DOXYGEN__ IDF_DOC_BUILD)
