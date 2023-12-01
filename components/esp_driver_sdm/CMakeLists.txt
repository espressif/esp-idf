set(srcs)
set(public_include "include")
if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "src/sdm.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "esp_pm" "esp_driver_gpio"
                       LDFRAGMENTS "linker.lf"
                      )
