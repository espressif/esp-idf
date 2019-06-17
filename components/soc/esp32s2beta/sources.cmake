set(SOC_SRCS    "cpu_util.c"
                "rtc_clk.c"
                "rtc_init.c"
                "rtc_pm.c"
                "rtc_sleep.c"
                "rtc_time.c"
                "soc_memory_layout.c"
                "spi_periph.c"
                "gpio_periph.c"
                "rtc_periph.c")

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    set_source_files_properties("esp32s2beta/rtc_clk.c" PROPERTIES
                                COMPILE_FLAGS "-fno-jump-tables -fno-tree-switch-conversion")
endif()
