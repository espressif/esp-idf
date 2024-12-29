# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.5)

list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers" "sdkconfig.defaults")

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(usb_cdc_vfs_test)
