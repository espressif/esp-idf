idf_build_get_property(target IDF_TARGET)

set(include_dirs "include" "include/${target}")

set(private_required_comp "")

set(sources "")

if(target STREQUAL "linux")
    list(APPEND sources "${target}/esp_rom_sys.c"
                        "${target}/esp_rom_crc.c"
                        "${target}/esp_rom_md5.c"
                        "${target}/esp_rom_efuse.c")
    list(APPEND include_dirs "${IDF_PATH}/tools/mocks/soc/include")
else()
    list(APPEND include_dirs "${target}")
    list(APPEND sources "patches/esp_rom_crc.c"
                        "patches/esp_rom_sys.c"
                        "patches/esp_rom_uart.c"
                        "patches/esp_rom_spiflash.c"
                        "patches/esp_rom_efuse.c")


    # Override regi2c implementation in ROM
    if(CONFIG_ESP_ROM_HAS_REGI2C_BUG OR CONFIG_ESP_ROM_WITHOUT_REGI2C)
        if(target STREQUAL "esp32c6")
            list(APPEND sources "patches/esp_rom_hp_regi2c_${target}.c")
        else()
            list(APPEND sources "patches/esp_rom_regi2c_${target}.c")
        endif()
    endif()

    if(CONFIG_HEAP_TLSF_USE_ROM_IMPL AND CONFIG_ESP_ROM_TLSF_CHECK_PATCH)
        # This file shall be included in the build if TLSF in ROM is activated
        list(APPEND sources "patches/esp_rom_tlsf.c")
    endif()

    list(APPEND private_required_comp soc hal)
endif()

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND sources "patches/esp_rom_longjmp.S")
endif()

if(CONFIG_SOC_SYSTIMER_SUPPORTED)
    list(APPEND sources "patches/esp_rom_systimer.c")
endif()

if(CONFIG_HAL_WDT_USE_ROM_IMPL)
    list(APPEND sources "patches/esp_rom_wdt.c")
endif()

if(CONFIG_ESP_ROM_HAS_FLASH_COUNT_PAGES_BUG OR CONFIG_ESP_ROM_HAS_CACHE_WRITEBACK_BUG)
    list(APPEND sources "patches/esp_rom_cache_esp32s2_esp32s3.c")
endif()

if(CONFIG_ESP_ROM_HAS_CACHE_WRITEBACK_BUG)
    list(APPEND sources "patches/esp_rom_cache_writeback_esp32s3.S")
endif()

idf_component_register(SRCS ${sources}
                       INCLUDE_DIRS ${include_dirs}
                       PRIV_REQUIRES ${private_required_comp}
                       LDFRAGMENTS linker.lf)

set(ld_folder "ld")

# Append a target linker script at the target-specific path,
# only the 'name' part is different for each script
function(rom_linker_script name)
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/${ld_folder}/${target}.rom.${name}.ld")
endfunction()

if(target STREQUAL "linux")
    # We need to disable some warnings due to the ROM code's printf implementation
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} GREATER "7.0.0")
        target_compile_options(${COMPONENT_LIB} PRIVATE -Wimplicit-fallthrough=0 -Wno-shift-count-overflow)
    endif()
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")  # Clang or AppleClang
        target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-integer-overflow -Wno-shift-count-overflow)
    endif()
else()
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/${ld_folder}/${target}.rom.ld")
    rom_linker_script("api")

    if(CONFIG_COMPILER_FLOAT_LIB_FROM_GCCLIB)
        rom_linker_script("libgcc")
    else()
        rom_linker_script("rvfp")
    endif()
endif()

idf_build_get_property(time_t_size TIME_T_SIZE)

if(BOOTLOADER_BUILD)
    if(target STREQUAL "esp32")
        rom_linker_script("newlib-funcs")
        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            rom_linker_script("spiflash")
        endif()
        if(CONFIG_ESP32_REV_MIN_FULL GREATER_EQUAL 300)
            rom_linker_script("eco3")
        endif()

    elseif(target STREQUAL "esp32s2")
        rom_linker_script("newlib-funcs")
        rom_linker_script("spiflash")

    elseif(target STREQUAL "esp32s3")
        rom_linker_script("newlib")

    elseif(target STREQUAL "esp32c3")
        rom_linker_script("newlib")

    elseif(target STREQUAL "esp32c2")
        rom_linker_script("newlib")

    elseif(target STREQUAL "esp32c6")
        rom_linker_script("newlib")
        # The linking of the bootloader needs to use the rom_i2c_writeReg_Mask in esp32c6.rom.phy.ld
        rom_linker_script("phy")
        if(CONFIG_HAL_WDT_USE_ROM_IMPL)
            rom_linker_script("wdt")
        endif()
        rom_linker_script("version")

    elseif(target STREQUAL "esp32h2")
        rom_linker_script("newlib")
        if(CONFIG_HAL_WDT_USE_ROM_IMPL)
            rom_linker_script("wdt")
        endif()

    elseif(target STREQUAL "esp32p4")
        rom_linker_script("newlib")
        if(CONFIG_HAL_WDT_USE_ROM_IMPL)
            rom_linker_script("wdt")
        endif()
        rom_linker_script("version")
    endif()

else() # Regular app build
    if(target STREQUAL "esp32")
        rom_linker_script("newlib-data")
        rom_linker_script("syscalls")

        if(NOT CONFIG_SPIRAM_CACHE_WORKAROUND)
            # ESP32 only: these ROM functions may only be used if PSRAM cache workaround is disabled.
            # Otherwise we need to link to a multilib version of libc compiled with PSRAM workaround.
            rom_linker_script("newlib-funcs")

            if(time_t_size EQUAL 4)
                # The ROM functions listed in this linker script depend on sizeof(time_t).
                # Since ROM for ESP32 was compiled for 32-bit time_t, only link these functions
                # if the toolchain is also using 32-bit time_t.
                rom_linker_script("newlib-time")

                if(CONFIG_NEWLIB_NANO_FORMAT)
                    # nano formatting functions in ROM are also built for 32-bit time_t.
                    rom_linker_script("newlib-nano")
                endif()
            endif()
        endif()

        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            rom_linker_script("spiflash")
        endif()

        if(CONFIG_ESP32_REV_MIN_FULL GREATER_EQUAL 300)
            rom_linker_script("eco3")
        endif()

    elseif(target STREQUAL "esp32s2")
        rom_linker_script("newlib-funcs")
        rom_linker_script("newlib-data")
        rom_linker_script("spiflash")

        if(time_t_size EQUAL 4)
            # The ROM functions listed in this linker script depend on sizeof(time_t).
            # Since ROM for ESP32-S2 was compiled for 32-bit time_t, only link these functions
            # if the toolchain is also using 32-bit time_t.
            rom_linker_script("newlib-time")

            if(CONFIG_NEWLIB_NANO_FORMAT)
                # nano formatting functions in ROM are also built for 32-bit time_t.
                rom_linker_script("newlib-nano")
            endif()
        endif()

    elseif(target STREQUAL "esp32s3")
        rom_linker_script("newlib")
        rom_linker_script("version")

        if(time_t_size EQUAL 4)
            # The ROM functions listed in this linker script depend on sizeof(time_t).
            # Since ROM for ESP32-S3 was compiled for 32-bit time_t, only link these functions
            # if the toolchain is also using 32-bit time_t.
            rom_linker_script("newlib-time")

            if(CONFIG_NEWLIB_NANO_FORMAT)
                # nano formatting functions in ROM are also built for 32-bit time_t.
                rom_linker_script("newlib-nano")
            endif()
        endif()

    elseif(target STREQUAL "esp32c3")
        rom_linker_script("newlib")
        rom_linker_script("version")

        if(time_t_size EQUAL 4)
            # The ROM functions listed in this linker script depend on sizeof(time_t).
            # Since ROM for ESP32-C3 was compiled for 32-bit time_t, only link these functions
            # if the toolchain is also using 32-bit time_t.
            rom_linker_script("newlib-time")

            if(CONFIG_NEWLIB_NANO_FORMAT)
                # nano formatting functions in ROM are also built for 32-bit time_t.
                rom_linker_script("newlib-nano")
            endif()
        endif()

        if(CONFIG_ESP32C3_REV_MIN_FULL GREATER_EQUAL 3)
            rom_linker_script("eco3")
        endif()

        if(CONFIG_ESP32C3_REV_MIN_FULL GREATER_EQUAL 101)
            rom_linker_script("eco7")
        endif()

    elseif(target STREQUAL "esp32c2")
        rom_linker_script("newlib")
        rom_linker_script("version")

        if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
            rom_linker_script("mbedtls")
        endif()

        if(CONFIG_NEWLIB_NANO_FORMAT)
            # nano formatting functions in ROM are also built for 64-bit time_t.
            rom_linker_script("newlib-nano")
        endif()

    elseif(target STREQUAL "esp32c6")
        rom_linker_script("newlib")
        rom_linker_script("version")

        # esp32c6.rom.api.ld has been split to several lds by components.
        rom_linker_script("phy")
        rom_linker_script("coexist")
        rom_linker_script("net80211")
        rom_linker_script("pp")

        if(CONFIG_SPI_FLASH_ROM_IMPL)
            rom_linker_script("spiflash")
        endif()

        if(CONFIG_HAL_WDT_USE_ROM_IMPL)
            rom_linker_script("wdt")
        endif()

        if(NOT CONFIG_NEWLIB_NANO_FORMAT)
            # Normal(Non-nano) formatting functions in ROM are also built for 64-bit time_t.
            rom_linker_script("newlib-normal")
        endif()

    elseif(target STREQUAL "esp32h2")
        rom_linker_script("newlib")
        rom_linker_script("version")

        if(CONFIG_SPI_FLASH_ROM_IMPL)
            rom_linker_script("spiflash")
        endif()

        if(CONFIG_HAL_WDT_USE_ROM_IMPL)
            rom_linker_script("wdt")
        endif()

        if(CONFIG_NEWLIB_NANO_FORMAT)
            # nano formatting functions in ROM are also built for 64-bit time_t.
            rom_linker_script("newlib-nano")
        endif()

    elseif(target STREQUAL "esp32p4")
        rom_linker_script("newlib")
        rom_linker_script("version")

        # esp32p4 rom doesn't have esp_flash driver

        if(CONFIG_HAL_WDT_USE_ROM_IMPL)
            rom_linker_script("wdt")
        endif()

        if(CONFIG_NEWLIB_NANO_FORMAT)
            # nano formatting functions in ROM are also built for 64-bit time_t.
            rom_linker_script("newlib-nano")
        endif()

    endif()

    if(CONFIG_HEAP_TLSF_USE_ROM_IMPL)
        # After registering the component, set the tlsf_set_rom_patches symbol as undefined
        # to force the linker to integrate the whole `esp_rom_tlsf.c` object file inside the
        # final binary. This is necessary because tlsf_set_rom_patches is a constructor, thus,
        # there as no explicit reference/call to it in IDF.
        if(CONFIG_ESP_ROM_TLSF_CHECK_PATCH)
            target_link_libraries(${COMPONENT_LIB} PRIVATE "-u tlsf_set_rom_patches")
        endif()

        rom_linker_script("heap")
    endif()

    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=longjmp")
    endif()
endif()

if(target STREQUAL "esp32s2")
    target_sources(${COMPONENT_LIB} PRIVATE "esp32s2/usb_patches.c")
endif()
