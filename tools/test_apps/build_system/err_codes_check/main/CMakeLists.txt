set(idf_path "$ENV{IDF_PATH}")
set(project_dir "${CMAKE_CURRENT_LIST_DIR}/..")

# Step 1: Extract all error codes from the entire ESP-IDF components directory
set(all_err_csv "${CMAKE_CURRENT_BINARY_DIR}/all_esp_err_codes.csv")
execute_process(
    COMMAND ${PYTHON} "${idf_path}/tools/err_codes_extract.py"
            --search-dirs "components"
            --base-path "${idf_path}"
            --output "${all_err_csv}"
    RESULT_VARIABLE extract_result
    OUTPUT_VARIABLE extract_output
    ERROR_VARIABLE extract_error
)
if(NOT extract_result EQUAL 0)
    message(FATAL_ERROR "err_codes_extract.py failed: ${extract_error}")
endif()

# Step 2: Generate the test source file from the CSV
set(test_src "${CMAKE_CURRENT_BINARY_DIR}/err_codes_test_cases.c")
execute_process(
    COMMAND ${PYTHON} "${project_dir}/gen_err_codes_test.py"
            "${all_err_csv}" "${test_src}"
    RESULT_VARIABLE gen_result
    OUTPUT_VARIABLE gen_output
    ERROR_VARIABLE gen_error
)
if(NOT gen_result EQUAL 0)
    message(FATAL_ERROR "gen_err_codes_test.py failed: ${gen_error}")
endif()

idf_component_register(SRCS "test_main.c" "${test_src}"
                    INCLUDE_DIRS ".")
