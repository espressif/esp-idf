set(srcs
    "adc_periph.c"
    "dedic_gpio_periph.c"
    "gdma_periph.c"
    "gpio_periph.c"
    "sdm_periph.c"
    "interrupts.c"
    "spi_periph.c"
    "ledc_periph.c"
    "pcnt_periph.c"
    "mcpwm_periph.c"
    "rmt_periph.c"
    "i2s_periph.c"
    "i2c_periph.c"
    "uart_periph.c"
    "temperature_sensor_periph.c"
    "timer_periph.c"
    "twai_periph.c")

# ESP32C6-TODO
list(REMOVE_ITEM srcs
        "adc_periph.c"                  # TODO: IDF-5310
        "ledc_periph.c"                 # TODO: IDF-5328
        "temperature_sensor_periph.c"   # TODO: IDF-5322
        )

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include)
