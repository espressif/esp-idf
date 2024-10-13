# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

set(COMPONENTS main)
set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")

list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers" "sdkconfig.defaults")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp_hw_support_unity)

message(STATUS "Checking etm registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "*etm" "pcr" "hp_sys_clkrst"
                            HAL_MODULES "etm")
