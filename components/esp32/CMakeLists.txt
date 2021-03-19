idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "esp32")
    return()
endif()

idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)

if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp32 is headers
    idf_component_register(INCLUDE_DIRS include REQUIRES xtensa)
    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32.peripherals.ld")
else()
    # Regular app build
    set(include_dirs "include")

    set(requires driver efuse soc xtensa) #unfortunately rom/uart uses SOC registers directly

    # app_update is added here because cpu_start.c uses esp_ota_get_app_description() function.
    # esp_timer is added here because cpu_start.c uses esp_timer
    set(priv_requires app_trace app_update bootloader_support esp_system log mbedtls nvs_flash pthread
        spi_flash vfs espcoredump esp_common perfmon esp_timer esp_ipc esp_pm)

    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS "${include_dirs}"
                        REQUIRES "${requires}"
                        PRIV_REQUIRES "${priv_requires}"
                        REQUIRED_IDF_TARGETS esp32)

    target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld")

    # Process the template file through the linker script generation mechanism, and use the output for linking the
    # final binary
    target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/ld/esp32.project.ld.in"
                        PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ld/esp32.project.ld")

    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32.peripherals.ld")
    target_link_libraries(${COMPONENT_LIB} INTERFACE gcc)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u call_user_start_cpu0")

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
