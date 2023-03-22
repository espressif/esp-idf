# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
project(no_embedded_paths)

idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(python PYTHON)
idf_build_get_property(elf EXECUTABLE)

# If the configuration is one that doesn't expect the IDF_PATH to be found in binaries then run this build step
# after building the ELF, will fail if it finds any file paths in binary files
if(CONFIG_OPTIMIZATION_ASSERTIONS_SILENT OR
        CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED OR
        CONFIG_COMPILER_HIDE_PATHS_MACROS)
    add_custom_command(
        TARGET ${elf}
        POST_BUILD
        COMMAND ${python} "${CMAKE_CURRENT_LIST_DIR}/check_for_file_paths.py" "${idf_path}" "${CMAKE_BINARY_DIR}"
        )
endif()
