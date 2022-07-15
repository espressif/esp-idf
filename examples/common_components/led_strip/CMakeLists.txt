set(srcs "src/led_strip_api.c")

if(CONFIG_SOC_RMT_SUPPORTED)
    list(APPEND srcs "src/led_strip_rmt_dev.c" "src/led_strip_rmt_encoder.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" "interface"
                       PRIV_REQUIRES "driver")
