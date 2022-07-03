cmake_minimum_required(VERSION 3.16)

# Since we're only interested in the prebuilt library, trim
# the build
set(COMPONENTS prebuilt main esptool_py)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(prebuilt C)
