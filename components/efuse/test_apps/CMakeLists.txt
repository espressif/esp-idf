#This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(efuse_test)

message(STATUS "Checking efuse registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "efuse"
                            HAL_MODULES "efuse")
