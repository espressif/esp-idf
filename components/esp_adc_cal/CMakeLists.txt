idf_build_get_property(target IDF_TARGET)
# ToDo: re-enable adc-cal for other target
if(NOT ${target} STREQUAL "esp32")
    return()
endif()

idf_component_register(SRCS "esp_adc_cal.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver)
