include_guard(GLOBAL)

#[[
    __ldgen_process_template(LIBRARY <target>
                             TEMPLATE <template>
                             OUTPUT <output>
                             [SUFFIX <suffix>])

    *TARGET[in]*

        A library interface target that already has the properties
        __LDGEN_LIBRARIES, __LDGEN_FRAGMENT_FILES, and __LDGEN_DEPENDS set.
        These properties include libraries and fragment files for components
        linked to the library interface.

    *TEMPLATE[in]*

        Linker script template to process.

    *OUTPUT[in]*

        The output filename where the generated linker script will be saved.

    *SUFFIX[in,opt]*

        Optional suffix for the generated files containing the list of linked
        library archives.

    Pass the linker script ``TEMPLATE`` to the linker script generation tool,
    ldgen, for processing. The processed linker script is stored in the file
    specified by the ``OUTPUT`` option.

    This function can be called multiple times to generate multiple linker
    scripts from a single template. For example, a component like esp_system
    may be linked to multiple library interface targets, each with a different
    set of components. This results in different sets of fragment files and
    library archives. The suffix is used to ensure that the files listing the
    archives are not overwritten.
#]]
function(__ldgen_process_template)
    set(options)
    set(one_value LIBRARY TEMPLATE SUFFIX OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_LIBRARY)
        idf_die("LIBRARY option is required")
    endif()

    if(NOT DEFINED ARG_TEMPLATE)
        idf_die("TEMPLATE option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(config_env_path __CONFIG_ENV_PATH)

    idf_library_get_property(ldgen_libraries "${ARG_LIBRARY}" __LDGEN_LIBRARIES)
    idf_library_get_property(ldgen_fragment_files "${ARG_LIBRARY}" __LDGEN_FRAGMENT_FILES)
    idf_library_get_property(ldgen_depends "${ARG_LIBRARY}" __LDGEN_DEPENDS)

    list(JOIN ldgen_libraries "\n" ldgen_libraries)

    file(WRITE "${build_dir}/ldgen_libraries.in${ARG_SUFFIX}" "${ldgen_libraries}")
    file(GENERATE OUTPUT "${build_dir}/ldgen_libraries${ARG_SUFFIX}"
         INPUT "${build_dir}/ldgen_libraries.in${ARG_SUFFIX}")

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES
        "${build_dir}/ldgen_libraries.in${ARG_SUFFIX}"
        "${build_dir}/ldgen_libraries${ARG_SUFFIX}")

    if($ENV{LDGEN_CHECK_MAPPING})
        set(ldgen_check "--check-mapping"
            "--check-mapping-exceptions" "${idf_path}/tools/ci/check_ldgen_mapping_exceptions.txt")
        idf_msg("Mapping check enabled in ldgen")
    endif()

    add_custom_command(
        OUTPUT "${ARG_OUTPUT}"
        COMMAND ${python} "${idf_path}/tools/ldgen/ldgen.py"
        --config    "${sdkconfig}"
        --fragments-list "${ldgen_fragment_files}"
        --input     "${ARG_TEMPLATE}"
        --output    "${ARG_OUTPUT}"
        --kconfig   "${root_kconfig}"
        --env-file  "${config_env_path}"
        --env "IDF_BUILD_V2=y"
        --libraries-file "${build_dir}/ldgen_libraries${ARG_SUFFIX}"
        --objdump   "${CMAKE_OBJDUMP}"
        ${ldgen_check}
        DEPENDS     ${ARG_TEMPLATE} ${ldgen_fragment_files} ${ldgen_depends} ${sdkconfig}
        VERBATIM
    )
endfunction()
