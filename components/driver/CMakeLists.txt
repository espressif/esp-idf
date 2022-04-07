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
    list(APPEND srcs
         "adc_single.c"
         "adc.c")
endif()

if(CONFIG_SOC_MCPWM_SUPPORTED)
    list(APPEND srcs "mcpwm.c")
endif()

if(CONFIG_SOC_GDMA_SUPPORTED)
    list(APPEND srcs "gdma.c")
endif()

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "gpio/dedic_gpio.c")
endif()

if(CONFIG_SOC_SIGMADELTA_SUPPORTED)
    list(APPEND srcs "gpio/sigmadelta_gpio.c")
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

if(${target} STREQUAL "esp32")
    list(APPEND srcs "dac_common.c"
                     "sdio_slave.c"
                     "touch_sensor_common.c"
                     "esp32/touch_sensor.c"
                     "esp32/adc.c"
                     "adc_deprecated.c"
                     "esp32/dac.c")
endif()

if(${target} STREQUAL "esp32s2")
    list(APPEND srcs "dac_common.c"
                     "touch_sensor_common.c"
                     "esp32s2/touch_sensor.c"
                     "esp32s2/adc.c"
                     "adc_deprecated.c"
                     "esp32s2/adc2_init_cal.c"
                     "esp32s2/dac.c")
endif()

if(${target} STREQUAL "esp32s3")
    list(APPEND srcs "touch_sensor_common.c"
                     "esp32s3/touch_sensor.c")
endif()

if(${target} STREQUAL "esp32c3")
    list(APPEND srcs "esp32c3/adc2_init_cal.c")
endif()

if(BOOTLOADER_BUILD)
    # Bootloader shall NOT depend on the drivers
    idf_component_register()
else()
    # (REQUIRES cannot hide soc headers, since many arguments in the driver headers are chip-dependent)
    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS ${includes}
                        PRIV_INCLUDE_DIRS "include/driver"
                        PRIV_REQUIRES efuse esp_timer
                        REQUIRES esp_pm esp_ringbuf freertos soc hal esp_hw_support
                        LDFRAGMENTS linker.lf)
endif()

# uses C11 atomic feature
set_source_files_properties(spi_master.c PROPERTIES COMPILE_FLAGS -std=gnu11)
