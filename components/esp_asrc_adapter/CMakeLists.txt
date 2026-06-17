idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(includes "include")
set(srcs)
# ASRC related source files
if(CONFIG_SOC_ASRC_SUPPORTED)
    list(APPEND srcs "asrc_adapter.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES esp_hal_asrc esp_driver_dma esp_mm)
