idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")

set(requires esp_hal_cordic)
set(priv_requires esp_pm)

if(CONFIG_SOC_CORDIC_SUPPORTED)
    list(APPEND srcs
                "cordic.c"
    )
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "${priv_requires}"
                       REQUIRES "${requires}"
                       LDFRAGMENTS linker.lf
                      )
