set(srcs
    "can.c"
    "gpio.c"
    "i2c.c"
    "i2s.c"
    "ledc.c"
    "pcnt.c"
    "periph_ctrl.c"
    "rmt.c"
    "rtc_module.c"
    "sdspi_crc.c"
    "sdspi_host.c"
    "sdspi_transaction.c"
    "sigmadelta.c"
    "spi_common.c"
    "spi_master.c"
    "spi_slave.c"
    "timer.c"
    "uart.c")

set(includes "include")

if(IDF_TARGET STREQUAL "esp32")
    # SDMMC and MCPWM are in ESP32 only.
    list(APPEND srcs "mcpwm.c"
                     "sdio_slave.c"
                     "sdmmc_host.c"
                     "sdmmc_transaction.c")
endif()

if(IDF_TARGET STREQUAL "esp32s2beta")
    list(APPEND srcs "esp32s2beta/rtc_tempsensor.c"
                     "esp32s2beta/rtc_touchpad.c")
    # currently only S2 beta has its own target-specific includes
    list(APPEND includes "esp32s2beta/include")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes}
                    PRIV_INCLUDE_DIRS "include/driver"
                    REQUIRES esp_ringbuf soc) #cannot totally hide soc headers, since there are a lot arguments in the driver are chip-dependent

if(GCC_NOT_5_2_0)
    # uses C11 atomic feature
    set_source_files_properties(spi_master.c PROPERTIES COMPILE_FLAGS -std=gnu11)
endif()
