set(SOC_SRCS "cpu_util.c"
    "gpio_periph.c"
    "rtc_clk.c"
    "rtc_clk_init.c"
    "rtc_init.c"
    "rtc_periph.c"
    "rtc_pm.c"
    "rtc_sleep.c"
    "rtc_time.c"
    "rtc_wdt.c"
    "sdio_slave_periph.c"
    "sdmmc_periph.c"
    "interrupts.c"
    "soc_memory_layout.c"
    "spi_periph.c"
    "ledc_periph.c"
    "i2s_periph.c")

if(NOT BOOTLOADER_BUILD AND CONFIG_ETH_USE_ESP32_EMAC)
    list(APPEND SOC_SRCS "emac_hal.c")
endif()

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    set_source_files_properties("esp32/rtc_clk.c" PROPERTIES
                                COMPILE_FLAGS "-fno-jump-tables -fno-tree-switch-conversion")
endif()
