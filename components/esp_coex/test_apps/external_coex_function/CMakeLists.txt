#This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/test_apps/components")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(external_coex_function)
