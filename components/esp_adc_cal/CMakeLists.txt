idf_build_get_property(target IDF_TARGET)

set(srcs "esp_adc_cal_${target}.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       REQUIRES driver efuse)
