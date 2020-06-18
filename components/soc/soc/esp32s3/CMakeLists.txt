add_library(soc_esp32s3 STATIC
    "adc_periph.c"
    "dac_periph.c"
    "gpio_periph.c"
    "i2c_periph.c"
    "i2s_periph.c"
    "interrupts.c"
    "ledc_periph.c"
    "rtc_io_periph.c"
    "rtc_periph.c"
    "sdio_slave_periph.c"
    "sdmmc_periph.c"
    "spi_periph.c"
    "touch_sensor_periph.c"
    "uart_periph.c")

target_include_directories(soc_esp32s3 PUBLIC . include ../include)
