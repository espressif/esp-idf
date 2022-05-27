# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ldgen_test)

idf_build_get_property(python PYTHON)
idf_build_get_property(elf EXECUTABLE)

add_custom_command(
    TARGET ${elf}
    POST_BUILD
    COMMAND ${python} ${CMAKE_CURRENT_LIST_DIR}/check_placements.py ${CMAKE_OBJDUMP} $<TARGET_FILE:${elf}>
)
