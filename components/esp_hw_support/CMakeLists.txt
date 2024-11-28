idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

# On Linux, we only support a few features, hence this simple component registration
if(${target} STREQUAL "linux")
    idf_component_register(SRCS "port/linux/esp_random.c"
                                 "port/linux/chip_info.c"
                           INCLUDE_DIRS "include")
    return()
endif()

set(requires soc)
# only esp_hw_support/adc_share_hw_ctrl.c requires efuse component
# TODO: remove esp_security from REQUIRES in ESP-IDF v6.0 (see IDF-10733)
set(priv_requires efuse spi_flash bootloader_support esp_security)

if(${target} STREQUAL "esp32c6")
    list(APPEND priv_requires hal)
endif()

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
                     "sleep_modem.c"
                     "sleep_modes.c"
                     "sleep_console.c"
                     "sleep_gpio.c"
                     "sleep_event.c"
                     "regi2c_ctrl.c"
                     "esp_gpio_reserve.c"
                     "sar_periph_ctrl_common.c"
                     "port/${target}/io_mux.c"
                     "port/${target}/esp_clk_tree.c"
                     "port/esp_clk_tree_common.c"
                     "dma/esp_dma_utils.c"
                     "dma/gdma_link.c"
                     "spi_share_hw_ctrl.c"
                     "spi_bus_lock.c")

    if(CONFIG_SOC_ADC_SUPPORTED)
        list(APPEND srcs "adc_share_hw_ctrl.c")
    endif()

    if(CONFIG_SOC_ISP_SHARE_CSI_BRG)
        list(APPEND srcs "mipi_csi_share_hw_ctrl.c")
    endif()
    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "sleep_retention.c"
                         "sleep_system_peripheral.c"
                         )
    endif()

    # [refactor-todo]
    list(APPEND priv_requires esp_driver_gpio   # for GPIO and RTC (by sleep_gpio and sleep_modes)
                              esp_timer
                              esp_pm)

    list(APPEND priv_requires esp_mm)

    if(CONFIG_IDF_TARGET_ESP32 OR CONFIG_IDF_TARGET_ESP32S2)
        list(APPEND srcs "rtc_wdt.c")
    endif()

    if(CONFIG_SOC_GDMA_SUPPORTED)
        list(APPEND srcs "dma/gdma.c" "deprecated/gdma_legacy.c")
        if(CONFIG_SOC_GDMA_SUPPORT_SLEEP_RETENTION AND CONFIG_SOC_PAU_SUPPORTED)
            list(APPEND srcs "dma/gdma_sleep_retention.c")
        endif()
        if(CONFIG_SOC_GDMA_SUPPORT_ETM)
            list(APPEND srcs "dma/gdma_etm.c")
        endif()
        if(CONFIG_SOC_GDMA_SUPPORT_CRC)
            list(APPEND srcs "dma/gdma_crc.c")
        endif()
    endif()

    if(CONFIG_SOC_GP_LDO_SUPPORTED)
        list(APPEND srcs "ldo/esp_ldo_regulator.c")
    endif()

    if(CONFIG_SOC_DEBUG_PROBE_SUPPORTED)
        list(APPEND srcs "debug_probe/debug_probe.c")
    endif()

    if(CONFIG_SOC_ASYNC_MEMCPY_SUPPORTED)
        list(APPEND srcs "dma/esp_async_memcpy.c")
        if(CONFIG_SOC_GDMA_SUPPORTED)
            list(APPEND srcs "dma/async_memcpy_gdma.c")
        endif() # CONFIG_SOC_GDMA_SUPPORTED
        if(CONFIG_SOC_CP_DMA_SUPPORTED)
            list(APPEND srcs "dma/async_memcpy_cp_dma.c")
        endif() # CONFIG_SOC_CP_DMA_SUPPORTED
    endif() # CONFIG_SOC_ASYNC_MEMCPY_SUPPORTED

    if(CONFIG_SOC_DW_GDMA_SUPPORTED)
        list(APPEND srcs "dma/dw_gdma.c")
    endif()

    if(CONFIG_SOC_DMA2D_SUPPORTED)
        list(APPEND srcs "dma/dma2d.c")
    endif()

    if(CONFIG_SOC_SYSTIMER_SUPPORTED)
        list(APPEND srcs "port/${target}/systimer.c")
    endif()

    if(CONFIG_SOC_ETM_SUPPORTED)
        list(APPEND srcs "esp_etm.c")
    endif()

    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "port/pau_regdma.c"
                         "port/regdma_link.c")
    endif()

    if(CONFIG_SOC_MODEM_CLOCK_IS_INDEPENDENT AND CONFIG_SOC_MODEM_CLOCK_SUPPORTED)
        list(APPEND srcs "modem_clock.c")
    endif()

    if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
        list(APPEND srcs "mspi_timing_tuning.c")
        if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY)
            list(APPEND srcs "mspi_timing_by_mspi_delay.c")
        endif()
        if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_DQS)
            list(APPEND srcs "mspi_timing_by_dqs.c")
        endif()
    endif()

    if(CONFIG_SOC_RTC_FAST_MEM_SUPPORTED AND CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB)
        list(APPEND srcs "sleep_wake_stub.c")
    endif()

    if(CONFIG_SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX OR CONFIG_SOC_GPIO_CLOCKOUT_BY_IO_MUX)
        list(APPEND srcs "esp_clock_output.c")
    endif()
else()
    # Requires "_esp_error_check_failed()" function
    list(APPEND priv_requires "esp_system")
endif()

set(public_include_dirs "include" "include/soc" "include/soc/${target}"
                        "dma/include" "ldo/include" "debug_probe/include")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include_dirs}
                       PRIV_INCLUDE_DIRS port/include include/esp_private
                       REQUIRES ${requires}
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS linker.lf dma/linker.lf ldo/linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(port/${target})
add_subdirectory(lowpower)

if(CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND OR CONFIG_PM_SLP_DISABLE_GPIO)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_sleep_gpio_include")
endif()

if(NOT non_os_build)
    if(CONFIG_SPIRAM)
        idf_component_optional_requires(PRIVATE esp_psram)
    endif()
endif()
