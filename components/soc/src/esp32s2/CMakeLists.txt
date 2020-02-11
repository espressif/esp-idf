set(srcs "brownout_hal.c" 
        "cpu_util.c"
        "rtc_clk.c"
        "rtc_init.c"
        "rtc_pm.c"
        "rtc_sleep.c"
        "rtc_time.c"
        "rtc_wdt.c"
        "soc_memory_layout.c"
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
