if(BOOTLOADER_BUILD)
    # For bootloader, all we need is headers
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_REQUIRES ${IDF_COMPONENTS})
    set(COMPONENT_SRCS)
    register_component()
    idf_build_get_property(target IDF_TARGET)
    set(scripts "${target}/ld/${target}.rom.ld")
    if(target STREQUAL "esp32")
        list(APPEND scripts
            "${target}/ld/${target}.rom.newlib-funcs.ld"
            "${target}/ld/${target}.rom.libgcc.ld")
    elseif(target STREQUAL "esp32s2beta")
        list(APPEND scripts
            "${target}/ld/${target}.rom.spiflash.ld"
            "${target}/ld/${target}.rom.spiram_incompatible_fns.ld")
    endif()
    target_linker_script(${COMPONENT_LIB} "${scripts}")
else()
    # Regular app build
    set(COMPONENT_SRCS "esp_rom.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    register_component()

    if (CONFIG_IDF_TARGET_ESP32)
        set(scripts
            "esp32/ld/esp32.rom.ld"
            "esp32/ld/esp32.rom.libgcc.ld"
            "esp32/ld/esp32.rom.syscalls.ld"
            "esp32/ld/esp32.rom.newlib-data.ld"
            )
        target_linker_script(${COMPONENT_LIB} "${scripts}")

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

    elseif(CONFIG_IDF_TARGET_ESP32S2BETA)
        target_linker_script(${COMPONENT_LIB}
                            "esp32s2beta/ld/esp32s2beta.rom.ld"
                            "esp32s2beta/ld/esp32s2beta.rom.nanofmt.ld"
                            "esp32s2beta/ld/esp32s2beta.rom.spiflash.ld"
                            "esp32s2beta/ld/esp32s2beta.rom.spiram_incompatible_fns.ld"
                            "esp32s2beta/ld/esp32s2beta.spiram.rom-functions-dram.ld"
                            "esp32s2beta/ld/esp32s2beta.spiram.rom-functions-iram.ld")
    endif()

endif()
