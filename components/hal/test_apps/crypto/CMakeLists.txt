cmake_minimum_required(VERSION 3.16)

set(COMPONENTS main)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(crypto_test)
