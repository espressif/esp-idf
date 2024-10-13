# fatfs_create_partition_image
#
# Create a fatfs image of the specified directory on the host during build and optionally
# have the created image flashed using `idf.py flash`
function(fatfs_create_partition_image partition base_dir)
    set(options FLASH_IN_PROJECT WL_INIT PRESERVE_TIME ONE_FAT)
    cmake_parse_arguments(arg "${options}" "" "${multi}" "${ARGN}")


    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(python PYTHON)

    if(arg_WL_INIT)
        set(fatfsgen_py ${python} ${idf_path}/components/fatfs/wl_fatfsgen.py)
    else()
        set(fatfsgen_py ${python} ${idf_path}/components/fatfs/fatfsgen.py)
    endif()

    if(arg_PRESERVE_TIME)
        set(default_datetime_option)
    else()
        set(default_datetime_option --use_default_datetime)
    endif()

    if(arg_ONE_FAT)
        set(fatfsgen_fat_count --fat_count=1)
    else()
        set(fatfsgen_fat_count)
    endif()

    if("${CONFIG_FATFS_SECTOR_512}")
        set(fatfs_sector_size 512)
    elseif("${CONFIG_FATFS_SECTOR_1024}")
        set(fatfs_sector_size 1024)
    elseif("${CONFIG_FATFS_SECTOR_2048}")
        set(fatfs_sector_size 2048)
    else()
        set(fatfs_sector_size 4096)
    endif()

    if("${CONFIG_FATFS_LFN_NONE}")
        set(fatfs_long_names_option)
    elseif("${CONFIG_FATFS_LFN_HEAP}")
        set(fatfs_long_names_option --long_name_support)
    elseif("${CONFIG_FATFS_LFN_STACK}")
        set(fatfs_long_names_option --long_name_support)
    endif()

    get_filename_component(base_dir_full_path ${base_dir} ABSOLUTE)
    partition_table_get_partition_info(size "--partition-name ${partition}" "size")
    partition_table_get_partition_info(offset "--partition-name ${partition}" "offset")

    if("${size}" AND "${offset}")
        set(image_file ${CMAKE_BINARY_DIR}/${partition}.bin)
        # Execute FATFS image generation; this always executes as there is no way to specify for CMake to watch for
        # contents of the base dir changing.
        add_custom_target(fatfs_${partition}_bin ALL
            COMMAND ${fatfsgen_py} ${base_dir_full_path}
            ${fatfs_long_names_option}
            ${default_datetime_option}
            ${fatfsgen_fat_count}
            --partition_size ${size}
            --output_file ${image_file}
            --sector_size "${fatfs_sector_size}"
            )
        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
            ADDITIONAL_CLEAN_FILES
            ${image_file})

        idf_component_get_property(main_args esptool_py FLASH_ARGS)
        idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
        # Last (optional) parameter is the encryption for the target. In our
        # case, fatfs is not encrypt so pass FALSE to the function.
        esptool_py_flash_target(${partition}-flash "${main_args}" "${sub_args}" ALWAYS_PLAINTEXT)
        esptool_py_flash_to_partition(${partition}-flash "${partition}" "${image_file}")

        add_dependencies(${partition}-flash fatfs_${partition}_bin)
        if(arg_FLASH_IN_PROJECT)
            esptool_py_flash_to_partition(flash "${partition}" "${image_file}")
            add_dependencies(flash fatfs_${partition}_bin)
        endif()
    else()
        set(message "Failed to create FATFS image for partition '${partition}'. "
                    "Check project configuration if using the correct partition table file.")
        fail_at_build_time(fatfs_${partition}_bin "${message}")
    endif()
endfunction()


function(fatfs_create_rawflash_image partition base_dir)
    set(options FLASH_IN_PROJECT PRESERVE_TIME ONE_FAT)
    cmake_parse_arguments(arg "${options}" "" "${multi}" "${ARGN}")

    set(argument_list)

    if(arg_FLASH_IN_PROJECT)
        list(APPEND argument_list FLASH_IN_PROJECT)
    endif()
    if(arg_PRESERVE_TIME)
        list(APPEND argument_list PRESERVE_TIME)
    endif()
    if(arg_ONE_FAT)
        list(APPEND argument_list ONE_FAT)
    endif()

    fatfs_create_partition_image(${partition} ${base_dir} ${argument_list})
endfunction()

function(fatfs_create_spiflash_image partition base_dir)
    set(options FLASH_IN_PROJECT PRESERVE_TIME ONE_FAT)
    cmake_parse_arguments(arg "${options}" "" "${multi}" "${ARGN}")

    set(argument_list WL_INIT)

    if(arg_FLASH_IN_PROJECT)
        list(APPEND argument_list FLASH_IN_PROJECT)
    endif()
    if(arg_PRESERVE_TIME)
        list(APPEND argument_list PRESERVE_TIME)
    endif()
    if(arg_ONE_FAT)
        list(APPEND argument_list ONE_FAT)
    endif()

    fatfs_create_partition_image(${partition} ${base_dir} ${argument_list})
endfunction()
