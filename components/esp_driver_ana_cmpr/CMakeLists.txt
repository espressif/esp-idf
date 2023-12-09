set(srcs)

# Analog comparator related source files
if(CONFIG_SOC_ANA_CMPR_SUPPORTED)
    list(APPEND srcs "ana_cmpr.c")
    if(CONFIG_SOC_ANA_CMPR_SUPPORT_ETM)
        list(APPEND srcs "ana_cmpr_etm.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES esp_pm esp_driver_gpio
                           LDFRAGMENTS "linker.lf"
                          )
