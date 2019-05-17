register_component()

# Do not generate flash file when building bootloader or is in early expansion of the build
if(BOOTLOADER_BUILD)
    return()
endif()

# Set values used in flash_bootloader_args.in and generate flash file
# for bootloader
set(BOOTLOADER_OFFSET 0x1000)
esptool_py_flash_project_args(bootloader ${BOOTLOADER_OFFSET}
                            ${BOOTLOADER_BUILD_DIR}/bootloader.bin
                            FLASH_IN_PROJECT
                            FLASH_FILE_TEMPLATE flash_bootloader_args.in)