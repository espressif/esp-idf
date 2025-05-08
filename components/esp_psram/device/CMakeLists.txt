set(srcs)

if(CONFIG_SPIRAM_MODE_QUAD)
    if(NOT ${target} STREQUAL "esp32" AND NOT ${target} STREQUAL "esp32s2")
        list(APPEND srcs "esp_psram_impl_ap_quad.c")
    endif()
endif()

if(CONFIG_SPIRAM_MODE_HEX)
    list(APPEND srcs "esp_psram_impl_ap_hex.c")
endif()

target_sources(${COMPONENT_LIB} PRIVATE "${srcs}")
