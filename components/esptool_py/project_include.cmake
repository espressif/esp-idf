# Set some global esptool.py variables
#
# Many of these are read when generating flash_app_args & flash_project_args
idf_build_get_property(target IDF_TARGET)
idf_build_get_property(python PYTHON)
idf_build_get_property(idf_path IDF_PATH)


set(chip_model ${target})

# TODO: [ESP32C5] IDF-9197 remove this 'if' block when esp32C5 beta3 is no longer supported
if(target STREQUAL "esp32c5")
    if(CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION)
        set(chip_model esp32c5beta3)
    elseif(CONFIG_IDF_TARGET_ESP32C5_MP_VERSION)
        set(chip_model esp32c5)
    endif()
endif()

set(ESPTOOLPY ${python} "$ENV{ESPTOOL_WRAPPER}" "${CMAKE_CURRENT_LIST_DIR}/esptool/esptool.py" --chip ${chip_model})
set(ESPSECUREPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/espsecure.py")
set(ESPEFUSEPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/espefuse.py")
set(ESPMONITOR ${python} -m esp_idf_monitor)
set(ESPMKUF2 ${python} "${idf_path}/tools/mkuf2.py" write --chip ${chip_model})
set(ESPTOOLPY_CHIP "${chip_model}")

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

    if(NOT CONFIG_SECURE_BOOT_ALLOW_SHORT_APP_PARTITION AND
        NOT BOOTLOADER_BUILD)
        if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
            list(APPEND esptool_elf2image_args --secure-pad)
        elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
            list(APPEND esptool_elf2image_args --secure-pad-v2)
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

if(CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE)
    # Set ESPFLASHSIZE to 'detect' *after* esptool_elf2image_args are generated,
    # as elf2image can't have 'detect' as an option...
    set(ESPFLASHSIZE detect)
endif()

if(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME)
    set(ESPFLASHSIZE keep)
endif()

idf_build_get_property(build_dir BUILD_DIR)

idf_build_get_property(elf_name EXECUTABLE_NAME GENERATOR_EXPRESSION)
idf_build_get_property(elf EXECUTABLE GENERATOR_EXPRESSION)

if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES AND NOT BOOTLOADER_BUILD)
    set(unsigned_project_binary "${elf_name}-unsigned.bin")
else()
    set(unsigned_project_binary "${elf_name}.bin")
endif()

set(PROJECT_BIN "${elf_name}.bin")

#
# Add 'app.bin' target - generates with elf2image
#
if(CONFIG_APP_BUILD_GENERATE_BINARIES)
    add_custom_command(OUTPUT "${build_dir}/.bin_timestamp"
        COMMAND ${ESPTOOLPY} elf2image ${esptool_elf2image_args}
            -o "${build_dir}/${unsigned_project_binary}" "$<TARGET_FILE:$<GENEX_EVAL:${elf}>>"
        COMMAND ${CMAKE_COMMAND} -E echo "Generated ${build_dir}/${unsigned_project_binary}"
        COMMAND ${CMAKE_COMMAND} -E md5sum "${build_dir}/${unsigned_project_binary}" > "${build_dir}/.bin_timestamp"
        DEPENDS "$<TARGET_FILE:$<GENEX_EVAL:${elf}>>"
        VERBATIM
        WORKING_DIRECTORY ${build_dir}
        COMMENT "Generating binary image from built executable"
        )
    add_custom_target(gen_project_binary DEPENDS "${build_dir}/.bin_timestamp")
endif()

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    "${build_dir}/${unsigned_project_binary}"
    )

if(CONFIG_APP_BUILD_GENERATE_BINARIES)
    add_custom_target(app ALL DEPENDS gen_project_binary)
endif()

if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
    set(secure_boot_version "1")
elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
    set(secure_boot_version "2")
endif()

if(NOT BOOTLOADER_BUILD AND CONFIG_SECURE_SIGNED_APPS)
    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        # for locally signed secure boot image, add a signing step to get from unsigned app to signed app
        get_filename_component(secure_boot_signing_key "${CONFIG_SECURE_BOOT_SIGNING_KEY}"
            ABSOLUTE BASE_DIR "${project_dir}")
        add_custom_command(OUTPUT "${build_dir}/.signed_bin_timestamp"
            COMMAND ${ESPSECUREPY} sign_data --version ${secure_boot_version} --keyfile ${secure_boot_signing_key}
                -o "${build_dir}/${PROJECT_BIN}" "${build_dir}/${unsigned_project_binary}"
            COMMAND ${CMAKE_COMMAND} -E echo "Generated signed binary image ${build_dir}/${PROJECT_BIN}"
                                    "from ${build_dir}/${unsigned_project_binary}"
            COMMAND ${CMAKE_COMMAND} -E md5sum "${build_dir}/${PROJECT_BIN}" > "${build_dir}/.signed_bin_timestamp"
            DEPENDS "${build_dir}/.bin_timestamp"
            VERBATIM
            COMMENT "Generating signed binary image"
            )
        add_custom_target(gen_signed_project_binary DEPENDS "${build_dir}/.signed_bin_timestamp")
        add_dependencies(gen_project_binary gen_signed_project_binary)

        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            APPEND PROPERTY ADDITIONAL_CLEAN_FILES
            "${build_dir}/${PROJECT_BIN}"
            )
    else()
        string(REPLACE ";" " " espsecurepy "${ESPSECUREPY}")
        add_custom_command(TARGET app POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo
                "App built but not signed. Sign app before flashing"
            COMMAND ${CMAKE_COMMAND} -E echo
                "\t${espsecurepy} sign_data --keyfile KEYFILE --version ${secure_boot_version} \
                ${build_dir}/${PROJECT_BIN}"
            VERBATIM)
    endif()
endif()

add_custom_target(erase_flash
    COMMAND ${CMAKE_COMMAND}
    -D "IDF_PATH=${idf_path}"
    -D "SERIAL_TOOL=${ESPTOOLPY}"
    -D "SERIAL_TOOL_ARGS=erase_flash"
    -P run_serial_tool.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    USES_TERMINAL
    VERBATIM
    )

set(UF2_ARGS --json "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json")

add_custom_target(uf2
    COMMAND ${CMAKE_COMMAND}
    -D "IDF_PATH=${idf_path}"
    -D "SERIAL_TOOL=${ESPMKUF2}"
    -D "SERIAL_TOOL_ARGS=${UF2_ARGS};-o;${CMAKE_CURRENT_BINARY_DIR}/uf2.bin"
    -P run_serial_tool.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    USES_TERMINAL
    VERBATIM
    )

add_custom_target(uf2-app
    COMMAND ${CMAKE_COMMAND}
    -D "IDF_PATH=${idf_path}"
    -D "SERIAL_TOOL=${ESPMKUF2}"
    -D "SERIAL_TOOL_ARGS=${UF2_ARGS};-o;${CMAKE_CURRENT_BINARY_DIR}/uf2-app.bin;--bin;app"
    -P run_serial_tool.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
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
    -D "SERIAL_TOOL=${ESPTOOLPY}"
    -D "SERIAL_TOOL_ARGS=${MERGE_BIN_ARGS}"
    -D "WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
    -P run_serial_tool.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
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
    -D "SERIAL_TOOL=${ESPMONITOR}"
    -D "SERIAL_TOOL_ARGS=${MONITOR_ARGS}"
    -D "WORKING_DIRECTORY=${build_dir}"
    -P run_serial_tool.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    USES_TERMINAL
    VERBATIM
    )

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

idf_component_set_property(esptool_py FLASH_ARGS "${esptool_flash_main_args}")
idf_component_set_property(esptool_py FLASH_SUB_ARGS "--flash_mode ${ESPFLASHMODE} --flash_freq ${ESPFLASHFREQ} \
--flash_size ${ESPFLASHSIZE}")

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
        #   - nvs      0x02
        # If the partition is an app, an OTA or an NVS partition, then it should
        # be encrypted
        if(
                (${type} EQUAL 0) OR
                (${type} EQUAL 1 AND ${subtype} EQUAL 0) OR
                (${type} EQUAL 1 AND ${subtype} EQUAL 2)
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

# This function takes a fifth optional named parameter: "ALWAYS_PLAINTEXT". As
# its name states, it marks whether the image should be flashed as plain text or
# not. If build macro CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT is set and
# this parameter is provided, then the image will be flashed as plain text
# (not encrypted) on the target. This parameter will be ignored if build macro
# CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT is not set.
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


function(esptool_py_flash_target target_name main_args sub_args)
    set(single_value OFFSET IMAGE) # template file to use to be able to
                                   # flash the image individually using esptool
    set(options ALWAYS_PLAINTEXT)
    cmake_parse_arguments(_ "${options}" "${single_value}" "" "${ARGN}")

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)

    add_custom_target(${target_name}
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_PATH=${idf_path}"
        -D "SERIAL_TOOL=${ESPTOOLPY}"
        -D "SERIAL_TOOL_ARGS=${main_args};write_flash;@${target_name}_args"
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
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_args.in"
         CONTENT "${flash_args_content}")

    # Generate the actual expression value from the content of the file
    # we just wrote
    file(GENERATE OUTPUT "${build_dir}/${target_name}_args"
                INPUT "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_args.in")

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
        add_custom_target(encrypted-${target_name}
            COMMAND ${CMAKE_COMMAND}
            -D "IDF_PATH=${idf_path}"
            -D "SERIAL_TOOL=${ESPTOOLPY}"
            -D "SERIAL_TOOL_ARGS=${main_args};write_flash;@encrypted_${target_name}_args"
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
        file_generate("${CMAKE_CURRENT_BINARY_DIR}/encrypted_${target_name}_args.in"
                      CONTENT "${flash_args_content}")

        # Generate the actual string from the content of the file we just wrote
        file_generate("${build_dir}/encrypted_${target_name}_args"
                      INPUT "${CMAKE_CURRENT_BINARY_DIR}/encrypted_${target_name}_args.in")
    else()
        fail_target(encrypted-${target_name} "Error: The target encrypted-${target_name} requires"
                    "CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT to be enabled.")

    endif()
endfunction()


function(esptool_py_custom_target target_name flasher_filename dependencies)
    idf_component_get_property(main_args esptool_py FLASH_ARGS)
    idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)

    idf_build_get_property(build_dir BUILD_DIR)

    esptool_py_flash_target(${target_name} "${main_args}" "${sub_args}")

    # Copy the file to flash_xxx_args for compatibility for select target
    file_generate("${build_dir}/flash_${flasher_filename}_args"
                INPUT "${build_dir}/${target_name}_args")

    add_dependencies(${target_name} ${dependencies})

    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
        file_generate("${build_dir}/flash_encrypted_${flasher_filename}_args"
                    INPUT "${build_dir}/encrypted_${target_name}_args")

        add_dependencies(encrypted-${target_name} ${dependencies})
    endif()
endfunction()

if(NOT BOOTLOADER_BUILD)
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

    esptool_py_custom_target(flash project "${flash_deps}")
endif()

# Adds espefuse functions for global use
idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)
include(${esptool_py_dir}/espefuse.cmake)
