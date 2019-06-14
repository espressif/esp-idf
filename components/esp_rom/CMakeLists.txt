idf_build_get_property(target IDF_TARGET)

if(BOOTLOADER_BUILD)
    # For bootloader, all we need is headers
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_REQUIRES ${IDF_COMPONENTS})
    set(COMPONENT_SRCS)
    register_component()
    set(scripts "${target}/ld/${target}.rom.ld"
                "${target}/ld/${target}.rom.newlib-funcs.ld"
                "${target}/ld/${target}.rom.libgcc.ld")
    if (target STREQUAL "esp32s2beta")
        list(APPEND scripts "esp32s2beta/ld/esp32s2beta.rom.spiflash.ld")
    endif()
    target_linker_script(${COMPONENT_LIB} "${scripts}")
else()
    # Regular app build
    set(COMPONENT_SRCS "esp_rom.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    register_component()

    set(scripts
        "${target}/ld/${target}.rom.ld"
        "${target}/ld/${target}.rom.libgcc.ld"
        "${target}/ld/${target}.rom.newlib-data.ld")

    if (target STREQUAL "esp32")
        list(APPEND scripts "${target}/ld/${target}.rom.syscalls.ld")

        if(NOT CONFIG_SPIRAM_CACHE_WORKAROUND)
            target_linker_script(${COMPONENT_LIB} "esp32/ld/esp32.rom.newlib-funcs.ld")
        endif()

        if(CONFIG_NEWLIB_NANO_FORMAT)
            target_linker_script(${COMPONENT_LIB} "esp32/ld/esp32.rom.newlib-nano.ld")
        endif()

        if(NOT GCC_NOT_5_2_0)
            target_linker_script(${COMPONENT_LIB} "esp32/ld/esp32.rom.newlib-locale.ld")
        endif()

        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            target_linker_script(${COMPONENT_LIB} "esp32/ld/esp32.rom.spiflash.ld")
        endif()

    elseif(target STREQUAL "esp32s2beta")
        # no SPIRAM workaround for esp32s2beta
        # no nano formatting function in ROM
        # no GCC 5.2.0 for esp32s2beta, hence not linking locale functions

        list(APPEND scripts "esp32s2beta/ld/esp32s2beta.rom.newlib-funcs.ld"
                            "esp32s2beta/ld/esp32s2beta.rom.spiflash.ld")
    endif()

    target_linker_script(${COMPONENT_LIB} "${scripts}")

endif()
