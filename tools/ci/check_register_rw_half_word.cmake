function(check_register_rw_half_word)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOC_MODULES HAL_MODULES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SOC_MODULES)
        message(FATAL_ERROR "SOC_MODULES are required")
    endif()

    if(NOT ARG_HAL_MODULES)
        message(FATAL_ERROR "HAL_MODULES are required")
    endif()

    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_target IDF_TARGET)

    execute_process(
        COMMAND ${python} $ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.py
                          --target-chip ${idf_target}
                          --soc-modules ${ARG_SOC_MODULES}
                          --hal-modules ${ARG_HAL_MODULES}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )

    if(result)
        message(FATAL_ERROR "${output}")
    endif()
endfunction()
