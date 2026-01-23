idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_PPA_SUPPORTED)
    list(APPEND srcs "src/ppa_core.c"
                     "src/ppa_srm.c"
                     "src/ppa_blend.c"
                     "src/ppa_fill.c")
endif()

if(${target} STREQUAL "linux")
    set(priv_requires "")
else()
    set(priv_requires esp_mm esp_pm esp_driver_dma)
endif()


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES esp_hal_ppa
                       PRIV_REQUIRES "${priv_requires}"
                      )
