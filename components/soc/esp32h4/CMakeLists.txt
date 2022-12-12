set(srcs
    "adc_periph.c"
    "dedic_gpio_periph.c"
    "gdma_periph.c"
    "gpio_periph.c"
    "sdm_periph.c"
    "interrupts.c"
    "spi_periph.c"
    "ledc_periph.c"
    "rmt_periph.c"
    "i2s_periph.c"
    "i2c_periph.c"
    "uart_periph.c"
    "temperature_sensor_periph.c"
    "timer_periph.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")

set(inc_path "." "include")

if(target STREQUAL "esp32h4")
    if(CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_1)
        list(APPEND inc_path "include/rev1")
    elseif(CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_2)
        list(APPEND inc_path "include/rev2")
    endif()
endif()

target_include_directories(${COMPONENT_LIB} PUBLIC ${inc_path})
