set(srcs
    "adc_periph.c"
    "dac_periph.c"
    "gpio_periph.c"
    "i2c_periph.c"
    "i2s_periph.c"
    "interrupts.c"
    "pcnt_periph.c"
    "ledc_periph.c"
    "rtc_clk.c"
    "rtc_clk_init.c"
    "rtc_init.c"
    "rtc_io_periph.c"
    "rtc_periph.c"
    "rtc_pm.c"
    "rtc_sleep.c"
    "rtc_time.c"
    "rtc_wdt.c"
    "sdio_slave_periph.c"
    "sdmmc_periph.c"
    "soc_memory_layout.c"
    "spi_periph.c"
    "touch_sensor_periph.c"
    "uart_periph.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/rtc_clk.c" PROPERTIES
                                COMPILE_FLAGS "-fno-jump-tables -fno-tree-switch-conversion")
endif()
