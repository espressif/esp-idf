set(COMPONENT_SRCS "can.c"
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
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "include/driver")

set(COMPONENT_REQUIRES esp_ringbuf)

register_component()
