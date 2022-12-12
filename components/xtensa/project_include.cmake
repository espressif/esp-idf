# Check toolchain is configured properly in cmake
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    # without '--target' option 'clang -dumpmachine' prints default target arch and it might be not Xtensa
    # so use `-print-targets` option
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -print-targets
        OUTPUT_VARIABLE dump_machine
    )
else()
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -dumpmachine
        OUTPUT_VARIABLE dump_machine
    )
endif()
message("Compiler supported targets: ${dump_machine}")

if(NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Generic" AND ${dump_machine} MATCHES xtensa))
    message(FATAL_ERROR "Internal error, toolchain has not been set correctly by project "
        "(or an invalid CMakeCache.txt file has been generated somehow)")
endif()
