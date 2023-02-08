idf_build_get_property(target IDF_TARGET)
set(priv_req)
if(NOT ${target} STREQUAL "linux")
    list(APPEND priv_req esptool_py)
endif()

idf_component_register(PRIV_REQUIRES ${priv_req})

if(BOOTLOADER_BUILD)
    return()
endif()

set(partition_csv "${PARTITION_CSV_PATH}")

if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES AND CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
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

if(CONFIG_SECURE_BOOT AND NOT CONFIG_SECURE_BOOT_ALLOW_SHORT_APP_PARTITION)
    if(CONFIG_SECURE_BOOT_V2_ENABLED)
        set(partition_secure_opt --secure v2)
    else()
        set(partition_secure_opt --secure v1)
    endif()
else()
    set(partition_secure_opt "")
endif()

idf_build_get_property(extra_subtypes EXTRA_PARTITION_SUBTYPES)
if(extra_subtypes)
    # Remove all white spaces from the string
    string(REPLACE " " "" extra_subtypes "${extra_subtypes}")
    set(extra_partition_subtypes --extra-partition-subtypes ${extra_subtypes})
else()
    set(extra_partition_subtypes "")
endif()

idf_build_get_property(build_dir BUILD_DIR)
idf_build_get_property(python PYTHON)
idf_build_get_property(extra_subtypes EXTRA_PARTITION_SUBTYPES)

set(gen_partition_table "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py" "-q"
                        "--offset" "${PARTITION_TABLE_OFFSET}" "${md5_opt}" "${flashsize_opt}"
                        "${partition_secure_opt}" ${extra_partition_subtypes} "--")

set(partition_table_display
    COMMAND ${CMAKE_COMMAND} -E echo "Partition table binary generated. Contents:"
    COMMAND ${CMAKE_COMMAND} -E echo "*******************************************************************************"
    COMMAND ${gen_partition_table} "${build_dir}/partition_table/${unsigned_partition_bin}"
    COMMAND ${CMAKE_COMMAND} -E echo "*******************************************************************************"
)

add_custom_command(OUTPUT "${build_dir}/partition_table/${unsigned_partition_bin}"
    COMMAND ${gen_partition_table} "${partition_csv}" "${build_dir}/partition_table/${unsigned_partition_bin}"
    ${partition_table_display}
    DEPENDS ${partition_csv} "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py"
    VERBATIM)

if(extra_subtypes)
    set(extra_subtypes_h "${build_dir}/config/extra_partition_subtypes.inc")

    add_custom_command(OUTPUT ${extra_subtypes_h}
        COMMAND ${python} ${CMAKE_CURRENT_SOURCE_DIR}/gen_extra_subtypes_inc.py ${extra_subtypes_h} ${extra_subtypes}
        COMMENT "Generating extra partition subtype header file"
    )
    add_custom_target(extra_subtype_hdr DEPENDS ${extra_subtypes_h})
    add_dependencies(${COMPONENT_LIB} extra_subtype_hdr)
endif()

add_custom_target(partition_table_bin DEPENDS "${build_dir}/partition_table/${unsigned_partition_bin}"
                                              "${build_dir}/partition_table/${final_partition_bin}"
                                              )

if(EXISTS ${partition_csv})
    add_custom_target(partition-table
                        DEPENDS partition_table_bin
                        ${partition_table_display}
                        VERBATIM)
    add_deprecated_target_alias(partition_table partition-table)
else()
    # If the partition input CSV is not found, create a phony partition_table target that
    # fails the build. fail_at_build_time also touches CMakeCache.txt to cause a cmake run next time
    # (to pick up a new CSV if one exists, etc.)
    fail_at_build_time(partition-table
        "Partition table CSV ${partition_csv} does not exist."
        "Either change partition table in menuconfig or create this input file.")
endif()

if(${target} STREQUAL "linux" AND EXISTS ${partition_csv})
    # partition-table target is normally invoked as a dependency of 'flash' target.
    # However, when building for "linux" target, 'flash' target doesn't exist,
    # so we need to attach the partition table build to the executable target.
    #
    # The problem is that the executable target is not yet defined
    # when the component CMakeLists.txt file is evaluated, so we
    # can only get it as a generator expression. But generator expressions
    # can't be used in 'add_dependencies':
    # https://gitlab.kitware.com/cmake/cmake/-/issues/19467
    #
    # Therefore attach partition-table to the internal __idf_build_target
    # target. This is a hack, since that target name is an implementation detail
    # of the build system.

    add_dependencies(__idf_build_target partition-table)
endif()

# Add signing steps
if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        add_custom_target(gen_unsigned_partition_bin ALL DEPENDS
                        "${build_dir}/partition_table/${unsigned_partition_bin}")

        add_custom_command(OUTPUT "${build_dir}/partition_table/${final_partition_bin}"
            COMMAND ${ESPSECUREPY} sign_data --version 1 --keyfile "${SECURE_BOOT_SIGNING_KEY}"
            -o "${build_dir}/partition_table/${final_partition_bin}"
            "${build_dir}/partition_table/${unsigned_partition_bin}"
            DEPENDS "${build_dir}/partition_table/${unsigned_partition_bin}"
            VERBATIM)
    else()
        string(REPLACE ";" " " espsecurepy "${ESPSECUREPY}")
        add_custom_command(TARGET partition-table POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo
                "Partition table built but not signed. Sign partition data before flashing:"
            COMMAND ${CMAKE_COMMAND} -E echo
                "\t${espsecurepy} sign_data --keyfile KEYFILE ${build_dir}/partition_table/${final_partition_bin}"
            VERBATIM)
    endif()
elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
    add_custom_command(TARGET partition-table POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Partition table built:"
    VERBATIM)
endif()

idf_component_get_property(main_args esptool_py FLASH_ARGS)
idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)

if(CONFIG_APP_BUILD_GENERATE_BINARIES AND CONFIG_APP_BUILD_TYPE_APP_2NDBOOT)
    esptool_py_flash_target(partition-table-flash "${main_args}" "${sub_args}")
    esptool_py_flash_target_image(partition-table-flash partition-table "${PARTITION_TABLE_OFFSET}"
                                        "${build_dir}/partition_table/${final_partition_bin}")
    esptool_py_flash_target_image(flash partition-table "${PARTITION_TABLE_OFFSET}"
                                        "${build_dir}/partition_table/${final_partition_bin}")
    add_deprecated_target_alias(partition_table-flash partition-table-flash)
endif()
