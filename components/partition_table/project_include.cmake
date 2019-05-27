if(NOT BOOTLOADER_BUILD)
    set(PARTITION_TABLE_OFFSET ${CONFIG_PARTITION_TABLE_OFFSET})

    # Set PARTITION_CSV_PATH to the configured partition CSV file
    # absolute path
    if(CONFIG_PARTITION_TABLE_CUSTOM)
        idf_build_get_property(project_dir PROJECT_DIR)
        # Custom filename expands any path relative to the project
        get_filename_component(PARTITION_CSV_PATH "${CONFIG_PARTITION_TABLE_FILENAME}"
                                ABSOLUTE BASE_DIR "${project_dir}")

        if(NOT EXISTS "${PARTITION_CSV_PATH}")
            message(WARNING "Partition table CSV file ${PARTITION_CSV_PATH} not found. "
                "Change custom partition CSV path in menuconfig.")
            # Note: partition_table CMakeLists.txt contains some logic to create a dummy
            # partition_table target in this case, see comments in that file.
        endif()
    else()
        # Other .csv files are always in the component directory
        get_filename_component(PARTITION_CSV_PATH "${COMPONENT_DIR}/${CONFIG_PARTITION_TABLE_FILENAME}" ABSOLUTE)

        if(NOT EXISTS "${PARTITION_CSV_PATH}")
            message(FATAL_ERROR "Internal error, built-in ${PARTITION_CSV_PATH} not found.")
        endif()
    endif()

    # need to re-run CMake if the partition CSV changes, as the offsets/sizes of partitions may change
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${PARTITION_CSV_PATH})
endif()

# partition_table_get_partition_info
#
# Get information about a partition from the partition table
function(partition_table_get_partition_info result get_part_info_args part_info)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)
    separate_arguments(get_part_info_args)
    execute_process(COMMAND ${python}
        ${idf_path}/components/partition_table/parttool.py -q
        --partition-table-offset ${PARTITION_TABLE_OFFSET}
        --partition-table-file ${PARTITION_CSV_PATH}
        get_partition_info ${get_part_info_args} --info ${part_info}
        OUTPUT_VARIABLE info
        RESULT_VARIABLE exit_code
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ${exit_code} EQUAL 0 AND NOT ${exit_code} EQUAL 1)
        # can't fail here as it would prevent the user from running 'menuconfig' again
        message(WARNING "parttool.py execution failed (${result}), problem with partition CSV file (see above)")
    endif()
    set(${result} ${info} PARENT_SCOPE)
endfunction()
