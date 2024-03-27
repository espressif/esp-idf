idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(include "include")

# I2S related source files
if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs "i2s_common.c"
                     "i2s_platform.c"
                     "i2s_std.c")
    if(CONFIG_SOC_I2S_SUPPORTS_PDM)
        list(APPEND srcs "i2s_pdm.c")
    endif()
    if(CONFIG_SOC_I2S_SUPPORTS_TDM)
        list(APPEND srcs "i2s_tdm.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${include}
                    PRIV_REQUIRES esp_driver_gpio esp_pm esp_mm
                    )
