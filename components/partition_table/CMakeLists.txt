register_config_only_component()

if(NOT BOOTLOADER_BUILD)

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

add_custom_command(OUTPUT "${unsigned_partition_bin}"
    COMMAND "${PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py" -q --offset ${PARTITION_TABLE_OFFSET} ${md5_opt} ${flashsize_opt}
    ${partition_csv} ${unsigned_partition_bin}
    DEPENDS ${partition_csv} "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py"
    VERBATIM)

# Add signing steps
if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
    get_filename_component(secure_boot_signing_key
        "${CONFIG_SECURE_BOOT_SIGNING_KEY}"
        ABSOLUTE BASE_DIR "${PROJECT_PATH}")

    add_custom_command(OUTPUT "${final_partition_bin}"
        COMMAND "${PYTHON}" "${ESPSECUREPY}" sign_data --keyfile "${secure_boot_signing_key}"
        -o "${final_partition_bin}" "${unsigned_partition_bin}"
        DEPENDS "${unsigned_partition_bin}"
        VERBATIM)

endif()

if(EXISTS ${partition_csv})
    add_custom_target(partition_table ALL DEPENDS "${final_partition_bin}")
else()
    # This is a bit of a hack: If the partition input CSV is not found, create a phony partition_table target that
    # fails the build. Have it also touch CMakeCache.txt to cause a cmake run next time
    # (to pick up a new CSV if one exists, etc.)
    #
    # This is because partition CSV is required at CMake runtime (to generate metadata files with flashing data, etc) but we can't
    # fail the build if it is not found, because the "menuconfig" target may be required to fix the problem. CMAKE_CONFIGURE_DEPENDS
    # only works for files which exist at CMake runtime.
    add_custom_target(partition_table ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Partition table CSV ${partition_csv} does not exist. Either change partition table in menuconfig or create this input file."
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/CMakeCache.txt"
        COMMAND ${CMAKE_COMMAND} -P ${IDF_PATH}/tools/cmake/scripts/fail.cmake)
endif()

add_dependencies(bootloader partition_table)
add_dependencies(app partition_table)

# Use global properties ESPTOOL_WRITE_FLASH_ARGS to pass this info to build
# the list of esptool write arguments for flashing
set_property(GLOBAL APPEND_STRING PROPERTY
    ESPTOOL_WRITE_FLASH_ARGS
    "${PARTITION_TABLE_OFFSET} ${final_partition_bin} ")

endif()
