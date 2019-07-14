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
        ADDITIONAL_MAKE_CLEAN_FILES
        ${image_file})

    if(arg_FLASH_IN_PROJECT)
        esptool_py_flash_project_args(${partition} ${offset} ${image_file} FLASH_IN_PROJECT)
    else()
        esptool_py_flash_project_args(${partition} ${offset} ${image_file})
    endif()
endfunction()
