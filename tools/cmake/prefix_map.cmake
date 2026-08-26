# Utilities for remapping path prefixes
#
# __generate_prefix_map
# Prepares the list of compiler flags for remapping various paths
# to fixed names. This is used when reproducible builds are required.
# This function also creates a gdbinit file for the debugger to
# remap the substituted paths back to the real paths in the filesystem.
#
# On Windows hosts, very long compile commands are avoided by enabling Ninja response
# files in toolchain.cmake (CMAKE_NINJA_FORCE_RESPONSE_FILE), not by dropping these
# options when paths contain spaces.
function(__generate_prefix_map compile_options_var)
    set(compile_options)
    set(gdbinit_dir ${BUILD_DIR}/gdbinit)
    set(gdbinit_path "${gdbinit_dir}/prefix_map")
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_components BUILD_COMPONENTS)

    if(CONFIG_COMPILER_HIDE_PATHS_MACROS)
        file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}" _src_dir)
        file(TO_CMAKE_PATH "${idf_path}" _idf_path)
        list(APPEND compile_options "-fmacro-prefix-map=${_src_dir}=.")
        list(APPEND compile_options "-fmacro-prefix-map=${_idf_path}=/IDF")
    endif()

    if(CONFIG_APP_REPRODUCIBLE_BUILD)
        file(TO_CMAKE_PATH "${idf_path}" _idf_path)
        file(TO_CMAKE_PATH "${PROJECT_DIR}" _project_dir)
        file(TO_CMAKE_PATH "${BUILD_DIR}" _build_dir)
        list(APPEND compile_options "-fdebug-prefix-map=${_idf_path}=/IDF")
        list(APPEND compile_options "-fdebug-prefix-map=${_project_dir}=/IDF_PROJECT")
        list(APPEND compile_options "-fdebug-prefix-map=${_build_dir}=/IDF_BUILD")

        set(gdbinit_file_lines)
        string(APPEND gdbinit_file_lines "set substitute-path /IDF ${idf_path}\n")
        string(APPEND gdbinit_file_lines "set substitute-path /IDF_PROJECT ${PROJECT_DIR}\n")
        string(APPEND gdbinit_file_lines "set substitute-path /IDF_BUILD ${BUILD_DIR}\n")

        # Map components outside IDF_PATH to stable synthetic paths (reproducible builds).
        foreach(component_name ${build_components})
            idf_component_get_property(component_dir ${component_name} COMPONENT_DIR)
            file(TO_CMAKE_PATH "${component_dir}" _component_dir)
            file(RELATIVE_PATH _component_rel "${_idf_path}" "${_component_dir}")
            if(_component_rel MATCHES "^\\.\\.(/|$)")
                string(TOUPPER ${component_name} component_name_uppercase)
                set(substituted_path "/COMPONENT_${component_name_uppercase}_DIR")
                list(APPEND compile_options "-fdebug-prefix-map=${_component_dir}=${substituted_path}")
                string(APPEND gdbinit_file_lines "set substitute-path ${substituted_path} ${component_dir}\n")
            endif()
        endforeach()

        execute_process(
            COMMAND ${CMAKE_C_COMPILER} -print-sysroot
            OUTPUT_VARIABLE compiler_sysroot
        )
        if(compiler_sysroot STREQUAL "")
            message(FATAL_ERROR "Failed to determine toolchain sysroot")
        endif()
        string(STRIP "${compiler_sysroot}" compiler_sysroot)
        get_filename_component(compiler_sysroot "${compiler_sysroot}/.." REALPATH)
        file(TO_CMAKE_PATH "${compiler_sysroot}" _compiler_sysroot)
        list(APPEND compile_options "-fdebug-prefix-map=${_compiler_sysroot}=/TOOLCHAIN")
        string(APPEND gdbinit_file_lines "set substitute-path /TOOLCHAIN ${compiler_sysroot}\n")
    else()
        set(gdbinit_file_lines "# There is no prefix map defined for the project.\n")
    endif()
    # Write prefix_map file even it is empty.
    file(MAKE_DIRECTORY ${gdbinit_dir})
    file(WRITE "${gdbinit_path}" "${gdbinit_file_lines}")
    idf_build_set_property(GDBINIT_FILES_PREFIX_MAP "${gdbinit_path}")

    set(${compile_options_var} ${compile_options} PARENT_SCOPE)
endfunction()
