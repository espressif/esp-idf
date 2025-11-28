target_include_directories(${COMPONENT_LIB} PUBLIC .)

set(srcs
         "rtc_clk.c"
         "rtc_time.c"
         "chip_info.c"
         )

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" "${srcs}")

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
target_include_directories(${COMPONENT_LIB} PUBLIC . include private_include)
