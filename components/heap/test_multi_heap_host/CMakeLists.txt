cmake_minimum_required(VERSION 3.22)

add_compile_options(-m32)
add_link_options(-m32)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)

# This test app doesn't require FreeRTOS, using mock instead
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")

project(multi_heap_test)
