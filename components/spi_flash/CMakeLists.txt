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
                   "spi_flash_rom_patch.c")
    set(COMPONENT_REQUIRES app_update)
    set(COMPONENT_PRIV_REQUIRES bootloader_support soc)
endif()

set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

register_component()
