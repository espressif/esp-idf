# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(jpeg_test)

target_add_binary_data(jpeg_test.elf "${IDF_PATH}/examples/peripherals/jpeg/jpeg_decode/resources/esp720.jpg" BINARY)
target_add_binary_data(jpeg_test.elf "${IDF_PATH}/examples/peripherals/jpeg/jpeg_decode/resources/esp1080.jpg" BINARY)
target_add_binary_data(jpeg_test.elf "resources/esp480.rgb" BINARY)

message(STATUS "Checking jpeg registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "jpeg" "hp_sys_clkrst"
                            HAL_MODULES "jpeg")
