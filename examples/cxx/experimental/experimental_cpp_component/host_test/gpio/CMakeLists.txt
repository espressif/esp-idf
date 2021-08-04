cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)

idf_build_set_property(COMPILE_DEFINITIONS "-DNO_DEBUG_STORAGE" APPEND)
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/mocks/driver/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/examples/cxx/experimental/experimental_cpp_component/")
project(test_gpio_cxx_host)
