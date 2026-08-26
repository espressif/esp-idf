idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(includes "include")
set(srcs)
# ASRC related source files
if(CONFIG_SOC_ASRC_SUPPORTED)
    list(APPEND srcs "asrc_adapter.c")
    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "${target}/asrc_retention.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS "."
                       REQUIRES esp_hal_asrc esp_driver_dma esp_mm
                       PRIV_REQUIRES esp_hw_support esp_pm)
