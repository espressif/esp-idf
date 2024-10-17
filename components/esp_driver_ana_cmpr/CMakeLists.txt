idf_build_get_property(target IDF_TARGET)

set(srcs)

# Analog comparator related source files
if(CONFIG_SOC_ANA_CMPR_SUPPORTED)
    list(APPEND srcs "ana_cmpr.c")
    if(CONFIG_SOC_ANA_CMPR_SUPPORT_ETM)
        list(APPEND srcs "ana_cmpr_etm.c")
    endif()
endif()

if(${target} STREQUAL "linux")
    set(priv_requires "")
else()
    set(priv_requires esp_pm esp_driver_gpio)
endif()

idf_component_register(SRCS ${srcs}
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES "${priv_requires}"
                           LDFRAGMENTS "linker.lf"
                          )
