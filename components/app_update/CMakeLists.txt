set(COMPONENT_SRCS "esp_ota_ops.c"
                   "esp_app_desc.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES spi_flash partition_table bootloader_support)

register_component()

# esp_app_desc structure is added as an undefined symbol because otherwise the 
# linker will ignore this structure as it has no other files depending on it.
target_link_libraries(${COMPONENT_TARGET} "-u esp_app_desc")

# cut PROJECT_VER and PROJECT_NAME to required 32 characters.
string(SUBSTRING "${PROJECT_VER}" 0 31 PROJECT_VER_CUT)
string(SUBSTRING "${PROJECT_NAME}" 0 31 PROJECT_NAME_CUT)

set_source_files_properties(
    SOURCE "esp_app_desc.c"
    PROPERTIES COMPILE_DEFINITIONS 
    "PROJECT_VER=\"${PROJECT_VER_CUT}\"; PROJECT_NAME=\"${PROJECT_NAME_CUT}\"")

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
