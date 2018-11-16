# Utilities for supporting linker script generation in the build system

# ldgen_create_target
#
# Create the custom target to attach the fragment files and template files
# for the build to.
function(ldgen_set_variables)
    add_custom_target(ldgen_section_infos)
    add_custom_target(ldgen DEPENDS ldgen_section_infos)
endfunction()

# ldgen_add_fragment_file
#
# Add one or more linker fragment files, and append it to the list of fragment
# files found so far.
function(ldgen_add_fragment_files target fragment_files)
    spaces2list(fragment_files)

    foreach(fragment_file ${fragment_files})
        get_filename_component(fragment_file_abs_dir ${fragment_file} ABSOLUTE BASE_DIR ${component_dir})
        list(APPEND fragment_files_full_path ${fragment_file_abs_dir})
    endforeach()

    set_property(TARGET ldgen APPEND PROPERTY FRAGMENT_FILES ${fragment_files_full_path})

    get_filename_component(target_sections_info ${CMAKE_CURRENT_BINARY_DIR}/${target}.sections_info ABSOLUTE)

    add_custom_command(
        OUTPUT ${target_sections_info}
        COMMAND ${CMAKE_OBJDUMP} $<TARGET_FILE:${target}> -h >  ${target_sections_info}
        DEPENDS ${target}
        )

    add_custom_target(${target}_sections_info DEPENDS ${target_sections_info})
    add_dependencies(ldgen_section_infos ${target}_sections_info)

    set_property(TARGET ldgen_section_infos APPEND PROPERTY SECTIONS_INFO_FILES ${target_sections_info})
endfunction()

# ldgen_process_template
#
# Passes a linker script template to the linker script generation tool for
# processing
function(ldgen_process_template template output)
    # Create command to invoke the linker script generator tool.
    add_custom_command(
        OUTPUT ${output}
        COMMAND ${IDF_PATH}/tools/ldgen/ldgen.py
        --config    ${SDKCONFIG}
        --fragments "$<JOIN:$<TARGET_PROPERTY:ldgen,FRAGMENT_FILES>,\t>"
        --input     ${template}
        --output    ${output}
        --sections  "$<JOIN:$<TARGET_PROPERTY:ldgen_section_infos,SECTIONS_INFO_FILES>,\t>"
        --kconfig   ${IDF_PATH}/Kconfig
        --env       "COMPONENT_KCONFIGS=${COMPONENT_KCONFIGS}"
        --env       "COMPONENT_KCONFIGS_PROJBUILD=${COMPONENT_KCONFIGS_PROJBUILD}"
        --env       "IDF_CMAKE=y"
        --env       "IDF_TARGET=${IDF_TARGET}"
        DEPENDS     ${template} $<TARGET_PROPERTY:ldgen,FRAGMENT_FILES> ${SDKCONFIG} ldgen_section_infos
    )

    get_filename_component(output_name ${output} NAME)
    add_custom_target(ldgen_${output_name}_script DEPENDS ${output})
    add_dependencies(ldgen ldgen_${output_name}_script)
endfunction()

# ldgen_create_commands
#
# Create the command to generate the output scripts from templates presented.
function(ldgen_add_dependencies executable_name)
    add_dependencies(${executable_name} ldgen)
endfunction()