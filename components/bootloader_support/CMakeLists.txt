set(COMPONENT_SRCS "src/bootloader_clock.c"
                   "src/bootloader_common.c"
                   "src/bootloader_flash.c"
                   "src/bootloader_init.c"
                   "src/bootloader_random.c"
                   "src/bootloader_sha.c"
                   "src/bootloader_utility.c"
                   "src/efuse.c"
                   "src/esp_image_format.c"
                   "src/flash_encrypt.c"
                   "src/flash_partitions.c"
                   "src/flash_qio_mode.c"
                   "src/secure_boot.c"
                   "src/secure_boot_signatures.c")

if(${BOOTLOADER_BUILD})
    set(COMPONENT_ADD_INCLUDEDIRS "include include_bootloader")
    set(COMPONENT_REQUIRES)
    set(COMPONENT_PRIV_REQUIRES spi_flash micro-ecc)
else()
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_PRIV_INCLUDEDIRS "include_bootloader")
    set(COMPONENT_REQUIRES)
    set(COMPONENT_PRIV_REQUIRES spi_flash mbedtls micro-ecc)
endif()

register_component()
