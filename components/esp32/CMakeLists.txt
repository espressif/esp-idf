idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)
if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp32 is headers
    idf_component_register(INCLUDE_DIRS include)
    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32.peripherals.ld")
else()
    # Regular app build
    set(srcs 
        "brownout.c"
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
    set(include_dirs "include")

    set(requires driver esp_event efuse soc) #unfortunately rom/uart uses SOC registers directly

    # driver is a public requirement because esp_sleep.h uses gpio_num_t & touch_pad_t
    # app_update is added here because cpu_start.c uses esp_ota_get_app_description() function.
    set(priv_requires app_trace app_update bootloader_support log mbedtls nvs_flash pthread
        spi_flash vfs wpa_supplicant espcoredump esp_common esp_wifi)
    set(fragments linker.lf ld/esp32_fragments.lf)

    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS "${include_dirs}"
                        LDFRAGMENTS "${fragments}"
                        REQUIRES "${requires}"
                        PRIV_REQUIRES "${priv_requires}"
                        REQUIRED_IDF_TARGETS esp32)

    target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld")

    # Rely on user code to define app_main
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u app_main")

    if(CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY)
        # This has to be linked before esp32.project.ld
        target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32.extram.bss.ld")
    endif()

    # Process the template file through the linker script generation mechanism, and use the output for linking the
    # final binary
    target_linker_script(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/ld/esp32.project.ld.in" 
                        PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ld/esp32.project.ld")

    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/esp32.peripherals.ld")
    target_link_libraries(${COMPONENT_LIB} PUBLIC gcc)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u call_user_start_cpu0")

    #ld_include_panic_highint_hdl is added as an undefined symbol because otherwise the
    #linker will ignore panic_highint_hdl.S as it has no other files depending on any
    #symbols in it.
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ld_include_panic_highint_hdl")

    idf_build_get_property(config_dir CONFIG_DIR)
    # Preprocess esp32.ld linker script to include configuration, becomes esp32_out.ld
    set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ld)
    add_custom_command(
        OUTPUT esp32_out.ld
        COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o esp32_out.ld -I ${config_dir} ${LD_DIR}/esp32.ld
        MAIN_DEPENDENCY ${LD_DIR}/esp32.ld ${sdkconfig_header}
        COMMENT "Generating linker script..."
        VERBATIM)

    add_custom_target(esp32_linker_script DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/esp32_out.ld)
    add_dependencies(${COMPONENT_LIB} esp32_linker_script)

    # disable stack protection in files which are involved in initialization of that feature
    set_source_files_properties(
        cpu_start.c
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)

    if(CONFIG_SPIRAM_CACHE_WORKAROUND)
        # Note: Adding as a PUBLIC compile option here causes this option to propagate to all components that depend on esp32.
        #
        # To handle some corner cases, the same flag is set in project_include.cmake
        target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-issue)
        # also, make sure we link with this option so correct toolchain libs are pulled in
        target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-issue)
    endif()
endif()
