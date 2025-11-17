include(${CMAKE_CURRENT_LIST_DIR}/toolchain_flags.cmake)

if(NOT CMAKE_PARENT_LIST_FILE)
    message(FATAL_ERROR "toolchain.cmake cannot be used standalone (use chip-specific toolchain file instead)")
endif()

# Paths normalization
get_filename_component(_idf_toolchain_dir "$ENV{IDF_PATH}/tools/cmake" REALPATH)
file(TO_CMAKE_PATH "${_idf_toolchain_dir}" _idf_toolchain_dir)

get_filename_component(_toolchain_filename "${CMAKE_TOOLCHAIN_FILE}" NAME)
get_filename_component(_current_toolchain_dir "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY REALPATH)
file(TO_CMAKE_PATH "${_current_toolchain_dir}" _current_toolchain_dir)

set(CMAKE_SYSTEM_NAME Generic)

# Set compiler tools according to the toolchain type
string(FIND "${_toolchain_filename}" "clang" found_clang)
if(NOT found_clang EQUAL -1)
    set(IDF_TOOLCHAIN "clang" CACHE STRING "IDF Build Toolchain Type" FORCE)

    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_ASM_COMPILER clang)
    set(CMAKE_LINKER ${_CMAKE_TOOLCHAIN_PREFIX}ld)
    set(CMAKE_AR llvm-ar)
    set(CMAKE_RANLIB llvm-ranlib)
    set(CMAKE_OBJDUMP ${_CMAKE_TOOLCHAIN_PREFIX}objdump)
else()
    set(IDF_TOOLCHAIN "gcc" CACHE STRING "IDF Build Toolchain Type" FORCE)

    set(CMAKE_C_COMPILER ${_CMAKE_TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_CXX_COMPILER ${_CMAKE_TOOLCHAIN_PREFIX}g++)
    set(CMAKE_ASM_COMPILER ${_CMAKE_TOOLCHAIN_PREFIX}gcc)
endif()

# Handle different execution contexts for the toolchain file.
# CMake may execute this toolchain file in different contexts:
#
# 1. First execution (IDF project build):
#    When CMAKE_TOOLCHAIN_FILE points to IDF sources, the toolchain file is
#    executed for the main project. In this case, we create the response files
#    directory in the build directory, copy the toolchain file there, initialize
#    the response files, and update CMAKE_TOOLCHAIN_FILE to point to the copy.
#
# 2. Subsequent executions (External project builds):
#    When CMAKE_TOOLCHAIN_FILE points to a copied toolchain file (from a
#    previous root project build), the response files already exist in the same
#    directory as the toolchain file. We simply set IDF_TOOLCHAIN_BUILD_DIR to
#    point to that existing directory.
if(_idf_toolchain_dir STREQUAL _current_toolchain_dir)
    set(IDF_TOOLCHAIN_BUILD_DIR "${CMAKE_BINARY_DIR}/toolchain"
        CACHE PATH "Path to toolchain build directory containing response files and toolchain file copy" FORCE)

    # Copy toolchain file into the build directory and update CMAKE_TOOLCHAIN_FILE
    # to point to the copy. This approach allows us to avoid worrying about different
    # CMAKE_BINARY_DIR values between base IDF-project builds and external projects.
    # For external project builds, compiler response files are located in the same
    # directory as CMAKE_TOOLCHAIN_FILE, making them easy to find.
    file(MAKE_DIRECTORY "${IDF_TOOLCHAIN_BUILD_DIR}")
    file(COPY "${CMAKE_TOOLCHAIN_FILE}" DESTINATION "${IDF_TOOLCHAIN_BUILD_DIR}")
    set(CMAKE_TOOLCHAIN_FILE "${IDF_TOOLCHAIN_BUILD_DIR}/${_toolchain_filename}")

    # Create response files before CMake performs compiler checks.
    # These files are required for the compiler detection process to succeed.
    # The files are created even when the flag variables are empty, ensuring
    # they exist when referenced by CMAKE_*_FLAGS variables below.
    idf_toolchain_add_flags(C_COMPILE_OPTIONS "${CMAKE_C_FLAGS}"
                            CXX_COMPILE_OPTIONS "${CMAKE_CXX_FLAGS}"
                            ASM_COMPILE_OPTIONS "${CMAKE_ASM_FLAGS}"
                            LINK_OPTIONS "${CMAKE_EXE_LINKER_FLAGS}")
else()
    set(IDF_TOOLCHAIN_BUILD_DIR "${_current_toolchain_dir}"
        CACHE PATH "Path to toolchain build directory containing response files and toolchain file copy" FORCE)
endif()

# Configure CMake to use response files for compiler and linker flags.
# Some compilation options enabled by IDF configuration options are not yet
# defined at this very early CMake stage (toolchain.cmake execution). Response
# files allow these flags to be dynamically updated during the CMake configuration
# phase, after the options become available.
set(CMAKE_C_FLAGS "@\"${IDF_TOOLCHAIN_BUILD_DIR}/cflags\"" CACHE STRING "C Compiler Base Flags" FORCE)
set(CMAKE_CXX_FLAGS "@\"${IDF_TOOLCHAIN_BUILD_DIR}/cxxflags\"" CACHE STRING "C++ Compiler Base Flags" FORCE)
set(CMAKE_ASM_FLAGS "@\"${IDF_TOOLCHAIN_BUILD_DIR}/asmflags\"" CACHE STRING "Asm Compiler Base Flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "@\"${IDF_TOOLCHAIN_BUILD_DIR}/ldflags\"" CACHE STRING "Linker Base Flags" FORCE)
