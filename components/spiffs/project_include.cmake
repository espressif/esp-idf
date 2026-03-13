# spiffs_create_partition_image
#
# Create a spiffs image of the specified directory on the host during build and optionally
# have the created image flashed using `idf.py flash`
function(spiffs_create_partition_image partition base_dir)
    set(options FLASH_IN_PROJECT)
    set(multi DEPENDS)
    cmake_parse_arguments(arg "${options}" "" "${multi}" "${ARGN}")

    idf_build_get_property(idf_path IDF_PATH)
    set(spiffsgen_py ${PYTHON} ${idf_path}/components/spiffs/spiffsgen.py)

    get_filename_component(base_dir_full_path ${base_dir} ABSOLUTE)

    partition_table_get_partition_info(size "--partition-name ${partition}" "size")
    partition_table_get_partition_info(offset "--partition-name ${partition}" "offset")

    if("${size}" AND "${offset}")
        set(image_file ${CMAKE_BINARY_DIR}/${partition}.bin)

        if(CONFIG_SPIFFS_USE_MAGIC)
            set(use_magic "--use-magic")
        endif()

        if(CONFIG_SPIFFS_USE_MAGIC_LENGTH)
            set(use_magic_len "--use-magic-len")
        endif()

        if(CONFIG_SPIFFS_FOLLOW_SYMLINKS)
            set(follow_symlinks "--follow-symlinks")
        endif()

        # Execute SPIFFS image generation; this always executes as there is no way to specify for CMake to watch for
        # contents of the base dir changing.
        add_custom_target(spiffs_${partition}_bin ALL
            COMMAND ${spiffsgen_py} ${size} ${base_dir_full_path} ${image_file}
            --page-size=${CONFIG_SPIFFS_PAGE_SIZE}
            --obj-name-len=${CONFIG_SPIFFS_OBJ_NAME_LEN}
            --meta-len=${CONFIG_SPIFFS_META_LENGTH}
            ${follow_symlinks}
            ${use_magic}
            ${use_magic_len}
            DEPENDS ${arg_DEPENDS}
            )

        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
            ADDITIONAL_CLEAN_FILES
            ${image_file})

        # Encryption for SPIFFS is not supported, so optional parameter ALWAYS_PLAINTEXT is passed to the function.
        set(esp_partition_register_target_optional_args)
        list(APPEND esp_partition_register_target_optional_args ALWAYS_PLAINTEXT DEPENDS spiffs_${partition}_bin)

        if(arg_FLASH_IN_PROJECT)
            list(APPEND esp_partition_register_target_optional_args FLASH_IN_PROJECT)
        endif()

        esp_partition_register_target(${partition} "${image_file}" ${esp_partition_register_target_optional_args})
    else()
        set(message "Failed to create SPIFFS image for partition '${partition}'. "
                    "Check project configuration if using the correct partition table file.")
        fail_at_build_time(spiffs_${partition}_bin "${message}")
    endif()
endfunction()
