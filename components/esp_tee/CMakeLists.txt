idf_build_get_property(esp_tee_build ESP_TEE_BUILD)
idf_build_get_property(custom_secure_service_yaml CUSTOM_SECURE_SERVICE_YAML)
idf_build_get_property(custom_secure_service_dir CUSTOM_SECURE_SERVICE_COMPONENT_DIR)
idf_build_get_property(custom_secure_service_component CUSTOM_SECURE_SERVICE_COMPONENT)
idf_build_get_property(target IDF_TARGET)
# headers & sources here are compiled into the app, not the esp_tee binary
# (see subproject/ for the esp_tee binary build files)

# ESP-TEE is currently supported only on the ESP32-C6, H2, C5 and C61 SoCs
set(SUPPORTED_TARGETS "esp32c6" "esp32h2" "esp32c5" "esp32c61")
if(NOT target IN_LIST SUPPORTED_TARGETS)
    message(STATUS "ESP-TEE is currently supported only on the ${SUPPORTED_TARGETS} SoCs")
    return()
endif()

if(BOOTLOADER_BUILD)
    idf_component_register()
elseif(esp_tee_build)
    # TEE build currently only uses the shared headers.
    idf_component_register(INCLUDE_DIRS include)
else()
    if(CONFIG_SECURE_ENABLE_TEE)
        if(NOT CMAKE_BUILD_EARLY_EXPANSION)
            # Add custom flash target for TEE binary
            partition_table_get_partition_info(partition "--partition-type app --partition-subtype tee_0" "name")
            if(NOT partition)
                message(FATAL_ERROR "Partition table missing TEE partition entry!")
            endif()
            add_dependencies(esp_tee partition_table_bin)
            add_dependencies(flash esp_tee)
            if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
                # Also add ESP-TEE binary as a dependency to the encrypted-flash target
                add_dependencies(encrypted-flash esp_tee)
            endif()
            set(image_file ${TEE_BUILD_DIR}/esp_tee.bin)
            partition_table_get_partition_info(offset "--partition-name ${partition}" "offset")
            esptool_py_flash_target_image(flash "${partition}" "${offset}" "${image_file}")
        endif()

        partition_table_get_partition_info(tee_otadata_offset
            "--partition-type data --partition-subtype tee_ota" "offset")
        partition_table_get_partition_info(tee_otadata_size
            "--partition-type data --partition-subtype tee_ota" "size")

        # Add custom target for generating empty otadata partition for flashing
        if(tee_otadata_offset AND tee_otadata_size)
            idf_build_get_property(build_dir BUILD_DIR)
            set(blank_tee_otadata_file ${build_dir}/tee_ota_data_initial.bin)

            idf_build_get_property(python PYTHON)
            idf_component_get_property(partition_table_dir partition_table COMPONENT_DIR)
            add_custom_command(OUTPUT ${blank_tee_otadata_file}
                COMMAND ${python} ${partition_table_dir}/gen_empty_partition.py
                ${tee_otadata_size} ${blank_tee_otadata_file})
            add_custom_target(blank_tee_ota_data ALL DEPENDS ${blank_tee_otadata_file})

            add_dependencies(flash blank_tee_ota_data)
            if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
                add_dependencies(encrypted-flash blank_tee_ota_data)
            endif()

            partition_table_get_partition_info(tee_otadata_part
                "--partition-type data --partition-subtype tee_ota" "name")

            idf_component_get_property(main_args esptool_py FLASH_ARGS)
            idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
            esptool_py_flash_target(tee_otadata-flash "${main_args}" "${sub_args}")

            esptool_py_flash_target_image(tee_otadata-flash
                "${tee_otadata_part}" "${tee_otadata_offset}" "${blank_tee_otadata_file}")
            esptool_py_flash_target_image(flash
                "${tee_otadata_part}" "${tee_otadata_offset}" "${blank_tee_otadata_file}")
        endif()

        set(srcs "src/esp_tee.c"
                 "src/esp_tee_config.c"
                 "src/esp_secure_service_wrapper.c"
                 "src/esp_tee_u2m_switch.S")
    endif()

    idf_component_register(INCLUDE_DIRS include
                           SRCS ${srcs}
                           PRIV_REQUIRES efuse esp_hal_wdt esp_security esp_stdio
                                         esp_system esptool_py mbedtls spi_flash)
    if(CONFIG_SECURE_ENABLE_TEE)
        set(EXTRA_LINK_FLAGS)
        list(APPEND EXTRA_LINK_FLAGS "-u esp_tee_app_config")
        target_link_libraries(${COMPONENT_LIB} INTERFACE "${EXTRA_LINK_FLAGS}")
    endif()
endif()

# TODO: IDF-14145
if((CONFIG_SECURE_ENABLE_TEE OR esp_tee_build) AND CMAKE_C_COMPILER_ID MATCHES "GNU")
    if(CONFIG_IDF_TARGET_ESP32C5 OR CONFIG_IDF_TARGET_ESP32C61)
        # Disable zcmp extension to avoid hardware issue with interrupts (DIG-661)
        set(march_option "-march=rv32imac_zicsr_zifencei_zaamo_zalrsc")
        idf_build_set_property(COMPILE_OPTIONS "${march_option}" APPEND)
        idf_build_set_property(LINK_OPTIONS "${march_option}" APPEND)
    endif()
endif()

if(BOOTLOADER_BUILD)
    return()
endif()

set(secure_service_yml
    ${COMPONENT_DIR}/scripts/${IDF_TARGET}/sec_srv_tbl_default.yml ${custom_secure_service_yaml}
)

set(secure_service_yml_parser_py
    ${COMPONENT_DIR}/scripts/secure_service_yml_parser.py
)

if(CONFIG_SECURE_ENABLE_TEE AND NOT esp_tee_build)
    # Default secure service API families: flash_protection_spi0, flash_protection_spi1,
    # interrupt_handling, hal, crypto, efuse, secure_storage, ota, attestation
    set(exclude_srv)
    if(NOT CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1)
        list(APPEND exclude_srv "flash_protection_spi1")
    endif()

    if(NOT CONFIG_SECURE_TEE_ATTESTATION)
        list(APPEND exclude_srv "attestation")
    endif()

    idf_build_get_property(secure_service_headers_dir SECURE_SERVICE_HEADERS_DIR)
    execute_process(
        COMMAND python ${secure_service_yml_parser_py}
        "--sec_srv" ${secure_service_yml}
        "--exclude" ${exclude_srv}
        WORKING_DIRECTORY ${secure_service_headers_dir}
    )

    execute_process(
        COMMAND python ${secure_service_yml_parser_py}
        "--sec_srv" ${secure_service_yml}
        "--exclude" ${exclude_srv} "--wrap"
        OUTPUT_VARIABLE wrap_list
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(STRIP "${wrap_list}" wrap_list)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "${wrap_list}")
endif()
