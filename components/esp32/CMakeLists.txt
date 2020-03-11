require_idf_targets(esp32)

if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp32 is headers
    set(COMPONENT_ADD_INCLUDEDIRS include)
    set(COMPONENT_REQUIRES ${IDF_COMPONENTS})
    set(COMPONENT_SRCS )
    register_component()
else()
    # Regular app build

    set(COMPONENT_SRCS "brownout.c"
                   "cache_err_int.c"
                   "cache_sram_mmu.c"
                   "clk.c"
                   "coexist.c"
                   "cpu_start.c"
                   "crosscore_int.c"
                   "dbg_stubs.c"
                   "dport_access.c"
                   "dport_panic_highint_hdl.S"
                   "esp_adapter.c"
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
                   "hwcrypto/aes.c"
                   "hwcrypto/sha.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    if(NOT CONFIG_FREERTOS_UNICORE)
        list(APPEND COMPONENT_SRCS "ipc.c")
    endif()

    set(COMPONENT_REQUIRES driver tcpip_adapter esp_event efuse)
    # driver is a public requirement because esp_sleep.h uses gpio_num_t & touch_pad_t
    # tcpip_adapter is a public requirement because esp_event.h uses tcpip_adapter types
    # app_update is added here because cpu_start.c uses esp_ota_get_app_description() function.
    set(COMPONENT_PRIV_REQUIRES
        app_trace app_update bootloader_support ethernet log mbedtls nvs_flash
        pthread smartconfig_ack spi_flash vfs wpa_supplicant xtensa-debug-module espcoredump)

    set(COMPONENT_ADD_LDFRAGMENTS linker.lf ld/esp32_fragments.lf)

    register_component()

    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
    if(NOT CONFIG_NO_BLOBS)
        target_link_libraries(${COMPONENT_TARGET} coexist core espnow mesh net80211 phy pp rtc smartconfig wpa2 wpa wps)
    endif()
    target_linker_script(${COMPONENT_TARGET} "${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld")

    if(CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY)
        # This has to be linked before esp32.project.ld
        target_linker_script(${COMPONENT_TARGET} "ld/esp32.extram.bss.ld")
    endif()

    if(IDF_PROJECT_EXECUTABLE)
        # Process the template file through the linker script generation mechanism, and use the output for linking the
        # final binary
        set(esp32_project_script "${CMAKE_CURRENT_BINARY_DIR}/esp32.project.ld")
        set(esp32_project_template "${CMAKE_CURRENT_LIST_DIR}/ld/esp32.project.ld.in")

        ldgen_process_template(${esp32_project_template} ${esp32_project_script})

        target_link_libraries(${COMPONENT_TARGET} "-T ${esp32_project_script}")

        set_property(TARGET ${IDF_PROJECT_EXECUTABLE} APPEND PROPERTY LINK_DEPENDS ${esp32_project_script})
    endif()

    target_linker_script(${COMPONENT_TARGET}
        "ld/esp32.rom.ld"
        "ld/esp32.peripherals.ld"
        "ld/esp32.rom.libgcc.ld"
        )

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        # Note: Adding as a PUBLIC compile option here causes this option to propagate to all components that depend on esp32.
        #
        # To handle some corner cases, the same flag is set in project_include.cmake
        target_compile_options(${COMPONENT_TARGET} PUBLIC -mfix-esp32-psram-cache-issue)
    else()
        target_linker_script(${COMPONENT_TARGET} "ld/esp32.rom.spiram_incompatible_fns.ld")
    endif()

    if(CONFIG_NEWLIB_NANO_FORMAT)
        target_linker_script(${COMPONENT_TARGET} "ld/esp32.rom.nanofmt.ld")
    endif()

    if(NOT CONFIG_SPI_FLASH_ROM_DRIVER_PATCH)
        target_linker_script(${COMPONENT_TARGET} "ld/esp32.rom.spiflash.ld")
    endif()

    target_link_libraries(${COMPONENT_TARGET} "${CMAKE_CURRENT_SOURCE_DIR}/libhal.a")
    target_link_libraries(${COMPONENT_TARGET} gcc)
    target_link_libraries(${COMPONENT_TARGET} "-u call_user_start_cpu0")

    #ld_include_panic_highint_hdl is added as an undefined symbol because otherwise the
    #linker will ignore panic_highint_hdl.S as it has no other files depending on any
    #symbols in it.
    target_link_libraries(${COMPONENT_TARGET} "-u ld_include_panic_highint_hdl")

    # Preprocess esp32.ld linker script to include configuration, becomes esp32_out.ld
    set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ld)
    add_custom_command(
        OUTPUT esp32_out.ld
        COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o esp32_out.ld -I ${CONFIG_DIR} ${LD_DIR}/esp32.ld
        MAIN_DEPENDENCY ${LD_DIR}/esp32.ld ${SDKCONFIG_H}
        COMMENT "Generating linker script..."
        VERBATIM)

    add_custom_target(esp32_linker_script DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld)
    add_dependencies(${COMPONENT_TARGET} esp32_linker_script)

    if(CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION AND IDF_BUILD_ARTIFACTS)
        set(PHY_INIT_DATA_BIN ${IDF_BUILD_ARTIFACTS_DIR}/${PHY_PARTITION_BIN_FILE})

        # To get the phy_init_data.bin file, compile phy_init_data.h as a C file and then objcopy
        # the object file to a raw binary
        add_custom_command(
            OUTPUT ${PHY_INIT_DATA_BIN}
            DEPENDS ${CMAKE_CURRENT_LIST_DIR}/phy_init_data.h
            COMMAND ${CMAKE_C_COMPILER} -x c -c
            -I ${CMAKE_CURRENT_LIST_DIR} -I ${CMAKE_CURRENT_LIST_DIR}/include -I ${IDF_BUILD_ARTIFACTS_DIR}
            -I ${CONFIG_DIR} -o phy_init_data.obj
            ${CMAKE_CURRENT_LIST_DIR}/phy_init_data.h
            COMMAND ${CMAKE_OBJCOPY} -O binary phy_init_data.obj ${PHY_INIT_DATA_BIN}
            )
        add_custom_target(phy_init_data ALL DEPENDS ${PHY_INIT_DATA_BIN})
        add_dependencies(flash phy_init_data)
    endif()

    # Enable dynamic esp_timer overflow value if building unit tests
    if(esp32_test IN_LIST BUILD_TEST_COMPONENTS)
        add_definitions(-DESP_TIMER_DYNAMIC_OVERFLOW_VAL)
    endif()

    # disable stack protection in files which are involved in initialization of that feature
    set_source_files_properties(
        stack_check.c cpu_start.c
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)
endif()
