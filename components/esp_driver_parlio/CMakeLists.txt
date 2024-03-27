set(srcs)
set(public_include "include")
if(CONFIG_SOC_PARLIO_SUPPORTED)
    list(APPEND srcs "src/parlio_common.c"
                     "src/parlio_tx.c"
                     "src/parlio_rx.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "esp_pm" "esp_driver_gpio" "esp_mm"
                      )
