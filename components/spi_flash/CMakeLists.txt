if(BOOTLOADER_BUILD)
    # Bootloader needs SPIUnlock from this file, but doesn't
    # need other parts of this component
    set(COMPONENT_SRCS "spi_flash_rom_patch.c")
    set(COMPONENT_PRIV_REQUIRES bootloader_support soc)
else()
    set(COMPONENT_SRCS "cache_utils.c"
                   "flash_mmap.c"
                   "flash_ops.c"
                   "partition.c"
                   "spi_flash_rom_patch.c"
                   "spi_flash_chip_drivers.c"
                   "spi_flash_chip_generic.c"
                   "spi_flash_chip_issi.c"
                   "spi_flash_os_func_app.c"
                   "spi_flash_os_func_noos.c"
                   "memspi_host_driver.c"
                   )
    if(NOT CONFIG_SPI_FLASH_USE_LEGACY_IMPL)
        list(APPEND COMPONENT_SRCS "esp_flash_api.c")
    endif()
    set(COMPONENT_PRIV_REQUIRES bootloader_support app_update soc)
endif()

set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS private_include)
set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

register_component()
