idf_build_get_property(target IDF_TARGET)

idf_component_register(SRCS "patches/esp_rom_crc.c"
                            "patches/esp_rom_sys.c"
                            "patches/esp_rom_uart.c"
                       INCLUDE_DIRS include
                       PRIV_INCLUDE_DIRS "${target}"
                       PRIV_REQUIRES soc hal)

if(BOOTLOADER_BUILD)
    set(scripts
        "${target}/ld/${target}.rom.api.ld"
        "${target}/ld/${target}.rom.ld"
        "${target}/ld/${target}.rom.newlib-funcs.ld"
        "${target}/ld/${target}.rom.libgcc.ld"
        )
    if(target STREQUAL "esp32s2")
        list(APPEND scripts "esp32s2/ld/esp32s2.rom.spiflash.ld")
    endif()

    if(target STREQUAL "esp32s3")
        list(APPEND scripts "esp32s3/ld/esp32s3.rom.spiflash.ld")
    endif()

    if(CONFIG_ESP32_REV_MIN_3)
        list(APPEND scripts "esp32/ld/esp32.rom.eco3.ld")
    endif()

    target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
else() # Regular app build
    set(scripts
        "${target}/ld/${target}.rom.api.ld"
        "${target}/ld/${target}.rom.ld"
        "${target}/ld/${target}.rom.libgcc.ld"
        "${target}/ld/${target}.rom.newlib-data.ld")

    if(target STREQUAL "esp32")
        list(APPEND scripts "${target}/ld/${target}.rom.syscalls.ld")

        if(NOT CONFIG_SPIRAM_CACHE_WORKAROUND)
            list(APPEND scripts "esp32/ld/esp32.rom.newlib-funcs.ld")
            if(NOT CONFIG_SDK_TOOLCHAIN_SUPPORTS_TIME_WIDE_64_BITS)
                # If SDK_TOOLCHAIN_SUPPORTS_TIME_WIDE_64_BITS option is defined
                # then all time functions from the ROM memory will not be linked.
                # Instead, those functions can be used from the toolchain by ESP-IDF.
                target_linker_script(${COMPONENT_LIB} INTERFACE "esp32/ld/esp32.rom.newlib-time.ld")
            endif()

            # Include in newlib nano from ROM only if SPIRAM cache workaround is disabled
            if(CONFIG_NEWLIB_NANO_FORMAT)
                list(APPEND scripts "esp32/ld/esp32.rom.newlib-nano.ld")
            endif()

        endif()

        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            list(APPEND scripts "esp32/ld/esp32.rom.spiflash.ld")
        endif()

        if(CONFIG_ESP32_REV_MIN_3)
            list(APPEND scripts "esp32/ld/esp32.rom.eco3.ld")
        endif()

    elseif(target STREQUAL "esp32s2")
        # no SPIRAM workaround for esp32s2
        # no nano formatting function in ROM

        list(APPEND scripts "esp32s2/ld/esp32s2.rom.newlib-funcs.ld"
                            "esp32s2/ld/esp32s2.rom.spiflash.ld")

        if(CONFIG_NEWLIB_NANO_FORMAT)
            list(APPEND scripts "esp32s2/ld/esp32s2.rom.newlib-nano.ld")
        endif()

    elseif(target STREQUAL "esp32s3")
        # no SPIRAM workaround for esp32s3
        list(APPEND scripts "esp32s3/ld/esp32s3.rom.newlib-funcs.ld"
                            "esp32s3/ld/esp32s3.rom.spiflash.ld")

        if(CONFIG_NEWLIB_NANO_FORMAT)
            list(APPEND scripts "esp32s3/ld/esp32s3.rom.newlib-nano.ld")
        endif()

    endif()

    target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
endif()

if(target STREQUAL "esp32s2")
    target_sources(${COMPONENT_LIB} PRIVATE "esp32s2/usb_descriptors.c")
endif()
