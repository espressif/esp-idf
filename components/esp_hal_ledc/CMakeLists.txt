idf_build_get_property(target IDF_TARGET)

set(srcs)
set(includes "include")

# Add target-specific include directory if it exists
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

if(CONFIG_SOC_LEDC_SUPPORTED)
    list(APPEND srcs "ledc_hal.c" "ledc_hal_iram.c" "${target}/ledc_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
