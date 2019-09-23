idf_component_register(REQUIRES bootloader)

if(NOT BOOTLOADER_BUILD)
    string(REPLACE ";" " " ESPTOOLPY_FLASH_PROJECT_OPTIONS "${ESPTOOLPY_FLASH_OPTIONS}")
    set(ESPTOOLPY_FLASH_PROJECT_OPTIONS
        "${ESPTOOLPY_FLASH_PROJECT_OPTIONS}"
        )

    if(CONFIG_SECURE_BOOT_ENABLED)
        set(ESPTOOLPY_FLASH_PROJECT_OPTIONS "")
    endif()

    # FLASH_PROJECT_ARGS, FLASH_PROJECT_ARGS_JSON, FLASH_PROJECT_ARGS_ENTRY_JSON
    # are used in the flasher args input files (flash_project_args.in, flasher_args.json.in)
    idf_component_get_property(FLASH_PROJECT_ARGS ${COMPONENT_NAME} 
                            FLASH_PROJECT_ARGS GENERATOR_EXPRESSION)
    idf_component_get_property(FLASH_PROJECT_ARGS_JSON ${COMPONENT_NAME}  
                            FLASH_PROJECT_ARGS_JSON GENERATOR_EXPRESSION)
    idf_component_get_property(FLASH_PROJECT_ARGS_ENTRY_JSON ${COMPONENT_NAME} 
                            FLASH_PROJECT_ARGS_ENTRY_JSON GENERATOR_EXPRESSION)

    # Generate the flash project args and the flasher args json file using the accumulated values
    # from esptool_py_flash_project_args calls. The file is first configured using configure_file() for all variable values,
    # and then generated using file(GENERATE... for generator expressions.
    configure_file(${COMPONENT_DIR}/flash_project_args.in
                ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in)

    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in2
                INPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in)
    file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/flash_project_args
                INPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in2)

    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
        configure_file(${COMPONENT_DIR}/flash_encrypted_project_args.in
                    ${CMAKE_CURRENT_BINARY_DIR}/flash_encrypted_project_args.in)

        file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_encrypted_project_args.in2
                    INPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_encrypted_project_args.in)
        file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/flash_encrypted_project_args
                    INPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_encrypted_project_args.in2)
    endif()

    configure_file(${COMPONENT_DIR}/flasher_args.json.in
                ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in)

    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in2
                INPUT ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in)
    file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/flasher_args.json
                INPUT ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in2)

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        "${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in"
        "${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in2"
        "${CMAKE_BINARY_DIR}/flash_project_args"
        "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in"
        "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in2"
        "${CMAKE_BINARY_DIR}/flasher_args.json")

    idf_build_get_property(build_dir BUILD_DIR)
    partition_table_get_partition_info(app_partition_offset "--partition-boot-default" "offset")
    esptool_py_flash_project_args(app ${app_partition_offset} ${build_dir}/${PROJECT_BIN} FLASH_IN_PROJECT)

    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/flash_encrypted_app_args.in "--encrypt ${app_partition_offset} ${PROJECT_BIN}")
        esptool_py_flash_project_args(encrypted_app ${app_partition_offset} ${build_dir}/${PROJECT_BIN}
                                    FLASH_FILE_TEMPLATE ${CMAKE_CURRENT_BINARY_DIR}/flash_encrypted_app_args.in)
    endif()

    add_dependencies(flash partition_table)

    # If anti-rollback option is set then factory partition should not be in Partition Table.
    # In this case, should be used the partition table with two ota app without the factory.
    partition_table_get_partition_info(factory_offset "--partition-type app --partition-subtype factory" "offset")
    if(CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK AND factory_offset)
        fail_at_build_time(check_table_contents
            "ERROR: Anti-rollback option is enabled. Partition table should consist of two ota app without factory partition.")
        add_dependencies(app check_table_contents)
    endif()
endif()

