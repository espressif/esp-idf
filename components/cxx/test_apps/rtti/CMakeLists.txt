# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_cxx_rtti)
