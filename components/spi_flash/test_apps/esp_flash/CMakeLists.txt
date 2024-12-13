# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on
# esptool_py as we set CONFIG_ESPTOOLPY_... options.
set(COMPONENTS main esptool_py)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_esp_flash_drv)

message(STATUS "Checking memspi registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "spi_mem*" "spi1_mem*"
                            HAL_MODULES "spimem_flash")
