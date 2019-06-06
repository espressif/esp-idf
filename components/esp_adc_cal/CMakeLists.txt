if(IDF_TARGET STREQUAL "esp32s2beta")
    return()
endif()
set(COMPONENT_SRCS "esp_adc_cal.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES)

register_component()
