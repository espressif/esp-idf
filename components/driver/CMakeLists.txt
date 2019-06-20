set(COMPONENT_SRCS "can.c"
                   "gpio.c"
                   "i2c.c"
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

if(CONFIG_IDF_TARGET_ESP32)
    # SDMMC and MCPWM are in ESP32 only, I2S not ported yet.
    list(APPEND COMPONENT_SRCS "i2s.c"
                               "mcpwm.c"
                               "sdio_slave.c"
                               "sdmmc_host.c"
                               "sdmmc_transaction.c")
endif()

set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "include/driver")
set(COMPONENT_REQUIRES esp_ringbuf soc) #cannot totally hide soc headers, since there are a lot arguments in the driver are chip-dependent

if(CONFIG_IDF_TARGET_ESP32S2BETA)
    list(APPEND COMPONENT_SRCS "${CONFIG_IDF_TARGET}/rtc_tempsensor.c"
                               "${CONFIG_IDF_TARGET}/rtc_touchpad.c")

    list(APPEND COMPONENT_ADD_INCLUDEDIRS "${CONFIG_IDF_TARGET}/include")
endif()

register_component()


if(GCC_NOT_5_2_0)
    # uses C11 atomic feature
    set_source_files_properties(spi_master.c PROPERTIES COMPILE_FLAGS -std=gnu11)
endif()
