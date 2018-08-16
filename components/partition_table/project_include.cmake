if(NOT BOOTLOADER_BUILD)

set(PARTITION_TABLE_OFFSET ${CONFIG_PARTITION_TABLE_OFFSET})

# Set PARTITION_CSV_PATH to the configured partition CSV file
# absolute path
if(CONFIG_PARTITION_TABLE_CUSTOM)
    # Custom filename expands any path relative to the project
    get_filename_component(PARTITION_CSV_PATH "${CONFIG_PARTITION_TABLE_FILENAME}" ABSOLUTE BASE_DIR "${PROJECT_PATH}")

    if(NOT EXISTS "${PARTITION_CSV_PATH}")
        message(FATAL_ERROR "Partition table CSV file ${PARTITION_CSV_PATH} not found. Change custom partition CSV path in menuconfig.")
    endif()
else()
    # Other .csv files are always in the component directory
    get_filename_component(PARTITION_CSV_PATH "${COMPONENT_PATH}/${CONFIG_PARTITION_TABLE_FILENAME}" ABSOLUTE)

    if(NOT EXISTS "${PARTITION_CSV_PATH}")
        message(FATAL_ERROR "Internal error, built-in ${PARTITION_CSV_PATH} not found.")
    endif()
endif()

# need to re-run CMake if the partition CSV changes, as the offsets/sizes of partitions may change
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${PARTITION_CSV_PATH})

# Parse the partition table to get variable partition offsets & sizes which must be known at CMake runtime
set(get_part_info ${COMPONENT_PATH}/parttool.py -q)

if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION)
    execute_process(COMMAND
        ${get_part_info} --type data --subtype phy --offset ${PARTITION_CSV_PATH}
        OUTPUT_VARIABLE PHY_PARTITION_OFFSET)
    set(PHY_PARTITION_BIN_FILE "${CMAKE_BINARY_DIR}/esp32/phy_init_data.bin")
endif()

execute_process(COMMAND
    ${get_part_info} --default-boot-partition --offset ${PARTITION_CSV_PATH}
    OUTPUT_VARIABLE APP_PARTITION_OFFSET)

execute_process(COMMAND
    ${get_part_info} --type data --subtype ota --offset ${PARTITION_CSV_PATH}
    OUTPUT_VARIABLE OTADATA_PARTITION_OFFSET)

execute_process(COMMAND
    ${get_part_info} --type data --subtype ota --size ${PARTITION_CSV_PATH}
    OUTPUT_VARIABLE OTADATA_PARTITION_SIZE)

endif()
