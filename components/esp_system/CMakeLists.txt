idf_build_get_property(target IDF_TARGET)

# On Linux, we only support a few features, hence this simple component registration
if(${target} STREQUAL "linux")
    idf_component_register(SRCS "esp_system.c"
                        "esp_err.c"
                        "port/soc/linux/reset_reason.c"
                        "port/soc/linux/system_internal.c"
                        "port/esp_system_linux.c"
                        INCLUDE_DIRS "include"
                        PRIV_REQUIRES spi_flash)
    return()
endif()

set(srcs "esp_err.c")

if(CONFIG_IDF_ENV_FPGA OR CONFIG_ESP_BRINGUP_BYPASS_CPU_CLK_SETTING)
    list(APPEND srcs "fpga_overrides_clk.c")
endif()

if(CONFIG_IDF_ENV_FPGA OR CONFIG_ESP_BRINGUP_BYPASS_RANDOM_SETTING)
    list(APPEND srcs "fpga_overrides_rng.c")
endif()

if(BOOTLOADER_BUILD)
        # "_esp_error_check_failed()" requires spi_flash module
        # Bootloader relies on some Kconfig options defined in esp_system.
        idf_component_register(SRCS "${srcs}" REQUIRES spi_flash)
else()
    list(APPEND srcs "crosscore_int.c"
            "esp_ipc.c"
            "esp_err.c"
            "freertos_hooks.c"
            "int_wdt.c"
            "panic.c"
            "esp_system.c"
            "startup.c"
            "startup_funcs.c"
            "system_time.c"
            "stack_check.c"
            "ubsan.c"
            "xt_wdt.c")

    if(CONFIG_ESP_TASK_WDT_EN)
        list(APPEND srcs "task_wdt/task_wdt.c")

        if(CONFIG_ESP_TASK_WDT_USE_ESP_TIMER)
            list(APPEND srcs "task_wdt/task_wdt_impl_esp_timer.c")
        else()
            list(APPEND srcs "task_wdt/task_wdt_impl_timergroup.c")
        endif()
    endif()

    if(CONFIG_ESP_SYSTEM_USE_EH_FRAME)
        list(APPEND srcs "eh_frame_parser.c")
    endif()

    if(CONFIG_SOC_SYSTIMER_SUPPORT_ETM)
        list(APPEND srcs "systick_etm.c")
    endif()

    if(CONFIG_ESP_SYSTEM_HW_STACK_GUARD)
        list(APPEND srcs "hw_stack_guard.c")
    endif()

    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS include
                        PRIV_REQUIRES spi_flash esp_timer esp_mm
                                    # [refactor-todo] requirements due to init code,
                                    # should be removable once using component init functions
                                    # link-time registration is used.
                                    bootloader_support esp_pm
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
        "startup.c" "stack_check.c" "port/cpu_start.c"
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)

    include(${CMAKE_CURRENT_LIST_DIR}/ld/ld.cmake)
endif()

if(CONFIG_IDF_ENV_FPGA OR CONFIG_ESP_BRINGUP_BYPASS_CPU_CLK_SETTING)
    # Forces the linker to include fpga stubs from this component
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_common_include_fpga_overrides_clk")
endif()

if(CONFIG_IDF_ENV_FPGA OR CONFIG_ESP_BRINGUP_BYPASS_RANDOM_SETTING)
    # Forces the linker to include fpga stubs from this component
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_common_include_fpga_overrides_rng")
endif()

# Force linking UBSAN hooks. If UBSAN is not enabled, the hooks will ultimately be removed
# due to -ffunction-sections -Wl,--gc-sections options.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __ubsan_include")

target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_system_include_startup_funcs")

# [refactor-todo] requirements due to init code, should be removable
# once link-time registration of component init functions is used.
if(CONFIG_APPTRACE_ENABLE)
    idf_component_optional_requires(PRIVATE app_trace)
endif()

if(CONFIG_ESP_COREDUMP_ENABLE)
    idf_component_optional_requires(PRIVATE espcoredump)
endif()

# [refactor-todo] requirement from the panic handler,
# need to introduce panic "event" concept to remove this dependency (IDF-2194)
idf_component_optional_requires(PRIVATE esp_gdbstub)

if(NOT CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT)
    idf_component_optional_requires(PRIVATE esp_app_format)
endif()

if(CONFIG_PM_ENABLE)
    idf_component_optional_requires(PRIVATE pm)
endif()

if(CONFIG_VFS_SUPPORT_IO)
    idf_component_optional_requires(PRIVATE vfs)
endif()

if(CONFIG_SW_COEXIST_ENABLE OR CONFIG_EXTERNAL_COEX_ENABLE)
    idf_component_optional_requires(PRIVATE esp_coex)
endif()

if(NOT BOOTLOADER_BUILD)
    if(CONFIG_SPIRAM)
        idf_component_optional_requires(PRIVATE esp_psram)
    endif()
endif()

if(CONFIG_SOC_MEM_NON_CONTIGUOUS_SRAM)
    target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,--enable-non-contiguous-regions")
endif()

if(CONFIG_ESP_DEBUG_INCLUDE_OCD_STUB_BINS)
    add_subdirectory(openocd_stub_bins)
endif()
