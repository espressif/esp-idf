set(srcs
    "adc_common.c"
    "dac_common.c"
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
    "touch_sensor_common.c"
    "twai.c"
    "uart.c")

set(includes "include")

if(IDF_TARGET STREQUAL "esp32")
    # SDMMC and MCPWM are in ESP32 only.
    list(APPEND srcs "mcpwm.c"
                     "sdio_slave.c"
                     "sdmmc_host.c"
                     "sdmmc_transaction.c"
                     "esp32/touch_sensor.c"
                     "esp32/adc.c"
                     "esp32/dac.c")
    list(APPEND includes "esp32/include")
endif()

if(IDF_TARGET STREQUAL "esp32s2")
    list(APPEND srcs "esp32s2/rtc_tempsensor.c"
                     "esp32s2/touch_sensor.c"
                     "esp32s2/adc.c"
                     "esp32s2/adc2_init_cal.c"
                     "esp32s2/dac.c")
    # currently only S2 beta has its own target-specific includes
    list(APPEND includes "esp32s2/include")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes}
                    PRIV_INCLUDE_DIRS "include/driver"
                    PRIV_REQUIRES efuse esp_timer esp_ipc
                    REQUIRES esp_ringbuf freertos soc  #cannot totally hide soc headers, since there are a lot arguments in the driver are chip-dependent
                    LDFRAGMENTS linker.lf)

# uses C11 atomic feature
set_source_files_properties(spi_master.c PROPERTIES COMPILE_FLAGS -std=gnu11)
