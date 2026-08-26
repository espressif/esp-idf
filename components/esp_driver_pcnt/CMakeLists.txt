idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "src/pulse_cnt.c")
endif()

if(${target} STREQUAL "linux")
    set(requires "")
    set(priv_requires "")
else()
    set(requires esp_hal_pcnt)
    set(priv_requires esp_pm esp_driver_gpio)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES "${requires}"
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS "linker.lf"
                      )
