cmake_minimum_required(VERSION 3.16)

list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_csi)


message(STATUS "Checking camera registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "lcd_cam" "hp_sys_clkrst" "*csi*"
                            HAL_MODULES "cam" "*csi*")
