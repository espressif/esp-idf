idf_build_get_property(target IDF_TARGET)

# On Linux, we only support a few features, hence this simple component registration
if(${target} STREQUAL "linux")
    idf_component_register(SRCS "port/linux/esp_random.c"
                                 "port/linux/chip_info.c"
                           INCLUDE_DIRS "include")
    return()
endif()

set(requires soc)
# only esp_hw_support/adc_share_hw_ctrl.c requires efuse component
set(priv_requires efuse spi_flash bootloader_support)

if(${target} STREQUAL "esp32c6")
    list(APPEND priv_requires hal)
endif()

set(srcs "cpu.c" "esp_memory_utils.c" "port/${IDF_TARGET}/cpu_region_protect.c")
if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "esp_clk.c"
                     "clk_ctrl_os.c"
                     "hw_random.c"
                     "intr_alloc.c"
                     "mac_addr.c"
                     "periph_ctrl.c"
                     "revision.c"
                     "rtc_module.c"
                     "sleep_modes.c"
                     "sleep_console.c"
                     "sleep_gpio.c"
                     "sleep_event.c"
                     "sleep_modem.c"
                     "regi2c_ctrl.c"
                     "esp_gpio_reserve.c"
                     "sar_periph_ctrl_common.c"
                     "port/${target}/io_mux.c"
                     "port/${target}/esp_clk_tree.c"
                     "port/esp_clk_tree_common.c"
                     "dma/esp_dma_utils.c")

    if(CONFIG_SOC_ADC_SUPPORTED)
        list(APPEND srcs "adc_share_hw_ctrl.c")
    endif()

    if(CONFIG_SOC_PM_SUPPORT_CPU_PD)
        list(APPEND srcs "sleep_cpu.c")
    endif()

    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "sleep_retention.c"  "sleep_system_peripheral.c" "sleep_clock.c")
    endif()

    # [refactor-todo]: requires "driver" for GPIO and RTC  (by sleep_gpio and sleep_modes)
    list(APPEND priv_requires driver esp_timer)

    list(APPEND priv_requires esp_mm)

    if(CONFIG_IDF_TARGET_ESP32 OR CONFIG_IDF_TARGET_ESP32S2)
        list(APPEND srcs "rtc_wdt.c")
    endif()

    if(CONFIG_SOC_GDMA_SUPPORTED)
        list(APPEND srcs "dma/gdma.c")
        if(CONFIG_SOC_PAU_SUPPORTED)
            list(APPEND srcs "dma/gdma_sleep_retention.c")
        endif()
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

    if(CONFIG_SOC_GDMA_SUPPORT_ETM)
        list(APPEND srcs "dma/gdma_etm.c")
    endif()

    if(CONFIG_SOC_SYSTIMER_SUPPORTED)
        list(APPEND srcs "port/${target}/systimer.c")
    endif()

    if(CONFIG_SOC_HMAC_SUPPORTED)
        list(APPEND srcs "esp_hmac.c")
    endif()

    if(CONFIG_SOC_ETM_SUPPORTED)
        list(APPEND srcs "esp_etm.c")
    endif()

    if(CONFIG_SOC_CRYPTO_DPA_PROTECTION_SUPPORTED)
        list(APPEND srcs "esp_dpa_protection.c")
    endif()

    if(CONFIG_SOC_DIG_SIGN_SUPPORTED)
        list(APPEND srcs "esp_ds.c")
    endif()

    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "port/pau_regdma.c"
                         "port/regdma_link.c")
    endif()

    if(CONFIG_SOC_PM_CPU_RETENTION_BY_SW)
        list(APPEND srcs "sleep_cpu_asm.S")
        set_property(TARGET ${COMPONENT_LIB}
                    APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u rv_core_critical_regs_save")
        set_property(TARGET ${COMPONENT_LIB}
                    APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u rv_core_critical_regs_restore")
    endif()

    if(CONFIG_SOC_MODEM_CLOCK_IS_INDEPENDENT)
        list(APPEND srcs "modem_clock.c")
    endif()

    if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
        list(APPEND srcs "mspi_timing_tuning.c" "mspi_timing_config.c")
        if(CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY)
            list(APPEND srcs "mspi_timing_by_mspi_delay.c")
        endif()
    endif()

    if(CONFIG_SOC_RTC_FAST_MEM_SUPPORTED)
        list(APPEND srcs "sleep_wake_stub.c")
    endif()

    if(CONFIG_SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX OR CONFIG_SOC_GPIO_CLOCKOUT_BY_IO_MUX)
        list(APPEND srcs "esp_clock_output.c")
    endif()

    if(CONFIG_IDF_TARGET_ESP32P4)
        list(REMOVE_ITEM srcs
                "sleep_cpu.c"         # TODO: IDF-7528, IDF-7529
                "sleep_modes.c"       # TODO: IDF-7528, IDF-7529
                "sleep_wake_stub.c"   # TODO: IDF-7529
                "sleep_gpio.c"        # TODO: IDF-7528, IDF-7529
                "port/esp_clk_tree_common.c"    # TODO: IDF-7526
            )
    endif()
else()
    # Requires "_esp_error_check_failed()" function
    list(APPEND priv_requires "esp_system")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include include/soc include/soc/${target}
                       PRIV_INCLUDE_DIRS port/include include/esp_private
                       REQUIRES ${requires}
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS linker.lf dma/linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(port/${target})

if(NOT BOOTLOADER_BUILD)
    if(CONFIG_SPIRAM)
        idf_component_optional_requires(PRIVATE esp_psram)
    endif()
    if(CONFIG_SOC_CRYPTO_DPA_PROTECTION_SUPPORTED)
        target_link_libraries(${COMPONENT_LIB} PRIVATE "-u esp_crypto_dpa_prot_include_impl")
    endif()
endif()
