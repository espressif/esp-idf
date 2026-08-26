set(srcs "rtc_clk_init.c"
         "rtc_clk.c"
         "rtc_init.c"
         "rtc_sleep.c"
         "rtc_time.c"
         "chip_info.c")

if(NOT BOOTLOADER_BUILD)

    list(APPEND srcs "sar_periph_ctrl.c")

endif()

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include)
