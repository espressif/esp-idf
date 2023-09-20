# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components/spi_flash/test_apps/components")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on esp_psram
# and esptool_py as we set CONFIG_SPIRAM_... and CONFIG_ESPTOOLPY_... options.
set(COMPONENTS main esp_psram esptool_py)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_esp_flash_stress)
