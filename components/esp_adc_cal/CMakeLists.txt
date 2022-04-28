idf_build_get_property(target IDF_TARGET)

set(srcs "esp_adc_cal_common.c")
set(src_target "${target}/esp_adc_cal.c")
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${src_target}")
    list(APPEND srcs ${src_target})
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       REQUIRES driver
                       PRIV_REQUIRES efuse)
