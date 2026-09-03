# esp_partition_register_target
#
# @brief Register a binary file to be flashed to a named partition.
#
# Creates a per-partition flash target (e.g., ``idf.py <partition>-flash``) and optionally
# includes the binary in ``idf.py flash``. Automatically resolves the partition offset from
# the partition table and determines encryption requirements.
#
# @note On the 'linux' target, this function registers the binary for pre-loading into
#       the emulated flash image. The ALWAYS_PLAINTEXT, FLASH_IN_PROJECT, and
#       FLASH_IN_PROJECT_DEPENDENCY_TARGETS options are ignored for 'linux'.
#
# @param[in] partition_name  Partition name as defined in the partition table
# @param[in] binary_path  Path to the binary image file
# @param[in, optional] ALWAYS_PLAINTEXT (option)  If specified, the binary is flashed in plain text even
#                                                 when flash encryption is enabled. Must not be used with
#                                                 partitions marked as 'encrypted' in the partition table.
# @param[in, optional] DEPENDS (multi value)  Additional dependencies of the custom target.
# @param[in, optional] FLASH_IN_PROJECT (option)  If specified, the binary is flashed in the project
#                                                 when running `idf.py flash`.
# @param[in, optional] FLASH_IN_PROJECT_DEPENDENCY_TARGETS (multi value)  Additional targets that will be
#                                                                         dependent on dependencies set in DEPENDS.
#                                                                         FLASH_IN_PROJECT has to be set.
#
# Example usage (from spiffs_create_partition_image):
#
#     # Create a custom target that generates the SPIFFS image file
#     add_custom_target(spiffs_${partition}_bin ALL
#         COMMAND ${spiffsgen_py} ${size} ${base_dir} ${image_file}
#         ...)
#
#     # Flash the generated image to the partition.
#     # ALWAYS_PLAINTEXT is used because SPIFFS does not support flash encryption.
#     # DEPENDS ensures the image is generated before flashing.
#     # FLASH_IN_PROJECT includes it in `idf.py flash`.
#     esp_partition_register_target(${partition} "${image_file}"
#         ALWAYS_PLAINTEXT
#         DEPENDS spiffs_${partition}_bin
#         FLASH_IN_PROJECT
#     )
#
function(esp_partition_register_target partition_name binary_path)
    set(options ALWAYS_PLAINTEXT FLASH_IN_PROJECT)
    set(multi DEPENDS FLASH_IN_PROJECT_DEPENDENCY_TARGETS)
    cmake_parse_arguments(arg "${options}" "" "${multi}" "${ARGN}")

    # Look up partition offset from the partition table
    partition_table_get_partition_info(offset "--partition-name ${partition_name}" "offset")
    if(NOT offset)
        message(FATAL_ERROR "esp_partition_register_target: "
            "Could not find partition '${partition_name}' in the partition table.")
    endif()

    idf_build_get_property(idf_target IDF_TARGET)
    if(idf_target STREQUAL "linux")
        # On linux, register the binary for loading into the emulated flash at startup.
        # The partition_linux.c emulation layer reads a manifest file listing each
        # (offset, binary_path) pair and copies the data into the memory-mapped flash image.
        get_filename_component(binary_abs_path "${binary_path}" ABSOLUTE)

        # Use a global CMake target to accumulate entries via generator expressions.
        if(NOT TARGET linux_flash_data)
            add_custom_target(linux_flash_data)
        endif()
        set_property(TARGET linux_flash_data APPEND PROPERTY FLASH_DATA_ENTRIES
                     "${offset} ${binary_abs_path}")

        # Register build dependency so the binary is generated before the app runs
        if(arg_DEPENDS)
            add_dependencies(linux_flash_data ${arg_DEPENDS})
        endif()

        return()
    endif()

    # Validate: ALWAYS_PLAINTEXT must not be used with a partition marked as encrypted
    if(arg_ALWAYS_PLAINTEXT)
        partition_table_get_partition_info(pt_encrypted "--partition-name ${partition_name}" "encrypted")
        if(pt_encrypted STREQUAL "True")
            message(FATAL_ERROR
                "esp_partition_register_target: ALWAYS_PLAINTEXT was specified for partition '${partition_name}', "
                "but this partition is marked as 'encrypted' in the partition table. "
                "These settings are contradictory. Either remove ALWAYS_PLAINTEXT or "
                "remove the 'encrypted' flag from the partition table entry.")
        endif()
    endif()

    # Determine encryption requirements.
    # If caller passed ALWAYS_PLAINTEXT, use it. Otherwise, auto-detect from partition table.
    if(arg_ALWAYS_PLAINTEXT)
        set(flash_plaintext TRUE)
    else()
        esptool_py_partition_needs_encryption(needs_encryption ${partition_name})
        if(NOT needs_encryption)
            set(flash_plaintext TRUE)
        else()
            set(flash_plaintext FALSE)
        endif()
    endif()

    # Build the esptool_py_flash_target arg list
    set(esptool_py_flash_target_arg_list)
    if(flash_plaintext)
        list(APPEND esptool_py_flash_target_arg_list ALWAYS_PLAINTEXT)
    endif()

    # Create the per-partition flash target (e.g., "mypart-flash")
    idf_component_get_property(main_args esptool_py FLASH_ARGS)
    idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
    esptool_py_flash_target(${partition_name}-flash "${main_args}" "${sub_args}" ${esptool_py_flash_target_arg_list})
    esptool_py_flash_target_image(${partition_name}-flash ${partition_name} ${offset}
                                  "${binary_path}" ${esptool_py_flash_target_arg_list})

    if(arg_DEPENDS)
        add_dependencies(${partition_name}-flash ${arg_DEPENDS})
    endif()

    # Add to the main "flash" target if FLASH_IN_PROJECT is set
    if(arg_FLASH_IN_PROJECT)
        esptool_py_flash_target_image(flash ${partition_name} ${offset}
                                      "${binary_path}" ${esptool_py_flash_target_arg_list})

        if(arg_DEPENDS)
            add_dependencies(flash ${arg_DEPENDS})
            foreach(target ${arg_FLASH_IN_PROJECT_DEPENDENCY_TARGETS})
                add_dependencies(${target} ${arg_DEPENDS})
            endforeach()
        endif()
    endif()
endfunction()
