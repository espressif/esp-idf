idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")
set(priv_requires esp_pm esp_driver_gpio)
set(requires esp_hal_gpio)
if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "src/sdm.c")
    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "src/${target}/sdm_retention.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_INCLUDE_DIRS "src"
                       PRIV_REQUIRES "${priv_requires}"
                       REQUIRES "${requires}"
                       LDFRAGMENTS "linker.lf"
                      )
