cmake_minimum_required(VERSION 3.22)

set(COMPONENTS main)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/test_apps/components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(crypto_test)
