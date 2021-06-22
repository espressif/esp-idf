# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ldalign_test)

idf_build_get_property(python PYTHON)
idf_build_get_property(elf EXECUTABLE)

set(check_alignment  "${CMAKE_CURRENT_LIST_DIR}/check_alignment.py")
set(readelf "${CMAKE_TOOLCHAIN_PREFIX}readelf")

add_custom_command(
    TARGET ${elf}
    POST_BUILD
    COMMAND ${python} ${check_alignment} ${readelf} $<TARGET_FILE:${elf}>
)
