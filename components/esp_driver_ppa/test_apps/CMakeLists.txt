# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

project(ppa_test)

message(STATUS "Checking ppa registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "ppa" "hp_sys_clkrst"
                            HAL_MODULES "ppa")
