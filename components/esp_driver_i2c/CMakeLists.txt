idf_build_get_property(target IDF_TARGET)

set(srcs)
set(include "include")

# I2C related source files
if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs
                     "i2c_master.c"
                     "i2c_common.c"
        )
    if(CONFIG_SOC_I2C_SUPPORT_SLAVE)
        list(APPEND srcs "i2c_slave.c")
    endif()

endif()

if(${target} STREQUAL "linux")
    set(priv_requires esp_ringbuf)
else()
    set(priv_requires esp_driver_gpio esp_pm esp_ringbuf)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${include}
                    PRIV_REQUIRES "${priv_requires}"
                    LDFRAGMENTS "linker.lf"
                    )
