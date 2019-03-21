if(BOOTLOADER_BUILD)
    # For bootloader, all we need is headers
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_REQUIRES ${IDF_COMPONENTS})
    set(COMPONENT_SRCS)
    register_component()
else()
    # Regular app build
    set(COMPONENT_SRCS "esp_rom.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    register_component()

    target_linker_script(${COMPONENT_TARGET}
                        "esp32/ld/esp32.rom.ld"
                        "esp32/ld/esp32.rom.libgcc.ld")

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        target_compile_options(${COMPONENT_TARGET} PUBLIC -mfix-esp32-psram-cache-issue)
    else()
        target_linker_script(${COMPONENT_TARGET} "esp32/ld/esp32.rom.spiram_incompatible_fns.ld")
    endif()

    if(CONFIG_NEWLIB_NANO_FORMAT)
        target_linker_script(${COMPONENT_TARGET} "esp32/ld/esp32.rom.nanofmt.ld")
    endif()

    if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
        target_linker_script(${COMPONENT_TARGET} "esp32/ld/esp32.rom.spiflash.ld")
    endif()
endif()
