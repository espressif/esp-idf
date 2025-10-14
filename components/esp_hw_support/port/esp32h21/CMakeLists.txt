set(srcs "rtc_clk.c"
         "rtc_time.c"
         "chip_info.c"
         "rtc_clk_init.c"
         )

if(CONFIG_SOC_PMU_SUPPORTED)
    list(APPEND srcs "pmu_param.c"
                     "pmu_init.c"
                     "pmu_sleep.c"
        )
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include private_include)
