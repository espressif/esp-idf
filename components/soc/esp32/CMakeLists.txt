set(srcs
    "adc_periph.c"
    "dac_periph.c"
    "dport_access.c"
    "gpio_periph.c"
    "sdm_periph.c"
    "i2c_periph.c"
    "i2s_periph.c"
    "interrupts.c"
    "lcd_periph.c"
    "ledc_periph.c"
    "mcpwm_periph.c"
    "pcnt_periph.c"
    "rmt_periph.c"
    "rtc_io_periph.c"
    "sdio_slave_periph.c"
    "sdmmc_periph.c"
    "spi_periph.c"
    "timer_periph.c"
    "twai_periph.c"
    "touch_sensor_periph.c"
    "uart_periph.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include)

#esp_dport_access_reg_read is added as an undefined symbol because otherwise
#the linker can ignore dport_access.c as it would no other files depending on any symbols in it.
set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u esp_dport_access_reg_read")
