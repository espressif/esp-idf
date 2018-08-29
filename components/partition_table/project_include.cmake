if(NOT BOOTLOADER_BUILD)

set(PARTITION_TABLE_OFFSET ${CONFIG_PARTITION_TABLE_OFFSET})

# Set PARTITION_CSV_PATH to the configured partition CSV file
# absolute path
if(CONFIG_PARTITION_TABLE_CUSTOM)
    # Custom filename expands any path relative to the project
    get_filename_component(PARTITION_CSV_PATH "${CONFIG_PARTITION_TABLE_FILENAME}" ABSOLUTE BASE_DIR "${PROJECT_PATH}")

    if(NOT EXISTS "${PARTITION_CSV_PATH}")
        message(WARNING "Partition table CSV file ${PARTITION_CSV_PATH} not found. "
            "Change custom partition CSV path in menuconfig.")
        # Note: partition_table CMakeLists.txt contains some logic to create a dummy
        # partition_table target in this case, see comments in that file.
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
function(get_partition_info variable get_part_info_args)
    separate_arguments(get_part_info_args)
    execute_process(COMMAND ${PYTHON}
        ${COMPONENT_PATH}/parttool.py -q
        --partition-table-offset ${PARTITION_TABLE_OFFSET}
        ${get_part_info_args}
        ${PARTITION_CSV_PATH}
        OUTPUT_VARIABLE result
        RESULT_VARIABLE exit_code
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ${exit_code} EQUAL 0 AND NOT ${exit_code} EQUAL 1)
        # can't fail here as it would prevent the user from running 'menuconfig' again
        message(WARNING "parttool.py execution failed (${result}), problem with partition CSV file (see above)")
    endif()
    set(${variable} ${result} PARENT_SCOPE)
endfunction()

if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION)
    get_partition_info(PHY_PARTITION_OFFSET "--type data --subtype phy --offset")
    set(PHY_PARTITION_BIN_FILE "${CMAKE_BINARY_DIR}/esp32/phy_init_data.bin")
endif()

get_partition_info(APP_PARTITION_OFFSET "--default-boot-partition --offset")

get_partition_info(OTADATA_PARTITION_OFFSET "--type data --subtype ota --offset")

get_partition_info(OTADATA_PARTITION_SIZE "--type data --subtype ota --size")

endif()
