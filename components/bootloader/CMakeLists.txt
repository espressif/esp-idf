idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_component_register(PRIV_REQUIRES partition_table esptool_py)

# Do not generate flash file when building bootloader or is in early expansion of the build
# This also applies to the ESP-TEE build, as the esp_tee component only requires the
# Kconfig options from the bootloader
if(BOOTLOADER_BUILD OR esp_tee_build OR NOT CONFIG_APP_BUILD_BOOTLOADER)
    return()
endif()

add_dependencies(bootloader partition_table_bin)

# When secure boot is enabled and CONFIG_SECURE_BOOT_FLASH_BOOTLOADER_DEFAULT is not enabled
# do not flash the bootloader along with the other artifacts using the command `idf.py flash`
if(NOT CONFIG_SECURE_BOOT OR CONFIG_SECURE_BOOT_FLASH_BOOTLOADER_DEFAULT)
    set(flash_bootloader FLASH_IN_PROJECT)
endif()

esptool_py_custom_target(bootloader-flash bootloader "bootloader")
esptool_py_flash_target_image(bootloader-flash bootloader
    ${CONFIG_BOOTLOADER_OFFSET_IN_FLASH}
    "${BOOTLOADER_BUILD_DIR}/bootloader.bin")

# Also attach an image to the project flash target
if(NOT CONFIG_SECURE_BOOT OR CONFIG_SECURE_BOOT_FLASH_BOOTLOADER_DEFAULT)
    esptool_py_flash_target_image(flash bootloader
        ${CONFIG_BOOTLOADER_OFFSET_IN_FLASH}
        "${BOOTLOADER_BUILD_DIR}/bootloader.bin")
endif()
