idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

set(public_include_dirs "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/port/${target}/include")
    list(APPEND public_include_dirs "port/${target}/include")
endif()

# Headers are still needed by esp_hw_support (e.g. rtc_clk.c includes esp_sleep.h) in the bootloader,
# ULP and Linux builds, but none of the sleep implementation is built there.
if(${target} STREQUAL "linux" OR non_os_build OR __ULP_BUILDV2)
    idf_component_register(INCLUDE_DIRS ${public_include_dirs})
    return()
endif()

set(srcs)
set(priv_requires esp_driver_gpio      # for GPIO and RTC (by sleep_gpio and sleep_modes)
                  esp_timer
                  esp_hal_touch_sens   # for touch sensor wakeup (introduced in sleep_modes.c)
                  esp_mm
                  esp_mspi
                  esp_hal_mspi
                  esp_hal_uart
                  esp_hal_gpio
                  esp_hal_usb
                  esp_hal_pmu
                  esp_hal_regi2c
                  esp_hal_cache
                  esp_hal_clock
                  esp_hal_rtc_timer
                  esp_hal_wdt
                  esp_hal_systimer
                  esp_hal_security     # for APM (pmu_sleep.c)
                  esp_hal_ana_conv     # sleep process requires backup/restore some ADC, TSENS registers
                  )

if(CONFIG_SOC_PAU_SUPPORTED)
    list(APPEND srcs "sleep_retention.c"
                     "port/pau_regdma.c"
                     "port/regdma_link.c")
endif()

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    list(APPEND srcs "sleep_modem.c"
                     "sleep_cache.c"
                     "sleep_modes.c"
                     "sleep_uart.c"
                     "sleep_console.c"
                     "sleep_mspi.c"
                     "sleep_usb.c"
                     "sleep_gpio.c"
                     "sleep_event.c"
        )
    if(CONFIG_SOC_PAU_SUPPORTED AND CONFIG_SOC_PM_SUPPORT_TOP_PD)
        list(APPEND srcs "sleep_system_peripheral.c"
                         "port/${target}/peripheral_domain_pd.c"
                         "port/${target}/system_periph_retention.c")
    endif()

    if(CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP OR
        (CONFIG_ESP32P4_SELECTS_REV_LESS_V3 AND CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP))
        list(APPEND srcs "port/${target}/sleep_cpu.c")
        if(CONFIG_PM_CPU_RETENTION_DYNAMIC)
            list(APPEND srcs "port/${target}/sleep_cpu_dynamic.c")
        elseif(CONFIG_PM_CPU_RETENTION_STATIC)
            list(APPEND srcs "port/${target}/sleep_cpu_static.c")
        endif()
        if(CONFIG_SOC_PM_CPU_RETENTION_BY_SW)
            list(APPEND srcs "port/${target}/sleep_cpu_asm.S")
            if(CONFIG_SOC_CPU_HAS_FPU AND CONFIG_SOC_PM_FPU_RETENTION_BY_SW)
                list(APPEND srcs "port/${target}/sleep_fpu_asm.S")
            endif()
        endif()
    endif()

    if(CONFIG_P4_REV3_MSPI_CRASH_AFTER_POWER_UP_WORKAROUND)
        list(APPEND srcs "port/esp32p4/p4_rev3_mspi_workaround.S")
    endif()

    if((CONFIG_SOC_PM_SUPPORT_MODEM_PD OR CONFIG_SOC_PM_SUPPORT_TOP_PD) AND CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "port/${target}/sleep_clock.c")
    endif()

    if(CONFIG_SOC_PM_SUPPORT_PMU_CLK_ICG)
        list(APPEND srcs "port/${target}/sleep_clock_icg.c")
    endif()

    if(CONFIG_SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY)
        list(APPEND srcs "port/${target}/sleep_phy.c")
    endif()
endif()

if(CONFIG_SOC_PMU_SUPPORTED)
    list(APPEND srcs "port/${target}/pmu_sleep.c")
    if(CONFIG_IDF_TARGET_ESP32P4 AND CONFIG_PM_SLEEP_CLK_ICG_ENABLE
       AND NOT CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)
        list(APPEND srcs "port/esp32p4/pmu_sleep_clock_icg.c")
    endif()
    if(CONFIG_IDF_TARGET_ESP32H4 AND CONFIG_PM_MODEM_STATE_ENABLED)
        list(APPEND srcs "port/esp32h4/pmu_sleep_power.c")
    endif()
endif()

if(CONFIG_SOC_RTC_FAST_MEM_SUPPORTED AND CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB)
    list(APPEND srcs "sleep_wake_stub.c")
endif()

# Sleep code uses esp_hw_support internals (e.g. esp_hw_log.h) which are not exported as public headers
idf_component_get_property(hw_support_dir esp_hw_support COMPONENT_DIR)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include_dirs}
                       PRIV_INCLUDE_DIRS "${hw_support_dir}/port/include"
                       REQUIRES esp_hw_support
                       PRIV_REQUIRES ${priv_requires}
                       LDFRAGMENTS linker.lf)

if(CONFIG_SPIRAM)
    idf_component_optional_requires(PRIVATE esp_psram)
endif()

if(CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND OR CONFIG_PM_SLP_DISABLE_GPIO)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_sleep_gpio_include")
endif()

if(CONFIG_SOC_PAU_SUPPORTED AND CONFIG_SOC_PM_SUPPORT_TOP_PD)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u sleep_system_peripheral_dummy")
endif()

if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    if(CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP OR
        (CONFIG_ESP32P4_SELECTS_REV_LESS_V3 AND CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP))
        if(CONFIG_SOC_PM_CPU_RETENTION_BY_SW)
            target_link_libraries(${COMPONENT_LIB} INTERFACE "-u rv_core_critical_regs_save")
            target_link_libraries(${COMPONENT_LIB} INTERFACE "-u rv_core_critical_regs_restore")
            if(CONFIG_SOC_CPU_HAS_FPU AND CONFIG_SOC_PM_FPU_RETENTION_BY_SW)
                target_link_libraries(${COMPONENT_LIB} INTERFACE "-u rv_core_fpu_save")
                target_link_libraries(${COMPONENT_LIB} INTERFACE "-u rv_core_fpu_restore")
            endif()
        endif()
    endif()

    if(CONFIG_P4_REV3_MSPI_CRASH_AFTER_POWER_UP_WORKAROUND)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-u p4_rev3_mspi_workaround")
    endif()
endif()

if(CONFIG_COMPILER_STATIC_ANALYZER AND CONFIG_IDF_TOOLCHAIN_GCC)
    set_source_files_properties(
        sleep_retention.c
        PROPERTIES COMPILE_FLAGS
        -Wno-analyzer-null-dereference
    )
endif()
