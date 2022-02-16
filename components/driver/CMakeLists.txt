idf_build_get_property(target IDF_TARGET)

set(srcs
    "gpio.c"
    "gptimer.c"
    "timer_legacy.c"
    "i2c.c"
    "ledc.c"
    "legacy_new_driver_coexist.c"
    "periph_ctrl.c"
    "rtc_io.c"
    "rtc_module.c"
    "sdspi_crc.c"
    "sdspi_host.c"
    "sdspi_transaction.c"
    "spi_common.c"
    "spi_master.c"
    "spi_slave.c"
    "spi_bus_lock.c"
    "uart.c")

set(includes "include" "${target}/include" "deprecated")

if(CONFIG_SOC_ADC_SUPPORTED)
    list(APPEND srcs
         "adc_common.c"
         "adc.c")
endif()

if(CONFIG_SOC_MCPWM_SUPPORTED)
    list(APPEND srcs "mcpwm.c")
endif()

if(CONFIG_SOC_GDMA_SUPPORTED)
    list(APPEND srcs "gdma.c")
endif()

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "dedic_gpio.c")
endif()

if(CONFIG_SOC_SIGMADELTA_SUPPORTED)
    list(APPEND srcs "sigmadelta.c")
endif()

if(CONFIG_SOC_RMT_SUPPORTED)
    list(APPEND srcs "rmt.c")
endif()

if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "pcnt.c")
endif()

if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "sdmmc_transaction.c" "sdmmc_host.c")
endif()

if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs "i2s.c")
endif()

if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "twai.c")
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

if(IDF_TARGET STREQUAL "esp32s2")
    list(APPEND srcs "dac_common.c"
                     "spi_slave_hd.c"
                     "touch_sensor_common.c"
                     "esp32s2/rtc_tempsensor.c"
                     "esp32s2/touch_sensor.c"
                     "esp32s2/adc.c"
                     "adc_deprecated.c"
                     "esp32s2/adc2_init_cal.c"
                     "esp32s2/dac.c")
endif()

if(${target} STREQUAL "esp32s3")
    list(APPEND srcs "usb_serial_jtag.c"
                     "spi_slave_hd.c"
                     "touch_sensor_common.c"
                     "esp32s3/touch_sensor.c")
endif()

if(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND srcs "spi_slave_hd.c"
                     "usb_serial_jtag.c"
                     "esp32c3/adc2_init_cal.c"
                     "esp32c3/rtc_tempsensor.c")
endif()

if(IDF_TARGET STREQUAL "esp32h2")
    list(APPEND srcs "spi_slave_hd.c"
                     "esp32h2/rtc_tempsensor.c")
endif()

if(IDF_TARGET STREQUAL "esp32c2")
    list(APPEND srcs "spi_slave_hd.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes}
                    PRIV_INCLUDE_DIRS "include/driver"
                    PRIV_REQUIRES efuse esp_timer
                    REQUIRES esp_pm esp_ringbuf freertos soc hal esp_hw_support
                    LDFRAGMENTS linker.lf)
# (REQUIRES cannot hide soc headers, since many arguments in the driver headers are chip-dependent)

# uses C11 atomic feature
set_source_files_properties(spi_master.c PROPERTIES COMPILE_FLAGS -std=gnu11)
