idf_build_get_property(non_os_build NON_OS_BUILD)

set(srcs "rtc_clk_init.c"
    "rtc_time.c"
    "rtc_clk.c"
    "pmu_param.c"
    "pmu_init.c"
    "pmu_sleep.c"
    "chip_info.c"
    "ocode_init.c"
)

if(NOT non_os_build)
    list(APPEND srcs "sar_periph_ctrl.c")
endif()

if(NOT BOOTLOADER_BUILD AND CONFIG_ESP_ENABLE_PVT)
    list(APPEND srcs "pmu_pvt.c")
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include private_include)
