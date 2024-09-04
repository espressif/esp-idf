cmake_minimum_required(VERSION 3.16)

list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/esp_driver_sdmmc/test_apps/sd_test_utils/components")
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sdmmc_test_console)

message(STATUS "Checking sdmmc registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "sdmmc" "pcr" "hp_sys_clkrst"
                            HAL_MODULES "sdmmc")
