cmake_minimum_required(VERSION 3.22)

set(COMPONENTS main)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../test_fatfs_common")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_fatfs_sdcard)
