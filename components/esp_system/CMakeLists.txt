idf_build_get_property(target IDF_TARGET)

set(srcs "esp_err.c")

if(CONFIG_IDF_ENV_FPGA)
    list(APPEND srcs "fpga_overrides.c")
endif()

if(BOOTLOADER_BUILD)
        # "_esp_error_check_failed()" requires spi_flash module
        # Bootloader relies on some Kconfig options defined in esp_system.
        idf_component_register(SRCS "${srcs}" REQUIRES spi_flash)
else()
    list(APPEND srcs "crosscore_int.c"
            "esp_err.c"
            "freertos_hooks.c"
            "int_wdt.c"
            "panic.c"
            "esp_system.c"
            "startup.c"
            "system_time.c"
            "stack_check.c"
            "task_wdt.c"
            "ubsan.c"
            "xt_wdt.c"
            "debug_stubs.c")

    if(CONFIG_ESP_SYSTEM_USE_EH_FRAME)
        list(APPEND srcs "eh_frame_parser.c")
    endif()

    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS include
                        PRIV_REQUIRES spi_flash
                                    # [refactor-todo] requirements due to init code,
                                    # should be removable once using component init functions
                                    # link-time registration is used.
                                    esp_pm app_update nvs_flash pthread app_trace esp_gdbstub
                                    espcoredump esp_phy efuse esp_ipc
                        LDFRAGMENTS "linker.lf" "app.lf")
    add_subdirectory(port)

    # After system initialization, `start_app` (and its other cores variant) is called.
    # This is provided by the user or from another component. Since we can't establish
    # dependency on what we don't know, force linker to not drop the symbol regardless
    # of link line order.
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u start_app")

    if(NOT CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-u start_app_other_cores")
    endif()

    # Disable stack protection in files which are involved in initialization of that feature
    set_source_files_properties(
        "startup.c" "stack_check.c"
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)

    include(${CMAKE_CURRENT_LIST_DIR}/ld/ld.cmake)
endif()

if(CONFIG_IDF_ENV_FPGA)
    # Forces the linker to include fpga stubs from this component
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_common_include_fpga_overrides")
endif()

# Force linking UBSAN hooks. If UBSAN is not enabled, the hooks will ultimately be removed
# due to -ffunction-sections -Wl,--gc-sections options.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __ubsan_include")
