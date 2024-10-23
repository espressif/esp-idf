set(PARTITION_TABLE_OFFSET ${CONFIG_PARTITION_TABLE_OFFSET})

set(PARTITION_TABLE_CHECK_SIZES_TOOL_PATH "${CMAKE_CURRENT_LIST_DIR}/check_sizes.py")

idf_build_get_property(build_dir BUILD_DIR)
idf_build_set_property(PARTITION_TABLE_BIN_PATH "${build_dir}/partition_table/partition-table.bin")
idf_build_get_property(non_os_build NON_OS_BUILD)

if(NOT non_os_build)
    # Set PARTITION_CSV_PATH to the configured partition CSV file
    # absolute path
    if(CONFIG_PARTITION_TABLE_CUSTOM)
        # If the partition CSV file config already has the absolute path then set PARTITION_CSV_PATH directly
        if(IS_ABSOLUTE ${CONFIG_PARTITION_TABLE_FILENAME})
            set(PARTITION_CSV_PATH "${CONFIG_PARTITION_TABLE_FILENAME}")
        else()
            idf_build_get_property(project_dir PROJECT_DIR)
            # Custom filename expands any path relative to the project
            get_filename_component(PARTITION_CSV_PATH "${CONFIG_PARTITION_TABLE_FILENAME}"
                                    ABSOLUTE BASE_DIR "${project_dir}")
        endif()

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

    idf_build_get_property(extra_subtypes EXTRA_PARTITION_SUBTYPES)
    if(extra_subtypes)
        # Remove all white spaces from the string
        string(REPLACE " " "" extra_subtypes "${extra_subtypes}")
        set(extra_partition_subtypes --extra-partition-subtypes ${extra_subtypes})
    else()
        set(extra_partition_subtypes "")
    endif()
    separate_arguments(get_part_info_args)
    execute_process(COMMAND ${python}
        ${idf_path}/components/partition_table/parttool.py -q
        --partition-table-offset ${PARTITION_TABLE_OFFSET}
        --partition-table-file ${PARTITION_CSV_PATH}
        get_partition_info ${get_part_info_args} --info ${part_info}
        ${extra_partition_subtypes}
        OUTPUT_VARIABLE info
        RESULT_VARIABLE exit_code
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ${exit_code} EQUAL 0 AND NOT ${exit_code} EQUAL 1)
        # can't fail here as it would prevent the user from running 'menuconfig' again
        message(WARNING "parttool.py execution failed (${result}), problem with partition CSV file (see above)")
    endif()
    set(${result} ${info} PARENT_SCOPE)
endfunction()

# Add a custom target (see add_custom_target) that checks a given binary built by the build system
# doesn't overflow any partitions with the given partition type and (optional) sub-type.
#
# Adding the target doesn't mean it gets called during the build, use add_dependencies to make another
# target depend on this one.
#
# Arguments:
# - target name - (first argument) name of the target to create
# - DEPENDS - dependencies the target should have (i.e. whatever target generates the binary).
# - BINARY_PATH - path to the target binary file to check
# - PARTITION_TYPE - partition type to check against
# - PARTITION_SUBTYPE - (optional) partition subtype to check against

function(partition_table_add_check_size_target target_name)
    # result binary_path partition_type partition_subtype
    set(args BINARY_PATH PARTITION_TYPE PARTITION_SUBTYPE)
    set(multi_args DEPENDS)
    cmake_parse_arguments(CMD "" "${args}" "${multi_args}" ${ARGN})

    idf_build_get_property(python PYTHON)
    idf_build_get_property(table_bin PARTITION_TABLE_BIN_PATH)
    if(CMD_PARTITION_SUBTYPE)
        set(subtype_arg --subtype ${CMD_PARTITION_SUBTYPE})
    else()
        set(subtype_arg)
    endif()
    set(command ${python} ${PARTITION_TABLE_CHECK_SIZES_TOOL_PATH}
        --offset ${PARTITION_TABLE_OFFSET}
        partition --type ${CMD_PARTITION_TYPE} ${subtype_arg}
        ${table_bin} ${CMD_BINARY_PATH})

    add_custom_target(${target_name} COMMAND ${command} DEPENDS ${CMD_DEPENDS} partition_table_bin)
endfunction()

# Add a custom target (see add_custom_target) that checks a bootloader binary
# built by the build system will not overlap the partition table.
#
# Adding the target doesn't mean it gets called during the build, use add_dependencies to make another
# target depend on this one.
#
# Arguments:
# - target name - (first argument) name of the target to create
# - DEPENDS - dependencies the new target should have (i.e. whatever target generates the bootloader binary)
# - BOOTLOADER_BINARY_PATH - path to bootloader binary file
function(partition_table_add_check_bootloader_size_target target_name)
    cmake_parse_arguments(CMD "" "BOOTLOADER_BINARY_PATH" "DEPENDS" ${ARGN})
    idf_build_get_property(python PYTHON)

    set(command ${python} ${PARTITION_TABLE_CHECK_SIZES_TOOL_PATH}
        --offset ${PARTITION_TABLE_OFFSET}
        bootloader ${BOOTLOADER_OFFSET} ${CMD_BOOTLOADER_BINARY_PATH})
    add_custom_target(${target_name} COMMAND ${command} DEPENDS ${CMD_DEPENDS})
endfunction()
