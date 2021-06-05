# Set some global esptool.py variables
#
# Many of these are read when generating flash_app_args & flash_project_args
idf_build_get_property(target IDF_TARGET)
idf_build_get_property(python PYTHON)
idf_build_get_property(idf_path IDF_PATH)

set(chip_model ${target})
if(target STREQUAL "esp32s3")
    if(CONFIG_IDF_TARGET_ESP32S3_BETA_VERSION_3)
        set(chip_model "esp32s3beta3")
    endif()
endif()

set(ESPTOOLPY ${python} "$ENV{ESPTOOL_WRAPPER}" "${CMAKE_CURRENT_LIST_DIR}/esptool/esptool.py" --chip ${chip_model})
set(ESPSECUREPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/espsecure.py")
set(ESPEFUSEPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/espefuse.py")
set(ESPMONITOR ${python} "${idf_path}/tools/idf_monitor.py")

set(ESPFLASHMODE ${CONFIG_ESPTOOLPY_FLASHMODE})
set(ESPFLASHFREQ ${CONFIG_ESPTOOLPY_FLASHFREQ})
set(ESPFLASHSIZE ${CONFIG_ESPTOOLPY_FLASHSIZE})

set(ESPTOOLPY_FLASH_OPTIONS
    --flash_mode ${ESPFLASHMODE}
    --flash_freq ${ESPFLASHFREQ}
    --flash_size ${ESPFLASHSIZE}
    )

if(NOT BOOTLOADER_BUILD)
    set(esptool_elf2image_args --elf-sha256-offset 0xb0)
endif()

if(NOT CONFIG_SECURE_BOOT_ALLOW_SHORT_APP_PARTITION AND
    NOT BOOTLOADER_BUILD)
    if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
        list(APPEND esptool_elf2image_args --secure-pad)
    elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME)
        list(APPEND esptool_elf2image_args --secure-pad-v2)
    endif()
endif()

if(CONFIG_ESP32_REV_MIN)
    set(min_rev ${CONFIG_ESP32_REV_MIN})
endif()
if(CONFIG_ESP32C3_REV_MIN)
    set(min_rev ${CONFIG_ESP32C3_REV_MIN})
endif()

if(min_rev)
    list(APPEND esptool_elf2image_args --min-rev ${min_rev})
    set(monitor_rev_args "--revision;${min_rev}")
    unset(min_rev)
endif()

if(CONFIG_ESPTOOLPY_FLASHSIZE_DETECT)
    # Set ESPFLASHSIZE to 'detect' *after* elf2image options are generated,
    # as elf2image can't have 'detect' as an option...
    set(ESPFLASHSIZE detect)
endif()

if(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME)
    set(ESPFLASHSIZE keep)
endif()

idf_build_get_property(build_dir BUILD_DIR)

idf_build_get_property(elf_name EXECUTABLE_NAME GENERATOR_EXPRESSION)
idf_build_get_property(elf EXECUTABLE GENERATOR_EXPRESSION)
idf_build_get_property(elf_dir EXECUTABLE_DIR GENERATOR_EXPRESSION)

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
        COMMAND ${ESPTOOLPY} elf2image ${ESPTOOLPY_FLASH_OPTIONS} ${esptool_elf2image_args}
            -o "${build_dir}/${unsigned_project_binary}" "${elf_dir}/${elf}"
        COMMAND ${CMAKE_COMMAND} -E echo "Generated ${build_dir}/${unsigned_project_binary}"
        COMMAND ${CMAKE_COMMAND} -E md5sum "${build_dir}/${unsigned_project_binary}" > "${build_dir}/.bin_timestamp"
        DEPENDS ${elf}
        VERBATIM
        WORKING_DIRECTORY ${build_dir}
        COMMENT "Generating binary image from built executable"
        )
    add_custom_target(gen_project_binary DEPENDS "${build_dir}/.bin_timestamp")
endif()

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    "${build_dir}/${unsigned_project_binary}"
    )

if(CONFIG_APP_BUILD_GENERATE_BINARIES)
    add_custom_target(app ALL DEPENDS gen_project_binary)
endif()

if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
    set(secure_boot_version "1")
elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME)
    set(secure_boot_version "2")
endif()

if(NOT BOOTLOADER_BUILD AND CONFIG_SECURE_SIGNED_APPS)
    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        # for locally signed secure boot image, add a signing step to get from unsigned app to signed app
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
            APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
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
    -D IDF_PATH="${idf_path}"
    -D SERIAL_TOOL="${ESPTOOLPY}"
    -D SERIAL_TOOL_ARGS="erase_flash"
    -P run_serial_tool.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    USES_TERMINAL
    )

add_custom_target(monitor
    COMMAND ${CMAKE_COMMAND}
    -D IDF_PATH="${idf_path}"
    -D SERIAL_TOOL="${ESPMONITOR}"
    -D SERIAL_TOOL_ARGS="--target;${target};${monitor_rev_args};${elf_dir}/${elf}"
    -D WORKING_DIRECTORY="${build_dir}"
    -P run_serial_tool.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    USES_TERMINAL
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
idf_component_set_property(esptool_py FLASH_SUB_ARGS "${ESPTOOLPY_FLASH_OPTIONS}")

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
# this parameter is provided, then the image will be flahsed as plain text
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

# Use this function to generate a ternary expression that will be evaluated.
# - retexpr is the expression returned by the function
# - condition is the expression evaluated to a boolean
# - condtrue is the expression to evaluate if condition is true
# - condfalse is the expression to evaluate if condition is false
# This function can be summarized as:
#   retexpr = condition ? condtrue : condfalse
function(if_expr_generator retexpr condition condtrue condfalse)
    # CMake version 3.8 and above provide a ternary operator for expression
    # generator. For version under, we must simulate this behaviour
    if(${CMAKE_VERSION} VERSION_LESS "3.8.0")

        # If condtrue is not empty, then we have to do something in case the
        # condition is true. Generate the expression that will be used in that
        # case
        if(condtrue)
            set(iftrue "$<${condition}:${condtrue}>")
        endif()

        # Same for condfalse. If it is empty, it is useless to create an
        # expression that will be evaluated later
        if(condfalse)
            set(iffalse "$<$<NOT:${condition}>:${condfalse}>")
        endif()

        # Concatenate the previously generated expressions. If one of them was
        # not initialized (because of empty condtrue/condfalse) it will be
        # replaced by an empty string
        set(${retexpr} "${iftrue}${iffalse}" PARENT_SCOPE)

    else()
        # CMake 3.8 and above implement what we want, making the expression
        # simpler
        set(${retexpr} "$<IF:${condition},${condtrue},${condfalse}>" PARENT_SCOPE)
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
        -D IDF_PATH="${idf_path}"
        -D SERIAL_TOOL="${ESPTOOLPY}"
        -D SERIAL_TOOL_ARGS="${main_args};write_flash;@${target_name}_args"
        -D WORKING_DIRECTORY="${build_dir}"
        -P ${esptool_py_dir}/run_serial_tool.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        USES_TERMINAL
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
            -D IDF_PATH="${idf_path}"
            -D SERIAL_TOOL="${ESPTOOLPY}"
            -D SERIAL_TOOL_ARGS="${main_args};write_flash;@encrypted_${target_name}_args"
            -D WORKING_DIRECTORY="${build_dir}"
            -P ${esptool_py_dir}/run_serial_tool.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            USES_TERMINAL
            )

        # Generate the parameters for esptool.py command
        # In case we have both non encrypted and encrypted files to flash, we
        # can use --encrypt-files parameter to specify which ones should be
        # encrypted.
        # If we only have encrypted images to flash, we must use legacy
        # --encrypt parameter.
        # As the properties ENCRYPTED_IMAGES and NON_ENCRYPTED_IMAGES have not
        # been geenrated yet, we must use CMake expression generator to test
        # which esptool.py options we can use.

        # The variable has_non_encrypted_image will be evaluated to "1" if some
        # images must not be encrypted. This variable will be used in the next
        # expression
        set(has_non_encrypted_images "$<BOOL:$<TARGET_PROPERTY:\
encrypted-${target_name},NON_ENCRYPTED_IMAGES>>")

        # Prepare esptool arguments (--encrypt or --encrypt-files)
        if_expr_generator(if_non_enc_expr ${has_non_encrypted_images}
                          "" "--encrypt")
        set_target_properties(encrypted-${target_name} PROPERTIES SUB_ARGS
                             "${sub_args}; ${if_non_enc_expr}")

        # Generate the list of files to pass to esptool
        set(encrypted_files "$<JOIN:$<TARGET_PROPERTY\
:encrypted-${target_name},ENCRYPTED_IMAGES>,\n>")
        set(non_encrypted_files "$<JOIN:$<TARGET_PROPERTY:\
encrypted-${target_name},NON_ENCRYPTED_IMAGES>,\n>")

        # Put both lists together, use --encrypted-files if we do also have
        # plain images to flash
        if_expr_generator(if_enc_expr ${has_non_encrypted_images}
                          "--encrypt-files\n" "")
        set(flash_args_content "$<JOIN:$<TARGET_PROPERTY:\
encrypted-${target_name},SUB_ARGS>, >\
${non_encrypted_files}\n\
${if_enc_expr}\
${encrypted_files}")

        # The expression is ready to be geenrated, write it to the file which
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
    set(flash_deps "partition_table_bin")

    if(CONFIG_APP_BUILD_GENERATE_BINARIES)
        list(APPEND flash_deps "app")
    endif()

    if(CONFIG_APP_BUILD_BOOTLOADER)
        list(APPEND flash_deps "bootloader")
    endif()

    esptool_py_custom_target(flash project "${flash_deps}")
endif()
