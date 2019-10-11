idf_component_register(PRIV_REQUIRES partition_table)

# Do not generate flash file when building bootloader or is in early expansion of the build
if(BOOTLOADER_BUILD OR NOT CONFIG_APP_BUILD_BOOTLOADER)
    return()
endif()

# When secure boot is enabled, do not flash bootloader along with invocation of `idf.py flash`
if(NOT CONFIG_SECURE_BOOT_ENABLED)
    set(flash_bootloader FLASH_IN_PROJECT)
endif()

# Set values used in flash_bootloader_args.in and generate flash file
# for bootloader
esptool_py_flash_project_args(bootloader 0x1000
                            ${BOOTLOADER_BUILD_DIR}/bootloader.bin
                            ${flash_bootloader}
                            FLASH_FILE_TEMPLATE flash_bootloader_args.in)

esptool_py_custom_target(bootloader-flash bootloader "bootloader")
add_dependencies(bootloader partition_table)
