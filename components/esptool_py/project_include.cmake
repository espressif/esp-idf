# Set some global esptool.py variables
#
# Many of these are read when generating flash_app_args & flash_project_args
set(ESPTOOLPY "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/esptool/esptool.py" --chip esp32)
set(ESPSECUREPY "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/esptool/espsecure.py")

set(ESPFLASHMODE ${CONFIG_ESPTOOLPY_FLASHMODE})
set(ESPFLASHFREQ ${CONFIG_ESPTOOLPY_FLASHFREQ})
set(ESPFLASHSIZE ${CONFIG_ESPTOOLPY_FLASHSIZE})

set(ESPTOOLPY_SERIAL "${ESPTOOLPY}" --port "${ESPPORT}" --baud ${ESPBAUD})

set(ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS
    --flash_mode ${ESPFLASHMODE}
    --flash_freq ${ESPFLASHFREQ}
    --flash_size ${ESPFLASHSIZE}
    )

if(CONFIG_ESPTOOLPY_FLASHSIZE_DETECT)
    # Set ESPFLASHSIZE to 'detect' *after* elf2image options are generated,
    # as elf2image can't have 'detect' as an option...
    set(ESPFLASHSIZE detect)
endif()

# Set variables if the PHY data partition is in the flash
if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION)
    set(PHY_PARTITION_OFFSET   ${CONFIG_PHY_DATA_OFFSET})
    set(PHY_PARTITION_BIN_FILE "esp32/phy_init_data.bin")
endif()

#
# Add 'app.bin' target - generates with elf2image
#
add_custom_command(OUTPUT "${PROJECT_NAME}.bin"
    COMMAND ${ESPTOOLPY} elf2image ${ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS} -o "${PROJECT_NAME}.bin" "${PROJECT_NAME}.elf"
    DEPENDS ${PROJECT_NAME}.elf
    VERBATIM
    )
add_custom_target(app ALL DEPENDS "${PROJECT_NAME}.bin")

#
# Add 'flash' target - not all build systems can run this directly
#
function(esptool_py_custom_target target_name flasher_filename dependencies)
    add_custom_target(${target_name} DEPENDS ${dependencies}
        COMMAND ${CMAKE_COMMAND}
        -D IDF_PATH="${IDF_PATH}"
        -D ESPTOOLPY="${ESPTOOLPY}"
        -D ESPTOOL_ARGS="write_flash;@flash_${flasher_filename}_args"
        -D ESPTOOL_WORKING_DIR="${CMAKE_CURRENT_BINARY_DIR}"
        -P run_esptool.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        USES_TERMINAL
        )
endfunction()

esptool_py_custom_target(flash project "app;partition_table;bootloader")
esptool_py_custom_target(app-flash app "app")
esptool_py_custom_target(bootloader-flash bootloader "bootloader")
