# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#"Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

project(ldgen_test)

idf_build_get_property(python PYTHON)
idf_build_get_property(elf EXECUTABLE)

if(NOT CONFIG_SOC_RTC_MEM_SUPPORTED)
    set(args "--no-rtc")
endif()

if(CONFIG_SOC_MEM_NON_CONTIGUOUS_SRAM)
    set(args "--non-contiguous-sram")
endif()

add_custom_command(
    TARGET ${elf}
    POST_BUILD
    COMMAND ${python} ${CMAKE_CURRENT_LIST_DIR}/check_placements.py ${args} ${CMAKE_OBJDUMP} $<TARGET_FILE:${elf}>
)
