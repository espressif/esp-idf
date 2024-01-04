# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

idf_build_get_property(IDF_MAJOR IDF_VERSION_MAJOR)
idf_build_get_property(IDF_MINOR IDF_VERSION_MINOR)
idf_build_get_property(IDF_PATCH IDF_VERSION_PATCH)
set(IDF_RELEASE_VERSION "${IDF_MAJOR}.${IDF_MINOR}.${IDF_PATCH}")

if(IDF_RELEASE_VERSION VERSION_LESS "5.2")
  # "Add partition sub-type `littlefs` (0x83) for ESP-IDF versions below 5.2"
  idf_build_set_property(EXTRA_PARTITION_SUBTYPES "data, littlefs, 0x83" APPEND)
endif()

project(littlefs_example)
