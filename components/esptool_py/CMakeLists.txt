idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(esp_tee_build)
    idf_component_register()
    return()
endif()

idf_component_register(REQUIRES bootloader PRIV_REQUIRES partition_table)

if(NOT BOOTLOADER_BUILD)
    idf_build_get_property(build_dir BUILD_DIR)




    # Generate flasher_args.json for tools that need it. The variables below are used
    # in configuring the template flasher_args.json.in.
    # Some of the variables (flash mode, size, frequency, chip) are already set in project_include.cmake.

    set(ESPTOOLPY_BEFORE "${CONFIG_ESPTOOLPY_BEFORE}")
    set(ESPTOOLPY_AFTER  "${CONFIG_ESPTOOLPY_AFTER}")
    if(CONFIG_ESPTOOLPY_NO_STUB)
        set(ESPTOOLPY_WITH_STUB false)
    else()
        set(ESPTOOLPY_WITH_STUB true)
    endif()

    if(CONFIG_SECURE_BOOT OR CONFIG_SECURE_FLASH_ENC_ENABLED)
        # If security enabled then override post flash option
        set(ESPTOOLPY_AFTER "no_reset")
    endif()

    if(CONFIG_APP_BUILD_GENERATE_BINARIES)
        # Generate flasher args files
        file(READ "flasher_args.json.in" flasher_args_content)
        string(CONFIGURE "${flasher_args_content}" flasher_args_content)

        file_generate("${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in"
                    CONTENT "${flasher_args_content}")
        file_generate("${CMAKE_BINARY_DIR}/flasher_args.json"
                    INPUT "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in")
    endif()
endif()  # NOT BOOTLOADER_BUILD
