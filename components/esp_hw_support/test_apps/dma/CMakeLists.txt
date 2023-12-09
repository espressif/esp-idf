# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on esp_psram
# as we set CONFIG_SPIRAM_... options.
set(COMPONENTS main esp_psram)

project(dma_test)
