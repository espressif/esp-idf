cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
list(PREPEND SDKCONFIG_DEFAULTS
    "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers"
    "sdkconfig.defaults")

project(perfmon_test)
