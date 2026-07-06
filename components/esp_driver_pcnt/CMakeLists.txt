idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")
if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "src/pulse_cnt.c")
endif()

set(requires esp_hal_pcnt)
set(priv_requires esp_pm esp_driver_gpio)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES "${requires}"
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS "linker.lf"
                      )
