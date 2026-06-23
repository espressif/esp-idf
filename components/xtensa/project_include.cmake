idf_build_get_property(idf_target_arch IDF_TARGET_ARCH)
if(NOT "${idf_target_arch}" STREQUAL "xtensa")
    return()
endif()

# Check toolchain is configured properly in cmake
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    # without '--target' option 'clang -dumpmachine' prints default target arch and it might be not Xtensa
    # so use `-print-targets` option
    __compiler_query(dump_machine ${CMAKE_C_COMPILER} -print-targets)
else()
    __compiler_query(dump_machine ${CMAKE_C_COMPILER} -dumpmachine)
endif()
message(STATUS "Compiler supported targets: ${dump_machine}")

if(NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "Generic" AND "${dump_machine}" MATCHES "xtensa"))
    message(FATAL_ERROR "Internal error, toolchain has not been set correctly by project "
        "(or an invalid CMakeCache.txt file has been generated somehow)")
endif()
