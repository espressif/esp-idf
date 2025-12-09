# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

#[[
.. cmakev2:function:: idf_create_dfu

    .. code-block:: cmake

        idf_create_dfu(<binary>
                       TARGET <target>)

    *binary[in]*

        Binary target for which to create DFU targets. The binary target is
        created by :cmakev2:ref:`idf_build_binary`.

    *TARGET[in]*

        Name of the DFU generation target to be created (e.g., "dfu", "dfu-app").
        Also creates "<target>-list" and "<target>-flash" targets.

    Create DFU (Device Firmware Update) targets for the specified binary target.
    DFU is only supported on certain targets. For other
    targets, this function does nothing.

    Note: DFU is not supported when secure boot is enabled. This function will
    not create DFU targets if secure boot is enabled.

    Three targets are created:
    - <TARGET>: Generates the DFU binary file from flasher_args.json
    - <TARGET>-list: Lists connected DFU devices
    - <TARGET>-flash: Flashes the DFU binary to a connected device

    Example usage for default project:
        idf_build_binary(myapp TARGET myapp_binary OUTPUT_FILE myapp.bin)
        idf_create_dfu(myapp_binary TARGET dfu)

#]]
function(idf_create_dfu binary)
    set(options)
    set(one_value TARGET)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT TARGET "${binary}")
        idf_die("Binary target '${binary}' is not a cmake target")
    endif()

    # DFU is not supported when secure boot is enabled
    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        idf_msg("DFU not supported when secure boot is enabled, skipping DFU target creation")
        return()
    endif()

    # Determine DFU PID based on target
    idf_build_get_property(target IDF_TARGET)

    if("${target}" STREQUAL "esp32s2")
        set(dfu_pid "2")
    elseif("${target}" STREQUAL "esp32s3")
        set(dfu_pid "9")
    elseif("${target}" STREQUAL "esp32p4")
        set(dfu_pid "12")
    else()
        # DFU not supported on this target
        idf_msg("DFU not supported on ${target}, skipping DFU target creation")
        return()
    endif()

    # Get build properties
    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)

    # Path to DFU output file
    set(dfu_output_file "${build_dir}/${ARG_TARGET}.bin")

    # Create DFU generation target
    add_custom_target(${ARG_TARGET}
        COMMAND ${python} ${idf_path}/tools/mkdfu.py write
        -o "${dfu_output_file}"
        --json "${build_dir}/flasher_args.json"
        --pid "${dfu_pid}"
        --flash-size "${CONFIG_ESPTOOLPY_FLASHSIZE}"
        DEPENDS ${binary}
        VERBATIM
        USES_TERMINAL
        COMMENT "Generating DFU binary for ${binary}"
    )

    # Add dependency on bootloader if it's being built
    if(CONFIG_APP_BUILD_BOOTLOADER AND TARGET bootloader)
        add_dependencies(${ARG_TARGET} bootloader)
    endif()

    # Create DFU list target (lists connected DFU devices)
    add_custom_target(${ARG_TARGET}-list
        COMMAND ${CMAKE_COMMAND}
        -D ESP_DFU_LIST="1"
        -P ${idf_path}/tools/cmake/run_dfu_util.cmake
        USES_TERMINAL
        COMMENT "Listing DFU devices"
    )

    # Create DFU flash target (flashes the DFU binary)
    add_custom_target(${ARG_TARGET}-flash
        COMMAND ${CMAKE_COMMAND}
        -D ESP_DFU_BIN="${dfu_output_file}"
        -D ESP_DFU_PID="${dfu_pid}"
        -P ${idf_path}/tools/cmake/run_dfu_util.cmake
        DEPENDS ${ARG_TARGET}
        USES_TERMINAL
        COMMENT "Flashing DFU binary for ${binary}"
    )

    idf_msg("Created DFU targets: ${ARG_TARGET}, ${ARG_TARGET}-list, ${ARG_TARGET}-flash")
endfunction()
