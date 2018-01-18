set(partition_table_offset 0x8000)

# Set partition_csv to the configured partition source file
#
if(CONFIG_PARTITION_TABLE_CUSTOM)
  # Custom filename expands any path relative to the project
  get_filename_component(partition_csv "${CONFIG_PARTITION_TABLE_CUSTOM}" ABSOLUTE BASE_DIR "${PROJECT_PATH}")
else()
  # Other .csv files are always in the component directory
  set(partition_csv "${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_PARTITION_TABLE_FILENAME}")
endif()

if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
  set(unsigned_partition_bin "partition-unsigned.bin")
  set(final_partition_bin "partition.bin")
  set(final_partition_target "sign_partition_table")
else()
  set(unsigned_partition_bin "partition.bin")
  set(final_partition_bin "partition.bin")
  set(final_partition_target "build_partition_table")
endif()

add_custom_target(build_partition_table ALL
  "${PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py" -q
  ${partition_csv} ${unsigned_partition_bin}
  BYPRODUCTS ${unsigned_partition_bin}
  DEPENDS ${partition_csv} "${CMAKE_CURRENT_SOURCE_DIR}/gen_esp32part.py"
  VERBATIM)

# Add signing steps
if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
  get_filename_component(secure_boot_signing_key "${CONFIG_SECURE_BOOT_SIGNING_KEY}" ABSOLUTE BASE_DIR "${PROJECT_PATH}")

  add_custom_target(signpartition_table ALL
    "${PYTHON}" "${ESPSECUREPY}" sign_data --keyfile "${secure_boot_signing_key}" -o "${final_partition_bin}" "${unsigned_partition_bin}"
    BYPRODUCTS "${final_partition_bin}"
    DEPENDS "${unsigned_partition_bin}"
    VERBATIM)

endif()

# Use global properties ESPTOOL_WRITE_FLASH_ARGS & FLASH_DEPENDS to pass this info to the esptool component for flashing
set_property(GLOBAL APPEND_STRING PROPERTY ESPTOOL_WRITE_FLASH_ARGS "${final_partition_bin} ${partition_table_offset} ")
set_property(GLOBAL APPEND PROPERTY FLASH_DEPENDS "${final_partition_target}")

register_config_only_component()
