# fatfs_create_partition_image
#
# Create a fatfs image of the specified directory on the host during build and optionally
# have the created image flashed using `idf.py flash`
function(fatfs_create_partition_image partition base_dir)
    set(options FLASH_IN_PROJECT WL_INIT)
    cmake_parse_arguments(arg "${options}" "" "${multi}" "${ARGN}")


    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(python PYTHON)

    if(arg_WL_INIT)
        set(fatfsgen_py ${python} ${idf_path}/components/fatfs/wl_fatfsgen.py)
    else()
        set(fatfsgen_py ${python} ${idf_path}/components/fatfs/fatfsgen.py)
    endif()

    if("${CONFIG_FATFS_SECTORS_PER_CLUSTER_1}")
        set(sectors_per_cluster 1)
    elseif("${CONFIG_FATFS_SECTORS_PER_CLUSTER_2}")
        set(sectors_per_cluster 2)
    elseif("${CONFIG_FATFS_SECTORS_PER_CLUSTER_4}")
        set(sectors_per_cluster 4)
    elseif("${CONFIG_FATFS_SECTORS_PER_CLUSTER_8}")
        set(sectors_per_cluster 8)
    elseif("${CONFIG_FATFS_SECTORS_PER_CLUSTER_16}")
        set(sectors_per_cluster 16)
    elseif("${CONFIG_FATFS_SECTORS_PER_CLUSTER_32}")
        set(sectors_per_cluster 32)
    elseif("${CONFIG_FATFS_SECTORS_PER_CLUSTER_64}")
        set(sectors_per_cluster 64)
    elseif("${CONFIG_FATFS_SECTORS_PER_CLUSTER_128}")
        set(sectors_per_cluster 128)
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

    if("${CONFIG_FATFS_AUTO_TYPE}")
        set(fatfs_explicit_type 0)
    elseif("${CONFIG_FATFS_FAT12}")
        set(fatfs_explicit_type 12)
    elseif("${CONFIG_FATFS_FAT16}")
        set(fatfs_explicit_type 16)
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
            --partition_size ${size}
            --output_file ${image_file}
            --sector_size "${fatfs_sector_size}"
            --sectors_per_cluster "${sectors_per_cluster}"
            --fat_type "${fatfs_explicit_type}"
            )
        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
            ADDITIONAL_MAKE_CLEAN_FILES
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
    set(options FLASH_IN_PROJECT)
    cmake_parse_arguments(arg "${options}" "" "${multi}" "${ARGN}")
    if(arg_FLASH_IN_PROJECT)
        fatfs_create_partition_image(${partition} ${base_dir} FLASH_IN_PROJECT)
    else()
        fatfs_create_partition_image(${partition} ${base_dir})
    endif()
endfunction()

function(fatfs_create_spiflash_image partition base_dir)
    set(options FLASH_IN_PROJECT)
    cmake_parse_arguments(arg "${options}" "" "${multi}" "${ARGN}")
    if(arg_FLASH_IN_PROJECT)
        fatfs_create_partition_image(${partition} ${base_dir} FLASH_IN_PROJECT WL_INIT)
    else()
        fatfs_create_partition_image(${partition} ${base_dir} WL_INIT)
    endif()
endfunction()
