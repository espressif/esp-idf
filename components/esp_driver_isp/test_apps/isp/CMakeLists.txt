cmake_minimum_required(VERSION 3.16)

list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_isp)


message(STATUS "Checking isp registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "isp" "hp_sys_clkrst"
                            HAL_MODULES "isp")
