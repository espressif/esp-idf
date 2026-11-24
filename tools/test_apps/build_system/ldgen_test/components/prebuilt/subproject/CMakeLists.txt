cmake_minimum_required(VERSION 3.22)

project(prebuilt)
add_library(prebuilt STATIC prebuilt.c)
target_compile_options(prebuilt PRIVATE "-ffunction-sections" "-fdata-sections")
