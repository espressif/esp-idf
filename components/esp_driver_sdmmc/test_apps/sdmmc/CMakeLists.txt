cmake_minimum_required(VERSION 3.16)

list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/esp_driver_sdmmc/test_apps/sd_test_utils/components")
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sdmmc_test_console)
