if(BOOTLOADER_BUILD)
    # Bootloader needs SPIUnlock from this file, but doesn't
    # need other parts of this component
    set(COMPONENT_SRCS spi_flash_rom_patch.c)
    set(COMPONENT_PRIV_REQUIRES bootloader_support)
else()
    set(COMPONENT_SRCDIRS .)
    set(COMPONENT_PRIV_REQUIRES bootloader_support app_update)
endif()

set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_REQUIRES)

register_component()
