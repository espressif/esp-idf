idf_build_get_property(target IDF_TARGET)

set(target_folder "${target}")

set(include_dirs "include"
                 "${target_folder}/include"
                 "${target_folder}/include/${target_folder}"
                 "${target_folder}")

set(private_required_comp "")

set(sources "patches/esp_rom_sys.c"
            "patches/esp_rom_print.c")

if(target STREQUAL "linux")
    list(APPEND sources "${target}/esp_rom_sys.c"
                        "${target}/esp_rom_crc.c"
                        "${target}/esp_rom_md5.c"
                        "${target}/esp_rom_efuse.c")
else()
    list(APPEND sources "patches/esp_rom_crc.c"
                        "patches/esp_rom_serial_output.c"
                        "patches/esp_rom_spiflash.c"
                        "patches/esp_rom_efuse.c"
                        "patches/esp_rom_gpio.c")


    # Override regi2c implementation in ROM
    if(CONFIG_ESP_ROM_HAS_REGI2C_BUG OR CONFIG_ESP_ROM_WITHOUT_REGI2C)
        if(target STREQUAL "esp32c6" OR target STREQUAL "esp32c5")
            list(APPEND sources "patches/esp_rom_hp_regi2c_${target}.c")
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

    list(APPEND private_required_comp soc hal esp_hal_uart)
endif()

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND sources "patches/esp_rom_longjmp.S")
endif()

if(CONFIG_SOC_SYSTIMER_SUPPORTED)
    list(APPEND sources "patches/esp_rom_systimer.c")
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

if(CONFIG_ESP_ROM_NO_USB_SERIAL_OUTPUT_API)
    list(APPEND sources "patches/esp_rom_usb_serial.c")
endif()

if(CONFIG_SECURE_ENABLE_TEE AND CONFIG_IDF_TARGET_ESP32C5 AND NOT ESP_TEE_BUILD)
    list(APPEND sources "patches/esp_rom_cache_esp32c5.c")
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
    # TODO: IDF-13410. Update to (CONFIG_ESP32P4_REV_MIN_FULL GREATER_EQUAL 200) when chip efuse is correct.
    if(CONFIG_ESP32P4_REV_MIN_300)
        target_linker_script(${COMPONENT_LIB} INTERFACE "${target_folder}/${ld_folder}/${target}.rom.eco5.ld")
    elseif(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
        target_linker_script(${COMPONENT_LIB} INTERFACE "${target_folder}/${ld_folder}/${target}.rom.beta5.ld")
    else()
        target_linker_script(${COMPONENT_LIB} INTERFACE "${target_folder}/${ld_folder}/${target}.rom.ld")
    endif()
    rom_linker_script("api")
    if(NOT CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY)
        if(target STREQUAL "esp32s3" OR target STREQUAL "esp32c3")
            rom_linker_script("bt_funcs")
        endif()
    endif()

    if(CONFIG_COMPILER_FLOAT_LIB_FROM_GCCLIB)
        if(CONFIG_ESP32P4_REV_MIN_300) # TODO: IDF-13410
            rom_linker_script("eco5.libgcc")
        elseif(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
            rom_linker_script("beta5.libgcc")
        else()
            rom_linker_script("libgcc")
        endif()
    else()
        if(CONFIG_ESP32P4_REV_MIN_300) # TODO: IDF-13410.
            rom_linker_script("eco5.rvfp")
        else()
            rom_linker_script("rvfp")
        endif()
    endif()
endif()

# Common API which is linked both for bootloader and app builds

if(CONFIG_HAL_SYSTIMER_USE_ROM_IMPL)
    if(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
        rom_linker_script("beta5.systimer")
    else()
        rom_linker_script("systimer")
    endif()
endif()

if(CONFIG_ESP_ROM_HAS_VERSION)
    rom_linker_script("version")
endif()

if(ESP_TEE_BUILD)
    rom_linker_script("heap")
    if(CONFIG_LIBC_NEWLIB AND CONFIG_ESP_ROM_HAS_NEWLIB_NANO_FORMAT)
        rom_linker_script("newlib-nano")
    endif()
    rom_linker_script("libc")
    if(CONFIG_ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY)
        rom_linker_script("libc-suboptimal_for_misaligned_mem")
    endif()
endif()

if(BOOTLOADER_BUILD)
    if(target STREQUAL "esp32")
        if(CONFIG_ESP32_REV_MIN_FULL GREATER_EQUAL 300)
            rom_linker_script("eco3")
        endif()

    elseif(target STREQUAL "esp32s2")
        rom_linker_script("spiflash_legacy")
    endif()

    if(CONFIG_ESP_ROM_HAS_NEWLIB)
        if(target STREQUAL "esp32" OR target STREQUAL "esp32s2")
            rom_linker_script("libc-funcs")
        else()
            if(CONFIG_ESP32P4_REV_MIN_300) # TODO: IDF-13410
                rom_linker_script("eco5.libc")
            elseif(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
                rom_linker_script("beta5.libc")
            else()
                rom_linker_script("libc")
            endif()
            if(CONFIG_ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY)
                # For bootloader use only ROM functions due to 'relocation truncated to fit' error
                rom_linker_script("libc-suboptimal_for_misaligned_mem")
            endif()
            if(CONFIG_LIBC_NEWLIB)
                if(CONFIG_ESP32P4_REV_MIN_300) # TODO: IDF-13410
                    rom_linker_script("eco5.newlib")
                elseif(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
                    rom_linker_script("beta5.newlib")
                else()
                    rom_linker_script("newlib")
                endif()
            endif()
        endif()
    endif()

    if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER)
        rom_linker_script("mbedtls")
        # For ESP32C2(ECO4), mbedTLS in ROM has been updated to v3.6.0-LTS
        if(CONFIG_ESP32C2_REV_MIN_FULL GREATER_EQUAL 200)
            rom_linker_script("mbedtls.eco4")
        endif()
    endif()

else() # Regular app build
    if(target STREQUAL "esp32")
        if(CONFIG_LIBC_NEWLIB)
            rom_linker_script("newlib-data")
            rom_linker_script("syscalls")
        endif()

        if(NOT CONFIG_SPIRAM_CACHE_WORKAROUND)
            # ESP32 only: these ROM functions may only be used if PSRAM cache workaround is disabled.
            # Otherwise we need to link to a multilib version of libc compiled with PSRAM workaround.
            rom_linker_script("libc-funcs")
            if(CONFIG_LIBC_NEWLIB)
                rom_linker_script("newlib-reent-funcs")
            endif()
        endif()

        if(CONFIG_ESP32_REV_MIN_FULL GREATER_EQUAL 300)
            rom_linker_script("eco3")
        endif()

    elseif(target STREQUAL "esp32s2")
        rom_linker_script("libc-funcs")
        if(CONFIG_LIBC_NEWLIB)
            rom_linker_script("newlib-reent-funcs")
            rom_linker_script("newlib-data")
        endif()

        # For ESP32S2, inclusion of ROM driver do not depend on CONFIG_SPI_FLASH_ROM_IMPL
        rom_linker_script("spiflash_legacy")

    elseif(target STREQUAL "esp32c2")
        if(CONFIG_ESP32C2_REV_MIN_FULL GREATER_EQUAL 200)
            rom_linker_script("eco4")
        endif()

        if(NOT CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY)
            if(CONFIG_ESP32C2_REV_MIN_FULL GREATER_EQUAL 200)
                rom_linker_script("ble-eco4")
            else()
                rom_linker_script("ble")
            endif()
        endif()

    elseif(target STREQUAL "esp32c3")
        if(NOT CONFIG_BT_CTRL_HCI_MODE_UART_H4)
            if(NOT CONFIG_BT_CTRL_BLE_MASTER)
                rom_linker_script("ble_master")
            endif()
            if(NOT CONFIG_BT_CONTROLLER_ONLY)
                if(NOT CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT AND NOT CONFIG_BT_BLE_50_FEATURES_SUPPORTED)
                    rom_linker_script("ble_50")
                endif()
            endif()
            if(CONFIG_BT_BLE_CCA_MODE_NONE)
                rom_linker_script("ble_cca")
            endif()
            if(NOT CONFIG_BT_CTRL_BLE_SECURITY_ENABLE)
                rom_linker_script("ble_smp")
            endif()
            if(NOT CONFIG_BT_CTRL_DTM_ENABLE)
                rom_linker_script("ble_dtm")
            endif()
            if(NOT CONFIG_BT_CTRL_BLE_TEST)
                rom_linker_script("ble_test")
            endif()
            if(NOT CONFIG_BT_CTRL_BLE_SCAN)
                rom_linker_script("ble_scan")
            endif()
        endif()

        if(CONFIG_ESP32C3_REV_MIN_FULL GREATER_EQUAL 3)
            rom_linker_script("eco3")
            if(NOT CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY)
                rom_linker_script("eco3_bt_funcs")
            endif()
        endif()

        if(CONFIG_ESP32C3_REV_MIN_FULL GREATER_EQUAL 101)
            rom_linker_script("eco7")
            if(NOT CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY)
                rom_linker_script("eco7_bt_funcs")
            endif()
        endif()
    elseif(target STREQUAL "esp32s3")
        if(NOT CONFIG_BT_CTRL_HCI_MODE_UART_H4)
            if(NOT CONFIG_BT_CTRL_BLE_MASTER)
                rom_linker_script("ble_master")
            endif()
            if(NOT CONFIG_BT_CONTROLLER_ONLY)
                if(NOT CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT AND NOT CONFIG_BT_BLE_50_FEATURES_SUPPORTED)
                    rom_linker_script("ble_50")
                endif()
            endif()
            if(CONFIG_BT_BLE_CCA_MODE_NONE)
                rom_linker_script("ble_cca")
            endif()
            if(NOT CONFIG_BT_CTRL_BLE_SECURITY_ENABLE)
                rom_linker_script("ble_smp")
            endif()
            if(NOT CONFIG_BT_CTRL_DTM_ENABLE)
                rom_linker_script("ble_dtm")
            endif()
            if(NOT CONFIG_BT_CTRL_BLE_TEST)
                rom_linker_script("ble_test")
            endif()
            if(NOT CONFIG_BT_CTRL_BLE_SCAN)
                rom_linker_script("ble_scan")
            endif()
        endif()
    elseif(target STREQUAL "esp32c6")
        # esp32c6.rom.api.ld has been split to several lds by components.
        # esp32c6.rom.api.ld is still reserved to map the APIs
        rom_linker_script("phy")
        rom_linker_script("coexist")
        rom_linker_script("net80211")
        rom_linker_script("pp")
    elseif(target STREQUAL "esp32c5")
        # esp32c5.rom.api.ld has been split to several lds by components.
        # esp32c5.rom.api.ld is still reserved to map the APIs
        rom_linker_script("phy")
        rom_linker_script("coexist")
        rom_linker_script("net80211")
        rom_linker_script("pp")
        if(CONFIG_ESP32C5_REV_MIN_FULL GREATER_EQUAL 102)
            rom_linker_script("eco3")
        endif()
    elseif(target STREQUAL "esp32c61")
        # esp32c61.rom.api.ld has been split to several lds by components.
        # esp32c61.rom.api.ld is still reserved to map the APIs
        rom_linker_script("phy")
        rom_linker_script("coexist")
        rom_linker_script("net80211")
        rom_linker_script("pp")
        if(CONFIG_ESP32C61_REV_MIN_FULL GREATER_EQUAL 101)
            rom_linker_script("eco4")
        endif()
    endif()

    if(CONFIG_ESP_ROM_HAS_NEWLIB AND NOT target STREQUAL "esp32" AND NOT target STREQUAL "esp32s2")
        # ESP32 and S2 are a bit different, keep them as special cases in the target specific include section
        if(CONFIG_ESP32P4_REV_MIN_300) # TODO: IDF-13410
            rom_linker_script("eco5.libc")
        elseif(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
            rom_linker_script("beta5.libc")
        else()
            rom_linker_script("libc")
        endif()
        if(CONFIG_ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY AND NOT CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS)
            rom_linker_script("libc-suboptimal_for_misaligned_mem")
        endif()
        if(CONFIG_LIBC_NEWLIB)
            if(CONFIG_ESP32P4_REV_MIN_300) # TODO: IDF-13410
                rom_linker_script("eco5.newlib")
            elseif(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
                rom_linker_script("beta5.newlib")
            else()
                rom_linker_script("newlib")
            endif()
        endif()

        if(CONFIG_ESP_ROM_HAS_NEWLIB_NANO_FORMAT AND CONFIG_LIBC_NEWLIB AND CONFIG_LIBC_NEWLIB_NANO_FORMAT)
            if(NOT CONFIG_ESP_ROM_HAS_NEWLIB_32BIT_TIME AND NOT CONFIG_ESP_ROM_HAS_NEWLIB_NANO_PRINTF_FLOAT_BUG)
                # Newlib-nano functions contains time_t related functions
                # and cannot be used if they were compiled with 32 bit time_t
                if(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
                    rom_linker_script("beta5.newlib-nano")
                else()
                    rom_linker_script("newlib-nano")
                endif()
            endif()
        endif()

        if(CONFIG_ESP_ROM_HAS_NEWLIB_NORMAL_FORMAT AND CONFIG_LIBC_NEWLIB AND NOT CONFIG_LIBC_NEWLIB_NANO_FORMAT)
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

        if(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
            rom_linker_script("beta5.heap")
        else()
            rom_linker_script("heap")
        endif()
    endif()

    if(CONFIG_SPI_FLASH_ROM_IMPL)
        # Older targets do not have a separate ld file for spiflash
        if(NOT target STREQUAL "esp32c3" AND NOT target STREQUAL "esp32s3" AND NOT target STREQUAL "esp32c2")
            if(CONFIG_IDF_TARGET_ESP32H4 AND NOT CONFIG_ESP32H4_SELECTS_REV_MP) # TODO: ESP32H4 IDF-13835
                rom_linker_script("beta5.spiflash")
            else()
                rom_linker_script("spiflash")
            endif()
        endif()
    endif()

    if(CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL)
        rom_linker_script("mbedtls")
        # For ESP32C2(ECO4), mbedTLS in ROM has been updated to v3.6.0-LTS
        if(CONFIG_ESP32C2_REV_MIN_FULL GREATER_EQUAL 200)
            rom_linker_script("mbedtls.eco4")
        endif()
    endif()

    if(CONFIG_ESP_ROM_DELAY_US_PATCH AND CONFIG_SECURE_ENABLE_TEE AND
        (CONFIG_ESP32C5_REV_MIN_FULL LESS_EQUAL 100 OR CONFIG_ESP32C61_REV_MIN_FULL LESS_EQUAL 100))
        # Force the linker to include esp_rom_sys.c for ets_ops_set_rom_patches constructor
        target_link_libraries(${COMPONENT_LIB} PRIVATE "-u ets_ops_set_rom_patches")
    endif()

    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=longjmp")
    endif()
endif()

if(target STREQUAL "esp32s2")
    target_sources(${COMPONENT_LIB} PRIVATE "esp32s2/usb_patches.c")
endif()
