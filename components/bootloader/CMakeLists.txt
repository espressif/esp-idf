idf_component_register(PRIV_REQUIRES partition_table)

# Do not generate flash file when building bootloader or is in early expansion of the build
if(BOOTLOADER_BUILD OR NOT CONFIG_APP_BUILD_BOOTLOADER)
    return()
endif()

add_dependencies(bootloader partition_table_bin)

# When secure boot is enabled, do not flash bootloader along with invocation of `idf.py flash`
if(NOT CONFIG_SECURE_BOOT)
    set(flash_bootloader FLASH_IN_PROJECT)
endif()

esptool_py_custom_target(bootloader-flash bootloader "bootloader")
esptool_py_flash_target_image(bootloader-flash bootloader "0x1000" "${BOOTLOADER_BUILD_DIR}/bootloader.bin")

# Also attach an image to the project flash target
if(NOT CONFIG_SECURE_BOOT)
    esptool_py_flash_target_image(flash bootloader "0x1000" "${BOOTLOADER_BUILD_DIR}/bootloader.bin")
endif()