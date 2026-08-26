# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# On Linux/macOS, limit to components that support the host target.
# The generated test uses #ifdef guards, so error codes from excluded
# components are simply skipped.
if(IDF_TARGET STREQUAL "linux")
    set(COMPONENTS main)
endif()

project(err_codes_check)
