idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

# target specific include must be added before the generic one
set(includes)
list(APPEND includes "include")

set(srcs)

if(CONFIG_SOC_EMAC_SUPPORTED)
    list(APPEND srcs "emac_hal.c")
    list(APPEND srcs "${target}/emac_periph.c")
    list(APPEND includes "${target}/include")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES soc hal esp_hal_gpio)
