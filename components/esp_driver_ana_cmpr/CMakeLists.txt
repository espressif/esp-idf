idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)

# Analog comparator related source files
if(CONFIG_SOC_ANA_CMPR_SUPPORTED)
    list(APPEND srcs "ana_cmpr.c")
    if(CONFIG_SOC_ANA_CMPR_SUPPORT_ETM)
        list(APPEND srcs "ana_cmpr_etm.c")
    endif()
endif()

set(priv_requires esp_pm esp_driver_gpio)

idf_component_register(SRCS ${srcs}
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES ${priv_requires}
                           REQUIRES esp_hal_ana_cmpr
                           LDFRAGMENTS "linker.lf"
                          )
