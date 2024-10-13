# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

set(EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/tools/unit-test-app/components"
    "$ENV{IDF_PATH}/components/driver/test_apps/components"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sdio)

message(STATUS "Checking sdio registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "host" "hinf" "slc" "pcr"
                            HAL_MODULES "sdio")
