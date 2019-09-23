if(BOOTLOADER_BUILD)
    # For bootloader, all we need is headers
    idf_component_register(INCLUDE_DIRS include)
    set(scripts
        "esp32/ld/esp32.rom.ld"
        "esp32/ld/esp32.rom.newlib-funcs.ld"
        "esp32/ld/esp32.rom.libgcc.ld"
        )
    target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
else()
    # Regular app build
    idf_component_register(SRCS "esp_rom.c"
                        INCLUDE_DIRS include)

    set(scripts
        "esp32/ld/esp32.rom.ld"
        "esp32/ld/esp32.rom.libgcc.ld"
        "esp32/ld/esp32.rom.syscalls.ld"
        "esp32/ld/esp32.rom.newlib-data.ld"
        )
    target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")

    if(NOT CONFIG_SPIRAM_CACHE_WORKAROUND)
        target_linker_script(${COMPONENT_LIB} INTERFACE "esp32/ld/esp32.rom.newlib-funcs.ld")
    endif()

    if(CONFIG_NEWLIB_NANO_FORMAT)
        target_linker_script(${COMPONENT_LIB} INTERFACE "esp32/ld/esp32.rom.newlib-nano.ld")
    endif()

    if(NOT GCC_NOT_5_2_0)
        target_linker_script(${COMPONENT_LIB} INTERFACE "esp32/ld/esp32.rom.newlib-locale.ld")
    endif()

    if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
        target_linker_script(${COMPONENT_LIB} INTERFACE "esp32/ld/esp32.rom.spiflash.ld")
    endif()
endif()
