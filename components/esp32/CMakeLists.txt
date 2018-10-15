if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp32 is headers
    set(COMPONENT_ADD_INCLUDEDIRS include)
    set(COMPONENT_REQUIRES ${COMPONENTS})
    set(COMPONENT_SRCS )
    register_component(esp32)

    # as cmake won't attach linker args to a header-only library, attach
    # linker args directly to the bootloader.elf
    set(ESP32_BOOTLOADER_LINKER_SCRIPTS
        "${CMAKE_CURRENT_SOURCE_DIR}/ld/esp32.rom.ld"
        "${CMAKE_CURRENT_SOURCE_DIR}/ld/esp32.rom.spiram_incompatible_fns.ld"
        "${CMAKE_CURRENT_SOURCE_DIR}/ld/esp32.peripherals.ld"
        PARENT_SCOPE
        )

else()
    # Regular app build

    set(COMPONENT_SRCS "brownout.c"
                   "cache_err_int.c"
                   "cache_sram_mmu.c"
                   "clk.c"
                   "coexist.c"
                   "core_dump.c"
                   "cpu_start.c"
                   "crosscore_int.c"
                   "dbg_stubs.c"
                   "dport_access.c"
                   "dport_panic_highint_hdl.S"
                   "esp_err_to_name.c"
                   "esp_timer.c"
                   "esp_timer_esp32.c"
                   "esp_himem.c"
                   "ets_timer_legacy.c"
                   "event_default_handlers.c"
                   "event_loop.c"
                   "fast_crypto_ops.c"
                   "freertos_hooks.c"
                   "gdbstub.c"
                   "hw_random.c"
                   "int_wdt.c"
                   "intr_alloc.c"
                   "ipc.c"
                   "lib_printf.c"
                   "panic.c"
                   "phy_init.c"
                   "pm_esp32.c"
                   "pm_locks.c"
                   "pm_trace.c"
                   "reset_reason.c"
                   "restore.c"
                   "sleep_modes.c"
                   "spiram.c"
                   "spiram_psram.c"
                   "stack_check.c"
                   "system_api.c"
                   "task_wdt.c"
                   "wifi_init.c"
                   "wifi_os_adapter.c"
                   "hwcrypto/aes.c"
                   "hwcrypto/sha.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    set(COMPONENT_REQUIRES driver tcpip_adapter)
    # driver is a public requirement because esp_sleep.h uses gpio_num_t & touch_pad_t
    # tcpip_adapter is a public requirement because esp_event.h uses tcpip_adapter types
    set(COMPONENT_PRIV_REQUIRES
        app_trace bootloader_support ethernet log mbedtls nvs_flash
        pthread smartconfig_ack spi_flash vfs wpa_supplicant xtensa-debug-module)

    register_component()

    target_link_libraries(esp32 "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
    if(NOT CONFIG_NO_BLOBS)
        target_link_libraries(esp32 coexist core espnow mesh net80211 phy pp rtc smartconfig wpa2 wpa wps)
    endif()
    target_linker_script(esp32 "${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld")

    if(CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY)
        # This has to be linked before esp32.common.ld
        target_linker_script(esp32 "ld/esp32.extram.bss.ld")
    endif()

    target_linker_script(esp32 "ld/esp32.common.ld")

    target_linker_script(esp32
        "ld/esp32.rom.ld"
        "ld/esp32.peripherals.ld"
        "ld/esp32.rom.libgcc.ld"
        )

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        add_compile_options(-mfix-esp32-psram-cache-issue)
    else()
        target_linker_script(esp32 "ld/esp32.rom.spiram_incompatible_fns.ld")
    endif()

    if(CONFIG_NEWLIB_NANO_FORMAT)
        target_linker_script(esp32 "ld/esp32.rom.nanofmt.ld")
    endif()

    if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
        target_linker_script(esp32 "ld/esp32.rom.spiflash.ld")
    endif()

    target_link_libraries(esp32 "${CMAKE_CURRENT_SOURCE_DIR}/libhal.a")
    target_link_libraries(esp32 gcc)
    target_link_libraries(esp32 "-u call_user_start_cpu0")

    #ld_include_panic_highint_hdl is added as an undefined symbol because otherwise the
    #linker will ignore panic_highint_hdl.S as it has no other files depending on any
    #symbols in it.
    target_link_libraries(esp32 "-u ld_include_panic_highint_hdl")

    # Preprocess esp32.ld linker script to include configuration, becomes esp32_out.ld
    set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ld)
    add_custom_command(
        OUTPUT esp32_out.ld
        COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o esp32_out.ld -I ${CONFIG_DIR} ${LD_DIR}/esp32.ld
        MAIN_DEPENDENCY ${LD_DIR}/esp32.ld ${SDKCONFIG_H}
        COMMENT "Generating linker script..."
        VERBATIM)

    add_custom_target(esp32_linker_script DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld)
    add_dependencies(esp32 esp32_linker_script)

    if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION)
        set(PHY_INIT_DATA_BIN phy_init_data.bin)

        # To get the phy_init_data.bin file, compile phy_init_data.h as a C file and then objcopy
        # the object file to a raw binary
        add_custom_command(
            OUTPUT ${PHY_INIT_DATA_BIN}
            DEPENDS ${CMAKE_CURRENT_LIST_DIR}/phy_init_data.h
            COMMAND ${CMAKE_C_COMPILER} -x c -c
            -I ${CMAKE_CURRENT_LIST_DIR} -I ${CMAKE_CURRENT_LIST_DIR}/include -I ${CMAKE_BINARY_DIR}
            -o phy_init_data.obj
            ${CMAKE_CURRENT_LIST_DIR}/phy_init_data.h
            COMMAND ${CMAKE_OBJCOPY} -O binary phy_init_data.obj ${PHY_INIT_DATA_BIN}
            )
        add_custom_target(phy_init_data ALL DEPENDS ${PHY_INIT_DATA_BIN})
        add_dependencies(flash phy_init_data)

    endif()

endif()
