# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Helpers for selecting a user-provided toolchain file for the linux target.
#
# These helpers are shared between the legacy build system (tools/cmake) and the
# build system v2 (tools/cmakev2).

#
# Get the user-provided toolchain file for the linux target (e.g. from a Yocto or
# Buildroot SDK), given as toolchain_file (CMAKE_TOOLCHAIN_FILE from the command
# line or, via CMake, from the environment). out_var is empty when the target is
# not linux or no toolchain file other than ESP-IDF's own is given.
#
function(__get_linux_custom_toolchain_file target toolchain_file out_var)
    set(${out_var} "" PARENT_SCOPE)
    if(NOT target STREQUAL "linux")
        return()
    endif()

    # CMake caches ESP-IDF's own toolchain-linux.cmake after the first host build.
    get_filename_component(toolchain_filename "${toolchain_file}" NAME)
    if(toolchain_filename STREQUAL "toolchain-linux.cmake")
        set(toolchain_file "")
    endif()

    # Resolve the path the same way CMake does, so it can be compared below.
    if(toolchain_file AND NOT IS_ABSOLUTE "${toolchain_file}")
        if(EXISTS "${CMAKE_BINARY_DIR}/${toolchain_file}")
            set(toolchain_file "${CMAKE_BINARY_DIR}/${toolchain_file}")
        else()
            set(toolchain_file "${CMAKE_SOURCE_DIR}/${toolchain_file}")
        endif()
    endif()
    if(toolchain_file)
        get_filename_component(toolchain_file "${toolchain_file}" REALPATH)
    endif()

    # CMake silently ignores a changed toolchain file once the compiler has been
    # detected, so changing it is an error. The selection is kept in a separate
    # cache entry, as -DCMAKE_TOOLCHAIN_FILE overwrites the cached value before it
    # can be compared. An empty value means the host toolchain.
    if(DEFINED CACHE{__IDF_LINUX_TOOLCHAIN_FILE}
       AND NOT "$CACHE{__IDF_LINUX_TOOLCHAIN_FILE}" STREQUAL "${toolchain_file}")
        message(FATAL_ERROR " The build directory was configured with toolchain file"
                " '$CACHE{__IDF_LINUX_TOOLCHAIN_FILE}', not '${toolchain_file}'"
                " (empty means the host toolchain). To change the toolchain file,"
                " clear the build directory and build the project again.")
    endif()
    set(__IDF_LINUX_TOOLCHAIN_FILE "${toolchain_file}" CACHE INTERNAL "Toolchain file selected for the linux target")

    set(${out_var} "${toolchain_file}" PARENT_SCOPE)
endfunction()
