idf_component_register(SRCS "esp_ota_ops.c" 
                            "esp_app_desc.c"
                    INCLUDE_DIRS "include"
                    REQUIRES spi_flash partition_table bootloader_support)

# esp_app_desc structure is added as an undefined symbol because otherwise the
# linker will ignore this structure as it has no other files depending on it.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_app_desc")

# cut PROJECT_VER and PROJECT_NAME to required 32 characters.
idf_build_get_property(project_ver PROJECT_VER)
idf_build_get_property(project_name PROJECT_NAME)
string(SUBSTRING "${project_ver}" 0 31 PROJECT_VER_CUT)
string(SUBSTRING "${project_name}" 0 31 PROJECT_NAME_CUT)

set_source_files_properties(
    SOURCE "esp_app_desc.c"
    PROPERTIES COMPILE_DEFINITIONS
    "PROJECT_VER=\"${PROJECT_VER_CUT}\"; PROJECT_NAME=\"${PROJECT_NAME_CUT}\"")

if(NOT BOOTLOADER_BUILD)
    partition_table_get_partition_info(otadata_offset "--partition-type data --partition-subtype ota" "offset")
    partition_table_get_partition_info(otadata_size "--partition-type data --partition-subtype ota" "size")

    # Add custom target for generating empty otadata partition for flashing
    if(otadata_size AND otadata_offset)
        idf_build_get_property(build_dir BUILD_DIR)
        set(blank_otadata_file ${build_dir}/ota_data_initial.bin)

        idf_build_get_property(idf_path IDF_PATH)
        idf_build_get_property(python PYTHON)

        idf_component_get_property(partition_table_dir partition_table COMPONENT_DIR)

        add_custom_command(OUTPUT ${blank_otadata_file}
            COMMAND ${python} ${partition_table_dir}/gen_empty_partition.py
            ${otadata_size} ${blank_otadata_file})

        add_custom_target(blank_ota_data ALL DEPENDS ${blank_otadata_file})
        add_dependencies(flash blank_ota_data)

        set(otatool_py ${python} ${COMPONENT_DIR}/otatool.py)

        set(esptool_args --esptool-args before=${CONFIG_ESPTOOLPY_BEFORE} after=${CONFIG_ESPTOOLPY_AFTER})

        add_custom_target(read_otadata DEPENDS "${PARTITION_CSV_PATH}"
                        COMMAND ${otatool_py} ${esptool_args}
                        --partition-table-file ${PARTITION_CSV_PATH} 
                        --partition-table-offset ${PARTITION_TABLE_OFFSET}
                        read_otadata)

        add_custom_target(erase_otadata DEPENDS "${PARTITION_CSV_PATH}"
                        COMMAND ${otatool_py} ${esptool_args}
                        --partition-table-file ${PARTITION_CSV_PATH}
                        --partition-table-offset ${PARTITION_TABLE_OFFSET}
                        erase_otadata)

        esptool_py_flash_project_args(otadata ${otadata_offset} "${blank_otadata_file}" FLASH_IN_PROJECT)
    endif()
endif()
