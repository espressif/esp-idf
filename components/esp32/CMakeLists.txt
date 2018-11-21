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
                   "cpu_start.c"
                   "crosscore_int.c"
                   "dport_access.c"
                   "dport_panic_highint_hdl.S"
                   "esp_adapter.c"
                   "esp_timer_esp32.c"
                   "esp_himem.c"
                   "gdbstub.c"
                   "hw_random.c"
                   "int_wdt.c"
                   "intr_alloc.c"
                   "panic.c"
                   "pm_esp32.c"
                   "pm_trace.c"
                   "reset_reason.c"
                   "sleep_modes.c"
                   "spiram.c"
                   "spiram_psram.c"
                   "system_api.c"
                   "task_wdt.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    set(COMPONENT_REQUIRES driver esp_event efuse)
    # driver is a public requirement because esp_sleep.h uses gpio_num_t & touch_pad_t
    # app_update is added here because cpu_start.c uses esp_ota_get_app_description() function.
    set(COMPONENT_PRIV_REQUIRES
        app_trace app_update bootloader_support log mbedtls nvs_flash
        pthread smartconfig_ack spi_flash vfs wpa_supplicant espcoredump esp_common esp_wifi)

    set(COMPONENT_ADD_LDFRAGMENTS linker.lf ld/esp32_fragments.lf)

    register_component()

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

    target_linker_script(${COMPONENT_TARGET} "ld/esp32.peripherals.ld")

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

    # Enable dynamic esp_timer overflow value if building unit tests
    if(esp32_test IN_LIST BUILD_TEST_COMPONENTS)
        add_definitions(-DESP_TIMER_DYNAMIC_OVERFLOW_VAL)
    endif()

    # disable stack protection in files which are involved in initialization of that feature
    set_source_files_properties(
        cpu_start.c
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)
endif()
