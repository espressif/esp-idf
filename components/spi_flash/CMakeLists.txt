set(priv_requires bootloader_support soc)
if(BOOTLOADER_BUILD)
    # Bootloader needs SPIUnlock from this file, but doesn't
    # need other parts of this component
    set(srcs "spi_flash_rom_patch.c")
else()
    set(srcs
        "cache_utils.c"
        "flash_mmap.c"
        "flash_ops.c"
        "partition.c"
        "spi_flash_rom_patch.c"
    )
    # New implementation
    list(APPEND srcs
        "spi_flash_chip_drivers.c"
        "spi_flash_chip_generic.c"
        "spi_flash_chip_issi.c"
        "spi_flash_os_func_app.c"
        "spi_flash_os_func_noos.c"
        "memspi_host_driver.c"
        "esp_flash_api.c"
        "esp_flash_spi_init.c"
    )
    set(priv_requires bootloader_support app_update soc)
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES "${priv_requires}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    LDFRAGMENTS linker.lf)
