idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_component_register(SRCS "esp_ota_ops.c" "esp_ota_app_desc.c"
                    INCLUDE_DIRS "include"
                    REQUIRES partition_table bootloader_support esp_app_format esp_bootloader_format esp_partition
                    PRIV_REQUIRES esptool_py efuse spi_flash)

if(NOT BOOTLOADER_BUILD)
    partition_table_get_partition_info(otadata_offset "--partition-type data --partition-subtype ota" "offset")
    partition_table_get_partition_info(otadata_size "--partition-type data --partition-subtype ota" "size")

    # Add custom target for generating empty otadata partition for flashing
    if(otadata_size AND otadata_offset)
        idf_build_get_property(build_dir BUILD_DIR)
        set(blank_otadata_file ${build_dir}/ota_data_initial.bin)

        idf_build_get_property(idf_path IDF_PATH)
        idf_build_get_property(python PYTHON)

        idf_component_get_property(partition_table_dir partition_table COMPONENT_DIR)

        add_custom_command(OUTPUT ${blank_otadata_file}
            COMMAND ${python} ${partition_table_dir}/gen_empty_partition.py
            ${otadata_size} ${blank_otadata_file})

        add_custom_target(blank_ota_data ALL DEPENDS ${blank_otadata_file})
        add_dependencies(flash blank_ota_data)
        add_dependencies(encrypted-flash blank_ota_data)

        set(otatool_py "${python}" "${COMPONENT_DIR}/otatool.py")

        set(esptool_args --esptool-args before=${CONFIG_ESPTOOLPY_BEFORE} after=${CONFIG_ESPTOOLPY_AFTER})
        set(otatool_args --partition-table-file ${PARTITION_CSV_PATH})
        list(APPEND otatool_args --partition-table-offset ${PARTITION_TABLE_OFFSET})

        idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)

        add_custom_target(read-otadata DEPENDS "${PARTITION_CSV_PATH}"
            COMMAND ${CMAKE_COMMAND}
            -D "IDF_PATH=${idf_path}"
            -D "SERIAL_TOOL=${otatool_py}"
            -D "SERIAL_TOOL_ARGS=${esptool_args};${otatool_args};read_otadata"
            -D "WORKING_DIRECTORY=${build_dir}"
            -P ${esptool_py_dir}/run_serial_tool.cmake
            WORKING_DIRECTORY "${build_dir}"
            USES_TERMINAL
            VERBATIM
            )
        add_deprecated_target_alias(read_otadata read-otadata)

        add_custom_target(erase-otadata DEPENDS "${PARTITION_CSV_PATH}"
            COMMAND ${CMAKE_COMMAND}
            -D "IDF_PATH=${idf_path}"
            -D "SERIAL_TOOL=${otatool_py}"
            -D "SERIAL_TOOL_ARGS=${esptool_args};${otatool_args};erase_otadata"
            -D "WORKING_DIRECTORY=${build_dir}"
            -P ${esptool_py_dir}/run_serial_tool.cmake
            WORKING_DIRECTORY "${build_dir}"
            USES_TERMINAL
            VERBATIM
            )
        add_deprecated_target_alias(erase_otadata erase-otadata)

        idf_component_get_property(main_args esptool_py FLASH_ARGS)
        idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
        esptool_py_flash_target(otadata-flash "${main_args}" "${sub_args}")
        esptool_py_flash_target_image(otadata-flash otadata "${otadata_offset}" "${blank_otadata_file}")

        esptool_py_flash_target_image(flash otadata "${otadata_offset}" "${blank_otadata_file}")
    endif()
endif()
