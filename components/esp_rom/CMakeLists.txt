idf_build_get_property(target IDF_TARGET)

idf_component_register(INCLUDE_DIRS include)

if(BOOTLOADER_BUILD)
    set(scripts
        "${target}/ld/${target}.rom.ld"
        "${target}/ld/${target}.rom.newlib-funcs.ld"
        "${target}/ld/${target}.rom.libgcc.ld"
        )
    if(target STREQUAL "esp32s2beta")
        list(APPEND scripts "esp32s2beta/ld/esp32s2beta.rom.spiflash.ld")
    endif()

    target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
else() # Regular app build
    set(scripts
        "${target}/ld/${target}.rom.ld"
        "${target}/ld/${target}.rom.libgcc.ld"
        "${target}/ld/${target}.rom.newlib-data.ld")

    if(target STREQUAL "esp32")
        list(APPEND scripts "${target}/ld/${target}.rom.syscalls.ld")

        if(NOT CONFIG_SPIRAM_CACHE_WORKAROUND)
            list(APPEND scripts "esp32/ld/esp32.rom.newlib-funcs.ld")
        endif()

        if(CONFIG_NEWLIB_NANO_FORMAT)
            list(APPEND scripts "esp32/ld/esp32.rom.newlib-nano.ld")
        endif()

        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            list(APPEND scripts "esp32/ld/esp32.rom.spiflash.ld")
        endif()

    elseif(target STREQUAL "esp32s2beta")
        # no SPIRAM workaround for esp32s2beta
        # no nano formatting function in ROM

        list(APPEND scripts "esp32s2beta/ld/esp32s2beta.rom.newlib-funcs.ld"
                            "esp32s2beta/ld/esp32s2beta.rom.spiflash.ld")
    endif()

    target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")

endif()
