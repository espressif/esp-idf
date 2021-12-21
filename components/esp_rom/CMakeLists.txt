idf_build_get_property(target IDF_TARGET)

set(sources "patches/esp_rom_crc.c"
            "patches/esp_rom_sys.c"
            "patches/esp_rom_uart.c")

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND sources "patches/esp_rom_longjmp.S")
endif()

idf_component_register(SRCS ${sources}
                       INCLUDE_DIRS include "${target}" "include/${target}"
                       PRIV_REQUIRES soc hal)

# Append a target linker script at the target-specific path,
# only the 'name' part is different for each script
function(rom_linker_script name)
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/ld/${target}.rom.${name}.ld")
endfunction()

target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/ld/${target}.rom.ld")
rom_linker_script("api")
rom_linker_script("libgcc")

if(BOOTLOADER_BUILD)
    if(target STREQUAL "esp32")
        rom_linker_script("newlib-funcs")
        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            rom_linker_script("spiflash")
        endif()
        if(CONFIG_ESP32_REV_MIN_3)
            rom_linker_script("eco3")
        endif()

    elseif(target STREQUAL "esp32s2")
        rom_linker_script("newlib-funcs")
        rom_linker_script("spiflash")

    elseif(target STREQUAL "esp32s3")
        rom_linker_script("newlib-funcs")
        rom_linker_script("spiflash")

    elseif(target STREQUAL "esp32c3")
        rom_linker_script("newlib")
    endif()

else() # Regular app build
    if(target STREQUAL "esp32")
        rom_linker_script("newlib-data")
        rom_linker_script("syscalls")

        if(NOT CONFIG_SPIRAM_CACHE_WORKAROUND)
            rom_linker_script("newlib-funcs")
            if(NOT CONFIG_SDK_TOOLCHAIN_SUPPORTS_TIME_WIDE_64_BITS)
                # If SDK_TOOLCHAIN_SUPPORTS_TIME_WIDE_64_BITS option is defined
                # then all time functions from the ROM memory will not be linked.
                # Instead, those functions can be used from the toolchain by ESP-IDF.
                rom_linker_script("newlib-time")
            endif()

            # Include in newlib nano from ROM only if SPIRAM cache workaround is disabled
            if(CONFIG_NEWLIB_NANO_FORMAT)
                rom_linker_script("newlib-nano")
            endif()

        endif()

        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            rom_linker_script("spiflash")
        endif()

        if(CONFIG_ESP32_REV_MIN_3)
            rom_linker_script("eco3")
        endif()

    elseif(target STREQUAL "esp32s2")
        rom_linker_script("newlib-funcs")
        rom_linker_script("newlib-data")
        rom_linker_script("spiflash")

        if(CONFIG_NEWLIB_NANO_FORMAT)
            rom_linker_script("newlib-nano")
        endif()

        # descirptors used by ROM code
        target_sources(${COMPONENT_LIB} PRIVATE "esp32s2/usb_descriptors.c")

    elseif(target STREQUAL "esp32s3")
        rom_linker_script("newlib-funcs")
        rom_linker_script("newlib-data")
        rom_linker_script("spiflash")

        if(CONFIG_NEWLIB_NANO_FORMAT)
            rom_linker_script("newlib-nano")
        endif()

    elseif(target STREQUAL "esp32c3")
        rom_linker_script("newlib")
        rom_linker_script("version")

        if(CONFIG_NEWLIB_NANO_FORMAT)
            rom_linker_script("newlib-nano")
        endif()

        if(CONFIG_ESP32C3_REV_MIN_3)
            rom_linker_script("eco3")
        endif()
    endif()

    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=longjmp")
    endif()
endif()

if(target STREQUAL "esp32s2")
    target_sources(${COMPONENT_LIB} PRIVATE "esp32s2/usb_descriptors.c")
endif()
