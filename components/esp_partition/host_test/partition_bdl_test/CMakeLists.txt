cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
# Freertos is included via common components, however, currently only the mock component is compatible with linux
# target.
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")

project(partition_bdl_test)
