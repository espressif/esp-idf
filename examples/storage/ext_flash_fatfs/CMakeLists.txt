# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# external SPI flash driver not currently supported for ESP32-S2
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ext_flash_fatfs)
