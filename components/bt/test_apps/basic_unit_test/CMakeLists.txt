cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
list(PREPEND SDKCONFIG_DEFAULTS
    "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers"
    "sdkconfig.defaults")
# Classic RFCOMM is not available on the BLE-only targets this app also builds.
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND SDKCONFIG_DEFAULTS "sdkconfig.defaults.esp32")
endif()

# (set before project() so the bt component picks it up; not in Kconfig).
idf_build_set_property(COMPILE_DEFINITIONS "BT_BLUEDROID_UNIT_TEST=1" APPEND)
project(bt_test)
