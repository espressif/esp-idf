# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

foreach(required_var ULP_FSM_PREPROCESSOR ULP_FSM_ASSEMBLER ULP_FSM_TARGET ULP_FSM_SOURCE ULP_FSM_OBJECT)
    if(NOT DEFINED ${required_var} OR "${${required_var}}" STREQUAL "")
        message(FATAL_ERROR "${required_var} is required")
    endif()
endforeach()

set(extra_args)
set(extra_args_started OFF)
math(EXPR last_arg_index "${CMAKE_ARGC} - 1")
foreach(arg_index RANGE 0 ${last_arg_index})
    if("${CMAKE_ARGV${arg_index}}" STREQUAL "--")
        set(extra_args_started ON)
        continue()
    endif()

    if(extra_args_started)
        list(APPEND extra_args "${CMAKE_ARGV${arg_index}}")
    endif()
endforeach()

set(preprocessed_source "${ULP_FSM_OBJECT}.ulp.S")
set(listing_file "${ULP_FSM_OBJECT}.lst")

execute_process(
    COMMAND "${ULP_FSM_PREPROCESSOR}"
            -D__ASSEMBLER__ -E -P -x c
            ${extra_args}
            -o "${preprocessed_source}"
            "${ULP_FSM_SOURCE}"
    RESULT_VARIABLE preprocess_result
)
if(NOT preprocess_result EQUAL 0)
    message(FATAL_ERROR "Failed to preprocess ${ULP_FSM_SOURCE}")
endif()

execute_process(
    COMMAND "${ULP_FSM_ASSEMBLER}"
            "--mcpu=${ULP_FSM_TARGET}"
            "-al=${listing_file}"
            -o "${ULP_FSM_OBJECT}"
            -c "${preprocessed_source}"
    RESULT_VARIABLE assemble_result
)
if(NOT assemble_result EQUAL 0)
    message(FATAL_ERROR "Failed to assemble ${preprocessed_source}")
endif()
