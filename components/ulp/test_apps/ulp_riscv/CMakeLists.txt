# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

set(SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers")
list(APPEND SDKCONFIG_DEFAULTS "sdkconfig.defaults")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ulp_riscv_test)
