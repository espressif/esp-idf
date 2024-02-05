set(srcs)
set(public_include "include")
if(CONFIG_SOC_PPA_SUPPORTED)
    list(APPEND srcs "src/ppa.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES esp_mm
                    #    LDFRAGMENTS "linker.lf"
                      )
