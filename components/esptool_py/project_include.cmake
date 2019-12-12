# Set some global esptool.py variables
#
# Many of these are read when generating flash_app_args & flash_project_args
idf_build_get_property(python PYTHON)
set(ESPTOOLPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/esptool.py" --chip ${IDF_TARGET})
set(ESPSECUREPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/espsecure.py")
set(ESPEFUSEPY ${python} "${CMAKE_CURRENT_LIST_DIR}/esptool/espefuse.py")

set(ESPFLASHMODE ${CONFIG_ESPTOOLPY_FLASHMODE})
set(ESPFLASHFREQ ${CONFIG_ESPTOOLPY_FLASHFREQ})
set(ESPFLASHSIZE ${CONFIG_ESPTOOLPY_FLASHSIZE})

set(ESPTOOLPY_BEFORE "${CONFIG_ESPTOOLPY_BEFORE}")
set(ESPTOOLPY_AFTER  "${CONFIG_ESPTOOLPY_AFTER}")

if(CONFIG_SECURE_BOOT_ENABLED OR CONFIG_SECURE_FLASH_ENC_ENABLED)
    # If security enabled then override post flash option
    set(ESPTOOLPY_AFTER "no_reset")
endif()

set(ESPTOOLPY_SERIAL "${ESPTOOLPY}"
    --port "${ESPPORT}"
    --baud ${ESPBAUD}
    --before "${ESPTOOLPY_BEFORE}"
    --after "${ESPTOOLPY_AFTER}"
    )

if(CONFIG_ESPTOOLPY_COMPRESSED)
    set(ESPTOOLPY_COMPRESSED_OPT -z)
else()
    set(ESPTOOLPY_COMPRESSED_OPT -u)
endif()

set(ESPTOOLPY_FLASH_OPTIONS
    --flash_mode ${ESPFLASHMODE}
    --flash_freq ${ESPFLASHFREQ}
    --flash_size ${ESPFLASHSIZE}
    )

# String for printing flash command
string(REPLACE ";" " " ESPTOOLPY_WRITE_FLASH_STR
    "${ESPTOOLPY} --port (PORT) --baud (BAUD) --before ${ESPTOOLPY_BEFORE} --after ${ESPTOOLPY_AFTER} "
    "write_flash ${ESPTOOLPY_FLASH_OPTIONS} ${ESPTOOLPY_EXTRA_FLASH_OPTIONS} ${ESPTOOLPY_COMPRESSED_OPT}")

if(NOT BOOTLOADER_BUILD)
    set(ESPTOOLPY_ELF2IMAGE_OPTIONS --elf-sha256-offset 0xb0)
endif()

if(CONFIG_SECURE_BOOT_ENABLED AND
    NOT CONFIG_SECURE_BOOT_ALLOW_SHORT_APP_PARTITION
    AND NOT BOOTLOADER_BUILD)
    set(ESPTOOLPY_ELF2IMAGE_OPTIONS ${ESPTOOLPY_ELF2IMAGE_OPTIONS} --secure-pad)
endif()

if(CONFIG_ESP32_REV_MIN)
    set(ESPTOOLPY_ELF2IMAGE_OPTIONS ${ESPTOOLPY_ELF2IMAGE_OPTIONS} --min-rev ${CONFIG_ESP32_REV_MIN})
endif()

if(CONFIG_ESPTOOLPY_FLASHSIZE_DETECT)
    # Set ESPFLASHSIZE to 'detect' *after* elf2image options are generated,
    # as elf2image can't have 'detect' as an option...
    set(ESPFLASHSIZE detect)
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
        COMMAND ${ESPTOOLPY} elf2image ${ESPTOOLPY_FLASH_OPTIONS} ${ESPTOOLPY_ELF2IMAGE_OPTIONS}
            -o "${build_dir}/${unsigned_project_binary}" "${elf}"
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

if(NOT BOOTLOADER_BUILD AND CONFIG_SECURE_SIGNED_APPS)
    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        # for locally signed secure boot image, add a signing step to get from unsigned app to signed app
        add_custom_command(OUTPUT "${build_dir}/.signed_bin_timestamp"
            COMMAND ${ESPSECUREPY} sign_data --keyfile ${secure_boot_signing_key}
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
                "\t${espsecurepy} sign_data --keyfile KEYFILE ${build_dir}/${PROJECT_BIN}"
            VERBATIM)
    endif()
endif()

#
# Add 'flash' target - not all build systems can run this directly
#
function(esptool_py_custom_target target_name flasher_filename dependencies)
    idf_build_get_property(idf_path IDF_PATH)
    add_custom_target(${target_name} DEPENDS ${dependencies}
        COMMAND ${CMAKE_COMMAND}
        -D IDF_PATH="${idf_path}"
        -D ESPTOOLPY="${ESPTOOLPY}"
        -D ESPTOOL_ARGS="write_flash;@flash_${flasher_filename}_args"
        -D WORKING_DIRECTORY="${build_dir}"
        -P run_esptool.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        USES_TERMINAL
        )
endfunction()

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
    -D ELF_FILE="${elf}"
    -D WORKING_DIRECTORY="${build_dir}"
    -P run_idf_monitor.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    USES_TERMINAL
    )

esptool_py_custom_target(flash project "app;partition_table;bootloader")
esptool_py_custom_target(app-flash app "app")

if(CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT)
    esptool_py_custom_target(encrypted-flash encrypted_project "app;partition_table;bootloader")
    esptool_py_custom_target(encrypted-app-flash encrypted_app "app")
endif()

# esptool_py_flash_project_args
#
# Add file to the flasher args list, to be flashed at a particular offset.
#
# When a template FLASH_FILE_TEMPLATE is given, the variables OFFSET and IMAGE
# hold the value of arguments offset and image, respectively.
function(esptool_py_flash_project_args entry offset image)
    set(options FLASH_IN_PROJECT)  # flash the image when flashing the project
    set(single_value FLASH_FILE_TEMPLATE) # template file to use to be able to
                                        # flash the image individually using esptool
    cmake_parse_arguments(_ "${options}" "${single_value}" "" "${ARGN}")

    if(${entry} IN_LIST flash_project_entries)
        message(FATAL_ERROR "entry '${entry}' has already been added to flash project entries")
    endif()

    idf_component_set_property(esptool_py FLASH_PROJECT_ENTRIES "${entry}" APPEND)

    idf_build_get_property(build_dir BUILD_DIR)
    file(RELATIVE_PATH image ${build_dir} ${image})

    # Generate the standalone flash file to flash the image individually using esptool
    set(entry_flash_args ${build_dir}/flash_${entry}_args)
    if(NOT __FLASH_FILE_TEMPLATE)
        file(GENERATE OUTPUT ${entry_flash_args} CONTENT "${offset} ${image}")
    else()
        set(OFFSET ${offset})
        set(IMAGE ${image})

        get_filename_component(template_in "${__FLASH_FILE_TEMPLATE}" ABSOLUTE)
        get_filename_component(template_name "${template_in}" NAME)
        set(template_partial "${CMAKE_CURRENT_BINARY_DIR}/${template_name}.in2")

        configure_file("${template_in}" "${template_partial}")
        file(GENERATE OUTPUT ${entry_flash_args} INPUT "${template_partial}")
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    APPEND PROPERTY
                    ADDITIONAL_MAKE_CLEAN_FILES "${template_partial}")
        unset(OFFSET)
        unset(IMAGE)
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${entry_flash_args})

    # Generate standalone entries in the flasher args json file
    idf_component_set_property(esptool_py FLASH_PROJECT_ARGS_ENTRY_JSON
                "\"${entry}\" : { \"offset\" : \"${offset}\", \"file\" : \"${image}\" }" APPEND)

    # Generate entries in the flasher args json file
    if(__FLASH_IN_PROJECT)
        idf_component_set_property(esptool_py FLASH_PROJECT_ARGS
                                "${offset} ${image}" APPEND)

        idf_component_set_property(esptool_py FLASH_PROJECT_ARGS_JSON
                                "\"${offset}\" : \"${image}\"" APPEND)
    endif()
endfunction()
