# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# __compiler_query
#
# Run a compiler query command (e.g. "<compiler> -dumpmachine") given in ARGN
# and store its trimmed stdout in the variable named by "output_var".
#
# Fails with an actionable error if the command fails or returns no output. On
# some Windows systems, antivirus, endpoint-security or DLP/encryption software
# intercepts short-lived toolchain processes and strips their stdout when it is
# captured by the build system, while the same command works when run directly
# in a terminal. Without this guard the empty result collapses the callers'
# parsing into a cryptic CMake error (see
# https://github.com/espressif/esp-idf/issues/18727).
#
# This module is included by both the cmakev1 and cmakev2 utilities, because the
# esp_common and xtensa project_include.cmake files that call __compiler_query
# are shared by both build systems.
function(__compiler_query output_var)
    execute_process(
        COMMAND ${ARGN}
        OUTPUT_VARIABLE query_output
        RESULT_VARIABLE query_result
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT query_result EQUAL 0 OR query_output STREQUAL "")
        string(REPLACE ";" " " query_command "${ARGN}")
        message(FATAL_ERROR
            "Failed to query the compiler: '${query_command}' (result: ${query_result}).\n"
            "The command produced no output when run by the build system. This is usually caused "
            "by antivirus, endpoint-security or DLP/encryption software intercepting the compiler "
            "process and discarding its output; the same command often works when run directly in "
            "a terminal.\n"
            "Add an exclusion for the ESP-IDF tools directory in that software (on a managed "
            "machine you may need your IT department), then run 'idf.py fullclean' and build again.")
    endif()

    set(${output_var} "${query_output}" PARENT_SCOPE)
endfunction()
