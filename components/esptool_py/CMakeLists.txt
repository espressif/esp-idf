idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_component_register(REQUIRES bootloader PRIV_REQUIRES partition_table)

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
    partition_table_get_partition_info(test_offset "--partition-type app --partition-subtype test" "offset")
    if(CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK AND (factory_offset OR test_offset))
        fail_at_build_time(check_table_contents "\
ERROR: Anti-rollback option is enabled. Partition table should \
consist of two ota app without factory or test partitions.")
        add_dependencies(app check_table_contents)
    endif()

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
        if(CONFIG_APP_BUILD_TYPE_APP_2NDBOOT)
            # Generate app_check_size_command target to check the app size against the partition table parameters
            partition_table_add_check_size_target(app_check_size
                DEPENDS gen_project_binary
                BINARY_PATH "${build_dir}/${PROJECT_BIN}"
                PARTITION_TYPE app)
            add_dependencies(app app_check_size)
        endif()
    endif()
endif()  # NOT BOOTLOADER_BUILD

if(BOOTLOADER_BUILD)
  # Generate bootloader post-build check of the bootloader size against the offset
  partition_table_add_check_bootloader_size_target(bootloader_check_size
      DEPENDS gen_project_binary
      BOOTLOADER_BINARY_PATH "${build_dir}/${PROJECT_BIN}"
      RESULT bootloader_check_size_command)
  add_dependencies(app bootloader_check_size)  # note: in the subproject, so the target is 'app'...
endif()
