# The following five lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/toolchain.cmake)

project(hello-world ASM C CXX)

set(MAIN_SRCS main/hello_world_main.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
