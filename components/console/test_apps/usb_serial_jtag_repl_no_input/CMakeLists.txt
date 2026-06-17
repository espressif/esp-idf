# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers" "sdkconfig.defaults")

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(usb_serial_jtag_repl_no_input)
