# TODO esp32s2beta: Use require_idf_targets here
if(IDF_TARGET STREQUAL "esp32s2beta")
    return()
endif()

idf_component_register(SRCS "esp_adc_cal.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver)
