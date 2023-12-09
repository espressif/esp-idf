# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers" "sdkconfig.defaults")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ulp_riscv_test)
