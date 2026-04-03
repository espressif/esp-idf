# This example demonstrates a cmakev2 capability of importing an external
# C library via FetchContent, built by its own CMake, and linked directly
# to an IDF component without any IDF component wrapper for the library.

cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(import_lib_direct C CXX ASM)

idf_project_default()
