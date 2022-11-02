# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/tools/unit-test-app/components"
    "$ENV{IDF_PATH}/components/driver/test_apps/components"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(spi_param_test)
