# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

list(APPEND EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/tools/unit-test-app/components")    # For test_utils component

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on
# esptool_py and esp_psram as we set CONFIG_ESPTOOLPY_... and CONFIG_SPIRAM_... options.
set(COMPONENTS main esptool_py esp_psram)

project(mspi_psram_test_app)
