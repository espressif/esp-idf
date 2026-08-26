cmake_minimum_required(VERSION 3.22)

project(ext_subproject LANGUAGES C CXX ASM)
add_library(ext_lib STATIC check_flag.c check_flag.cpp check_flag.S)
add_executable(ext_link_test link_check.c)
target_link_libraries(ext_link_test PRIVATE ext_lib)
