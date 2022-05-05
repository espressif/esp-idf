# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

if(IDF_TARGET STREQUAL "esp32s3")
    #IDF-4514
    message(FATAL_ERROR "\n **** ERROR **** : DO NOT BUILD AND RUN THIS APP ON ESP32-S3 AS IT MAY BRICK YOUR DEVICE")
    return()
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ulp_example)
