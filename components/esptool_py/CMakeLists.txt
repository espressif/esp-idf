register_config_only_component()

string(REPLACE ";" " " ESPTOOLPY_FLASH_PROJECT_OPTIONS "${ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS}")
set(ESPTOOLPY_FLASH_PROJECT_OPTIONS
    "${ESPTOOLPY_FLASH_PROJECT_OPTIONS} ${BOOTLOADER_OFFSET} bootloader/bootloader.bin"
    )

if(CONFIG_SECURE_BOOT_ENABLED)
    set(ESPTOOLPY_FLASH_PROJECT_OPTIONS "")
endif()

if(IDF_BUILD_ARTIFACTS)
    # Generate pre-canned flasher args files suitable for passing to esptool.py
    foreach(part project app bootloader partition_table)
        configure_file(
            "${CMAKE_CURRENT_LIST_DIR}/flash_${part}_args.in"
            "${IDF_BUILD_ARTIFACTS_DIR}/flash_${part}_args"
            )
    endforeach()

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/flasher_args.json.in"
        "${IDF_BUILD_ARTIFACTS_DIR}/flasher_args.json"
        )
endif()

