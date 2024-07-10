# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on esp_pm
# as we set CONFIG_PM_... options.
set(COMPONENTS main esp_pm)

project(etm_test)

message(STATUS "Checking etm registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "*etm" "pcr" "hp_sys_clkrst"
                            HAL_MODULES "etm")
