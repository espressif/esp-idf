# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)

project(libgcc_iram)

# Fail the build when libgcc / compiler-rt placement does not match
# CONFIG_LIBC_LIBGCC_IN_IRAM. 64-bit helpers stay in IRAM either way.
idf_build_get_property(elf EXECUTABLE)
if(CONFIG_LIBC_LIBGCC_IN_IRAM)
    set(libgcc_double_in iram)
else()
    set(libgcc_double_in flash)
endif()
add_custom_target(check_libgcc_placement ALL
    COMMAND ${PYTHON} ${CMAKE_CURRENT_LIST_DIR}/check_libgcc_placement.py
            --objdump ${CMAKE_OBJDUMP}
            --elf-file ${CMAKE_BINARY_DIR}/libgcc_iram.elf
            --double-in ${libgcc_double_in}
    DEPENDS ${elf}
    )
