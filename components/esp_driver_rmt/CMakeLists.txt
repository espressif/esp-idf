set(srcs)
set(public_include "include")
if(CONFIG_SOC_RMT_SUPPORTED)
    list(APPEND srcs "src/rmt_common.c"
                     "src/rmt_encoder.c"
                     "src/rmt_rx.c"
                     "src/rmt_tx.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "esp_pm" "esp_driver_gpio" "esp_mm"
                       LDFRAGMENTS "linker.lf"
                      )
