# esptool_py component project_include.cmake

# esptool_py_partition_needs_encryption
#
# @brief Determine if a partition needs to be encrypted when flash encryption is enabled.
#
# When flash encryption is enabled in development mode, this function checks
# the type and subtype of a partition to determine if its contents should be
# encrypted before flashing.
#
# @param[out] retencrypted      Variable to store the result (TRUE if encryption needed, FALSE otherwise)
# @param[in]  partition_name    Name of the partition to check
function(esptool_py_partition_needs_encryption retencrypted partition_name)
    # Check if encryption is enabled
    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
        # Encryption is enabled, get partition type, subtype and encrypted flag
        # to determine whether it needs encryption or not.
        partition_table_get_partition_info(type "--partition-name ${partition_name}" "type")
        partition_table_get_partition_info(subtype "--partition-name ${partition_name}" "subtype")
        partition_table_get_partition_info(encrypted "--partition-name ${partition_name}" "encrypted")

        # As defined in gen_esp32part.py file:
        # Types:
        #   - APP  0x00
        #   - DATA 0x01
        # Subtypes:
        #   - ota      0x00
        #   - TEE ota  0x90
        #   - nvs_keys 0x04
        # If the partition is an app, an OTA or an NVS keys partition, then it should
        # be encrypted
        if(
                (${type} EQUAL 0) OR
                (${type} EQUAL 1 AND ${subtype} EQUAL 0) OR
                (${type} EQUAL 1 AND ${subtype} EQUAL 144) OR
                (${type} EQUAL 1 AND ${subtype} EQUAL 4)
          )
            set(encrypted TRUE)
        endif()

        # Return 'encrypted' value to the caller
        set(${retencrypted} ${encrypted} PARENT_SCOPE)
    else()
        # Encryption not enabled, return false
        set(${retencrypted} FALSE PARENT_SCOPE)
    endif()

endfunction()

# esptool_py_flash_to_partition
#
# @brief Add a binary image to be flashed to a specific partition.
#
# This function is a convenience wrapper that automatically determines the partition
# offset and encryption requirements, then calls esptool_py_flash_target_image() with
# the appropriate parameters. It simplifies flashing to named partitions.
#
# @param[in] target_name    Name of the flash target to add the image to
# @param[in] partition_name Name of the partition where the image should be flashed
# @param[in] binary_path    Path to the binary file to flash
function(esptool_py_flash_to_partition target_name partition_name binary_path)
    # Retrieve the offset for the partition to flash the image on
    partition_table_get_partition_info(offset "--partition-name ${partition_name}" "offset")

    if(NOT offset)
        message(FATAL_ERROR "Could not find offset of partition ${partition_name}")
    endif()

    # Check whether the partition needs encryption or not
    esptool_py_partition_needs_encryption(encrypted ${partition_name})

    # If the image should not be encrypted, we pass the option
    # ALWAYS_PLAINTEXT to the function esptool_py_flash_target_image
    if(NOT ${encrypted})
        set(option ALWAYS_PLAINTEXT)
    endif()

    # The image name is also the partition name
    esptool_py_flash_target_image(${target_name} ${partition_name} ${offset}
                                  ${binary_path} ${option})
endfunction()

# esptool_py_flash_target_image
#
# @brief Add a binary image to a flash target at a specific offset.
#
# This function adds a binary image to the specified flash target, which will be
# included when the target is executed. It handles both plain and encrypted flash
# scenarios, automatically setting up the appropriate target properties.
#
# @param[in] target_name    Name of the flash target to add the image to
# @param[in] image_name     Logical name for the image (used in flasher_args.json)
# @param[in] offset         Flash offset where the image should be written (in hex format like 0x1000)
# @param[in] image          Path to the binary file to flash
# @param[in, optional]      ALWAYS_PLAINTEXT (option) Force the image to be flashed as plain text
#                           even when flash encryption is enabled. Ignored if flash encryption
#                           is not configured.
function(esptool_py_flash_target_image target_name image_name offset image)
    set(options ALWAYS_PLAINTEXT)
    idf_build_get_property(build_dir BUILD_DIR)
    file(RELATIVE_PATH image ${build_dir} ${image})
    cmake_parse_arguments(arg "${options}" "" "" "${ARGN}")

    # Check if the image has to be plain text or not, depending on the macro
    # CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT and the parameter
    # ALWAYS_PLAINTEXT
    set(encrypted false)
    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT AND
       NOT arg_ALWAYS_PLAINTEXT)
        set(encrypted true)
    endif()

    # In the following snippet of code, some properties are defined for our
    # current target. These properties will be used to generate the actual
    # target_name_args files using the target_name_args.in files.
    # Please see function esptool_py_flash_target above for more information
    # about these properties and how they are used.

    # Add the image file, with its offset, to the list of files to
    # flash to the target. No matter whether flash encryption is
    # enabled or not, plain binaries (non-encrypted) need to be
    # generated
    set_property(TARGET ${target_name} APPEND PROPERTY FLASH_FILE
                "\"${offset}\" : \"${image}\"")
    set_property(TARGET ${target_name} APPEND PROPERTY FLASH_ENTRY
                "\"${image_name}\" : { \"offset\" : \"${offset}\", \"file\" : \"${image}\",\
 \"encrypted\" : \"${encrypted}\" }")
    set_property(TARGET ${target_name} APPEND PROPERTY IMAGES "${offset} ${image}")

    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
        # When flash encryption mode is enabled, if the current binary needs to
        # be encrypted, do the same as previously but prefixing target names
        # with "encrypted-".
        if(encrypted)
            set_property(TARGET encrypted-${target_name} APPEND PROPERTY FLASH_FILE
                         "\"${offset}\" : \"${image}\"")
            set_property(TARGET encrypted-${target_name} APPEND PROPERTY FLASH_ENTRY
                         "\"${image_name}\" : { \"offset\" : \"${offset}\", \"file\" : \"${image}\" , \
\"encrypted\" : \"${encrypted}\"  }")
            set_property(TARGET encrypted-${target_name} APPEND PROPERTY ENCRYPTED_IMAGES "${offset} ${image}")
        else()
            # The target doesn't need to be encrypted, thus, add the current
            # file to the NON_ENCRYPTED_IMAGES property
            set_property(TARGET encrypted-${target_name} APPEND PROPERTY NON_ENCRYPTED_IMAGES "${offset} ${image}")
        endif()
    endif()
endfunction()


# esptool_py_flash_target
#
# @brief Create a flash target that can flash multiple images using esptool.py.
#
# This function is the core of the flashing mechanism. It creates a custom target
# and attaches the actual esptool.py command to it as a POST_BUILD step. This
# ensures that the flash command only runs after all of the target's dependencies
# (like binary generation) have been successfully built.
#
# It works by generating an argument file (`<prefix>_args`) that contains all the
# necessary parameters for esptool.py. This file's content is constructed using
# CMake generator expressions, which are resolved at build time. This allows the
# final list of binaries to be flashed to be collected from properties on the
# target.
#
# If flash encryption is enabled, it also creates a corresponding `encrypted-`
# target, which handles the logic for encrypting all or a subset of the binaries.
#
# @param[in] target_name    Name of the flash target to create
# @param[in] main_args      Main esptool.py arguments (before write_flash command)
# @param[in] sub_args       Sub-arguments for write_flash command (flash mode, frequency, size)
# @param[in, optional]      FILENAME_PREFIX (single value) Prefix for generated argument files.
#                           If not specified, uses target_name as prefix.
# @param[in, optional]      ALWAYS_PLAINTEXT (option) Force all images to be flashed as plain text.
function(esptool_py_flash_target target_name main_args sub_args)
    set(single_value OFFSET IMAGE FILENAME_PREFIX) # template file to use to be able to
                                                   # flash the image individually using esptool
    set(options ALWAYS_PLAINTEXT)
    cmake_parse_arguments(_ "${options}" "${single_value}" "" "${ARGN}")

    if(__FILENAME_PREFIX)
        set(filename_prefix ${__FILENAME_PREFIX})
    else()
        set(filename_prefix ${target_name})
    endif()

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)
    idf_component_get_property(esptool_py_cmd esptool_py ESPTOOLPY_CMD)

    if(NOT TARGET ${target_name})
        add_custom_target(${target_name})
    endif()

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_PATH=${idf_path}"
        -D "SERIAL_TOOL=${esptool_py_cmd}"
        -D "SERIAL_TOOL_ARGS=${main_args};write_flash;@${filename_prefix}_args"
        -D "WORKING_DIRECTORY=${build_dir}"
        -P ${esptool_py_dir}/run_serial_tool.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        USES_TERMINAL
        VERBATIM
        )

    set_target_properties(${target_name} PROPERTIES SUB_ARGS "${sub_args}")

    # Create the expression that contains the list of file names to pass
    # to esptool script
    set(flash_args_content "$<JOIN:$<TARGET_PROPERTY:${target_name},SUB_ARGS>, >\n\
$<JOIN:$<TARGET_PROPERTY:${target_name},IMAGES>,\n>")

    # Write the previous expression to the target_name_args.in file
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${filename_prefix}_args.in"
         CONTENT "${flash_args_content}")

    # Generate the actual expression value from the content of the file
    # we just wrote
    file(GENERATE OUTPUT "${build_dir}/${filename_prefix}_args"
                INPUT "${CMAKE_CURRENT_BINARY_DIR}/${filename_prefix}_args.in")

    # Check if the target has to be plain text or not, depending on the macro
    # CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT and the parameter
    # ALWAYS_PLAINTEXT
    set(encrypted FALSE)
    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT AND
       NOT __ALWAYS_PLAINTEXT)
        set(encrypted TRUE)
    endif()

    # If the file needs to be encrypted, create a target file that lets the user
    # flash this partition independently from other files.
    # For example, if 'target_name' is app-flash and 'encrypted' is TRUE,
    # 'build' directory will contain a file name 'encrypted_app-flash_args'
    if(${encrypted})
        if(NOT TARGET encrypted-${target_name})
            add_custom_target(encrypted-${target_name})
        endif()

        add_custom_command(TARGET encrypted-${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            -D "IDF_PATH=${idf_path}"
            -D "SERIAL_TOOL=${esptool_py_cmd}"
            -D "SERIAL_TOOL_ARGS=${main_args};write_flash;@encrypted_${filename_prefix}_args"
            -D "WORKING_DIRECTORY=${build_dir}"
            -P ${esptool_py_dir}/run_serial_tool.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            USES_TERMINAL
            VERBATIM
            )

        # Generate the parameters for esptool.py command
        # In case we have both non encrypted and encrypted files to flash, we
        # can use --encrypt-files parameter to specify which ones should be
        # encrypted.
        # If we only have encrypted images to flash, we must use legacy
        # --encrypt parameter.
        # As the properties ENCRYPTED_IMAGES and NON_ENCRYPTED_IMAGES have not
        # been generated yet, we must use CMake expression generator to test
        # which esptool.py options we can use.

        # The variable has_non_encrypted_image will be evaluated to "1" if some
        # images must not be encrypted. This variable will be used in the next
        # expression
        set(has_non_encrypted_images "$<BOOL:$<TARGET_PROPERTY:\
encrypted-${target_name},NON_ENCRYPTED_IMAGES>>")

        # Prepare esptool arguments (--encrypt or --encrypt-files)
        set(if_non_enc_expr "$<IF:${has_non_encrypted_images},,--encrypt>")
        set_target_properties(encrypted-${target_name} PROPERTIES SUB_ARGS
                             "${sub_args}; ${if_non_enc_expr}")

        # Generate the list of files to pass to esptool
        set(encrypted_files "$<JOIN:$<TARGET_PROPERTY\
:encrypted-${target_name},ENCRYPTED_IMAGES>,\n>")
        set(non_encrypted_files "$<JOIN:$<TARGET_PROPERTY:\
encrypted-${target_name},NON_ENCRYPTED_IMAGES>,\n>")

        # Put both lists together, use --encrypted-files if we do also have
        # plain images to flash
        set(if_enc_expr "$<IF:${has_non_encrypted_images},--encrypt-files\n,>")
        set(flash_args_content "$<JOIN:$<TARGET_PROPERTY:\
encrypted-${target_name},SUB_ARGS>, >\
${non_encrypted_files}\n\
${if_enc_expr}\
${encrypted_files}")

        # The expression is ready to be generated, write it to the file which
        # extension is .in
        file_generate("${CMAKE_CURRENT_BINARY_DIR}/encrypted_${filename_prefix}_args.in"
                      CONTENT "${flash_args_content}")

        # Generate the actual string from the content of the file we just wrote
        file_generate("${build_dir}/encrypted_${filename_prefix}_args"
                      INPUT "${CMAKE_CURRENT_BINARY_DIR}/encrypted_${filename_prefix}_args.in")
    else()
        fail_target(encrypted-${target_name} "Error: The target encrypted-${target_name} requires"
                    "CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT to be enabled.")

    endif()
endfunction()


# esptool_py_custom_target
#
# @brief Create a custom flash target with dependencies.
#
# This function creates a flash target that depends on other build targets.
#
# @param[in] target_name        Name of the flash target to create
# @param[in] flasher_filename   Base name for generated flasher argument files
# @param[in] dependencies       List of CMake targets that this flash target depends on
# @param[in, optional]          FILENAME_PREFIX (single value) Custom prefix for argument files.
#                               If not specified, uses target_name as prefix.
function(esptool_py_custom_target target_name flasher_filename dependencies)
    __ensure_esptool_py_setup()

    idf_component_get_property(main_args esptool_py FLASH_ARGS)
    idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)

    idf_build_get_property(build_dir BUILD_DIR)

    # Parse optional arguments like FILENAME_PREFIX.
    set(one_value_args FILENAME_PREFIX)
    cmake_parse_arguments(arg "" "${one_value_args}" "" ${ARGN})

    # Call the underlying flash target function, explicitly passing the prefix if it exists.
    if(arg_FILENAME_PREFIX)
        esptool_py_flash_target(${target_name} "${main_args}" "${sub_args}" FILENAME_PREFIX "${arg_FILENAME_PREFIX}")
        set(filename_prefix ${arg_FILENAME_PREFIX})
    else()
        esptool_py_flash_target(${target_name} "${main_args}" "${sub_args}")
        set(filename_prefix ${target_name})
    endif()

    # Copy the file to flash_xxx_args for compatibility for select target
    file_generate("${build_dir}/flash_${flasher_filename}_args"
                INPUT "${build_dir}/${filename_prefix}_args")

    add_dependencies(${target_name} ${dependencies})

    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
        file_generate("${build_dir}/flash_encrypted_${flasher_filename}_args"
                    INPUT "${build_dir}/encrypted_${filename_prefix}_args")

        add_dependencies(encrypted-${target_name} ${dependencies})
    endif()
endfunction()

# __esptool_py_setup_tools
#
# @brief Sets up esptool.py, espsecure.py, and espefuse.py tool commands.
#
# This function retrieves the necessary paths and Python interpreter, then
# constructs the full command-line strings for `esptool.py`, `espsecure.py`,
# and `espefuse.py`. It stores these commands as properties of the `esptool_py`
# component for later use by other functions or components.
function(__esptool_py_setup_tools)
    idf_build_get_property(target IDF_TARGET)
    idf_build_get_property(python PYTHON)
    idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)

    set(esptool_py_cmd ${python} "$ENV{ESPTOOL_WRAPPER}" "${esptool_py_dir}/esptool/esptool.py" --chip ${target})
    idf_component_set_property(esptool_py ESPTOOLPY_CMD "${esptool_py_cmd}")

    set(espsecure_py_cmd ${python} "${esptool_py_dir}/esptool/espsecure.py")
    idf_component_set_property(esptool_py ESPSECUREPY_CMD "${espsecure_py_cmd}")

    set(espefuse_py_cmd ${python} "${esptool_py_dir}/esptool/espefuse.py")
    idf_component_set_property(esptool_py ESPEFUSEPY_CMD "${espefuse_py_cmd}")
endfunction()

# __esptool_py_setup_esptool_py_args
#
# @brief Sets up esptool.py arguments for elf2image and flash targets.
#
# This function determines the appropriate flash mode, frequency, and size based
# on the project configuration (Kconfig values). It assembles argument lists
# for both the `elf2image` operation (converting ELF to BIN), for general
# flashing commands and for creating the flasher_args.json file.
# These argument lists are then stored as properties of the `esptool_py`
# component for consistent use across the build system.
function(__esptool_py_setup_esptool_py_args)
    if(NOT CONFIG_APP_BUILD_TYPE_RAM AND CONFIG_APP_BUILD_GENERATE_BINARIES)
        if(CONFIG_BOOTLOADER_FLASH_DC_AWARE)
        # When set flash frequency to 120M, must keep 1st bootloader work under ``DOUT`` mode
        # because on some flash chips, 120M will modify the status register,
        # which will make ROM won't work.
        # This change intends to be for esptool only and the bootloader should keep use
        # ``DOUT`` mode.
            set(ESPFLASHMODE "dout")
            message("Note: HPM is enabled for the flash, force the ROM bootloader into DOUT mode for stable boot on")
        else()
            set(ESPFLASHMODE ${CONFIG_ESPTOOLPY_FLASHMODE})
        endif()
        set(ESPFLASHFREQ ${CONFIG_ESPTOOLPY_FLASHFREQ})
        set(ESPFLASHSIZE ${CONFIG_ESPTOOLPY_FLASHSIZE})

        set(esptool_elf2image_args
            --flash_mode ${ESPFLASHMODE}
            --flash_freq ${ESPFLASHFREQ}
            --flash_size ${ESPFLASHSIZE}
            )

        if(BOOTLOADER_BUILD AND CONFIG_SECURE_BOOT_V2_ENABLED)
            # The bootloader binary needs to be 4KB aligned in order to append a secure boot V2 signature block.
            # If CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES is NOT set, the bootloader
            # image generated is not 4KB aligned for external HSM to sign it readily.
            # Following esptool option --pad-to-size 4KB generates a 4K aligned bootloader image.
            # In case of signing during build, espsecure.py "sign_data" operation handles the 4K alignment of the image.
            if(NOT CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
                list(APPEND esptool_elf2image_args --pad-to-size 4KB)
            endif()
        endif()

        set(MMU_PAGE_SIZE ${CONFIG_MMU_PAGE_MODE})

        if(NOT BOOTLOADER_BUILD)
            list(APPEND esptool_elf2image_args --elf-sha256-offset 0xb0)
            # For chips that support configurable MMU page size feature
            # If page size is configured to values other than the default "64KB" in menuconfig,
            # then we need to pass the actual size to flash-mmu-page-size arg
            if(NOT MMU_PAGE_SIZE STREQUAL "64KB")
                list(APPEND esptool_elf2image_args --flash-mmu-page-size ${MMU_PAGE_SIZE})
            endif()
        endif()

        if(NOT CONFIG_SECURE_BOOT_ALLOW_SHORT_APP_PARTITION AND NOT BOOTLOADER_BUILD)
            if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
                list(APPEND esptool_elf2image_args --secure-pad)
            elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
                list(APPEND esptool_elf2image_args --secure-pad-v2)
            endif()
        endif()
    endif()

    if(CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE)
        # Set ESPFLASHSIZE to 'detect' *after* esptool_elf2image_args are generated,
        # as elf2image can't have 'detect' as an option...
        set(ESPFLASHSIZE detect)
    endif()

    if(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME)
        set(ESPFLASHSIZE keep)
    endif()

    # We still set "--min-rev" to keep the app compatible with older bootloaders where this field is controlled.
    if(CONFIG_IDF_TARGET_ESP32)
        # for this chip min_rev is major revision
        math(EXPR min_rev "${CONFIG_ESP_REV_MIN_FULL} / 100")
    endif()
    if(CONFIG_IDF_TARGET_ESP32C3)
        # for this chip min_rev is minor revision
        math(EXPR min_rev "${CONFIG_ESP_REV_MIN_FULL} % 100")
    endif()

    if(min_rev)
        list(APPEND esptool_elf2image_args --min-rev ${min_rev})
    endif()

    list(APPEND esptool_elf2image_args --min-rev-full ${CONFIG_ESP_REV_MIN_FULL})
    list(APPEND esptool_elf2image_args --max-rev-full ${CONFIG_ESP_REV_MAX_FULL})

    # Save esptool_elf2image_args to component property
    idf_component_set_property(esptool_py ESPTOOL_PY_ELF2IMAGE_ARGS "${esptool_elf2image_args}")

    set(esptool_flash_main_args "--before=${CONFIG_ESPTOOLPY_BEFORE}")

    if(CONFIG_SECURE_BOOT OR CONFIG_SECURE_FLASH_ENC_ENABLED)
        # If security enabled then override post flash option
        list(APPEND esptool_flash_main_args "--after=no_reset")
    else()
        list(APPEND esptool_flash_main_args "--after=${CONFIG_ESPTOOLPY_AFTER}")
    endif()

    if(CONFIG_ESPTOOLPY_NO_STUB)
        list(APPEND esptool_flash_main_args "--no-stub")
    endif()

    # Save flash arguments to component property
    idf_component_set_property(esptool_py FLASH_ARGS "${esptool_flash_main_args}")
    idf_component_set_property(esptool_py FLASH_SUB_ARGS
                "--flash_mode ${ESPFLASHMODE} --flash_freq ${ESPFLASHFREQ} --flash_size ${ESPFLASHSIZE}")

    # Save arguments for flasher_args.json
    idf_component_set_property(esptool_py ESPFLASHMODE "${ESPFLASHMODE}")
    idf_component_set_property(esptool_py ESPFLASHFREQ "${ESPFLASHFREQ}")
    idf_component_set_property(esptool_py ESPFLASHSIZE "${ESPFLASHSIZE}")
endfunction()

# __ensure_esptool_py_setup
#
# @brief Ensures that the esptool.py setup functions have been called once.
#
# This function acts as an initializer. It checks if the esptool_py
# setup has already been performed by checking a component property. If not, it
# calls __esptool_py_setup_tools() and __esptool_py_setup_esptool_py_args()
# to configure the component.
function(__ensure_esptool_py_setup)
    idf_component_get_property(esptool_py_setup_done esptool_py _ESPTOOL_PY_SETUP_DONE)
    if(NOT esptool_py_setup_done)
        __esptool_py_setup_tools()
        __esptool_py_setup_esptool_py_args()
        idf_component_set_property(esptool_py _ESPTOOL_PY_SETUP_DONE TRUE)
    endif()
endfunction()


# __idf_build_binary
#
# @brief Sets up the primary target for generating a .bin file from an .elf file.
#
# This function creates the custom command and target required to generate a
# project binary (`.bin`) file from the final `.elf` executable. It uses `esptool.py
# elf2image` to perform the conversion and manages dependencies to ensure the
# binary is regenerated whenever the ELF file changes.
#
# @param[in] OUTPUT_BIN_FILENAME The name of the output binary file to generate.
# @param[in] TARGET_NAME         The unique name for the custom target that
#                                generates the binary.
function(__idf_build_binary OUTPUT_BIN_FILENAME TARGET_NAME)
    __ensure_esptool_py_setup()

    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(elf EXECUTABLE GENERATOR_EXPRESSION)
    idf_component_get_property(esptool_py_cmd esptool_py ESPTOOLPY_CMD)

    # Collect post-ELF dependencies for the current ELF file
    idf_build_get_property(elf_name EXECUTABLE_NAME)
    idf_build_get_post_elf_dependencies("${elf_name}.elf" post_elf_deps)

    # Get esptool.py arguments for elf2image target
    idf_component_get_property(esptool_elf2image_args esptool_py ESPTOOL_PY_ELF2IMAGE_ARGS)

    # Create a custom command and target to generate binary from elf file
    add_custom_command(OUTPUT "${build_dir}/.bin_timestamp"
        COMMAND ${esptool_py_cmd} elf2image ${esptool_elf2image_args}
            -o "${build_dir}/${OUTPUT_BIN_FILENAME}" "$<TARGET_FILE:$<GENEX_EVAL:${elf}>>"
        COMMAND ${CMAKE_COMMAND} -E echo "Generated ${build_dir}/${OUTPUT_BIN_FILENAME}"
        COMMAND ${CMAKE_COMMAND} -E md5sum "${build_dir}/${OUTPUT_BIN_FILENAME}" > "${build_dir}/.bin_timestamp"
        DEPENDS "$<TARGET_FILE:$<GENEX_EVAL:${elf}>>" ${post_elf_deps}
        VERBATIM
        WORKING_DIRECTORY ${build_dir}
        COMMENT "Generating binary image from built executable"
        )
    # Create a custom target to generate the binary file
    add_custom_target(${TARGET_NAME} DEPENDS "${build_dir}/.bin_timestamp")

    # We need to create a gen_project_binary target for backward compatibility
    # since many other components depend on it. Add the new target as a dependency
    # to the gen_project_binary target.
    if(NOT TARGET gen_project_binary)
        add_custom_target(gen_project_binary DEPENDS ${TARGET_NAME})
    else()
        add_dependencies(gen_project_binary ${TARGET_NAME})
    endif()

    # Add an 'app' alias that is part of the default build
    if(NOT TARGET app)
        add_custom_target(app ALL DEPENDS gen_project_binary)
    else()
        add_dependencies(app gen_project_binary)
    endif()

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES
        "${build_dir}/${OUTPUT_BIN_FILENAME}"
        )
endfunction()

# __idf_build_secure_binary
#
# @brief Sets up targets for generating a signed binary for Secure Boot.
#
# If Secure Boot is enabled, this function adds a custom command to sign the
# previously generated application binary using `espsecure.py`. It creates a
# target that depends on the unsigned binary and produces a signed one, which
# is required for the bootloader to authenticate the application.
#
# @param[in] UNSIGNED_BIN_FILENAME The name of the unsigned input binary file.
# @param[in] SIGNED_BIN_FILENAME   The name of the signed output binary file.
# @param[in] TARGET_NAME           The unique name for the custom target that
#                                  generates the signed binary.
# @param[in, optional] KEYFILE     Path to the keyfile for signing.
# @param[in, optional] COMMENT     Custom message to display during build.
function(__idf_build_secure_binary UNSIGNED_BIN_FILENAME SIGNED_BIN_FILENAME TARGET_NAME)
    cmake_parse_arguments(arg "" "KEYFILE;COMMENT" "" ${ARGN})

    __ensure_esptool_py_setup()

    idf_build_get_property(build_dir BUILD_DIR)
    idf_component_get_property(espsecure_py_cmd esptool_py ESPSECUREPY_CMD)

    if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
        set(secure_boot_version "1")
    elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
        set(secure_boot_version "2")
    endif()

    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        if(arg_KEYFILE)
            # If a keyfile is provided, use it for signing.
            set(secure_boot_signing_key "${arg_KEYFILE}")
        else()
            # for locally signed secure boot image, add a signing step to get from unsigned app to signed app
            idf_build_get_property(project_dir PROJECT_DIR)
            get_filename_component(secure_boot_signing_key "${CONFIG_SECURE_BOOT_SIGNING_KEY}"
                ABSOLUTE BASE_DIR "${project_dir}")
        endif()

        if(arg_COMMENT)
            set(comment_text "${arg_COMMENT}")
        else()
            set(comment_text "Generating signed binary image")
        endif()

        add_custom_command(OUTPUT "${build_dir}/.signed_bin_timestamp"
            COMMAND ${espsecure_py_cmd} sign_data
                --version ${secure_boot_version} --keyfile "${secure_boot_signing_key}"
                -o "${build_dir}/${SIGNED_BIN_FILENAME}" "${build_dir}/${UNSIGNED_BIN_FILENAME}"
            COMMAND ${CMAKE_COMMAND} -E echo "Generated signed binary image ${build_dir}/${SIGNED_BIN_FILENAME}"
                                    "from ${build_dir}/${UNSIGNED_BIN_FILENAME}"
            COMMAND ${CMAKE_COMMAND} -E md5sum "${build_dir}/${SIGNED_BIN_FILENAME}"
                                                                    > "${build_dir}/.signed_bin_timestamp"
            DEPENDS "${build_dir}/.bin_timestamp"
            VERBATIM
            COMMENT "${comment_text}"
            )
        add_custom_target(${TARGET_NAME} DEPENDS "${build_dir}/.signed_bin_timestamp")

        # Add the new target as a dependency to the gen_project_binary target.
        if(NOT TARGET gen_project_binary)
            add_custom_target(gen_project_binary DEPENDS ${TARGET_NAME})
        else()
            add_dependencies(gen_project_binary ${TARGET_NAME})
        endif()

        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            APPEND PROPERTY ADDITIONAL_CLEAN_FILES
            "${build_dir}/${SIGNED_BIN_FILENAME}"
            )
    else()
        string(REPLACE ";" " " espsecurepy "${espsecure_py_cmd}")
        if(arg_COMMENT)
            set(comment_text "${arg_COMMENT}")
        else()
            set(comment_text "App built but not signed. Sign app before flashing.")
        endif()
        add_custom_command(TARGET app POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo
                "${comment_text}"
            COMMAND ${CMAKE_COMMAND} -E echo
                "\t${espsecurepy} sign_data --keyfile KEYFILE --version ${secure_boot_version} \
                ${build_dir}/${UNSIGNED_BIN_FILENAME}"
            VERBATIM)
    endif()
endfunction()

# __esptool_py_setup_utility_targets
#
# @brief Sets up common utility targets like `erase_flash`, `merge-bin`, and `monitor`
#
function(__esptool_py_setup_utility_targets)
    __ensure_esptool_py_setup()

    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(target IDF_TARGET)
    idf_build_get_property(elf_name EXECUTABLE_NAME GENERATOR_EXPRESSION)
    idf_build_get_property(elf EXECUTABLE GENERATOR_EXPRESSION)
    idf_component_get_property(esptool_py_cmd esptool_py ESPTOOLPY_CMD)
    idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)

    add_custom_target(erase_flash
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_PATH=${idf_path}"
        -D "SERIAL_TOOL=${esptool_py_cmd}"
        -D "SERIAL_TOOL_ARGS=erase_flash"
        -P run_serial_tool.cmake
        WORKING_DIRECTORY ${esptool_py_dir}
        USES_TERMINAL
        VERBATIM
        )

    set(MERGE_BIN_ARGS merge_bin)
    if(DEFINED ENV{ESP_MERGE_BIN_OUTPUT})
        list(APPEND MERGE_BIN_ARGS "-o" "$ENV{ESP_MERGE_BIN_OUTPUT}")
    else()
        if(DEFINED ENV{ESP_MERGE_BIN_FORMAT} AND "$ENV{ESP_MERGE_BIN_FORMAT}" STREQUAL "hex")
            list(APPEND MERGE_BIN_ARGS "-o" "${CMAKE_CURRENT_BINARY_DIR}/merged-binary.hex")
        else()
            list(APPEND MERGE_BIN_ARGS "-o" "${CMAKE_CURRENT_BINARY_DIR}/merged-binary.bin")
        endif()
    endif()

    if(DEFINED ENV{ESP_MERGE_BIN_FORMAT})
        list(APPEND MERGE_BIN_ARGS "-f" "$ENV{ESP_MERGE_BIN_FORMAT}")
    endif()

    list(APPEND MERGE_BIN_ARGS "@${CMAKE_CURRENT_BINARY_DIR}/flash_args")

    add_custom_target(merge-bin
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_PATH=${idf_path}"
        -D "SERIAL_TOOL=${esptool_py_cmd}"
        -D "SERIAL_TOOL_ARGS=${MERGE_BIN_ARGS}"
        -D "WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
        -P run_serial_tool.cmake
        WORKING_DIRECTORY ${esptool_py_dir}
        DEPENDS gen_project_binary bootloader
        USES_TERMINAL
        VERBATIM
        )

    set(MONITOR_ARGS "")

    list(APPEND MONITOR_ARGS "--toolchain-prefix;${_CMAKE_TOOLCHAIN_PREFIX};")

    if(CONFIG_ESP_COREDUMP_DECODE)
    list(APPEND MONITOR_ARGS "--decode-coredumps;${CONFIG_ESP_COREDUMP_DECODE};")
    endif()

    list(APPEND MONITOR_ARGS "--target;${target};")

    list(APPEND MONITOR_ARGS "--revision;${CONFIG_ESP_REV_MIN_FULL};")

    if(CONFIG_IDF_TARGET_ARCH_RISCV)
        list(APPEND MONITOR_ARGS "--decode-panic;backtrace;")
    endif()

    list(APPEND MONITOR_ARGS "$<TARGET_FILE:$<GENEX_EVAL:${elf}>>")

    add_custom_target(monitor
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_PATH=${idf_path}"
        -D "SERIAL_TOOL=${python} -m esp_idf_monitor"
        -D "SERIAL_TOOL_ARGS=${MONITOR_ARGS}"
        -D "WORKING_DIRECTORY=${build_dir}"
        -P run_serial_tool.cmake
        WORKING_DIRECTORY ${esptool_py_dir}
        USES_TERMINAL
        VERBATIM
        )
endfunction()

# __esptool_py_setup_main_flash_target
#
# @brief Sets up the main `flash` target and its dependencies.
#
# This function creates the main `flash` target, which is used to flash multiple
# images to the target device. It determines the dependencies for a full
# project flash (bootloader, partition table, the main app) and then calls
#
function(__esptool_py_setup_main_flash_target)
    __ensure_esptool_py_setup()

    idf_build_get_property(non_os_build NON_OS_BUILD)

    if(NOT non_os_build)
        set(flash_deps "")

        if(CONFIG_APP_BUILD_TYPE_APP_2NDBOOT)
            list(APPEND flash_deps "partition_table_bin")
        endif()

        if(CONFIG_APP_BUILD_GENERATE_BINARIES)
            list(APPEND flash_deps "app")
        endif()

        if(CONFIG_APP_BUILD_BOOTLOADER)
            list(APPEND flash_deps "bootloader")
        endif()

        # Create the flash target. If encryption is enabled, it will also create
        # an encrypted-flash target.
        esptool_py_custom_target(flash project "${flash_deps}" FILENAME_PREFIX "flash")
    endif()
endfunction()

# Adds espefuse functions for global use
idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)
include(${esptool_py_dir}/espefuse.cmake)

# Initialize the esptool_py component.
# This ensures that all its properties are set before any other components that
# depend on it try to access them.
__ensure_esptool_py_setup()
