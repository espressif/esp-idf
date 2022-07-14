idf_build_get_property(target IDF_TARGET)

set(srcs
    "gpio.c"
    "i2c.c"
    "i2s.c"
    "ledc.c"
    "pcnt.c"
    "periph_ctrl.c"
    "rmt.c"
    "rtc_io.c"
    "rtc_module.c"
    "sdspi_crc.c"
    "sdspi_host.c"
    "sdspi_transaction.c"
    "sigmadelta.c"
    "spi_common.c"
    "spi_master.c"
    "spi_slave.c"
    "spi_bus_lock.c"
    "timer.c"
    "twai.c"
    "uart.c")

set(includes "include" "${target}/include")

if(${target} STREQUAL "esp32")
    # SDMMC and MCPWM are in ESP32 only.
    list(APPEND srcs "adc_common.c"
                     "dac_common.c"
                     "mcpwm.c"
                     "sdio_slave.c"
                     "sdmmc_host.c"
                     "sdmmc_transaction.c"
                     "touch_sensor_common.c"
                     "esp32/touch_sensor.c"
                     "adc.c"
                     "esp32/adc.c"
                     "adc_deprecated.c"
                     "esp32/dac.c")
endif()

if(IDF_TARGET STREQUAL "esp32s2")
    list(APPEND srcs "adc_common.c"
                     "dac_common.c"
                     "dedic_gpio.c"
                     "spi_slave_hd.c"
                     "touch_sensor_common.c"
                     "esp32s2/rtc_tempsensor.c"
                     "esp32s2/touch_sensor.c"
                     "esp32s2/adc.c"
                     "adc.c"
                     "adc_deprecated.c"
                     "esp32s2/adc2_init_cal.c"
                     "esp32s2/dac.c")
endif()

if(${target} STREQUAL "esp32s3")
    list(APPEND srcs "adc_common.c"
                     "adc.c"
                     "dedic_gpio.c"
                     "gdma.c"
                     "sdmmc_host.c"
                     "sdmmc_transaction.c"
                     "mcpwm.c"
                     "usb_serial_jtag.c"
                     "spi_slave_hd.c"
                     "touch_sensor_common.c"
                     "esp32s3/touch_sensor.c"
                     "esp32s3/rtc_tempsensor.c"
                    )
endif()

if(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND srcs "gdma.c"
                     "spi_slave_hd.c"
                     "adc_common.c"
                     "dedic_gpio.c"
                     "usb_serial_jtag.c"
                     "adc.c"
                     "esp32c3/adc2_init_cal.c"
                     "esp32c3/rtc_tempsensor.c")
endif()

if(IDF_TARGET STREQUAL "esp32h2")
    list(APPEND srcs "gdma.c"
                     "spi_slave_hd.c"
                     "adc_common.c"
                     "dedic_gpio.c"
                     "adc.c"
                     "esp32h2/rtc_tempsensor.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes}
                    PRIV_INCLUDE_DIRS "include/driver"
                    PRIV_REQUIRES efuse esp_timer esp_ipc
                    REQUIRES esp_pm esp_ringbuf freertos soc hal esp_hw_support
                    LDFRAGMENTS linker.lf)
# (REQUIRES cannot hide soc headers, since many arguments in the driver headers are chip-dependent)

# uses C11 atomic feature
set_source_files_properties(spi_master.c PROPERTIES COMPILE_FLAGS -std=gnu11)
