add_library(soc_esp32 STATIC
    "adc_periph.c"
    "dac_periph.c"
    "gpio_periph.c"
    "rtc_io_periph.c"
    "rtc_periph.c"
    "sdio_slave_periph.c"
    "sdmmc_periph.c"
    "interrupts.c"
    "spi_periph.c"
    "ledc_periph.c"
    "i2s_periph.c"
    "i2c_periph.c"
    "uart_periph.c"
    "touch_sensor_periph.c")

target_include_directories(soc_esp32 PUBLIC include ../include private_include)