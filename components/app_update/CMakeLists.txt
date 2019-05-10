set(COMPONENT_SRCS "esp_ota_ops.c"
                   "esp_app_desc.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES spi_flash partition_table bootloader_support)

register_component()

# esp_app_desc structure is added as an undefined symbol because otherwise the
# linker will ignore this structure as it has no other files depending on it.
target_link_libraries(${COMPONENT_LIB} "-u esp_app_desc")

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
        add_custom_command(OUTPUT ${blank_otadata_file}
            COMMAND ${python} ${idf_path}/components/partition_table/parttool.py
            --partition-type data --partition-subtype ota -q
            --partition-table-file ${PARTITION_CSV_PATH} generate_blank_partition_file
            --output ${blank_otadata_file})

        add_custom_target(blank_ota_data ALL DEPENDS ${blank_otadata_file})
        add_dependencies(app blank_ota_data)

        set(otatool_py ${python} ${COMPONENT_DIR}/otatool.py)

        add_custom_target(read_otadata DEPENDS "${PARTITION_CSV_PATH}"
                        COMMAND ${otatool_py} --partition-table-file ${PARTITION_CSV_PATH} read_otadata)

        add_custom_target(erase_otadata DEPENDS "${PARTITION_CSV_PATH}"
                        COMMAND ${otatool_py} --partition-table-file ${PARTITION_CSV_PATH} erase_otadata)

        esptool_py_flash_project_args(otadata ${otadata_offset} "${blank_otadata_file}" FLASH_IN_PROJECT)
    endif()
endif()
