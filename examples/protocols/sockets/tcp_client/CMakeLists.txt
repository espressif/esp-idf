# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if("${IDF_TARGET}" STREQUAL "linux")
    set(COMPONENTS main)
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(tcp_client)
