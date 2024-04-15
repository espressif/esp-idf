set(srcs)
set(public_include "include")
if(CONFIG_SOC_PPA_SUPPORTED)
    list(APPEND srcs "src/ppa_core.c"
                     "src/ppa_srm.c"
                     "src/ppa_blend.c"
                     "src/ppa_fill.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES esp_mm esp_pm
                    #    LDFRAGMENTS "linker.lf"
                      )
