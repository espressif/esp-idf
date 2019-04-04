register_config_only_component()

if(NOT BOOTLOADER_BUILD AND IDF_BUILD_ARTIFACTS)

set(partition_csv "${PARTITION_CSV_PATH}")

if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
    set(unsigned_partition_bin "partition-table-unsigned.bin")
    set(final_partition_bin "partition-table.bin")
    set(final_partition_target "sign_partition_table")
else()
    set(unsigned_partition_bin "partition-table.bin")
    set(final_partition_bin "partition-table.bin")
    set(final_partition_target "build_partition_table")
endif()

if(CONFIG_PARTITION_TABLE_MD5)
    set(md5_opt --disable-md5sum)
endif()

if(CONFIG_ESPTOOLPY_FLASHSIZE)
    set(flashsize_opt --flash-size ${CONFIG_ESPTOOLPY_FLASHSIZE})
endif()

if(CONFIG_SECURE_BOOT_ENABLED AND NOT CONFIG_SECURE_BOOT_ALLOW_SHORT_APP_PARTITION)
    set(partition_secure_opt --secure)
else()
    set(partition_secure_opt "")
endif()

add_custom_command(OUTPUT "${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${unsigned_partition_bin}"
    COMMAND "${PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py"
    -q --offset ${PARTITION_TABLE_OFFSET} ${md5_opt} ${flashsize_opt}
    ${partition_secure_opt} ${partition_csv} ${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${unsigned_partition_bin}
    DEPENDS ${partition_csv} "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py"
    VERBATIM)

# Add signing steps
if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
    add_custom_target(gen_unsigned_partition_bin ALL DEPENDS
                    "${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${unsigned_partition_bin}")

    add_custom_command(OUTPUT "${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${final_partition_bin}"
        COMMAND ${ESPSECUREPY} sign_data --keyfile "${secure_boot_signing_key}"
        -o "${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${final_partition_bin}"
        "${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${unsigned_partition_bin}"
        DEPENDS "${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${unsigned_partition_bin}"
        VERBATIM)
endif()

if(EXISTS ${partition_csv})
    add_custom_target(partition_table ALL DEPENDS "${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${final_partition_bin}")
else()
    # If the partition input CSV is not found, create a phony partition_table target that
    # fails the build. fail_at_build_time also touches CMakeCache.txt to cause a cmake run next time
    # (to pick up a new CSV if one exists, etc.)
    fail_at_build_time(partition_table
        "Partition table CSV ${partition_csv} does not exist."
        "Either change partition table in menuconfig or create this input file.")
endif()

if(CONFIG_SECURE_BOOT_ENABLED AND
    NOT CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
    add_custom_command(TARGET partition_table POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
            "Partition table built but not signed. Sign partition data before flashing:"
        COMMAND ${CMAKE_COMMAND} -E echo
            "\t${ESPSECUREPY} sign_data --keyfile KEYFILE ${CMAKE_CURRENT_BINARY_DIR}/${final_partition_bin}"
        VERBATIM)
endif()

# If anti-rollback option is set then factory partition should not be in Partition Table.
# In this case, should be used the partition table with two ota app without the factory.
partition_table_get_partition_info(factory_offset "--partition-type app --partition-subtype factory" "offset")
if(CONFIG_APP_ANTI_ROLLBACK AND factory_offset)
    fail_at_build_time(check_table_contents
        "ERROR: Anti-rollback option is enabled. Partition table should consist of two ota app without factory partition.")
add_dependencies(bootloader check_table_contents)
add_dependencies(app check_table_contents)
endif()

add_dependencies(bootloader partition_table)
add_dependencies(app partition_table)

# Use global properties ESPTOOL_WRITE_FLASH_ARGS to pass this info to build
# the list of esptool write arguments for flashing
set_property(GLOBAL APPEND_STRING PROPERTY
    ESPTOOL_WRITE_FLASH_ARGS
    "${PARTITION_TABLE_OFFSET}  ${IDF_BUILD_ARTIFACTS_DIR}/partition_table/${final_partition_bin} ")

esptool_py_flash_project_args(partition_table ${PARTITION_TABLE_OFFSET}
                              ${IDF_BUILD_ARTIFACTS_DIR}/partition_table/partition-table.bin FLASH_IN_PROJECT)

if (NOT BOOTLOADER_BUILD)
    partition_table_get_partition_info(app_partition_offset "--partition-boot-default" "offset")
    esptool_py_flash_project_args(app ${app_partition_offset} ${CMAKE_BINARY_DIR}/${IDF_PROJECT_BIN} FLASH_IN_PROJECT)
endif()

endif()

