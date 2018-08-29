# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS main/pppos_client_main.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(pppos_client)
