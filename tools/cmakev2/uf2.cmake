# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

#[[
.. cmakev2:function:: idf_create_uf2

    .. code-block:: cmake

        idf_create_uf2(<executable>
                       TARGET <target>
                       [APP_ONLY])

    *executable[in]*

        Executable target for which to create UF2 target.

    *TARGET[in]*

        Name of the UF2 generation target to be created (e.g., "uf2", "uf2-app").

    *APP_ONLY[option]*

        If specified, creates a UF2 binary file with only the application binary
        (uses --bin app option). If not specified, creates a UF2 binary file with
        all binaries from flasher_args.json.

    Create a UF2 (USB Flashing Format) target for the specified executable.

    Example usage:
        idf_create_uf2(myapp TARGET uf2)              # All binaries
        idf_create_uf2(myapp TARGET uf2-app APP_ONLY) # App binary only
#]]
function(idf_create_uf2 executable)
    set(options APP_ONLY)
    set(one_value TARGET)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT TARGET "${executable}")
        idf_die("Executable '${executable}' is not a cmake target")
    endif()

    # Get build properties
    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(target IDF_TARGET)

    # Path to UF2 output file
    set(uf2_output_file "${build_dir}/${ARG_TARGET}.bin")

    # Build UF2 command and arguments (matching cmakev1 pattern)
    set(UF2_ARGS --json "${build_dir}/flasher_args.json")
    set(UF2_CMD ${python} "${idf_path}/tools/mkuf2.py" write --chip ${target})

    # Add output file and --bin app if APP_ONLY is specified
    set(uf2_args_list "${UF2_ARGS};-o;${uf2_output_file}")
    if(ARG_APP_ONLY)
        list(APPEND uf2_args_list --bin app)
        set(comment_msg "Generating UF2 app binary for ${executable}")
    else()
        set(comment_msg "Generating UF2 binary for ${executable}")
    endif()

    # Create UF2 target (matching cmakev1 pattern using run_uf2_cmds.cmake)
    add_custom_target(${ARG_TARGET}
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_PATH=${idf_path}"
        -D "UF2_CMD=${UF2_CMD}"
        -D "UF2_ARGS=${uf2_args_list}"
        -P "${idf_path}/tools/cmake/run_uf2_cmds.cmake"
        USES_TERMINAL
        VERBATIM
        COMMENT "${comment_msg}"
    )

    idf_msg("Created UF2 target: ${ARG_TARGET}")
endfunction()
