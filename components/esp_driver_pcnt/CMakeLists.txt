idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "src/pulse_cnt.c")
endif()

if(${target} STREQUAL "linux")
    set(priv_requires "")
else()
    set(priv_requires esp_pm esp_driver_gpio)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS "linker.lf"
                      )
