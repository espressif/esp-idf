# Set some global esptool.py variables
#
# Many of these are read when generating flash_app_args & flash_project_args
idf_build_get_property(target IDF_TARGET)
idf_build_get_property(python PYTHON)

set(ESPTOOLPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/esptool.py" --chip ${target})
set(ESPSECUREPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/espsecure.py")
set(ESPEFUSEPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/espefuse.py")

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
    list(APPEND esptool_elf2image_args --min-rev ${CONFIG_ESP32_REV_MIN})
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
    -D ESPTOOLPY="${ESPTOOLPY}"
    -D ESPTOOL_ARGS="erase_flash"
    -P run_esptool.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    USES_TERMINAL
    )

add_custom_target(monitor
    COMMAND ${CMAKE_COMMAND}
    -D IDF_PATH="${idf_path}"
    -D IDF_MONITOR="${idf_path}/tools/idf_monitor.py"
    -D ELF_FILE="${elf_dir}/${elf}"
    -D WORKING_DIRECTORY="${build_dir}"
    -P run_idf_monitor.cmake
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

if(NOT CONFIG_ESPTOOLPY_WITH_STUB)
    list(APPEND esptool_flash_main_args "--no-stub")
endif()

idf_component_set_property(esptool_py FLASH_ARGS "${esptool_flash_main_args}")
idf_component_set_property(esptool_py FLASH_SUB_ARGS "${ESPTOOLPY_FLASH_OPTIONS}")

function(esptool_py_flash_target_image target_name image_name offset image)
    idf_build_get_property(build_dir BUILD_DIR)
    file(RELATIVE_PATH image ${build_dir} ${image})

    set_property(TARGET ${target_name} APPEND PROPERTY FLASH_FILE
                "\"${offset}\" : \"${image}\"")
    set_property(TARGET ${target_name} APPEND PROPERTY FLASH_ENTRY
                "\"${image_name}\" : { \"offset\" : \"${offset}\", \"file\" : \"${image}\" }")
    set_property(TARGET ${target_name} APPEND PROPERTY IMAGES "${offset} ${image}")

    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
        set_property(TARGET encrypted-${target_name} APPEND PROPERTY FLASH_FILE
                    "\"${offset}\" : \"${image}\"")
        set_property(TARGET encrypted-${target_name} APPEND PROPERTY FLASH_ENTRY
                    "\"${image_name}\" : { \"offset\" : \"${offset}\", \"file\" : \"${image}\" }")
        set_property(TARGET encrypted-${target_name} APPEND PROPERTY IMAGES "${offset} ${image}")
    endif()
endfunction()

function(esptool_py_flash_target target_name main_args sub_args)
    set(single_value OFFSET IMAGE) # template file to use to be able to
                                        # flash the image individually using esptool
    cmake_parse_arguments(_ "" "${single_value}" "" "${ARGN}")

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)

    add_custom_target(${target_name}
        COMMAND ${CMAKE_COMMAND}
        -D IDF_PATH="${idf_path}"
        -D ESPTOOLPY="${ESPTOOLPY}"
        -D ESPTOOL_ARGS="${main_args};write_flash;@${target_name}_args"
        -D WORKING_DIRECTORY="${build_dir}"
        -P ${esptool_py_dir}/run_esptool.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        USES_TERMINAL
        )

    set_target_properties(${target_name} PROPERTIES SUB_ARGS "${sub_args}")

    set(flash_args_content "$<JOIN:$<TARGET_PROPERTY:${target_name},SUB_ARGS>, >\n\
$<JOIN:$<TARGET_PROPERTY:${target_name},IMAGES>,\n>")

    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_args.in"
                CONTENT "${flash_args_content}")
    file(GENERATE OUTPUT "${build_dir}/${target_name}_args"
                INPUT "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_args.in")

    if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
        add_custom_target(encrypted-${target_name}
            COMMAND ${CMAKE_COMMAND}
            -D IDF_PATH="${idf_path}"
            -D ESPTOOLPY="${ESPTOOLPY}"
            -D ESPTOOL_ARGS="${main_args};write_flash;@encrypted_${target_name}_args"
            -D WORKING_DIRECTORY="${build_dir}"
            -P ${esptool_py_dir}/run_esptool.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            USES_TERMINAL
            )

        set_target_properties(encrypted-${target_name} PROPERTIES SUB_ARGS "${sub_args};--encrypt")

        set(flash_args_content "$<JOIN:$<TARGET_PROPERTY:encrypted-${target_name},SUB_ARGS>, >\n\
$<JOIN:$<TARGET_PROPERTY:encrypted-${target_name},IMAGES>,\n>")

        file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/encrypted_${target_name}_args.in"
                    CONTENT "${flash_args_content}")
        file(GENERATE OUTPUT "${build_dir}/encrypted_${target_name}_args"
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