set(srcs "adc_hal.c"
        "dac_hal.c"
        "brownout_hal.c"
        "rtc_clk.c"
        "regi2c_ctrl.c"
        "rtc_clk_init.c"
        "rtc_init.c"
        "rtc_pm.c"
        "rtc_sleep.c"
        "rtc_time.c"
        "rtc_wdt.c"
        "soc_memory_layout.c"
        "systimer_hal.c"
        "touch_sensor_hal.c"
        "usb_hal.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include)
target_include_directories(${COMPONENT_LIB} PRIVATE ../hal)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/rtc_clk.c" PROPERTIES
                                COMPILE_FLAGS "-fno-jump-tables -fno-tree-switch-conversion")
endif()
