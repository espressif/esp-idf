cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(COMPONENTS main)
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/driver/")
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")

idf_build_set_property(COMPILE_DEFINITIONS "NO_DEBUG_STORAGE" APPEND)
project(test_nvs_page_host)


add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/coverage_report/index.html"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND gcovr --root $ENV{IDF_PATH}/components/nvs_flash --html-details
    --exclude ${CMAKE_CURRENT_SOURCE_DIR}/managed_components/*
    -o ${CMAKE_CURRENT_BINARY_DIR}/coverage_report/index.html ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generate coverage report"
    )

add_custom_target(coverage
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "coverage_report/index.html"
    )
