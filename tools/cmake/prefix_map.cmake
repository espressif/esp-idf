# Utilities for remapping path prefixes
#
# __generate_prefix_map
# Prepares the list of compiler flags for remapping various paths
# to fixed names. This is used when reproducible builds are required.
# This function also creates a gdbinit file for the debugger to
# remap the substituted paths back to the real paths in the filesystem.
function(__generate_prefix_map compile_options_var)
    set(compile_options)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_components BUILD_COMPONENTS)

    if(CONFIG_COMPILER_HIDE_PATHS_MACROS)
        list(APPEND compile_options "-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.")
        list(APPEND compile_options "-fmacro-prefix-map=${idf_path}=/IDF")
    endif()

    if(CONFIG_APP_REPRODUCIBLE_BUILD)
        list(APPEND compile_options "-fdebug-prefix-map=${idf_path}=/IDF")
        list(APPEND compile_options "-fdebug-prefix-map=${PROJECT_DIR}=/IDF_PROJECT")
        list(APPEND compile_options "-fdebug-prefix-map=${BUILD_DIR}=/IDF_BUILD")

        # Generate mapping for component paths
        set(gdbinit_file_lines)
        foreach(component_name ${build_components})
            idf_component_get_property(component_dir ${component_name} COMPONENT_DIR)

            string(TOUPPER ${component_name} component_name_uppercase)
            set(substituted_path "/COMPONENT_${component_name_uppercase}_DIR")
            list(APPEND compile_options "-fdebug-prefix-map=${component_dir}=${substituted_path}")
            string(APPEND gdbinit_file_lines "set substitute-path ${substituted_path} ${component_dir}\n")
        endforeach()

        # Mapping for toolchain path
        execute_process(
            COMMAND ${CMAKE_C_COMPILER} -print-sysroot
            OUTPUT_VARIABLE compiler_sysroot
        )
        if(compiler_sysroot STREQUAL "")
            message(FATAL_ERROR "Failed to determine toolchain sysroot")
        endif()
        string(STRIP "${compiler_sysroot}" compiler_sysroot)
        get_filename_component(compiler_sysroot "${compiler_sysroot}/.." REALPATH)
        list(APPEND compile_options "-fdebug-prefix-map=${compiler_sysroot}=/TOOLCHAIN")
        string(APPEND gdbinit_file_lines "set substitute-path /TOOLCHAIN ${compiler_sysroot}\n")

        # Write the final gdbinit file
        set(gdbinit_path "${BUILD_DIR}/prefix_map_gdbinit")
        file(WRITE "${gdbinit_path}" "${gdbinit_file_lines}")
        idf_build_set_property(DEBUG_PREFIX_MAP_GDBINIT "${gdbinit_path}")
    endif()

    set(${compile_options_var} ${compile_options} PARENT_SCOPE)
endfunction()
