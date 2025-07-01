set(srcs "rtc_clk.c"
         "rtc_time.c"
         "chip_info.c"
         )

if(CONFIG_SOC_PMU_SUPPORTED)
    list(APPEND srcs
                # "rtc_clk_init.c" //TODO: IDF-12313
                "pmu_param.c"
                "pmu_init.c"
                "pmu_sleep.c"
             )
endif()

if(NOT BOOTLOADER_BUILD)
    # list(APPEND srcs "sar_periph_ctrl.c") // TODO: [ESP32H4] IDF-12368

    if(CONFIG_ESP_SYSTEM_MEMPROT_FEATURE)
        list(APPEND srcs "esp_memprot.c" "../esp_memprot_conv.c")
    endif()

endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . private_include include)
target_include_directories(${COMPONENT_LIB} PRIVATE ../hal)
