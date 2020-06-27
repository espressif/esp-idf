idf_component_register(REQUIRES bootloader)

if(NOT BOOTLOADER_BUILD)
    idf_build_get_property(build_dir BUILD_DIR)

    if(CONFIG_APP_BUILD_GENERATE_BINARIES)
        partition_table_get_partition_info(app_partition_offset "--partition-boot-default" "offset")
        esptool_py_custom_target(app-flash app "app")

        esptool_py_flash_target_image(app-flash app "${app_partition_offset}" "${build_dir}/${PROJECT_BIN}")
        esptool_py_flash_target_image(flash app "${app_partition_offset}" "${build_dir}/${PROJECT_BIN}")
    endif()

    # If anti-rollback option is set then factory partition should not be in Partition Table.
    # In this case, should be used the partition table with two ota app without the factory.
    partition_table_get_partition_info(factory_offset "--partition-type app --partition-subtype factory" "offset")
    if(CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK AND factory_offset)
        fail_at_build_time(check_table_contents
            "ERROR: Anti-rollback option is enabled. Partition table should consist of two ota app without factory partition.")
        add_dependencies(app check_table_contents)
    endif()

    # Generate flasher_args.json for tools that need it. The variables below are used
    # in configuring the template flasher_args.json.in.
    # Some of the variables (flash mode, size, frequency) are already set in project_include.cmake.

    set(ESPTOOLPY_BEFORE "${CONFIG_ESPTOOLPY_BEFORE}")
    set(ESPTOOLPY_AFTER  "${CONFIG_ESPTOOLPY_AFTER}")
    set(ESPTOOLPY_CHIP "${target}")
    if(CONFIG_ESPTOOLPY_WITH_STUB)
        set(ESPTOOLPY_WITH_STUB true)
    else()
        set(ESPTOOLPY_WITH_STUB false)
    endif()

    if(CONFIG_SECURE_BOOT OR CONFIG_SECURE_FLASH_ENC_ENABLED)
        # If security enabled then override post flash option
        set(ESPTOOLPY_AFTER "no_reset")
    endif()

    if(CONFIG_APP_BUILD_GENERATE_BINARIES)
        file(READ "flasher_args.json.in" flasher_args_content)
        string(CONFIGURE "${flasher_args_content}" flasher_args_content)

        file_generate("${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in" 
                    CONTENT "${flasher_args_content}")
        file_generate("${CMAKE_BINARY_DIR}/flasher_args.json" 
                    INPUT "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in")
    endif()
endif()

