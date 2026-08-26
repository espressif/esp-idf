# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/test_apps/components")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/esp_driver_sdmmc/test_apps/sd_test_utils/components")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/esp_driver_sdmmc/test_apps/sdmmc/components")
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sdmmc_test)
