set(COMPONENT_SRCS "esp_ota_ops.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES spi_flash partition_table)
set(COMPONENT_PRIV_REQUIRES bootloader_support)

register_component()

# Add custom target for generating empty otadata partition for flashing
if(${OTADATA_PARTITION_OFFSET})
    add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/${BLANK_OTADATA_FILE}"
        COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/gen_empty_partition.py
        --size ${OTADATA_PARTITION_SIZE} "${PROJECT_BINARY_DIR}/${BLANK_OTADATA_FILE}")

    add_custom_target(blank_ota_data ALL DEPENDS "${PROJECT_BINARY_DIR}/${BLANK_OTADATA_FILE}")
    add_dependencies(flash blank_ota_data)
endif()
