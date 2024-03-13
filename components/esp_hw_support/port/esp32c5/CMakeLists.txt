set(srcs "rtc_clk_init.c"
         "rtc_clk.c"
         "rtc_time.c"
         "pmu_init.c"
         "pmu_param.c"
         "chip_info.c"
         )

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "sar_periph_ctrl.c"
                     "esp_crypto_lock.c")
endif()

# TODO: [ESP32C5] IDF-8667
if(IDF_TARGET STREQUAL "esp32c5")
    list(REMOVE_ITEM srcs "pmu_init.c"
                          "pmu_param.c"
                          "sar_periph_ctrl.c")
endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include private_include)
