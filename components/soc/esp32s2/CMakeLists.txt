set(srcs
    "adc_periph.c"
    "dac_periph.c"
    "dedic_gpio_periph.c"
    "gpio_periph.c"
    "sdm_periph.c"
    "i2c_periph.c"
    "i2s_periph.c"
    "interrupts.c"
    "lcd_periph.c"
    "ledc_periph.c"
    "pcnt_periph.c"
    "rmt_periph.c"
    "rtc_io_periph.c"
    "spi_periph.c"
    "timer_periph.c"
    "touch_sensor_periph.c"
    "twai_periph.c"
    "uart_periph.c"
    "usb_periph.c"
    "temperature_sensor_periph.c"
    "usb_otg_periph.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include)
