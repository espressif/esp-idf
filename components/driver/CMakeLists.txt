set(srcs 
    "can.c"
    "gpio.c"
    "i2c.c"
    "i2s.c"
    "ledc.c"
    "mcpwm.c"
    "pcnt.c"
    "periph_ctrl.c"
    "rmt.c"
    "rtc_module.c"
    "sdio_slave.c"
    "sdmmc_host.c"
    "sdmmc_transaction.c"
    "sdspi_crc.c"
    "sdspi_host.c"
    "sdspi_transaction.c"
    "sigmadelta.c"
    "spi_common.c"
    "spi_master.c"
    "spi_slave.c"
    "timer.c"
    "uart.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "include/driver"
                    REQUIRES esp_ringbuf soc) #cannot totally hide soc headers, since there are a lot arguments in the driver are chip-dependent

if(GCC_NOT_5_2_0)
    # uses C11 atomic feature
    set_source_files_properties(spi_master.c PROPERTIES COMPILE_FLAGS -std=gnu11)
endif()