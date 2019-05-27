if(BOOTLOADER_BUILD AND CONFIG_IDF_TARGET_ESP32S2BETA)
    # For bootloader, all we need from esp32s2beta is headers
    set(COMPONENT_ADD_INCLUDEDIRS include)
    set(COMPONENT_REQUIRES ${IDF_COMPONENTS} soc) #unfortunately rom/uart uses SOC registers directly
    set(COMPONENT_SRCS )
    register_component()
    idf_build_get_property(target IDF_TARGET)
    set(scripts "ld/esp32s2beta.peripherals.ld")
    target_linker_script(${COMPONENT_LIB} "${scripts}")
elseif(CONFIG_IDF_TARGET_ESP32S2BETA)
    # Regular app build

    set(COMPONENT_SRCS "brownout.c"
                   "cache_err_int.c"
                   "clk.c"
                   "cpu_start.c"
                   "crosscore_int.c"
                   "dport_access.c"
                   "dport_panic_highint_hdl.S"
                   "esp_timer_esp32s2beta.c"
                   "gdbstub.c"
                   "hw_random.c"
                   "int_wdt.c"
                   "intr_alloc.c"
                   "panic.c"
                   "pm_esp32s2beta.c"
                   "pm_trace.c"
                   "sleep_modes.c"
                   "spiram.c"
                   "spiram_psram.c"
                   "system_api.c"
                   "task_wdt.c")
    set(COMPONENT_ADD_INCLUDEDIRS "include")

    set(COMPONENT_REQUIRES driver esp_event efuse soc) #unfortunately rom/uart uses SOC registers directly

    # driver is a public requirement because esp_sleep.h uses gpio_num_t & touch_pad_t
    # app_update is added here because cpu_start.c uses esp_ota_get_app_description() function.
    set(COMPONENT_PRIV_REQUIRES
        app_trace app_update bootloader_support log mbedtls nvs_flash
        pthread smartconfig_ack spi_flash vfs wpa_supplicant espcoredump esp_common esp_wifi)

    set(COMPONENT_ADD_LDFRAGMENTS linker.lf ld/esp32s2beta_fragments.lf)

    register_component()

    target_linker_script(${COMPONENT_LIB} "${CMAKE_CURRENT_BINARY_DIR}/esp32s2beta_out.ld")

    # Process the template file through the linker script generation mechanism, and use the output for linking the
    # final binary
    target_linker_script(${COMPONENT_LIB} "${CMAKE_CURRENT_LIST_DIR}/ld/esp32s2beta.project.ld.in" PROCESS)

    target_linker_script(${COMPONENT_LIB} "ld/esp32s2beta.peripherals.ld")

    target_link_libraries(${COMPONENT_LIB} gcc)
    target_link_libraries(${COMPONENT_LIB} "-u call_user_start_cpu0")

    #ld_include_panic_highint_hdl is added as an undefined symbol because otherwise the
    #linker will ignore panic_highint_hdl.S as it has no other files depending on any
    #symbols in it.
    target_link_libraries(${COMPONENT_LIB} "-u ld_include_panic_highint_hdl")

    idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)
    get_filename_component(config_dir ${sdkconfig_header} DIRECTORY)
    # Preprocess esp32s2beta.ld linker script to include configuration, becomes esp32s2beta_out.ld
    set(LD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ld)
    add_custom_command(
        OUTPUT esp32s2beta_out.ld
        COMMAND "${CMAKE_C_COMPILER}" -C -P -x c -E -o esp32s2beta_out.ld -I ${config_dir} ${LD_DIR}/esp32s2beta.ld
        MAIN_DEPENDENCY ${LD_DIR}/esp32s2beta.ld ${sdkconfig_header}
        COMMENT "Generating linker script..."
        VERBATIM)

    add_custom_target(esp32s2beta_linker_script DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/esp32s2beta_out.ld)
    add_dependencies(${COMPONENT_LIB} esp32s2beta_linker_script)

    # disable stack protection in files which are involved in initialization of that feature
    set_source_files_properties(
        cpu_start.c
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)
else()
    register_config_only_component()
endif()
