set(COMPONENT_SRCS "esp_ota_ops.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES spi_flash partition_table)
set(COMPONENT_PRIV_REQUIRES bootloader_support)

register_component()

# Add custom target for generating empty otadata partition for flashing
if(OTADATA_PARTITION_OFFSET AND OTADATA_PARTITION_SIZE)
    add_custom_command(OUTPUT "${IDF_BUILD_ARTIFACTS_DIR}/${BLANK_OTADATA_FILE}"
        COMMAND ${PYTHON} ${IDF_PATH}/components/partition_table/parttool.py
        --partition-type data --partition-subtype ota -q
        --partition-table-file ${PARTITION_CSV_PATH} generate_blank_partition_file
        --output "${IDF_BUILD_ARTIFACTS_DIR}/${BLANK_OTADATA_FILE}")

    add_custom_target(blank_ota_data ALL DEPENDS "${IDF_BUILD_ARTIFACTS_DIR}/${BLANK_OTADATA_FILE}")
    add_dependencies(flash blank_ota_data)
endif()

set(otatool_py ${PYTHON} ${COMPONENT_PATH}/otatool.py)

add_custom_target(read_otadata DEPENDS "${PARTITION_CSV_PATH}"
                COMMAND ${otatool_py} --partition-table-file ${PARTITION_CSV_PATH} read_otadata)

add_custom_target(erase_otadata DEPENDS "${PARTITION_CSV_PATH}"
                COMMAND ${otatool_py} --partition-table-file ${PARTITION_CSV_PATH} erase_otadata)
