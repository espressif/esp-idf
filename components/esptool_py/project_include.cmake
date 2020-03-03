if(NOT IDF_BUILD_ARTIFACTS)
    return()
endif()

# Set some global esptool.py variables
#
# Many of these are read when generating flash_app_args & flash_project_args
set(ESPTOOLPY ${PYTHON} "${CMAKE_CURRENT_LIST_DIR}/esptool/esptool.py" --chip esp32)
set(ESPSECUREPY ${PYTHON} "${CMAKE_CURRENT_LIST_DIR}/esptool/espsecure.py")
set(ESPEFUSEPY ${PYTHON} "${CMAKE_CURRENT_LIST_DIR}/esptool/espefuse.py")

set(ESPFLASHMODE ${CONFIG_ESPTOOLPY_FLASHMODE})
set(ESPFLASHFREQ ${CONFIG_ESPTOOLPY_FLASHFREQ})
set(ESPFLASHSIZE ${CONFIG_ESPTOOLPY_FLASHSIZE})

set(ESPTOOLPY_BEFORE "${CONFIG_ESPTOOLPY_BEFORE}")
set(ESPTOOLPY_AFTER  "${CONFIG_ESPTOOLPY_AFTER}")

if(CONFIG_SECURE_BOOT_ENABLED OR CONFIG_FLASH_ENCRYPTION_ENABLED)
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

set(ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS
    --flash_mode ${ESPFLASHMODE}
    --flash_freq ${ESPFLASHFREQ}
    --flash_size ${ESPFLASHSIZE}
    )

# String for printing flash command
string(REPLACE ";" " " ESPTOOLPY_WRITE_FLASH_STR
    "${ESPTOOLPY} --port (PORT) --baud (BAUD) --before ${ESPTOOLPY_BEFORE} --after ${ESPTOOLPY_AFTER} "
    "write_flash ${ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS} ${ESPTOOLPY_EXTRA_FLASH_OPTIONS} ${ESPTOOLPY_COMPRESSED_OPT}")

if(CONFIG_SECURE_BOOT_ENABLED AND
    NOT CONFIG_SECURE_BOOT_ALLOW_SHORT_APP_PARTITION AND
    NOT BOOTLOADER_BUILD)
    set(ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS
        ${ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS} --secure-pad)
endif()

if(NOT BOOTLOADER_BUILD)
    set(ESPTOOLPY_ELF2IMAGE_OPTIONS --elf-sha256-offset 0xb0)
endif()

if(CONFIG_ESP32_REV_MIN)
    set(ESPTOOLPY_ELF2IMAGE_OPTIONS ${ESPTOOLPY_ELF2IMAGE_OPTIONS} --min-rev ${CONFIG_ESP32_REV_MIN})
endif()

if(CONFIG_ESPTOOLPY_FLASHSIZE_DETECT)
    # Set ESPFLASHSIZE to 'detect' *after* elf2image options are generated,
    # as elf2image can't have 'detect' as an option...
    set(ESPFLASHSIZE detect)
endif()

get_filename_component(IDF_PROJECT_NAME ${IDF_PROJECT_EXECUTABLE} NAME_WE)
if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES AND NOT BOOTLOADER_BUILD)
    set(unsigned_project_binary "${IDF_PROJECT_NAME}-unsigned.bin")
else()
    set(unsigned_project_binary "${IDF_PROJECT_NAME}.bin")
endif()

#
# Add 'app.bin' target - generates with elf2image
#
add_custom_command(OUTPUT "${IDF_BUILD_ARTIFACTS_DIR}/${unsigned_project_binary}"
    COMMAND ${ESPTOOLPY} elf2image ${ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS} ${ESPTOOLPY_ELF2IMAGE_OPTIONS}
        -o "${IDF_BUILD_ARTIFACTS_DIR}/${unsigned_project_binary}" "${IDF_PROJECT_EXECUTABLE}"
    DEPENDS ${IDF_PROJECT_EXECUTABLE}
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

get_filename_component(IDF_PROJECT_BIN ${IDF_PROJECT_EXECUTABLE} NAME_WE)
set(IDF_PROJECT_BIN ${IDF_PROJECT_BIN}.bin)

if(NOT BOOTLOADER_BUILD AND
    CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)

    # for locally signed secure boot image, add a signing step to get from unsigned app to signed app
    add_custom_target(gen_unsigned_project_binary ALL DEPENDS "${IDF_BUILD_ARTIFACTS_DIR}/${unsigned_project_binary}")
    add_custom_command(OUTPUT "${IDF_BUILD_ARTIFACTS_DIR}/${IDF_PROJECT_BIN}"
        COMMAND ${ESPSECUREPY} sign_data --keyfile ${secure_boot_signing_key}
            -o "${IDF_BUILD_ARTIFACTS_DIR}/${IDF_PROJECT_BIN}" "${IDF_BUILD_ARTIFACTS_DIR}/${unsigned_project_binary}"
        DEPENDS gen_unsigned_project_binary
        VERBATIM
        )
endif()

if(NOT BOOTLOADER_BUILD)
    add_custom_target(app ALL DEPENDS "${IDF_BUILD_ARTIFACTS_DIR}/${IDF_PROJECT_BIN}")
else()
    add_custom_target(bootloader ALL DEPENDS "${IDF_BUILD_ARTIFACTS_DIR}/${IDF_PROJECT_BIN}")
endif()

if(NOT BOOTLOADER_BUILD AND
    CONFIG_SECURE_BOOT_ENABLED AND
    NOT CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
    add_custom_command(TARGET app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
            "App built but not signed. Sign app before flashing"
        COMMAND ${CMAKE_COMMAND} -E echo
            "\t${ESPSECUREPY} sign_data --keyfile KEYFILE ${IDF_BUILD_ARTIFACTS_DIR}/${IDF_PROJECT_BIN}"
        VERBATIM)
endif()

#
# Add 'flash' target - not all build systems can run this directly
#
function(esptool_py_custom_target target_name flasher_filename dependencies)
    add_custom_target(${target_name} DEPENDS ${dependencies}
        COMMAND ${CMAKE_COMMAND}
        -D IDF_PATH="${IDF_PATH}"
        -D ESPTOOLPY="${ESPTOOLPY}"
        -D ESPTOOL_ARGS="write_flash;@flash_${flasher_filename}_args"
        -D ESPTOOL_WORKING_DIR="${IDF_BUILD_ARTIFACTS_DIR}"
        -P ${IDF_PATH}/components/esptool_py/run_esptool.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        USES_TERMINAL
        )
endfunction()

esptool_py_custom_target(flash project "app;partition_table;bootloader")
esptool_py_custom_target(app-flash app "app")
esptool_py_custom_target(bootloader-flash bootloader "bootloader")
