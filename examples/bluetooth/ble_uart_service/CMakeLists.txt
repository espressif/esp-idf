# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly.
cmake_minimum_required(VERSION 3.22)

# Shared `ble_uart` component (must be visible before `project()` so
# `main` can `REQUIRES ble_uart`; path is relative to this example root).
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../common/ble_uart")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)
project(ble_uart_service)
