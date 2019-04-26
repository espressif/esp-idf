# Utilities for supporting linker script generation in the build system

# ldgen_set_variables
#
# Create the custom target to attach the fragment files and template files
# for the build to.
function(ldgen_set_variables)
    add_custom_target(ldgen)
endfunction()

# ldgen_add_fragment_files
#
# Add one or more linker fragment files, and append it to the list of fragment
# files found so far.
function(ldgen_add_fragment_files component fragment_files)
    spaces2list(fragment_files)

    foreach(fragment_file ${fragment_files})
        get_filename_component(_fragment_file ${fragment_file} ABSOLUTE)
        list(APPEND _fragment_files ${_fragment_file})
    endforeach()

    set_property(TARGET ldgen APPEND PROPERTY FRAGMENT_FILES ${_fragment_files})
endfunction()

# ldgen_component_add
#
# Add component to known libraries for linker script generation
function(ldgen_component_add component_lib)
    set_property(TARGET ldgen APPEND PROPERTY OUTPUT_LIBRARIES "$<TARGET_FILE:${component_lib}>")
    set_property(TARGET ldgen APPEND PROPERTY LIBRARIES ${component_lib})
endfunction()

# ldgen_process_template
#
# Passes a linker script template to the linker script generation tool for
# processing
function(ldgen_process_template template output)
    get_property(output_libraries TARGET ldgen PROPERTY OUTPUT_LIBRARIES)
    file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/ldgen_libraries.in CONTENT "$<JOIN:${output_libraries},\n>")
    file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/ldgen_libraries INPUT ${CMAKE_BINARY_DIR}/ldgen_libraries.in)

    get_filename_component(filename "${template}" NAME_WE)

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        "${CMAKE_BINARY_DIR}/ldgen_libraries.in"
        "${CMAKE_BINARY_DIR}/ldgen_libraries")

    add_custom_command(
        OUTPUT ${output}
        COMMAND ${PYTHON} ${IDF_PATH}/tools/ldgen/ldgen.py
        --config    ${SDKCONFIG}
        --fragments "$<JOIN:$<TARGET_PROPERTY:ldgen,FRAGMENT_FILES>,\t>"
        --input     ${template}
        --output    ${output}
        --kconfig   ${ROOT_KCONFIG}
        --env       "COMPONENT_KCONFIGS=${COMPONENT_KCONFIGS}"
        --env       "COMPONENT_KCONFIGS_PROJBUILD=${COMPONENT_KCONFIGS_PROJBUILD}"
        --env       "IDF_CMAKE=y"
        --env       "IDF_PATH=${IDF_PATH}"
        --env       "IDF_TARGET=${IDF_TARGET}"
        --libraries-file ${CMAKE_BINARY_DIR}/ldgen_libraries
        --objdump   ${CMAKE_OBJDUMP}
        DEPENDS     ${template} $<TARGET_PROPERTY:ldgen,FRAGMENT_FILES>
                    $<TARGET_PROPERTY:ldgen,LIBRARIES> ${SDKCONFIG}
    )

    get_filename_component(output_name ${output} NAME)
    add_custom_target(ldgen_${output_name}_script DEPENDS ${output})
    add_dependencies(ldgen ldgen_${output_name}_script)
endfunction()

# ldgen_add_dependencies
#
# Add dependency of project executable to ldgen custom target.
function(ldgen_add_dependencies)
    if(IDF_PROJECT_EXECUTABLE)
        add_dependencies(${IDF_PROJECT_EXECUTABLE} ldgen)
    endif()
endfunction()