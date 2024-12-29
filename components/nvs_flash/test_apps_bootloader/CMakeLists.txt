# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")

# set(BOOT_DIR "$ENV{IDF_PATH}/components/nvs_flash/bootloader_test/bootloader_components/nvs_bootloader")
# set(BOOT_TEST_DIR "$ENV{IDF_PATH}/components/nvs_flash/bootloader_test/bootloader_components/nvs_bootloader_test")
list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers" "sdkconfig.defaults")
list(APPEND EXTRA_COMPONENT_DIRS ${BOOT_DIR} ${BOOT_TEST_DIR})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(nvs_bootloader_test)
