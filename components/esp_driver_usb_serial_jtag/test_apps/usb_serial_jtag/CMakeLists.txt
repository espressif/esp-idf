# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(usb_serial_test)


message(STATUS "Checking usb serial jtag registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "usb_serial_jtag" "pcr" "hp_sys_clkrst"
                            HAL_MODULES "usb_serial_jtag")
