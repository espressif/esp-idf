idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

# On Linux, we only support a few features, hence this simple component registration
if(${target} STREQUAL "linux")
    idf_component_register(SRCS "port/linux/esp_random.c"
                                 "port/linux/chip_info.c"
                           INCLUDE_DIRS "include" "etm/include")
    return()
endif()

set(requires esp_hal_gpio esp_hal_usb esp_hal_pmu)
# only esp_hw_support/adc_share_hw_ctrl.c requires efuse component
set(priv_requires efuse spi_flash bootloader_support esp_hal_wdt esp_hal_rtc_timer
    esp_hal_clock esp_hal_security)

set(srcs "cpu.c" "port/${IDF_TARGET}/esp_cpu_intr.c" "esp_memory_utils.c" "port/${IDF_TARGET}/cpu_region_protect.c")
if(NOT non_os_build)
    list(APPEND srcs "esp_clk.c"
                     "clk_ctrl_os.c"
                     "hw_random.c"
                     "intr_alloc.c"
                     "mac_addr.c"
                     "periph_ctrl.c"
                     "revision.c"
                     "rtc_module.c"
                     "regi2c_ctrl.c"
                     "esp_gpio_reserve.c"
                     "sar_tsens_ctrl.c"
                     "port/${target}/io_mux.c"
                     "port/${target}/esp_clk_tree.c"
                     "spi_bus_lock.c"
                     "clk_utils.c")
    if(CONFIG_SOC_USB_OTG_SUPPORTED)
        list(APPEND srcs "usb_phy/usb_phy.c")
    endif()
    if(CONFIG_SOC_CLK_TREE_SUPPORTED)
        list(APPEND srcs "port/esp_clk_tree_common.c")
    endif()
    if(CONFIG_SOC_GPSPI_SUPPORTED)
        list(APPEND srcs "spi_share_hw_ctrl.c")
    endif()
    if(CONFIG_SOC_ADC_SUPPORTED)
        list(APPEND srcs "adc_share_hw_ctrl.c")
    endif()
    if(CONFIG_SOC_ISP_SHARE_CSI_BRG)
        list(APPEND srcs "mipi_csi_share_hw_ctrl.c")
    endif()
    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "sleep_retention.c")
    endif()
    if(CONFIG_SOC_MODEM_SUPPORT_ETM)
        list(APPEND srcs "modem/modem_etm.c")
    endif()
    if(CONFIG_SOC_PMU_SUPPORTED)
        list(APPEND srcs "pmu_share_hw.c")
    endif()
    if(CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
        list(APPEND srcs "sleep_modem.c"
                         "sleep_modes.c"
                         "sleep_uart.c"
                         "sleep_console.c"
                         "sleep_mspi.c"
                         "sleep_usb.c"
                         "sleep_gpio.c"
                         "sleep_event.c"
            )
        if(CONFIG_SOC_PAU_SUPPORTED AND CONFIG_SOC_PM_SUPPORT_TOP_PD)
            list(APPEND srcs "sleep_system_peripheral.c")
            list(APPEND srcs "port/${target}/peripheral_domain_pd.c")
        endif()
    endif()

    # [refactor-todo]
    list(APPEND priv_requires esp_driver_gpio   # for GPIO and RTC (by sleep_gpio and sleep_modes)
                              esp_timer
                              esp_hal_touch_sens # for touch sensor wakeup (introduced in sleep_modes.c)
                              esp_pm)

    list(APPEND priv_requires esp_mm
                              esp_hal_mspi
                              esp_hal_uart
    )

    if(CONFIG_IDF_TARGET_ESP32 OR CONFIG_IDF_TARGET_ESP32S2)
        list(APPEND srcs "rtc_wdt.c")
    endif()

    if(CONFIG_SOC_GP_LDO_SUPPORTED)
        list(APPEND srcs "ldo/esp_ldo_regulator.c")
    endif()

    if(CONFIG_SOC_DEBUG_PROBE_SUPPORTED)
        list(APPEND srcs "debug_probe/debug_probe.c")
    endif()

    if(CONFIG_SOC_SYSTIMER_SUPPORTED)
        list(APPEND srcs "port/${target}/systimer.c")
    endif()

    if(CONFIG_SOC_ETM_SUPPORTED)
        list(APPEND srcs "etm/esp_etm.c")
    endif()

    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "port/pau_regdma.c"
                         "port/regdma_link.c")
    endif()

    if(CONFIG_SOC_MODEM_CLOCK_IS_INDEPENDENT AND CONFIG_SOC_MODEM_CLOCK_SUPPORTED)
        list(APPEND srcs "modem_clock.c")
    endif()

    if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
        list(APPEND srcs "mspi/mspi_timing_tuning/mspi_timing_tuning.c")
        if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY AND NOT CONFIG_IDF_TARGET_ESP32S3)
            set(mspi_delay_file
                "${CMAKE_CURRENT_LIST_DIR}/mspi/mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_mspi_delay.c"
            )
            if(EXISTS "${mspi_delay_file}")
                list(APPEND srcs "mspi/mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_mspi_delay.c")
            endif()
        endif()

        if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_DQS)
            list(APPEND srcs "mspi/mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_dqs.c")
        endif()
        if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY)
            list(APPEND srcs "mspi/mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_flash_delay.c")
        endif()

        list(APPEND srcs "mspi/mspi_intr/mspi_intr.c")
    endif()

    if(CONFIG_SOC_RTC_FAST_MEM_SUPPORTED AND CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB)
        list(APPEND srcs "sleep_wake_stub.c")
    endif()

    if(CONFIG_SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX OR CONFIG_SOC_GPIO_CLOCKOUT_BY_IO_MUX)
        list(APPEND srcs "esp_clock_output.c")
    endif()

    if(CONFIG_SOC_BOD_SUPPORTED)
        list(APPEND srcs "power_supply/brownout.c")
    endif()

    if(CONFIG_SOC_VBAT_SUPPORTED)
        list(APPEND srcs "power_supply/vbat.c")
    endif()

else()
    if(ESP_TEE_BUILD)
        list(APPEND srcs "esp_clk.c" "hw_random.c")
        if(CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1)
            list(APPEND srcs "mspi/mspi_timing_tuning/mspi_timing_tuning.c")
            if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY)
                list(APPEND srcs "mspi/mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_mspi_delay.c")
            endif()
            if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_DQS)
                list(APPEND srcs "mspi/mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_dqs.c")
            endif()
            if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY)
                list(APPEND srcs "mspi/mspi_timing_tuning/tuning_scheme_impl/mspi_timing_by_flash_delay.c")
            endif()
        endif()
    endif()

    # Requires "_esp_error_check_failed()" function
    list(APPEND priv_requires "esp_system")
endif()

set(public_include_dirs "include" "include/soc"
                        "ldo/include" "debug_probe/include" "etm/include"
                        "mspi/mspi_timing_tuning/include" "mspi/mspi_timing_tuning/tuning_scheme_impl/include"
                        "mspi/mspi_intr/include"
                        "power_supply/include" "modem/include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/include/soc/${target}")
    list(APPEND public_include_dirs
        "include/soc/${target}"
    )
endif()

if(NOT CONFIG_ESP32P4_SELECTS_REV_LESS_V3 AND CONFIG_IDF_TARGET_ESP32P4)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang") # TODO: LLVM-478 IDF-14338
        message(FATAL_ERROR "ESP32-P4 rev. 3.0 or higher is not supported in Clang-based toolchain")
    endif()
endif()

if(${target} STREQUAL "esp32p4")
    # for mipi_csi_share_hw_ctrl.c
    list(APPEND priv_requires esp_hal_cam)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include_dirs}
                       PRIV_INCLUDE_DIRS port/include include/esp_private
                       REQUIRES ${requires}
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS linker.lf ldo/linker.lf mspi/linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(port/${target})
if(CONFIG_SOC_SPI_MEM_SUPPORT_TIMING_TUNING)
    add_subdirectory(mspi/mspi_timing_tuning/port/${target})
endif()
add_subdirectory(lowpower)

if(CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND OR CONFIG_PM_SLP_DISABLE_GPIO)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_sleep_gpio_include")
endif()

if(CONFIG_SOC_PAU_SUPPORTED AND CONFIG_SOC_PM_SUPPORT_TOP_PD)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u sleep_system_peripheral_dummy")
endif()

if(NOT non_os_build)
    if(CONFIG_SPIRAM)
        idf_component_optional_requires(PRIVATE esp_psram)
    endif()
endif()

if(${target} STREQUAL "esp32p4")
    # for mipi_csi_share_hw_ctrl.c
    idf_component_optional_requires(PRIVATE esp_hal_cam)
endif()

# Required in `adc_share_hw_ctrl.h`
idf_component_optional_requires(PUBLIC esp_hal_ana_conv)
