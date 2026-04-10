idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "esp_twai.c")
set(public_include "include")
set(priv_req esp_driver_gpio esp_pm esp_timer)

if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "esp_twai_onchip.c")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${public_include}
    REQUIRES esp_hal_twai
    PRIV_REQUIRES ${priv_req}
    LDFRAGMENTS "linker.lf"
)
