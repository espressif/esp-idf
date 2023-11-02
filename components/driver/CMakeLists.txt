idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

# Always compiled source files
set(srcs
    "gpio/gpio.c"
    "gpio/gpio_glitch_filter_ops.c"
    "gpio/rtc_io.c"
    "spi/spi_bus_lock.c")

# Always included headers
set(includes "include"
             "deprecated"
             "analog_comparator/include"
             "dac/include"
             "gpio/include"
             "gptimer/include"
             "i2c/include"
             "i2s/include"
             "ledc/include"
             "mcpwm/include"
             "parlio/include"
             "pcnt/include"
             "rmt/include"
             "sdio_slave/include"
             "sdmmc/include"
             "sigma_delta/include"
             "spi/include"
             "temperature_sensor/include"
             "touch_sensor/include"
             "twai/include"
             "uart/include"
             "usb_serial_jtag/include")

# Always included linker fragments
set(ldfragments "linker.lf"
                "gpio/linker.lf")

# ADC related source files (dprecated)
if(CONFIG_SOC_ADC_SUPPORTED)
    list(APPEND srcs "deprecated/adc_legacy.c")
endif()

if(CONFIG_SOC_ADC_DMA_SUPPORTED)
    list(APPEND srcs "deprecated/adc_dma_legacy.c")
endif()

# Analog comparator related source files
if(CONFIG_SOC_ANA_CMPR_SUPPORTED)
    list(APPEND srcs "analog_comparator/ana_cmpr.c")
    if(CONFIG_SOC_ANA_CMPR_SUPPORT_ETM)
        list(APPEND srcs "analog_comparator/ana_cmpr_etm.c")
    endif()
endif()

# DAC related source files
if(CONFIG_SOC_DAC_SUPPORTED)
    list(APPEND srcs "dac/dac_oneshot.c"
                     "dac/dac_cosine.c"
                     "dac/dac_continuous.c"
                     "dac/dac_common.c"
                     "dac/${target}/dac_dma.c"
                     "deprecated/dac_common_legacy.c"
                     "deprecated/${target}/dac_legacy.c")
endif()

# Parallel IO related source files
if(CONFIG_SOC_PARLIO_SUPPORTED)
    list(APPEND srcs "parlio/parlio_common.c" "parlio/parlio_tx.c")
endif()

# GPIO related source files
if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "gpio/dedic_gpio.c")
endif()

if(CONFIG_SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER)
    list(APPEND srcs "gpio/gpio_pin_glitch_filter.c")
endif()

if(CONFIG_SOC_GPIO_FLEX_GLITCH_FILTER_NUM GREATER 0)
    list(APPEND srcs "gpio/gpio_flex_glitch_filter.c")
endif()

if(CONFIG_SOC_GPIO_SUPPORT_ETM)
    list(APPEND srcs "gpio/gpio_etm.c")
endif()

# GPTimer related source files
if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND srcs "gptimer/gptimer.c"
                     "gptimer/gptimer_priv.c"
                     "deprecated/timer_legacy.c")

    list(APPEND ldfragments "gptimer/linker.lf")
endif()

if(CONFIG_SOC_TIMER_SUPPORT_ETM)
    list(APPEND srcs "gptimer/gptimer_etm.c")
endif()

# I2C related source files
if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs "i2c/i2c.c"
                     "i2c/i2c_master.c"
                     "i2c/i2c_common.c"
        )
    if(CONFIG_SOC_I2C_SUPPORT_SLAVE)
        list(APPEND srcs "i2c/i2c_slave.c")
    endif()

    list(APPEND ldfragments "i2c/linker.lf")
endif()

# I2S related source files
if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs "i2s/i2s_common.c"
                     "i2s/i2s_platform.c"
                     "i2s/i2s_std.c"
                     "deprecated/i2s_legacy.c")
    if(CONFIG_SOC_I2S_SUPPORTS_PDM)
        list(APPEND srcs "i2s/i2s_pdm.c")
    endif()
    if(CONFIG_SOC_I2S_SUPPORTS_TDM)
        list(APPEND srcs "i2s/i2s_tdm.c")
    endif()
endif()

# LEDC related source files
if(CONFIG_SOC_LEDC_SUPPORTED)
    list(APPEND srcs "ledc/ledc.c")

    list(APPEND ldfragments "ledc/linker.lf")
endif()

# MCPWM related source files
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
    if(CONFIG_SOC_MCPWM_SUPPORT_ETM)
        list(APPEND srcs "mcpwm/mcpwm_etm.c")
    endif()
    list(APPEND ldfragments "mcpwm/linker.lf")
endif()

# PCNT related source files
if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "pcnt/pulse_cnt.c"
                     "deprecated/pcnt_legacy.c")
endif()

# RMT related source files
if(CONFIG_SOC_RMT_SUPPORTED)
    list(APPEND srcs "rmt/rmt_common.c"
                     "rmt/rmt_encoder.c"
                     "rmt/rmt_rx.c"
                     "rmt/rmt_tx.c"
                     "deprecated/rmt_legacy.c")
    list(APPEND ldfragments "rmt/linker.lf")
endif()

# SDIO Slave related source files
if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "sdio_slave/sdio_slave.c")
endif()

# SDMMC related source files
if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "sdmmc/sdmmc_transaction.c"
                     "sdmmc/sdmmc_host.c")
endif()

# Sigma-Delta Modulation related source files
if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "sigma_delta/sdm.c"
                     "deprecated/sigma_delta_legacy.c")
endif()

# SPI related source files
if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs "spi/gpspi/spi_common.c"
                     "spi/gpspi/spi_master.c"
                     "spi/gpspi/spi_slave.c"
                     "spi/sdspi/sdspi_crc.c"
                     "spi/sdspi/sdspi_host.c"
                     "spi/sdspi/sdspi_transaction.c")
endif()

if(CONFIG_SOC_SPI_SUPPORT_SLAVE_HD_VER2)
    list(APPEND srcs "spi/gpspi/spi_slave_hd.c")
endif()

# Temperature Sensor related source files
if(CONFIG_SOC_TEMP_SENSOR_SUPPORTED)
    list(APPEND srcs "temperature_sensor/temperature_sensor.c"
                     "deprecated/rtc_temperature_legacy.c")
endif()

# Touch Sensor related source files
if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    list(APPEND srcs "touch_sensor/touch_sensor_common.c"
                     "touch_sensor/${target}/touch_sensor.c")
    list(APPEND includes "touch_sensor/${target}/include")
endif()

# TWAI related source files
if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "twai/twai.c")

    list(APPEND ldfragments "twai/linker.lf")
endif()

# UART related source files
if(CONFIG_SOC_UART_SUPPORTED)
    list(APPEND srcs "uart/uart.c")

    list(APPEND ldfragments "uart/linker.lf")
endif()

# USB Serial JTAG related source files
if(CONFIG_SOC_USB_SERIAL_JTAG_SUPPORTED)
    list(APPEND srcs "usb_serial_jtag/usb_serial_jtag.c"
                     "usb_serial_jtag/usb_serial_jtag_connection_monitor.c")
endif()

# Other source files
if(${target} STREQUAL "esp32")
    list(APPEND srcs "deprecated/adc_i2s_deprecated.c")
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
                           PRIV_REQUIRES efuse esp_timer esp_mm
                           REQUIRES esp_pm esp_ringbuf freertos soc hal esp_hw_support
                           LDFRAGMENTS ${ldfragments}
                          )
endif()

# If system needs to monitor USJ connection status, then usb_serial_jtag_connection_monitor object file has to be linked
# to the binary, to allow tick hook to be registered
if(CONFIG_USJ_NO_AUTO_LS_ON_CONNECTION OR
   (CONFIG_SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP AND CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED))
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usb_serial_jtag_connection_monitor_include")
endif()
