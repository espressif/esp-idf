# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# CMake module for composable error code registration.
#
# Provides the idf_define_esp_err_codes() function that components call
# to register their error codes into a link-time array.
#

#
# idf_define_esp_err_codes
#
# Register error codes defined in the specified header files.
# This function:
#   1. Runs err_codes_extract.py to extract error codes from headers into a CSV
#   2. Runs err_codes_to_c.py to generate a C source file placing entries into
#      the .esp_err_msg_tbl linker section
#   3. Adds the generated C source file to the current component
#   4. Adds -Wl,--undefined to force-link the error code symbols
#
# Usage (must be called AFTER idf_component_register):
#   idf_define_esp_err_codes(HEADERS include/my_errors.h include/other.h)
#
# Note: This function relies on COMPONENT_LIB and COMPONENT_NAME which are
# only available after idf_component_register() has been called.
#
function(idf_define_esp_err_codes)
    # If error-to-name lookup is disabled, do nothing
    if(NOT CONFIG_ESP_ERR_TO_NAME_LOOKUP)
        return()
    endif()

    set(options "")
    set(single_value "")
    set(multi_value HEADERS)
    cmake_parse_arguments(ARG "${options}" "${single_value}" "${multi_value}" ${ARGN})

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(python PYTHON)

    # Determine component name
    if(NOT COMPONENT_NAME)
        get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
    endif()

    # Collect header files
    set(header_files "")

    # Process explicit headers (relative to component dir)
    foreach(header ${ARG_HEADERS})
        if(IS_ABSOLUTE ${header})
            list(APPEND header_files "${header}")
        else()
            list(APPEND header_files "${CMAKE_CURRENT_LIST_DIR}/${header}")
        endif()
    endforeach()

    if(NOT header_files)
        message(FATAL_ERROR "idf_define_esp_err_codes: No header files found for component ${COMPONENT_NAME}")
    endif()

    # Output paths
    set(csv_output "${build_dir}/esp_err_codes/${COMPONENT_NAME}_err_codes.csv")
    set(c_output "${build_dir}/esp_err_codes/${COMPONENT_NAME}_esp_err_codes.c")

    # Ensure output directory exists
    file(MAKE_DIRECTORY "${build_dir}/esp_err_codes")

    # Build the header arguments for the extract script
    # Always provide esp_err.h as a context header for ESP_ERR_*_BASE definitions
    set(header_args "")
    set(esp_err_h "${idf_path}/components/esp_common/include/esp_err.h")
    set(context_args "")
    set(need_context TRUE)
    foreach(h ${header_files})
        list(APPEND header_args "${h}")
        if("${h}" STREQUAL "${esp_err_h}")
            set(need_context FALSE)
        endif()
    endforeach()
    if(need_context)
        set(context_args "--context-headers" "${esp_err_h}")
    endif()

    # Step 1: Extract error codes from headers to CSV (build time)
    set(extract_script "${idf_path}/tools/err_codes_extract.py")

    add_custom_command(
        OUTPUT "${csv_output}"
        COMMAND ${python} "${extract_script}"
                --headers ${header_args}
                ${context_args}
                --base-path "${idf_path}"
                --output "${csv_output}"
        DEPENDS ${header_files} ${esp_err_h} "${extract_script}"
        COMMENT "Extracting error codes for ${COMPONENT_NAME}"
        VERBATIM
    )

    # Compute include-form headers for the generated C file
    set(include_headers "")
    foreach(h ${header_files})
        # Convert full path to include-relative path
        get_filename_component(h_name "${h}" NAME)
        # Try to find 'include' in the path
        string(FIND "${h}" "/include/" include_pos)
        if(include_pos GREATER -1)
            math(EXPR include_start "${include_pos} + 9")  # length of "/include/"
            string(SUBSTRING "${h}" ${include_start} -1 include_rel)
            list(APPEND include_headers "${include_rel}")
        else()
            list(APPEND include_headers "${h_name}")
        endif()
    endforeach()

    # Step 2: Generate C source from CSV (build time)
    set(gen_script "${idf_path}/tools/err_codes_to_c.py")
    set(header_flags "")
    foreach(inc ${include_headers})
        list(APPEND header_flags "--headers" "${inc}")
    endforeach()

    add_custom_command(
        OUTPUT "${c_output}"
        COMMAND ${python} "${gen_script}"
                --csv "${csv_output}"
                --output "${c_output}"
                --component "${COMPONENT_NAME}"
                ${header_flags}
        DEPENDS "${csv_output}" "${gen_script}"
        COMMENT "Generating error code table for ${COMPONENT_NAME}"
        VERBATIM
    )

    # Step 3: Add generated C source to the component
    target_sources(${COMPONENT_LIB} PRIVATE "${c_output}")

    # Step 4: Force-link the error codes symbol so it's not discarded
    string(REPLACE "-" "_" safe_component "${COMPONENT_NAME}")
    string(REPLACE "." "_" safe_component "${safe_component}")
    # On macOS (Mach-O) the linker sees C symbols with a leading '_' prepended
    # by the compiler. Pass the mangled name so -u resolves correctly.
    # On Linux (ELF) no prefix is added, so use the C identifier as-is.
    if(APPLE)
        set(force_link_symbol "_esp_err_msg_${safe_component}_include")
    else()
        set(force_link_symbol "esp_err_msg_${safe_component}_include")
    endif()

    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ${force_link_symbol}")

endfunction()
