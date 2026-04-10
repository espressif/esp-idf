# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on
# esptool_py as we set CONFIG_ESPTOOLPY_... options.
set(COMPONENTS main esptool_py)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_esp_flash_freq_limit)
