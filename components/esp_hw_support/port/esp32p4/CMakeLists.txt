target_include_directories(${COMPONENT_LIB} PUBLIC .)

set(srcs "rtc_clk_init.c"
         "rtc_clk.c"
         "pmu_param.c"
         "pmu_init.c"
         "pmu_sleep.c"
         "rtc_time.c"
         "chip_info.c"
         )

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "sar_periph_ctrl.c")

    if(CONFIG_SOC_PM_SLEEP_CLK_ICG_USE_REGDMA AND NOT CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)
        list(APPEND srcs "pmu_sleep_clock_icg.c")
    endif()
endif()

if(NOT BOOTLOADER_BUILD AND CONFIG_ESP_ENABLE_PVT)
    list(APPEND srcs "pmu_pvt.c")
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include private_include)
