#
# Warn if the toolchain version doesn't match
#
if(NOT (${target} STREQUAL "linux" OR CMAKE_C_COMPILER_ID MATCHES "Clang"))
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -dumpmachine
        OUTPUT_VARIABLE toolchain_name
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    check_expected_tool_version(${toolchain_name} ${CMAKE_C_COMPILER})
endif()

if(NOT ${target} STREQUAL "linux" AND CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(WARNING "Building ESP-IDF with clang is an experimental feature and is not yet officially supported.")
endif()
