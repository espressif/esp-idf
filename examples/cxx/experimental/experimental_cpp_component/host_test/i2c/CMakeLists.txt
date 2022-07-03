cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)

idf_build_set_property(COMPILE_DEFINITIONS "-DNO_DEBUG_STORAGE" APPEND)

# Overriding components which should be mocked
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/driver/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/esp_timer/")

# Including experimental component here because it's outside IDF's main component directory
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/examples/cxx/experimental/experimental_cpp_component/")

project(test_i2c_cxx_host)
