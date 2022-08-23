set(srcs
    "adc_periph.c"
    "dedic_gpio_periph.c"
    "gdma_periph.c"
    "gpio_periph.c"
    "interrupts.c"
    "spi_periph.c"
    "ledc_periph.c"
    "i2c_periph.c"
    "uart_periph.c"
    "temperature_sensor_periph.c"
    "timer_periph.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include)
