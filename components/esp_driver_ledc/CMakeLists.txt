
set(srcs)
set(public_include "include")

if(CONFIG_SOC_LEDC_SUPPORTED)
    list(APPEND srcs "src/ledc.c")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${public_include}
    PRIV_REQUIRES "esp_pm" "esp_driver_gpio"
    LDFRAGMENTS "linker.lf"
)
