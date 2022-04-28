# For each supported target, a memory.ld.in and sections.ld.in is processed and dictate the
# memory layout of the app.
#
# memory.ld.in goes through the preprocessor
# sections.ld.in goes through linker script generator

idf_build_get_property(target IDF_TARGET)
idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)

set(ld_input "${CMAKE_CURRENT_LIST_DIR}/${target}/memory.ld.in")
set(ld_output "${CMAKE_CURRENT_BINARY_DIR}/ld/memory.ld")
target_linker_script(${COMPONENT_LIB} INTERFACE "${ld_output}")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/ld")

# Process the template file through the linker script generation mechanism, and use the output for linking the
# final binary
target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/${target}/sections.ld.in"
                    PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ld/sections.ld")

idf_build_get_property(config_dir CONFIG_DIR)
# Preprocess memory.ld.in linker script to include configuration, becomes memory.ld
add_custom_command(
    OUTPUT ${ld_output}
    COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o ${ld_output} -I ${config_dir}
            -I "${CMAKE_CURRENT_LIST_DIR}" ${ld_input}
    MAIN_DEPENDENCY ${ld_input}
    DEPENDS ${sdkconfig_header}
    COMMENT "Generating memory.ld linker script..."
    VERBATIM)

add_custom_target(memory_ld DEPENDS ${ld_output})
add_dependencies(${COMPONENT_LIB} memory_ld)
