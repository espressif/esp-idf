idf_component_register()

if(BOOTLOADER_BUILD)
    return()
endif()

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

if(NOT CONFIG_PARTITION_TABLE_MD5)
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

idf_build_get_property(build_dir BUILD_DIR)
idf_build_get_property(python PYTHON)

add_custom_command(OUTPUT "${build_dir}/partition_table/${unsigned_partition_bin}"
    COMMAND "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py"
    -q --offset ${PARTITION_TABLE_OFFSET} ${md5_opt} ${flashsize_opt}
    ${partition_secure_opt} ${partition_csv} ${build_dir}/partition_table/${unsigned_partition_bin}
    DEPENDS ${partition_csv} "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py"
    VERBATIM)

if(EXISTS ${partition_csv})
    add_custom_target(partition_table ALL DEPENDS "${build_dir}/partition_table/${final_partition_bin}")
else()
    # If the partition input CSV is not found, create a phony partition_table target that
    # fails the build. fail_at_build_time also touches CMakeCache.txt to cause a cmake run next time
    # (to pick up a new CSV if one exists, etc.)
    fail_at_build_time(partition_table
        "Partition table CSV ${partition_csv} does not exist."
        "Either change partition table in menuconfig or create this input file.")
endif()

# Add signing steps
if(CONFIG_SECURE_SIGNED_APPS)
    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        add_custom_target(gen_unsigned_partition_bin ALL DEPENDS
                        "${build_dir}/partition_table/${unsigned_partition_bin}")

        add_custom_command(OUTPUT "${build_dir}/partition_table/${final_partition_bin}"
            COMMAND ${ESPSECUREPY} sign_data --keyfile "${SECURE_BOOT_SIGNING_KEY}"
            -o "${build_dir}/partition_table/${final_partition_bin}"
            "${build_dir}/partition_table/${unsigned_partition_bin}"
            DEPENDS "${build_dir}/partition_table/${unsigned_partition_bin}"
            VERBATIM)
    else()
        string(REPLACE ";" " " espsecurepy "${ESPSECUREPY}")
        add_custom_command(TARGET partition_table POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo
                "Partition table built but not signed. Sign partition data before flashing:"
            COMMAND ${CMAKE_COMMAND} -E echo 
                "\t${espsecurepy} sign_data --keyfile KEYFILE ${build_dir}/partition_table/${final_partition_bin}"
            VERBATIM)
    endif()
endif()

idf_component_get_property(main_args esptool_py FLASH_ARGS)
idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)

esptool_py_flash_target(partition_table-flash "${main_args}" "${sub_args}")
esptool_py_flash_target_image(partition_table-flash partition_table "${PARTITION_TABLE_OFFSET}"
                                      "${build_dir}/partition_table/partition-table.bin")
esptool_py_flash_target_image(flash partition_table "${PARTITION_TABLE_OFFSET}"
                                      "${build_dir}/partition_table/partition-table.bin")

