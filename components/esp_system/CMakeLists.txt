idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

# On Linux, we only support a few features, hence this simple component registration
if(${target} STREQUAL "linux")
    idf_component_register(SRCS "esp_system.c"
                        "esp_err.c"
                        "esp_sys_event.c"
                        "port/soc/linux/reset_reason.c"
                        "port/soc/linux/system_internal.c"
                        "port/esp_system_linux.c"
                        "freertos_hooks.c"
                        "startup.c"
                        INCLUDE_DIRS "include"
                        PRIV_REQUIRES spi_flash)

    # Add custom ld file (used to place system init functions).
    # Mach-O (Apple) does not support linker scripts; section bounds are resolved
    # at runtime via getsectiondata() so no -T flag is needed there.
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        target_link_options(${COMPONENT_TARGET} INTERFACE "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/ld/linux/sections.ld")
    endif()

    return()
endif()

set(srcs "esp_err.c")

if(CONFIG_IDF_ENV_FPGA OR CONFIG_ESP_BRINGUP_BYPASS_CPU_CLK_SETTING)
    list(APPEND srcs "fpga_overrides_clk.c")
endif()

if(CONFIG_IDF_ENV_FPGA OR CONFIG_ESP_BRINGUP_BYPASS_RANDOM_SETTING)
    list(APPEND srcs "fpga_overrides_rng.c")
endif()

if(BOOTLOADER_BUILD OR esp_tee_build)
    set(include_dirs "include")
    if(esp_tee_build)
        list(APPEND include_dirs "port/include/private")
    endif()
    # "_esp_error_check_failed()" requires spi_flash module
    # Bootloader relies on some Kconfig options defined in esp_system.
    idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}"
                           REQUIRES spi_flash)
else()
    list(APPEND srcs "crosscore_int.c"
            "esp_ipc.c"
            "esp_err.c"
            "esp_sys_event.c"
            "freertos_hooks.c"
            "panic.c"
            "esp_system.c"
            "startup.c"
            "startup_funcs.c"
            "system_time.c"
            "stack_check.c"
            "ubsan.c")

    if(CONFIG_COMPILER_KASAN)
        list(APPEND srcs "kasan.c")
    endif()
    if(CONFIG_SOC_WDT_SUPPORTED)
        list(APPEND srcs "int_wdt.c")
    endif()
    if(CONFIG_SOC_XT_WDT_SUPPORTED)
        list(APPEND srcs "xt_wdt.c")
    endif()

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

    if(CONFIG_ESP_SYSTEM_USE_FRAME_POINTER)
        list(APPEND srcs "fp_unwind.c")
    endif()

    if(CONFIG_SOC_SYSTIMER_SUPPORT_ETM)
        list(APPEND srcs "systick_etm.c")
    endif()

    if(CONFIG_ESP_SYSTEM_HW_STACK_GUARD OR CONFIG_ESP_SYSTEM_HW_PC_RECORD OR CONFIG_SOC_CPU_LOCKUP_DEBUG_SUPPORTED)
        list(APPEND srcs "debug_assist.c")
    endif()

    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS include
                        PRIV_REQUIRES spi_flash esp_timer esp_mm
                                      esp_hal_mspi esp_hal_wdt esp_hal_uart esp_hal_clock esp_hal_dma
                                    # [refactor-todo] requirements due to init code,
                                    # should be removable once using component init functions
                                    # link-time registration is used.
                                    bootloader_support esp_pm esp_usb_cdc_rom_console
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

    if(CONFIG_COMPILER_KASAN)
        # Files involved in stack-protector initialisation: disable stack protector.
        # Also exclude from KASAN: cpu_start.c runs before kasan_init_shadow() and
        # panic.c / startup.c must not recurse into KASAN during crash handling.
        set_source_files_properties(
            "startup.c" "stack_check.c" "port/cpu_start.c"
            PROPERTIES COMPILE_FLAGS
            "-fno-stack-protector -fno-sanitize=kernel-address")

        # kasan.c and ubsan.c implement sanitizer runtime stubs – must never be
        # instrumented themselves (infinite recursion).
        # panic.c / port/panic_handler.c must not be instrumented to avoid
        # recursion in the error path.
        set_source_files_properties(
            "kasan.c" "ubsan.c" "panic.c" "port/panic_handler.c"
            PROPERTIES COMPILE_FLAGS
            "-fno-sanitize=kernel-address")
    else()
        # Disable stack protection in files which are involved in initialization of that feature
        set_source_files_properties(
            "startup.c" "stack_check.c" "port/cpu_start.c"
            PROPERTIES COMPILE_FLAGS
            -fno-stack-protector)
    endif()

    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/${target}/memory.ld.in")

    # Generate sections.ld.in and pass it through linker script generator
    target_linker_script(${COMPONENT_LIB} INTERFACE "ld/${target}/sections.ld.in"
                        PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ld/sections.ld")
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

# Force-link the __asan_*_noabort stubs: GCC-instrumented code calls them but
# the linker cannot see the call sites at GC time, so without explicit -u flags
# they would be silently dropped (and any -u flag against the file is enough
# to pull kasan.c in as well).
if(CONFIG_COMPILER_KASAN)
    foreach(__kasan_stub
            __asan_load1_noabort __asan_load2_noabort
            __asan_load4_noabort __asan_load8_noabort
            __asan_load16_noabort __asan_loadN_noabort
            __asan_store1_noabort __asan_store2_noabort
            __asan_store4_noabort __asan_store8_noabort
            __asan_store16_noabort __asan_storeN_noabort
            __asan_handle_no_return)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ${__kasan_stub}")
    endforeach()
endif()

target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_system_include_startup_funcs")

# [refactor-todo] requirements due to init code, should be removable
# once link-time registration of component init functions is used.
if(NOT CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT)
    idf_component_optional_requires(PRIVATE esp_app_format)
endif()

if(CONFIG_PM_ENABLE)
    idf_component_optional_requires(PRIVATE pm)
endif()

if(NOT BOOTLOADER_BUILD)
    if(CONFIG_SPIRAM)
        idf_component_optional_requires(PRIVATE esp_psram)
    endif()
endif()

# For P4, since P4 REV2, the SRAM is contiguous
if(CONFIG_ESP32P4_SELECTS_REV_LESS_V3)
    target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,--enable-non-contiguous-regions")
endif()

if(CONFIG_ESP_DEBUG_INCLUDE_OCD_STUB_BINS)
    add_subdirectory(openocd_stub_bins)
endif()
