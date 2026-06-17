# This is the project CMakeLists.txt file for the generic partition test application
cmake_minimum_required(VERSION 3.22)

set(EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/tools/test_apps/components"
    "${CMAKE_CURRENT_LIST_DIR}/../../"
    "${CMAKE_CURRENT_LIST_DIR}/../../../esp_blockdev")

set(COMPONENTS main)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(generic_partition_test)
