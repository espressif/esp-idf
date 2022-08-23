idf_build_get_property(target IDF_TARGET)

set(srcs
    "gpio/gpio.c"
    "gpio/rtc_io.c"
    "gptimer.c"
    "i2c.c"
    "ledc.c"
    "sdspi_crc.c"
    "sdspi_host.c"
    "sdspi_transaction.c"
    "spi_common.c"
    "spi_master.c"
    "spi_slave.c"
    "spi_bus_lock.c"
    "uart.c")

# deprecated source files
list(APPEND srcs "deprecated/timer_legacy.c")

set(includes "include" "deprecated")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

if(CONFIG_SOC_ADC_SUPPORTED)
    list(APPEND srcs "deprecated/adc_legacy.c")
endif()

if(CONFIG_SOC_ADC_DMA_SUPPORTED)
    list(APPEND srcs "deprecated/adc_dma_legacy.c")
endif()

if(CONFIG_SOC_MCPWM_SUPPORTED)
    list(APPEND srcs "mcpwm/mcpwm_cap.c"
                     "mcpwm/mcpwm_cmpr.c"
                     "mcpwm/mcpwm_com.c"
                     "mcpwm/mcpwm_fault.c"
                     "mcpwm/mcpwm_gen.c"
                     "mcpwm/mcpwm_oper.c"
                     "mcpwm/mcpwm_sync.c"
                     "mcpwm/mcpwm_timer.c"
                     "deprecated/mcpwm_legacy.c")
endif()

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "gpio/dedic_gpio.c")
endif()

if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "sdm.c" "deprecated/sigma_delta_legacy.c")
endif()

if(CONFIG_SOC_RMT_SUPPORTED)
    list(APPEND srcs "rmt/rmt_common.c" "rmt/rmt_encoder.c" "rmt/rmt_rx.c" "rmt/rmt_tx.c" "deprecated/rmt_legacy.c")
endif()

if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "pulse_cnt.c" "deprecated/pcnt_legacy.c")
endif()

if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "sdmmc_transaction.c" "sdmmc_host.c")
endif()

if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs "i2s/i2s_common.c"
                     "i2s/i2s_std.c"
                     "deprecated/i2s_legacy.c")
    if(CONFIG_SOC_I2S_SUPPORTS_PDM)
        list(APPEND srcs "i2s/i2s_pdm.c")
    endif()
    if(CONFIG_SOC_I2S_SUPPORTS_TDM)
        list(APPEND srcs "i2s/i2s_tdm.c")
    endif()
endif()

if(CONFIG_SOC_TEMP_SENSOR_SUPPORTED)
    list(APPEND srcs "temperature_sensor.c"
                     "deprecated/rtc_temperature_legacy.c")
endif()

if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "twai.c")
endif()

if(CONFIG_SOC_USB_SERIAL_JTAG_SUPPORTED)
    list(APPEND srcs "usb_serial_jtag.c")
endif()

if(CONFIG_SOC_SPI_SUPPORT_SLAVE_HD_VER2)
    list(APPEND srcs "spi_slave_hd.c")
endif()

if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    list(APPEND srcs "touch_sensor_common.c" "${target}/touch_sensor.c")
endif()

if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "sdio_slave.c")
endif()

if(${target} STREQUAL "esp32")
    list(APPEND srcs "dac_common.c"
                     "deprecated/adc_i2s_deprecated.c"
                     "esp32/dac.c")
endif()

if(${target} STREQUAL "esp32s2")
    list(APPEND srcs "dac_common.c"
                     "esp32s2/dac.c")
endif()

if(BOOTLOADER_BUILD)
    # Bootloader shall NOT depend on the drivers
    idf_component_register()
else()
    # (REQUIRES cannot hide soc headers, since many arguments in the driver headers are chip-dependent)
    # (Legacy drivers requires `esp_adc`, due to ADC HW resource mutex logics are there.
    # Can be removed together with legacy drivers)
    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS ${includes}
                        PRIV_INCLUDE_DIRS "include/driver"
                        PRIV_REQUIRES efuse esp_timer
                        REQUIRES esp_pm esp_ringbuf freertos soc hal esp_hw_support
                        LDFRAGMENTS linker.lf)
endif()
