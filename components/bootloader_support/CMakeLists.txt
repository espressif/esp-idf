set(COMPONENT_SRCDIRS "src")

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
