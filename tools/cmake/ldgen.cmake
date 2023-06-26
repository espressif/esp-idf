# Utilities for supporting linker script generation in the build system

# __ldgen_add_fragment_files
#
# Add one or more linker fragment files, and append it to the list of fragment
# files found so far.
function(__ldgen_add_fragment_files fragment_files)
    spaces2list(fragment_files)

    foreach(fragment_file ${fragment_files})
        get_filename_component(abs_path ${fragment_file} ABSOLUTE)
        list(APPEND _fragment_files ${abs_path})
    endforeach()

    idf_build_set_property(__LDGEN_FRAGMENT_FILES "${_fragment_files}" APPEND)
endfunction()

# __ldgen_add_component
#
# Generate sections info for specified target to be used in linker script generation
function(__ldgen_add_component component_lib)
    idf_build_set_property(__LDGEN_LIBRARIES "$<TARGET_FILE:${component_lib}>" APPEND)
    idf_build_set_property(__LDGEN_DEPENDS ${component_lib} APPEND)
endfunction()

# __ldgen_process_template
#
# Passes a linker script template to the linker script generation tool for
# processing
function(__ldgen_process_template template output)
    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(idf_path IDF_PATH)

    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(ldgen_libraries __LDGEN_LIBRARIES GENERATOR_EXPRESSION)
    file(GENERATE OUTPUT ${build_dir}/ldgen_libraries.in CONTENT $<JOIN:${ldgen_libraries},\n>)
    file(GENERATE OUTPUT ${build_dir}/ldgen_libraries INPUT ${build_dir}/ldgen_libraries.in)

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES
        "${build_dir}/ldgen_libraries.in"
        "${build_dir}/ldgen_libraries")

    idf_build_get_property(ldgen_fragment_files __LDGEN_FRAGMENT_FILES GENERATOR_EXPRESSION)
    idf_build_get_property(ldgen_depends __LDGEN_DEPENDS GENERATOR_EXPRESSION)
    # Create command to invoke the linker script generator tool.
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)

    idf_build_get_property(python PYTHON)

    idf_build_get_property(config_env_path CONFIG_ENV_PATH)

    if($ENV{LDGEN_CHECK_MAPPING})
        set(ldgen_check "--check-mapping"
            "--check-mapping-exceptions" "${idf_path}/tools/ci/check_ldgen_mapping_exceptions.txt")
        message(STATUS "Mapping check enabled in ldgen")
    endif()

    add_custom_command(
        OUTPUT ${output}
        COMMAND ${python} "${idf_path}/tools/ldgen/ldgen.py"
        --config    "${sdkconfig}"
        --fragments-list "${ldgen_fragment_files}"
        --input     "${template}"
        --output    "${output}"
        --kconfig   "${root_kconfig}"
        --env-file  "${config_env_path}"
        --libraries-file "${build_dir}/ldgen_libraries"
        --objdump   "${CMAKE_OBJDUMP}"
        ${ldgen_check}
        DEPENDS     ${template} ${ldgen_fragment_files} ${ldgen_depends} ${SDKCONFIG}
        VERBATIM
    )

    get_filename_component(_name ${output} NAME)
    add_custom_target(__ldgen_output_${_name} DEPENDS ${output})
    add_dependencies(__idf_build_target __ldgen_output_${_name})
    idf_build_set_property(__LINK_DEPENDS ${output} APPEND)
endfunction()
