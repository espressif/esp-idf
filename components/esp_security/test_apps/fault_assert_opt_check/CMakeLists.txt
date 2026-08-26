# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

project(fault_assert_opt_check)

# Regression guard: fail the build if ESP_FAULT_ASSERT() gets optimized away.
idf_build_get_property(python PYTHON)
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${python} "${CMAKE_CURRENT_SOURCE_DIR}/check_fault_asserts.py"
            "$<TARGET_FILE:${CMAKE_PROJECT_NAME}.elf>" "${CMAKE_OBJDUMP}"
    COMMENT "Verifying ESP_FAULT_ASSERT() survived optimization"
    VERBATIM)
