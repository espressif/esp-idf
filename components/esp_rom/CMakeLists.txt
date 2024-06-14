idf_build_get_property(target IDF_TARGET)

# TODO: IDF-9197
if(CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION)
    set(target_folder "esp32c5/beta3/esp32c5")
elseif(CONFIG_IDF_TARGET_ESP32C5_MP_VERSION)
    set(target_folder "esp32c5/mp/esp32c5")
else()
    set(target_folder "${target}")
endif()

set(include_dirs "include" "include/${target_folder}")

set(private_required_comp "")

set(sources "")

if(target STREQUAL "linux")
    list(APPEND sources "${target}/esp_rom_sys.c"
                        "${target}/esp_rom_crc.c"
                        "${target}/esp_rom_md5.c"
                        "${target}/esp_rom_efuse.c")
else()
    list(APPEND include_dirs "${target_folder}")
    if(CONFIG_IDF_TARGET_ESP32C5)
        list(APPEND include_dirs "include/${target_folder}/..")
    endif()
    list(APPEND sources "patches/esp_rom_crc.c"
                        "patches/esp_rom_sys.c"
                        "patches/esp_rom_uart.c"
                        "patches/esp_rom_spiflash.c"
                        "patches/esp_rom_efuse.c")


    # Override regi2c implementation in ROM
    if(CONFIG_ESP_ROM_HAS_REGI2C_BUG OR CONFIG_ESP_ROM_WITHOUT_REGI2C)
        if(target STREQUAL "esp32c6" OR target STREQUAL "esp32c5")
            # TODO: [ESP32C5] IDF-8824
            if(NOT CONFIG_IDF_TARGET_ESP32C5_MP_VERSION)
                list(APPEND sources "patches/esp_rom_hp_regi2c_${target}.c")
            endif()
        else()
            list(APPEND sources "patches/esp_rom_regi2c_${target}.c")
        endif()
    endif()

    if(CONFIG_HEAP_TLSF_USE_ROM_IMPL AND CONFIG_ESP_ROM_TLSF_CHECK_PATCH)
        # This file shall be included in the build if TLSF in ROM is activated
        list(APPEND sources "patches/esp_rom_tlsf.c")
    endif()

    if(CONFIG_HEAP_TLSF_USE_ROM_IMPL AND CONFIG_ESP_ROM_MULTI_HEAP_WALK_PATCH)
        # This file shall be included in the build if TLSF in ROM is activated
        list(APPEND sources "patches/esp_rom_multi_heap.c")
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

if(CONFIG_ESP_ROM_CLIC_INT_TYPE_PATCH)
    list(APPEND sources "patches/esp_rom_clic.c")
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
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target_folder}/${ld_folder}/${target}.rom.${name}.ld")
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
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target_folder}/${ld_folder}/${target}.rom.ld")
    rom_linker_script("api")

    if(CONFIG_COMPILER_FLOAT_LIB_FROM_GCCLIB)
        rom_linker_script("libgcc")
    else()
        rom_linker_script("rvfp")
    endif()
endif()

# Common API which is linked both for bootloader and app builds

if(CONFIG_HAL_WDT_USE_ROM_IMPL)
    rom_linker_script("wdt")
endif()

if(CONFIG_HAL_SYSTIMER_USE_ROM_IMPL)
    rom_linker_script("systimer")
endif()

if(CONFIG_ESP_ROM_HAS_VERSION)
    rom_linker_script("version")
endif()

if(BOOTLOADER_BUILD)
    if(target STREQUAL "esp32")
        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            rom_linker_script("spiflash_legacy")
        endif()
        if(CONFIG_ESP32_REV_MIN_FULL GREATER_EQUAL 300)
            rom_linker_script("eco3")
        endif()

    elseif(target STREQUAL "esp32s2")
        rom_linker_script("spiflash_legacy")

    elseif(target STREQUAL "esp32c6")
        # The linking of the bootloader needs to use the rom_i2c_writeReg_Mask in esp32c6.rom.phy.ld
        rom_linker_script("phy")
    endif()

    if(CONFIG_ESP_ROM_HAS_NEWLIB)
        if(target STREQUAL "esp32" OR target STREQUAL "esp32s2")
            rom_linker_script("newlib-funcs")
        else()
            rom_linker_script("newlib")
        endif()
    endif()


else() # Regular app build
    if(target STREQUAL "esp32")
        rom_linker_script("newlib-data")
        rom_linker_script("syscalls")

        if(NOT CONFIG_SPIRAM_CACHE_WORKAROUND)
            # ESP32 only: these ROM functions may only be used if PSRAM cache workaround is disabled.
            # Otherwise we need to link to a multilib version of libc compiled with PSRAM workaround.
            rom_linker_script("newlib-funcs")
        endif()

        if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
            # For ESP32, inclusion of ROM driver do not depend on CONFIG_SPI_FLASH_ROM_IMPL
            rom_linker_script("spiflash_legacy")
        endif()

        if(CONFIG_ESP32_REV_MIN_FULL GREATER_EQUAL 300)
            rom_linker_script("eco3")
        endif()

    elseif(target STREQUAL "esp32s2")
        rom_linker_script("newlib-funcs")
        rom_linker_script("newlib-data")

        # For ESP32S2, inclusion of ROM driver do not depend on CONFIG_SPI_FLASH_ROM_IMPL
        rom_linker_script("spiflash_legacy")

    elseif(target STREQUAL "esp32c3")

        if(CONFIG_ESP32C3_REV_MIN_FULL GREATER_EQUAL 3)
            rom_linker_script("eco3")
        endif()

        if(CONFIG_ESP32C3_REV_MIN_FULL GREATER_EQUAL 101)
            rom_linker_script("eco7")
        endif()

    elseif(target STREQUAL "esp32c6")
        # esp32c6.rom.api.ld has been split to several lds by components.
        # esp32c6.rom.api.ld is still reserved to map the APIs
        rom_linker_script("phy")
        rom_linker_script("coexist")
        rom_linker_script("net80211")
        rom_linker_script("pp")
    endif()

    if(CONFIG_ESP_ROM_HAS_NEWLIB AND NOT target STREQUAL "esp32" AND NOT target STREQUAL "esp32s2")
        # ESP32 and S2 are a bit different, keep them as special cases in the target specific include section
        rom_linker_script("newlib")

        if(CONFIG_ESP_ROM_HAS_NEWLIB_NANO_FORMAT AND CONFIG_NEWLIB_NANO_FORMAT)
            if(NOT CONFIG_ESP_ROM_HAS_NEWLIB_32BIT_TIME)
                # Newlib-nano functions contains time_t related functions
                # and cannot be used if they were compiled with 32 bit time_t
                rom_linker_script("newlib-nano")
            endif()
        endif()

        if(CONFIG_ESP_ROM_HAS_NEWLIB_NORMAL_FORMAT AND NOT CONFIG_NEWLIB_NANO_FORMAT)
            rom_linker_script("newlib-normal")
        endif()
    endif()

    if(CONFIG_HEAP_TLSF_USE_ROM_IMPL)
        # After registering the component, set the tlsf_set_rom_patches symbol as undefined
        # to force the linker to integrate the whole `esp_rom_tlsf.c` object file inside the
        # final binary. This is necessary because tlsf_set_rom_patches is a constructor, thus,
        # there as no explicit reference/call to it in IDF.
        if((CONFIG_ESP_ROM_TLSF_CHECK_PATCH OR CONFIG_HEAP_TLSF_CHECK_PATCH))
            target_link_libraries(${COMPONENT_LIB} PRIVATE "-u tlsf_set_rom_patches")
        endif()

        if((CONFIG_ESP_ROM_TLSF_CHECK_PATCH OR CONFIG_ESP_ROM_MULTI_HEAP_WALK_PATCH))
            target_link_libraries(${COMPONENT_LIB} PRIVATE "-u esp_rom_include_multi_heap_patch")
        endif()

        rom_linker_script("heap")
    endif()

    if(CONFIG_SPI_FLASH_ROM_IMPL)
        # Older targets do not have a separate ld file for spiflash
        if(NOT target STREQUAL "esp32c3" AND NOT target STREQUAL "esp32s3" AND NOT target STREQUAL "esp32c2")
            rom_linker_script("spiflash")
        endif()
    endif()

    if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
        rom_linker_script("mbedtls")
    endif()

    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=longjmp")
    endif()
endif()

if(target STREQUAL "esp32s2")
    target_sources(${COMPONENT_LIB} PRIVATE "esp32s2/usb_patches.c")
endif()
