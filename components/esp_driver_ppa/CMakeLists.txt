idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")
if(CONFIG_SOC_PPA_SUPPORTED)
    list(APPEND srcs "src/ppa_core.c"
                     "src/ppa_srm.c"
                     "src/ppa_blend.c"
                     "src/ppa_fill.c")
    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "${target}/ppa_retention.c")
    endif()
endif()

set(priv_requires esp_mm esp_pm esp_driver_dma efuse)


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_INCLUDE_DIRS "src"
                       REQUIRES esp_hal_ppa
                       PRIV_REQUIRES "${priv_requires}"
                      )
