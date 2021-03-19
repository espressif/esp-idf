idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "esp32")
    return()
endif()

if(NOT BOOTLOADER_BUILD)
    # [refactor-todo] propagate these requirements for compatibility
    # remove in the future
    set(legacy_reqs driver efuse soc)
endif()

idf_component_register(INCLUDE_DIRS include
                       REQUIRES xtensa "${legacy_reqs}"
                       REQUIRED_IDF_TARGETS esp32)

target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32.peripherals.ld")

if(NOT BOOTLOADER_BUILD)
    idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)
    target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld")

    # Process the template file through the linker script generation mechanism, and use the output for linking the
    # final binary
    target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/ld/esp32.project.ld.in"
                        PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ld/esp32.project.ld")

    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32.peripherals.ld")

    idf_build_get_property(config_dir CONFIG_DIR)
    # Preprocess esp32.ld linker script to include configuration, becomes esp32_out.ld
    set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ld)
    add_custom_command(
        OUTPUT esp32_out.ld
        COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o esp32_out.ld -I ${config_dir} ${LD_DIR}/esp32.ld
        MAIN_DEPENDENCY ${LD_DIR}/esp32.ld
        DEPENDS ${sdkconfig_header}
        COMMENT "Generating linker script..."
        VERBATIM)

    add_custom_target(esp32_linker_script DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld)
    add_dependencies(${COMPONENT_LIB} esp32_linker_script)
endif()
